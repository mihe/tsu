import 'source-map-support/register';

import * as path from 'path';
import * as ts from 'typescript';

interface ParsedType {
	name: string;
	dimensions: number;
}

interface ParsedParameter {
	name: string;
	types: ParsedType[];
	optional: boolean;
}

interface ParsedFunction {
	name: string;
	parameters: ParsedParameter[];
	returnTypes?: ParsedType[];
	character: number;
	line: number;
}

interface Request {
	file: string;
}

interface ResponseSuccess {
	fileName: string;
	name: string;
	path: string;
	source: string;
	errors: string[];
	exports: ParsedFunction[];
	dependencies: string[];
}

interface ResponseFailure {
	errors: string[];
}

type Response = ResponseSuccess | ResponseFailure;

interface ScriptVersion {
	version: number;
	modifiedTime: number;
}

const arg = process.argv[2];
if (typeof arg !== 'string') {
	throw new Error('Invalid project directory specified');
}

const projectDirectory = path.resolve(arg);
const responseCache = new Map<string, string>();
const scriptFileNames = new Array<string>();
const scriptVersions = new Map<string, ScriptVersion>();
const fileCache = new Map<string, ts.IScriptSnapshot>();
const compilerOptions = findCompilerOptions();
const languageService = createLanguageService();

function findCompilerOptions() {
	const configPath = ts.findConfigFile(projectDirectory, ts.sys.fileExists);
	if (!configPath) {
		throw new Error(`Failed to find tsconfig in: ${projectDirectory}`);
	}

	const configJson = ts.readConfigFile(configPath, ts.sys.readFile);
	if (configJson.error) {
		throw new Error(formatDiagnostic(configJson.error));
	}

	const config = ts.parseJsonConfigFileContent(
		configJson.config,
		{
			readDirectory: ts.sys.readDirectory,
			fileExists: ts.sys.fileExists,
			readFile: ts.sys.readFile,
			useCaseSensitiveFileNames: false
		},
		projectDirectory
	);

	if (config.errors.length) {
		throw new Error(config.errors.map(error => (
			formatDiagnostic(error)
		)).join('\n'));
	}

	return config.options;
}

function formatDiagnostic(diagnostic: ts.Diagnostic) {
	const msg = ts.flattenDiagnosticMessageText(diagnostic.messageText, '\n');

	if (diagnostic.file) {
		const pos = diagnostic.file.getLineAndCharacterOfPosition(
			diagnostic.start || 0
		);

		const file = path.basename(diagnostic.file.fileName);
		const line = pos.line + 1;
		const char = pos.character + 1;
		const category = diagnostic.category;
		const type = ts.DiagnosticCategory[category].toLowerCase();
		const code = diagnostic.code;

		return `[TS]: ${file}(${line},${char}): ${type} TS${code}: ${msg}`;
	}
	else {
		return `[TS]: ${msg}`;
	}
}

function createLanguageService() {
	return ts.createLanguageService({
		getScriptFileNames: () => scriptFileNames,
		getScriptVersion: fileName => {
			const scriptVersion = scriptVersions.get(fileName);
			return scriptVersion ? scriptVersion.version.toString() : '0';
		},
		getScriptSnapshot: fileName => {
			const cachedSnapshot = fileCache.get(fileName);
			if (cachedSnapshot) { return cachedSnapshot; }

			const fileContent = ts.sys.readFile(fileName);
			if (fileContent == undefined) {
				scriptVersions.delete(fileName);
				return undefined;
			}

			const snapshot = ts.ScriptSnapshot.fromString(fileContent);

			// Cache node_modules files since they shouldn't change
			if (fileName.search(/node_modules/) !== -1) {
				fileCache.set(fileName, snapshot);
			}

			if (!scriptVersions.get(fileName)) {
				scriptVersions.set(fileName, {
					version: 0,
					modifiedTime: Date.now()
				});
			}

			const scriptVersion = scriptVersions.get(fileName);
			if (!scriptVersion) { return undefined; }

			const modifiedTime = getModifiedTime(fileName);
			if (modifiedTime > scriptVersion.modifiedTime) {
				scriptVersion.modifiedTime = modifiedTime;
				scriptVersion.version += 1;
			}

			return snapshot;
		},
		getCurrentDirectory: () => projectDirectory,
		getCompilationSettings: () => compilerOptions,
		getDefaultLibFileName: ts.getDefaultLibFilePath,
		fileExists: ts.sys.fileExists,
		readFile: ts.sys.readFile,
		readDirectory: ts.sys.readDirectory
	}, ts.createDocumentRegistry());
}

