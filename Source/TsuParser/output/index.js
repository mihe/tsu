"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
require("source-map-support/register");
const path = require("path");
const ts = require("typescript");
const arg = process.argv[2];
if (typeof arg !== 'string') {
    throw new Error('Invalid project directory specified');
}
const projectDirectory = path.resolve(arg);
const responseCache = new Map();
const scriptFileNames = new Array();
const scriptVersions = new Map();
const fileCache = new Map();
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
    const config = ts.parseJsonConfigFileContent(configJson.config, {
        readDirectory: ts.sys.readDirectory,
        fileExists: ts.sys.fileExists,
        readFile: ts.sys.readFile,
        useCaseSensitiveFileNames: false
    }, projectDirectory);
    if (config.errors.length) {
        throw new Error(config.errors.map(error => (formatDiagnostic(error))).join('\n'));
    }
    return config.options;
}
function formatDiagnostic(diagnostic) {
    const msg = ts.flattenDiagnosticMessageText(diagnostic.messageText, '\n');
    if (diagnostic.file) {
        const pos = diagnostic.file.getLineAndCharacterOfPosition(diagnostic.start || 0);
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
            if (cachedSnapshot) {
                return cachedSnapshot;
            }
            const fileContent = ts.sys.readFile(fileName);
            if (fileContent == undefined) {
                scriptVersions.delete(fileName);
                return undefined;
            }
            const snapshot = ts.ScriptSnapshot.fromString(fileContent);
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
            if (!scriptVersion) {
                return undefined;
            }
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
function getModifiedTime(filePath) {
    const modifiedTime = ts.sys.getModifiedTime(filePath);
    if (!modifiedTime) {
        throw new Error(`Failed to get last modified time for: '${filePath}'`);
    }
    return modifiedTime.getTime();
}
function parseFile(filePath) {
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
    const sources = new Map();
    sources.set(filePath, output.text);
    const fileExtension = path.parse(filePath).ext;
    const fileName = path.basename(filePath);
    const plainName = fileName.replace(fileExtension, '');
    const functions = (sourceFile.statements
        .filter(ts.isFunctionDeclaration)
        .filter(f => f.body != undefined)
        .filter(isExported)
        .map(declaration => parseFunction(declaration, filePath, program, sourceFile, errors))
        .filter(isNonEmpty));
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
function isExported(node) {
    const modifiers = node.modifiers;
    return modifiers && modifiers.some(mod => mod.kind === ts.SyntaxKind.ExportKeyword);
}
function isNonEmpty(value) {
    return value != undefined;
}
function parseFunction(declaration, filePath, program, sourceFile, errors) {
    const typeChecker = program.getTypeChecker();
    const signature = typeChecker.getSignatureFromDeclaration(declaration);
    const returnType = typeChecker.getReturnTypeOfSignature(signature);
    const returnTypeStr = typeChecker.typeToString(returnType);
    const returnTypes = parseType(returnTypeStr);
    const position = getPosition(declaration, sourceFile);
    if (returnTypes.length > 1) {
        const file = path.basename(filePath);
        const line = position.line;
        const char = position.character;
        errors.push(`[TSU] ${file}(${line},${char}): ` +
            `Disallowed union return type (${returnTypeStr})`);
        return null;
    }
    const parameters = declaration.parameters.map(param => {
        return parseParameter(param, typeChecker);
    });
    return {
        name: declaration.name.getText(),
        parameters: parameters,
        returnTypes: returnTypes,
        line: position.line,
        character: position.character
    };
}
function parseParameter(param, typeChecker) {
    const type = typeChecker.getTypeAtLocation(param.type);
    const types = parseType(typeChecker.typeToString(type));
    const optional = (param.initializer == undefined
        ? typeChecker.isOptionalParameter(param)
        : false);
    return {
        name: param.name.getText(),
        types: types,
        optional: optional
    };
}
function parseType(typeStr) {
    return typeStr.split(' | ').map(str => {
        const name = str.replace(/\[\]/g, '');
        const dimensions = (str.match(/\[\]/g) || []).length;
        return {
            name: name,
            dimensions: dimensions
        };
    });
}
function getPosition(node, sourceFile) {
    const pos = sourceFile.getLineAndCharacterOfPosition(node.getStart(sourceFile, true));
    pos.line += 1;
    pos.character += 1;
    return pos;
}
function findDependencies(program, sourceFile) {
    const typeChecker = program.getTypeChecker();
    const isUObject = (type) => {
        const typeName = typeChecker.typeToString(type);
        if (typeName === 'UObject') {
            return true;
        }
        const baseTypes = type.getBaseTypes() || [];
        return !!baseTypes.find(isUObject);
    };
    const dependencies = new Set();
    const traverseTree = (node) => {
        ts.forEachChild(node, traverseTree);
        let type;
        try {
            type = typeChecker.getTypeAtLocation(node);
        }
        catch (_a) {
            return;
        }
        if (!type) {
            return;
        }
        const flagName = (ts.TypeFlags[type.flags] || '');
        const isLiteral = (flagName.search(/Literal/) !== -1);
        const isCallable = !type.getCallSignatures().length;
        const isValidNode = !isLiteral && isCallable;
        if (!isValidNode) {
            return;
        }
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
function writeResponse(response) {
    if (typeof response !== 'string') {
        response = JSON.stringify(response, null, '  ');
    }
    process.stdout.write(response + '\n');
    return response;
}
function processRequest(requestStr) {
    const request = JSON.parse(requestStr);
    const cachedResponse = responseCache.get(request.file);
    if (cachedResponse) {
        return writeResponse(cachedResponse);
    }
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
process.stdin.on('data', (chunk) => {
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
//# sourceMappingURL=data:application/json;base64,eyJ2ZXJzaW9uIjozLCJmaWxlIjoiaW5kZXguanMiLCJzb3VyY2VSb290IjoiIiwic291cmNlcyI6WyIuLi9zb3VyY2UvaW5kZXgudHMiXSwibmFtZXMiOltdLCJtYXBwaW5ncyI6Ijs7QUFBQSx1Q0FBcUM7QUFFckMsNkJBQTZCO0FBQzdCLGlDQUFpQztBQThDakMsTUFBTSxHQUFHLEdBQUcsT0FBTyxDQUFDLElBQUksQ0FBQyxDQUFDLENBQUMsQ0FBQztBQUM1QixJQUFJLE9BQU8sR0FBRyxLQUFLLFFBQVEsRUFBRTtJQUM1QixNQUFNLElBQUksS0FBSyxDQUFDLHFDQUFxQyxDQUFDLENBQUM7Q0FDdkQ7QUFFRCxNQUFNLGdCQUFnQixHQUFHLElBQUksQ0FBQyxPQUFPLENBQUMsR0FBRyxDQUFDLENBQUM7QUFDM0MsTUFBTSxhQUFhLEdBQUcsSUFBSSxHQUFHLEVBQWtCLENBQUM7QUFDaEQsTUFBTSxlQUFlLEdBQUcsSUFBSSxLQUFLLEVBQVUsQ0FBQztBQUM1QyxNQUFNLGNBQWMsR0FBRyxJQUFJLEdBQUcsRUFBeUIsQ0FBQztBQUN4RCxNQUFNLFNBQVMsR0FBRyxJQUFJLEdBQUcsRUFBOEIsQ0FBQztBQUN4RCxNQUFNLGVBQWUsR0FBRyxtQkFBbUIsRUFBRSxDQUFDO0FBQzlDLE1BQU0sZUFBZSxHQUFHLHFCQUFxQixFQUFFLENBQUM7QUFFaEQsU0FBUyxtQkFBbUI7SUFDM0IsTUFBTSxVQUFVLEdBQUcsRUFBRSxDQUFDLGNBQWMsQ0FBQyxnQkFBZ0IsRUFBRSxFQUFFLENBQUMsR0FBRyxDQUFDLFVBQVUsQ0FBQyxDQUFDO0lBQzFFLElBQUksQ0FBQyxVQUFVLEVBQUU7UUFDaEIsTUFBTSxJQUFJLEtBQUssQ0FBQywrQkFBK0IsZ0JBQWdCLEVBQUUsQ0FBQyxDQUFDO0tBQ25FO0lBRUQsTUFBTSxVQUFVLEdBQUcsRUFBRSxDQUFDLGNBQWMsQ0FBQyxVQUFVLEVBQUUsRUFBRSxDQUFDLEdBQUcsQ0FBQyxRQUFRLENBQUMsQ0FBQztJQUNsRSxJQUFJLFVBQVUsQ0FBQyxLQUFLLEVBQUU7UUFDckIs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