function getModifiedTime(filePath: string) {
	const modifiedTime = ts.sys.getModifiedTime!(filePath);
	if (!modifiedTime) {
		throw new Error(`Failed to get last modified time for: '${filePath}'`);
	}

	return modifiedTime.getTime();
}

function parseFile(filePath: string): Response {
	const modifiedTime = getModifiedTime(filePath);

	let scriptVersion = scriptVersions.get(filePath);
	if (!scriptVersion) {
		scriptVersion = {
			version: 0,
			modifiedTime: modifiedTime
		};

		scriptFileNames.push(filePath);
		scriptVersions.set(filePath, scriptVersion);
	}
	else {
		scriptVersion.version += 1;
	}

	const program = languageService.getProgram();
	if (program == undefined) {
		throw new Error('Failed to get program');
	}

	const sourceFile = program.getSourceFile(filePath);
	if (sourceFile == undefined) {
		throw new Error(`Failed to get source file: ${filePath}`);
	}

	const diagnostics = ts.getPreEmitDiagnostics(program, sourceFile);
	const errors = diagnostics.map(formatDiagnostic);
	if (errors.length > 0) {
		return { errors: errors };
	}

	const emitOutput = languageService.getEmitOutput(filePath);
	if (emitOutput.emitSkipped) {
		throw new Error('Emit skipped');
	}

	const numOutput = emitOutput.outputFiles.length;
	if (numOutput !== 1) {
		throw new Error(`Unexpected number of output files: ${numOutput}`);
	}

	scriptVersion.modifiedTime = modifiedTime;

	const output = emitOutput.outputFiles[0];
	const sources = new Map<string, string>();
	sources.set(filePath, output.text);

	const fileExtension = path.parse(filePath).ext;
	const fileName = path.basename(filePath);
	const plainName = fileName.replace(fileExtension, '');

	const functions = (
		sourceFile.statements
			.filter(ts.isFunctionDeclaration)
			.filter(f => f.body != undefined)
			.filter(isExported)
			.map(declaration =>
				parseFunction(
					declaration,
					filePath,
					program,
					sourceFile,
					errors))
			.filter(isNonEmpty)
	);

	const dependencies = findDependencies(program, sourceFile);

	return {
		fileName: fileName,
		name: plainName,
		path: filePath,
		source: sources.get(filePath) || '',
		errors: errors,
		exports: functions,
		dependencies: dependencies
	};
}

function isExported(node: ts.Node) {
	const modifiers = node.modifiers;
	return modifiers && modifiers.some(mod =>
		mod.kind === ts.SyntaxKind.ExportKeyword
	);
}

function isNonEmpty<T>(value: T | null | undefined): value is T {
	return value != undefined;
}

function parseFunction(
	declaration: ts.FunctionDeclaration,
	filePath: string,
	program: ts.Program,
	sourceFile: ts.SourceFile,
	errors: string[]
): ParsedFunction | null {
	const typeChecker = program.getTypeChecker();

	const signature = typeChecker.getSignatureFromDeclaration(declaration)!;
	const returnType = typeChecker.getReturnTypeOfSignature(signature);
	const returnTypeStr = typeChecker.typeToString(returnType);
	const returnTypes = parseType(returnTypeStr);

	const position = getPosition(declaration, sourceFile);

	// Don't allow union types
	if (returnTypes.length > 1) {
		const file = path.basename(filePath);
		const line = position.line;
		const char = position.character;

		errors.push(
			`[TSU] ${file}(${line},${char}): ` +
			`Disallowed union return type (${returnTypeStr})`
		);

		return null;
	}

	const parameters = declaration.parameters.map(param => {
		return parseParameter(param, typeChecker);
	});

	return {
		name: declaration.name!.getText(),
		parameters: parameters,
		returnTypes: returnTypes,
		line: position.line,
		character: position.character
	};
}

function parseParameter(
	param: ts.ParameterDeclaration,
	typeChecker: ts.TypeChecker
) {
	const type = typeChecker.getTypeAtLocation(param.type!);
	const types = parseType(typeChecker.typeToString(type));
	const optional = (
		param.initializer == undefined
			? typeChecker.isOptionalParameter(param)
			: false
	);

	return {
		name: param.name.getText(),
		types: types,
		optional: optional
	};
}

function parseType(typeStr: string): ParsedType[] {
	return typeStr.split(' | ').map(str => {
		const name = str.replace(/\[\]/g, '');
		const dimensions = (str.match(/\[\]/g) || []).length;
		return {
			name: name,
			dimensions: dimensions
		};
	});
}

function getPosition(node: ts.Node, sourceFile: ts.SourceFile) {
	const pos = sourceFile.getLineAndCharacterOfPosition(
		node.getStart(sourceFile, true)
	);

	pos.line += 1;
	pos.character += 1;

	return pos;
}

function findDependencies(
	program: ts.Program,
	sourceFile: ts.SourceFile
) {
	const typeChecker = program.getTypeChecker();

	const isUObject = (type: ts.Type): boolean => {
		const typeName = typeChecker.typeToString(type);
		if (typeName === 'UObject') { return true; }

		const baseTypes = type.getBaseTypes() || [];
		return !!baseTypes.find(isUObject);
	};

	const dependencies = new Set<string>();

	const traverseTree = (node: ts.Node) => {
		ts.forEachChild(node, traverseTree);

		let type;
		try { type = typeChecker.getTypeAtLocation(node); }
		catch { return; }

		if (!type) { return; }

		const flagName = (ts.TypeFlags[type.flags] || '');
		const isLiteral = (flagName.search(/Literal/) !== -1);
		const isCallable = !type.getCallSignatures().length;
		const isValidNode = !isLiteral && isCallable;

		if (!isValidNode) { return; }

		const constructSignatures = type.getConstructSignatures();
		if (constructSignatures.length) {
			type = constructSignatures[0].getReturnType();
		}

		if (isUObject(type)) {
			const typeName = typeChecker.typeToString(type);
			dependencies.add(typeName);
		}
	};

	traverseTree(sourceFile);

	return Array.from(dependencies);
}

function writeResponse(response: Response | string) {
	if (typeof response !== 'string') {
		response = JSON.stringify(response, null, '  ');
	}

	process.stdout.write(response + '\n');

	return response;
}

function processRequest(requestStr: string) {
	const request = JSON.parse(requestStr) as Request;

	// Cache responses for a short period since Unreal compiles the source file
	// multiple times when compiling the Blueprint
	const cachedResponse = responseCache.get(request.file);
	if (cachedResponse) { return writeResponse(cachedResponse); }

	const response = parseFile(request.file);
	const responseStr = writeResponse(response);

	responseCache.set(request.file, responseStr);
	setTimeout(() => {
		responseCache.delete(request.file);
	}, 1000);

	return responseStr;
}

let input = '';

process.stdin.setEncoding('utf8');
process.stdin.on('data', (chunk: string) => {
	const lines = chunk.split(/\r?\n/);
	if (lines.length <= 1) {
		input += chunk;
		return;
	}

	const lastLine = lines.splice(-1, 1)[0];
	const firstLine = lines.splice(0, 1)[0];

	processRequest(input + firstLine);

	for (const line of lines) {
		processRequest(line);
	}

	input = lastLine;
});
