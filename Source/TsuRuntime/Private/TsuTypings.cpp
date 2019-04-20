#include "TsuTypings.h"

#include "TsuBlueprintGeneratedClass.h"
#include "TsuPaths.h"
#include "TsuReflection.h"
#include "TsuRuntimeLog.h"
#include "TsuUtilities.h"

#include "Engine/Engine.h"
#include "HAL/PlatformFilemanager.h"
#include "Internationalization/Regex.h"
#include "Misc/FileHelper.h"
#include "UObject/EnumProperty.h"
#include "UObject/Package.h"
#include "UObject/TextProperty.h"
#include "UObject/UnrealType.h"

#define TSU_WRITE(Format) Output.Append(TEXT(Format), ARRAY_COUNT(Format) - 1)
#define TSU_WRITELN(Format) TSU_WRITE(Format "\n")

#define TSU_WRITEF(Format, ...) Output.Append(FString::Printf(TEXT(Format), __VA_ARGS__))
#define TSU_WRITELNF(Format, ...) TSU_WRITEF(Format "\n", __VA_ARGS__)

const TCHAR* FTsuTypings::MetaHidden = TEXT("Hidden");
const TCHAR* FTsuTypings::MetaDisplayName = TEXT("DisplayName");

FString& FTsuTypings::ResetPersistentOutputBuffer()
{
	static FString Buffer;
	Buffer.Reserve(8 * 1024 * 1024);
	Buffer.Reset();
	return Buffer;
}

bool FTsuTypings::IsReservedIdentifier(const FString& Word)
{
	// List of types: https://bit.ly/2I1W2q1
	// List of keywords: https://bit.ly/2I1nhRw
	// Culled using: https://bit.ly/2ToaI2T

	switch (TsuHash(*Word))
	{
	case TSU_HASH("break"):
	case TSU_HASH("case"):
	case TSU_HASH("catch"):
	case TSU_HASH("class"):
	case TSU_HASH("const"):
	case TSU_HASH("continue"):
	case TSU_HASH("debugger"):
	case TSU_HASH("default"):
	case TSU_HASH("delete"):
	case TSU_HASH("do"):
	case TSU_HASH("else"):
	case TSU_HASH("enum"):
	case TSU_HASH("export"):
	case TSU_HASH("extends"):
	case TSU_HASH("false"):
	case TSU_HASH("finally"):
	case TSU_HASH("for"):
	case TSU_HASH("function"):
	case TSU_HASH("global"):
	case TSU_HASH("if"):
	case TSU_HASH("implements"):
	case TSU_HASH("import"):
	case TSU_HASH("in"):
	case TSU_HASH("infer"):
	case TSU_HASH("instanceof"):
	case TSU_HASH("interface"):
	case TSU_HASH("keyof"):
	case TSU_HASH("let"):
	case TSU_HASH("never"):
	case TSU_HASH("new"):
	case TSU_HASH("null"):
	case TSU_HASH("object"):
	case TSU_HASH("package"):
	case TSU_HASH("private"):
	case TSU_HASH("protected"):
	case TSU_HASH("public"):
	case TSU_HASH("readonly"):
	case TSU_HASH("return"):
	case TSU_HASH("static"):
	case TSU_HASH("super"):
	case TSU_HASH("switch"):
	case TSU_HASH("this"):
	case TSU_HASH("throw"):
	case TSU_HASH("true"):
	case TSU_HASH("try"):
	case TSU_HASH("typeof"):
	case TSU_HASH("undefined"):
	case TSU_HASH("unique"):
	case TSU_HASH("unknown"):
	case TSU_HASH("var"):
	case TSU_HASH("void"):
	case TSU_HASH("while"):
	case TSU_HASH("with"):
	case TSU_HASH("yield"):
	case TSU_HASH("staticClass"):
		return true;
	}

	return false;
}

void FTsuTypings::WriteAllTypings()
{
	WriteCoreTypings();
	WriteGlobalTypings();
	WriteKeyTypings();

	FTsuReflection::VisitAllTypes([&](UField* Type, const FTsuTypeSet& References)
	{
		WriteTypings(Type, References);
	});
}

void FTsuTypings::WriteCoreTypings()
{
	FString& Output = ResetPersistentOutputBuffer();

	TSU_WRITELN("// Generated file, any changes will be overwritten");
	TSU_WRITELN("");
	TSU_WRITELN("interface DeepReadonlyArray<T>");
	TSU_WRITELN("\textends ReadonlyArray<DeepReadonly<T>> { }");
	TSU_WRITELN("");
	TSU_WRITELN("type DeepReadonlyObject<T> = {");
	TSU_WRITELN("\t+readonly [P in keyof T]: DeepReadonly<T[P]>;");
	TSU_WRITELN("};");
	TSU_WRITELN("");
	TSU_WRITELN("type DeepReadonly<T> = (");
	TSU_WRITELN("\tT extends (infer E)[]");
	TSU_WRITELN("\t\t? DeepReadonlyArray<E>");
	TSU_WRITELN("\t\t: T extends Function");
	TSU_WRITELN("\t\t\t? T");
	TSU_WRITELN("\t\t\t: T extends Object");
	TSU_WRITELN("\t\t\t\t? DeepReadonlyObject<T>");
	TSU_WRITELN("\t\t\t\t: T");
	TSU_WRITELN(");");
	TSU_WRITELN("");
	TSU_WRITELN("declare class Delegate<T extends any[] = []> {");
	TSU_WRITELN("\tbind(fn: (...args: T) => void): void;");
	TSU_WRITELN("\tunbind(): void;");
	TSU_WRITELN("\texecute(...args: T): void;");
	TSU_WRITELN("\treadonly isBound: boolean;");
	TSU_WRITELN("\tprivate constructor(fn?: (...args: T) => void);");
	TSU_WRITELN("}");
	TSU_WRITELN("");
	TSU_WRITELN("type DelegateHandle = number;");
	TSU_WRITELN("");
	TSU_WRITELN("declare class MulticastDelegate<T extends any[] = []> {");
	TSU_WRITELN("\tadd(fn: (...args: T) => void): DelegateHandle;");
	TSU_WRITELN("\tremove(handle: DelegateHandle): void;");
	TSU_WRITELN("\tbroadcast(...args: T): void;");
	TSU_WRITELN("\treadonly isBound: boolean;");
	TSU_WRITELN("\tprivate constructor();");
	TSU_WRITELN("}");
	TSU_WRITELN("");
	TSU_WRITELN("export {");
	TSU_WRITELN("\tDeepReadonly,");
	TSU_WRITELN("\tDeepReadonlyArray,");
	TSU_WRITELN("\tDeepReadonlyObject,");
	TSU_WRITELN("\tDelegate,");
	TSU_WRITELN("\tMulticastDelegate");
	TSU_WRITELN("};");

	SaveTypings(TEXT("TsuCore"), Output);
}

void FTsuTypings::WriteGlobalTypings()
{
	WriteTypings(UEngine::StaticClass());
	WriteTypings(UWorld::StaticClass());

	FString& Output = ResetPersistentOutputBuffer();

	TSU_WRITELN("// Generated file, any changes will be overwritten");
	TSU_WRITELN("");
	TSU_WRITELN("import { TimerHandle } from '../TimerHandle';");
	TSU_WRITELN("");
	TSU_WRITELN("declare global {");
	TSU_WRITELN("\tfunction require(id: string): any;");
	TSU_WRITELN("");
	TSU_WRITELN("\tfunction setTimeout(callback: () => void, delay: number): TimerHandle;");
	TSU_WRITELN("\tfunction clearTimeout(handle: TimerHandle): void;");
	TSU_WRITELN("\tfunction setInterval(callback: () => void, interval: number): TimerHandle;");
	TSU_WRITELN("\tfunction clearInterval(handle: TimerHandle): void;");
	TSU_WRITELN("");
	TSU_WRITELN("\tvar console: {");
	TSU_WRITELN("\t\tlog(message: any, ...optionalParams: any[]): void;");
	TSU_WRITELN("\t\tinfo(message: any, ...optionalParams: any[]): void;");
	TSU_WRITELN("\t\twarn(message: any, ...optionalParams: any[]): void;");
	TSU_WRITELN("\t\terror(message: any, ...optionalParams: any[]): void;");
	TSU_WRITELN("\t\ttrace(message: any, ...optionalParams: any[]): void;");
	TSU_WRITELN("\t\tdisplay(duration: number, message: any, ...optionalParams: any[]): void;");
	TSU_WRITELN("\t\ttime(label: string): void;");
	TSU_WRITELN("\t\ttimeEnd(label: string): void;");
	TSU_WRITELN("\t}");
	TSU_WRITELN("}");

	SaveTypings(TEXT("TsuGlobals"), Output);
}

void FTsuTypings::WriteKeyTypings()
{
	WriteTypings(FKey::StaticStruct());

	FString& Output = ResetPersistentOutputBuffer();

	TSU_WRITELN("// Generated file, any changes will be overwritten");
	TSU_WRITELN("");
	TSU_WRITELN("import { DeepReadonly } from '../TsuCore';");
	TSU_WRITELN("import { Key } from '../Key';");
	TSU_WRITELN("");
	TSU_WRITELN("declare const EKeys: DeepReadonly<{");

	TArray<FKey> AllKeys;
	EKeys::GetAllKeys(AllKeys);

	for (const FKey& Key : AllKeys)
	{
		TSU_WRITELNF("\t%s: Key;", *Key.ToString());
	}

	TSU_WRITELN("}>;");
	TSU_WRITELN("");
	TSU_WRITELN("export { EKeys };");

	SaveTypings(TEXT("EKeys"), Output);
}

bool FTsuTypings::WriteTypings(UField* Type, const FTsuTypeSet& References)
{
	//const double TimeStart = FPlatformTime::Seconds();

	FString& Output = ResetPersistentOutputBuffer();

	TSU_WRITELN("// Generated file, any changes will be overwritten");
	TSU_WRITELN("");

	if (Type->IsA<UStruct>())
	{
		TSU_WRITELN("import { Delegate, MulticastDelegate, DeepReadonly } from '../TsuCore';");
		TSU_WRITELN("");
	}

	TArray<UField*> SortedReferences = References.Array();
	SortedReferences.Sort([](UField& Lhs, UField& Rhs)
	{
		return TailorNameOfType(&Lhs) < TailorNameOfType(&Rhs);
	});

	for (UField* ReferencedType : SortedReferences)
	{
		const FString TypeName = TailorNameOfType(ReferencedType);
		TSU_WRITELNF("import { %s } from '../%s';", *TypeName, *TypeName);
	}

	if (SortedReferences.Num() > 0)
		TSU_WRITELN("");

	if (auto Enum = Cast<UEnum>(Type))
		WriteEnum(Output, Enum);
	else if (auto Struct = Cast<UStruct>(Type))
		WriteObject(Output, Struct);

	//const double TimeEndWork = FPlatformTime::Seconds();

	const FString TypeName = TailorNameOfType(Type);
	if (!SaveTypings(TypeName, Output))
		return false;

	//const double TimeEndIo = FPlatformTime::Seconds();

	//UE_LOG(
	//	LogTsuRuntime,
	//	Log,
	//	TEXT("[%s] Generated typings in %.1f ms (%.1f ms work, %.1f ms I/O)"),
	//	*TypeName,
	//	(TimeEndIo - TimeStart) * 1000,
	//	(TimeEndWork - TimeStart) * 1000,
	//	(TimeEndIo - TimeEndWork) * 1000);

	return true;
}

void FTsuTypings::WriteTypings(UField* Type)
{
	FTsuTypeSet References;
	if (FTsuReflection::GetReferencesInType(Type, References))
		WriteTypings(Type, References);
}

void FTsuTypings::WriteDependencyTypings(UTsuBlueprintGeneratedClass* Class)
{
	for (const FString& DependencyName : Class->Exports.Dependencies)
	{
		if (auto Dependency = FindObject<UBlueprint>(ANY_PACKAGE, *DependencyName, false))
		{
			for (
				UStruct* DependencyClass = Dependency->GeneratedClass;
				DependencyClass != nullptr;
				DependencyClass = DependencyClass->GetSuperStruct())
			{
				WriteTypings(DependencyClass);
			}
		}
	}
}

bool FTsuTypings::DoTypingsExist(UField* Type)
{
	return DoTypingsExist(*TailorNameOfType(Type));
}

bool FTsuTypings::DoTypingsExist(const TCHAR* TypeName)
{
	return FPlatformFileManager::Get()
		.GetPlatformFile()
		.FileExists(*FTsuPaths::TypingPath(TypeName));
}

void FTsuTypings::WriteEnum(FString& Output, UEnum* Enum)
{
	WriteToolTip(Output, Enum, false);

	const FString TypeName = TailorNameOfType(Enum);
	
	TSU_WRITELNF("declare const enum %s {", *TypeName);

	const int32 NumEnums = FMath::Max(0, Enum->NumEnums() - 1);
	for (int64 Index = 0; Index < NumEnums; ++Index)
	{
		if (Enum->HasMetaData(MetaHidden, Index))
			continue;

		FString Name = Enum->GetNameStringByIndex(Index);
		const int64 Value = Enum->GetValueByIndex(Index);

		// #hack(#mihe): These two must use DisplayName, so we special case them
		static const auto ObjectTypeQueryEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EObjectTypeQuery"), true);
		static const auto TraceTypeQueryEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("ETraceTypeQuery"), true);

		if (Enum == ObjectTypeQueryEnum)
			Name = TEXT("OTQ_") + Enum->GetMetaData(MetaDisplayName, Index);
		else if (Enum == TraceTypeQueryEnum)
			Name = TEXT("TTQ_") + Enum->GetMetaData(MetaDisplayName, Index);

		if (Index > 0)
			TSU_WRITELN("");

		WriteToolTip(Output, Enum->GetToolTipTextByIndex(Index).ToString(), true);
		TSU_WRITELNF("\t%s = %lld,", *Name, Value);
	}

	TSU_WRITELN("}");
	TSU_WRITELN("");
	TSU_WRITELNF("export { %s };", *TypeName);
}

void FTsuTypings::WriteObject(FString& Output, UStruct* Struct)
{
	WriteToolTip(Output, Struct, false);

	FString ExtendsStatement;

	if (UStruct* SuperStruct = Struct->GetSuperStruct())
	{
		ExtendsStatement = 
			TEXT(" extends ") +
			TailorNameOfType(SuperStruct);
	}

	bool bIsClass = false;
	bool bIsInterface = false;
	bool bIsAbstractClass = false;

	FString ImplementsStatement;
	if (auto Class = Cast<UClass>(Struct))
	{
		bIsClass = true;
		bIsInterface = Class->IsChildOf<UInterface>();
		bIsAbstractClass = Class->HasAnyClassFlags(CLASS_Abstract);

		if (Class->Interfaces.Num() > 0)
		{
			TArray<FString> InterfaceNames;
			for (FImplementedInterface& Interface : Class->Interfaces)
				InterfaceNames.Add(TailorNameOfType(Interface.Class));

			ImplementsStatement = 
				TEXT(" implements ") +
				FString::Join(InterfaceNames, TEXT(", "));
		}
	}

	const FString TypeName = TailorNameOfType(Struct);

	TSU_WRITELNF(
		"declare %s %s%s%s {",
		bIsInterface ? TEXT("interface") : TEXT("class"),
		*TypeName,
		*ExtendsStatement,
		*ImplementsStatement);

	FTsuReflection::VisitProperties([&](UProperty* Property, bool bIsReadOnly)
	{
		WriteProperty(Output, Property, bIsReadOnly);
	}, Struct);

	FTsuReflection::VisitMethods([&](UFunction* Function)
	{
		WriteMethod(Output, Function);
	}, Struct);

	FTsuReflection::VisitExtensionMethods([&](UFunction* Function)
	{
		WriteExtensionMethod(Output, Function);
	}, Struct);

	FTsuReflection::VisitStaticExtensionMethods([&](UFunction* Function)
	{
		WriteStaticExtensionMethod(Output, Function);
	}, Struct);

	if (!bIsInterface)
	{
		if (bIsClass)
		{
			TSU_WRITELN("\t/** The meta class for this type */");
			TSU_WRITELN("\tstatic readonly staticClass: Class;");
			TSU_WRITELN("");
		}

		if (UFunction* MakeFunction = FTsuReflection::FindMakeFunction(Struct))
		{
			WriteToolTip(Output, MakeFunction, false, true);
			TSU_WRITE("\tpublic constructor(");
			WriteParameters(Output, MakeFunction);
			TSU_WRITELN(");");
		}
		else if (bIsAbstractClass)
		{
			TSU_WRITELN("\tprotected constructor();");
		}
		else if (bIsClass)
		{
			TSU_WRITELN("\tpublic constructor(outer?: UObject | null);");
		}
		else
		{
			TSU_WRITELN("\tpublic constructor();");
		}
	}

	TSU_WRITELN("}");
	TSU_WRITELN("");
	TSU_WRITELNF("export { %s };", *TypeName);
}

void FTsuTypings::WriteToolTip(FString& Output, UFunction* Function, bool bSkipFirst, bool bIndent)
{
	FString ToolTip = Function->GetToolTipText().ToString();

	// Add any default values to the tooltip
	FTsuReflection::VisitFunctionParameters([&](UProperty* Parameter)
	{
		const FString MetaDefaultValue = TEXT("CPP_Default_") + Parameter->GetName();
		if (Function->HasMetaData(*MetaDefaultValue))
		{
			const FString& DefaultValue = Function->GetMetaData(*MetaDefaultValue);
			ToolTip.Append(
				FString::Printf(
					TEXT("\n@param %s Default value: '%s'"),
					*Parameter->GetName(),
					*DefaultValue));
		}
	}, Function, bSkipFirst);

	WriteToolTip(Output, ToolTip, bIndent);
}

void FTsuTypings::WriteToolTip(FString& Output, UField* Field, bool bIndent)
{
	FString Tooltip = Field->GetToolTipText().ToString();

	auto WriteDelegateTooltip = [&](UFunction* SignatureFunction)
	{
		FTsuReflection::VisitFunctionParameters([&](UProperty* Parameter)
		{
			Tooltip += FString::Printf(
				TEXT("\n@param {%s} %s"),
				*GetPropertyType(Parameter),
				*Parameter->GetName());
		}, SignatureFunction);
	};

	if (auto DelegateProperty = Cast<UDelegateProperty>(Field))
		WriteDelegateTooltip(DelegateProperty->SignatureFunction);
	else if (auto MulticastDelegateProperty = Cast<UMulticastDelegateProperty>(Field))
		WriteDelegateTooltip(MulticastDelegateProperty->SignatureFunction);

	WriteToolTip(Output, MoveTemp(Tooltip), bIndent);
}

void FTsuTypings::WriteToolTip(FString& Output, FString ToolTip, bool bIndent)
{
	if (ToolTip.IsEmpty())
	{
		if (bIndent) TSU_WRITE("\t");
		TSU_WRITELN("/** */");
		return;
	}

	// Tailor the names of any '@param' instances in the tooltip
	{
		FString TailoredToolTip;
		TailoredToolTip.Reserve(ToolTip.Len() + 10);

		static const FRegexPattern TooltipParamPattern{TEXT("@param\\s+(\\{.+?\\}\\s+)?(.+?)\\b")};

		int32 PrevIndex = 0;
		FRegexMatcher ParamMatcher(TooltipParamPattern, ToolTip);
		while (ParamMatcher.FindNext())
		{
			const int32 StartIndex = ParamMatcher.GetCaptureGroupBeginning(2);
			const int32 EndIndex = ParamMatcher.GetCaptureGroupEnding(2);
			const int32 StartToEndCount = EndIndex - StartIndex;
			const int32 PrevToStartCount = StartIndex - PrevIndex;
			TailoredToolTip += ToolTip.Mid(PrevIndex, PrevToStartCount);
			TailoredToolTip += TailorNameOfField(ToolTip.Mid(StartIndex, StartToEndCount));
			PrevIndex = EndIndex;
		}

		TailoredToolTip += ToolTip.Mid(PrevIndex);
		ToolTip = MoveTemp(TailoredToolTip);
	}

	static const FString Asterisk = TEXT("*");

	TArray<FString> Lines;
	ToolTip.ParseIntoArrayLines(Lines, false);
	for (FString& Line : Lines)
		Line.RemoveFromStart(Asterisk);

	if (Lines.Num() == 1)
	{
		if (bIndent) TSU_WRITE("\t");
		TSU_WRITELNF("/** %s */", *Lines[0]);
	}
	else if (Lines.Num() > 1)
	{
		if (bIndent) TSU_WRITE("\t");
		TSU_WRITELN("/**");

		for (const FString& Line : Lines)
		{
			if (bIndent) TSU_WRITE("\t");
			TSU_WRITELNF(" * %s", *Line);
		}

		if (bIndent) TSU_WRITE("\t");
		TSU_WRITELN(" */");
	}
}

void FTsuTypings::WriteMethod(FString& Output, UFunction* Function)
{
	const TCHAR* StaticKeyword =
		Function->HasAllFunctionFlags(FUNC_Static)
			? TEXT("static ")
			: TEXT("");

	WriteToolTip(Output, Function, false, true);
	TSU_WRITEF("\t%s%s(", StaticKeyword, *TailorNameOfField(Function));
	WriteParameters(Output, Function, false);
	TSU_WRITE("): ");
	WriteReturns(Output, Function);
	TSU_WRITELN(";");
	TSU_WRITELN("");
}

void FTsuTypings::WriteExtensionMethod(FString& Output, UFunction* Function)
{
	WriteToolTip(Output, Function, true, true);
	TSU_WRITEF("\t%s(", *TailorNameOfExtension(Function));
	WriteParameters(Output, Function, true);
	TSU_WRITE("): ");
	WriteReturns(Output, Function);
	TSU_WRITELN(";");
	TSU_WRITELN("");
}

void FTsuTypings::WriteStaticExtensionMethod(FString& Output, UFunction* Function)
{
	WriteToolTip(Output, Function, false, true);
	TSU_WRITEF("\tstatic %s(", *TailorNameOfExtension(Function));
	WriteParameters(Output, Function);
	TSU_WRITE("): ");
	WriteReturns(Output, Function);
	TSU_WRITELN(";");
	TSU_WRITELN("");
}

void FTsuTypings::WriteParameters(FString& Output, UFunction* Function, bool bSkipFirst)
{
	bool bIsRestOptional = FTsuReflection::IsK2Method(Function);

	int32 NumWritten = 0;
	FTsuReflection::VisitFunctionParameters([&](UProperty* Parameter)
	{
		if (NumWritten++ > 0)
			TSU_WRITE(", ");

		const FString MetaDefaultValue = TEXT("CPP_Default_") + Parameter->GetName();
		if (!bIsRestOptional && Function->HasMetaData(*MetaDefaultValue))
			bIsRestOptional = true;

		const TCHAR* OptionalToken = bIsRestOptional ? TEXT("?") : TEXT("");

		TSU_WRITEF("%s%s: %s", *TailorNameOfField(Parameter), OptionalToken, *GetPropertyType(Parameter));
	}, Function, bSkipFirst);
}

void FTsuTypings::WriteReturns(FString& Output, UFunction* Function, bool bSkipFirst)
{
	if (FTsuReflection::HasOutputParameters(Function))
	{
		TSU_WRITE("{ ");

		int32 NumWritten = 0;

		for (UProperty* Parameter : FParamRange(Function))
		{
			if (!Parameter->HasAllPropertyFlags(CPF_Parm))
				continue;

			if (bSkipFirst)
			{
				bSkipFirst = false;
				continue;
			}

			if (FTsuReflection::IsOutputParameter(Parameter, true))
			{
				if (NumWritten++ > 0)
					TSU_WRITE(", ");

				TSU_WRITEF("%s: %s", *TailorNameOfField(Parameter), *GetPropertyType(Parameter));
			}
		}

		TSU_WRITE(" }");
	}
	else if (UProperty* ReturnProperty = Function->GetReturnProperty())
	{
		TSU_WRITEF("%s", *GetPropertyType(ReturnProperty));
	}
	else
	{
		TSU_WRITE("void");
	}
}

void FTsuTypings::WriteProperty(FString& Output, UProperty* Property, bool bIsReadOnly)
{
	WriteToolTip(Output, Property, true);

	TSU_WRITELNF(
		"\t%s%s: %s;",
		bIsReadOnly ? TEXT("readonly ") : TEXT(""),
		*TailorNameOfField(Property),
		*GetPropertyType(Property, bIsReadOnly));

	TSU_WRITELN("");
}

bool FTsuTypings::SaveTypings(const FString& TypeName, const FString& Output)
{
	const FString OutputPath = FTsuPaths::TypingPath(*TypeName);

	FString ExistingOutput;
	FFileHelper::LoadFileToString(ExistingOutput, *OutputPath);

	if (Output.Equals(ExistingOutput, ESearchCase::CaseSensitive))
		return true;

	if (!FFileHelper::SaveStringToFile(Output, *OutputPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
	{
		UE_LOG(LogTsuRuntime, Error, TEXT("[%s] Failed to save typings"), *TypeName);
		return false;
	}

	return true;
}

FString FTsuTypings::GetPropertyType(UProperty* Property, bool bIsReadOnly)
{
	FString Result;

	if (Property->IsA<UStrProperty>())
	{
		Result = TEXT("string");
	}
	else if (auto ByteProperty = Cast<UByteProperty>(Property))
	{
		Result = ByteProperty->Enum
			? TailorNameOfType(ByteProperty->Enum)
			: TEXT("number");
	}
	else if (Property->IsA<UNumericProperty>())
	{
		Result = TEXT("number");
	}
	else if (Property->IsA<UBoolProperty>())
	{
		Result = TEXT("boolean");
	}
	else if (Property->IsA<UNameProperty>())
	{
		Result = TEXT("string");
	}
	else if (Property->IsA<UTextProperty>())
	{
		Result = TEXT("string");
	}
	else if (auto EnumProperty = Cast<UEnumProperty>(Property))
	{
		Result = TailorNameOfType(EnumProperty->GetEnum());
	}
	else if (auto StructProperty = Cast<UStructProperty>(Property))
	{
		Result = TailorNameOfType(StructProperty->Struct);
	}
	else if (auto ObjectProperty = Cast<UObjectPropertyBase>(Property))
	{
		Result = TailorNameOfType(ObjectProperty->PropertyClass);

		if (FTsuReflection::IsInputParameter(ObjectProperty))
			Result += TEXT(" | null");
	}
	else if (auto InterfaceProperty = Cast<UInterfaceProperty>(Property))
	{
		Result = TailorNameOfType(InterfaceProperty->InterfaceClass);
	}
	else if (auto ArrayProperty = Cast<UArrayProperty>(Property))
	{
		UProperty* Inner = ArrayProperty->Inner;
		FString InnerType = GetPropertyType(Inner);

		// #hack(#mihe): Writable proxied structs from dynamic collections cause problems
		if (Inner->IsA<UStructProperty>())
			InnerType = FString::Printf(TEXT("DeepReadonly<%s>"), *InnerType);

		Result = FString::Printf(TEXT("Array<%s>"), *InnerType);
	}
	else if (auto SetProperty = Cast<USetProperty>(Property))
	{
		UProperty* ElementProp = SetProperty->ElementProp;
		FString ElementType = GetPropertyType(ElementProp);

		// #hack(#mihe): Writable proxied structs from dynamic collections cause problems
		if (ElementProp->IsA<UStructProperty>())
			ElementType = FString::Printf(TEXT("DeepReadonly<%s>"), *ElementType);

		Result = FString::Printf(TEXT("ReadonlySet<%s>"), *ElementType);
	}
	else if (auto MapProperty = Cast<UMapProperty>(Property))
	{
		UProperty* KeyProp = MapProperty->KeyProp;
		UProperty* ValueProp = MapProperty->ValueProp;

		FString KeyType = GetPropertyType(KeyProp);
		FString ValueType = GetPropertyType(ValueProp);

		// #hack(#mihe): Writable proxied structs from dynamic collections cause problems
		if (KeyProp->IsA<UStructProperty>())
			KeyType = FString::Printf(TEXT("DeepReadonly<%s>"), *KeyType);

		// #hack(#mihe): Writable proxied structs from dynamic collections cause problems
		if (ValueProp->IsA<UStructProperty>())
			ValueType = FString::Printf(TEXT("DeepReadonly<%s>"), *ValueType);

		Result = FString::Printf(TEXT("ReadonlyMap<%s, %s>"), *KeyType, *ValueType);
	}
	else if (auto DelegateProperty = Cast<UDelegateProperty>(Property))
	{
		if (FTsuReflection::IsInputParameter(DelegateProperty))
		{
			Result = TEXT("(");

			WriteParameters(Result, DelegateProperty->SignatureFunction, false);

			Result += TEXT(") => void");
		}
		else
		{
			Result = TEXT("Delegate<[");

			int32 NumWritten = 0;
			FTsuReflection::VisitFunctionParameters([&](UProperty* Parameter)
			{
				if (NumWritten++ > 0)
					Result += TEXT(", ");

				Result += GetPropertyType(Parameter);
			}, DelegateProperty->SignatureFunction);

			Result += TEXT("]>");
		}
	}
	else if (auto MulticastDelegateProperty = Cast<UMulticastDelegateProperty>(Property))
	{
		if (FTsuReflection::IsInputParameter(MulticastDelegateProperty))
		{
			Result = TEXT("(");

			WriteParameters(Result, MulticastDelegateProperty->SignatureFunction, false);

			Result += TEXT(") => void");
		}
		else
		{
			Result = TEXT("MulticastDelegate<[");

			int32 NumWritten = 0;
			FTsuReflection::VisitFunctionParameters([&](UProperty* Parameter)
			{
				if (NumWritten++ > 0)
					Result += TEXT(", ");

				Result += GetPropertyType(Parameter);
			}, MulticastDelegateProperty->SignatureFunction);

			Result += TEXT("]>");
		}
	}
	else
	{
		checkNoEntry();
		Result = TEXT("any");
	}

	if (Property->ArrayDim > 1)
		Result = FString::Printf(TEXT("ReadonlyArray<%s>"), *Result);
	
	return Result;
}

FString& FTsuTypings::Deduplicate(FString& Name)
{
	if (Name.StartsWith(TEXT("EqualEqual_")))
		Name = TEXT("Equal_") + Name.Mid(11);

	static const FRegexPattern Pattern{TEXT("_([A-Za-z0-9]{2,})\\1$")};
	FRegexMatcher Matcher{Pattern, Name};
	if (Matcher.FindNext())
	{
		const int32 Begin = Matcher.GetMatchBeginning();
		const FString Pre = Name.Mid(0, Begin);
		const FString Group = Matcher.GetCaptureGroup(1);
		Name = Pre + TEXT("_") + Group;
	}

	return Name;
}

FString& FTsuTypings::CamelCase(FString& Name)
{
	for (TCHAR& Char : Name)
	{
		if (!FChar::IsAlpha(Char) || FChar::IsLower(Char))
			break;

		Char = FChar::ToLower(Char);
	}

	return Name;
}

FString& FTsuTypings::AlphaNumerize(FString& Name)
{
	Name.GetCharArray().RemoveAll([](TCHAR Char)
	{
		return !FChar::IsAlnum(Char) && Char != TEXT('\0');
	});

	return Name;
}

FString& FTsuTypings::TrimRedundancy(FString& Name, const FString& ParentName)
{
	const int32 Count = ParentName.Len();

	int32 Begin = 0;
	int32 End = MAX_int32;

	auto FindNext = [&]
	{
		Begin = Name.Find(*ParentName, ESearchCase::IgnoreCase, ESearchDir::FromStart, Begin);
		End = Begin + Count;
		return Begin != INDEX_NONE;
	};

	while (FindNext())
	{
		const int32 Last = Name.Len() - 1;
		if (End < Last && FChar::IsLower(Name[End]))
			Begin = End;
		else
			Name.RemoveAt(Begin, Count);
	}

	return Name;
}

const FString& FTsuTypings::TailorNameOfType(UField* Type)
{
	using FCachedName = TPair<FName, FString>;
	using FCache = TMap<UField*, FCachedName>;

	static FCache Cache = []
	{
		FCache Result;
		Result.Reserve(6'000);
		return Result;
	}();

	const FName TypeFName = Type->GetFName();

	FCachedName& CachedName = Cache.FindOrAdd(Type);
	if (CachedName.Key != TypeFName)
	{
		FString TypeName = Type->GetName();

		if (auto Class = Cast<UClass>(Type))
		{
			if (Class == UObject::StaticClass())
				TypeName = TEXT("UObject");
			else if (Class == UFunction::StaticClass())
				TypeName = TEXT("UFunction");
			else if (auto GeneratedClass = Cast<UBlueprintGeneratedClass>(Class))
				TypeName = GeneratedClass->ClassGeneratedBy->GetName();
		}

		CachedName = FCachedName(TypeFName, MoveTemp(TypeName));
	}
	
	return CachedName.Value;
}

FString& FTsuTypings::TailorNameOfField(FString& Name)
{
	if (FTsuReflection::IsK2Method(Name))
		Name.RemoveAt(0, 3);

	if (Name.StartsWith(TEXT("Conv_")))
		Name.RemoveAt(0, 5);

	Deduplicate(Name);
	AlphaNumerize(Name);
	CamelCase(Name);

	if (IsReservedIdentifier(Name))
		Name += TEXT("_");

	return Name;
}

FString FTsuTypings::TailorNameOfField(FString&& InName)
{
	FString Name = Forward<FString>(InName);
	TailorNameOfField(Name);
	return Name;
}

const FString& FTsuTypings::TailorNameOfField(UField* Field)
{
	using FCachedName = TPair<FName, FString>;
	using FCache = TMap<const UField*, FCachedName>;

	static FCache Cache = []
	{
		FCache Result;
		Result.Reserve(25'000);
		return Result;
	}();

	const FName FieldFName = Field->GetFName();

	FCachedName& CachedName = Cache.FindOrAdd(Field);
	if (CachedName.Key != FieldFName)
	{
		FString FieldName = Field->GetName();

		if (FTsuReflection::IsExplicitExtension(Field))
			CamelCase(FieldName);
		else
			TailorNameOfField(FieldName);

		CachedName = FCachedName(FieldFName, MoveTemp(FieldName));
	}

	return CachedName.Value;
}

const FString& FTsuTypings::TailorNameOfExtension(UFunction* Function)
{
	using FCachedName = TPair<FName, FString>;
	using FCache = TMap<const UFunction*, FCachedName>;

	static FCache Cache = []
	{
		FCache Result;
		Result.Reserve(1'000);
		return Result;
	}();

	const FName FunctionFName = Function->GetFName();

	FCachedName& CachedName = Cache.FindOrAdd(Function);
	if (CachedName.Key != FunctionFName)
	{
		FString FunctionName = Function->GetName();

		if (FTsuReflection::IsExplicitExtension(Function))
		{
			CamelCase(FunctionName);
		}
		else
		{
			UStruct* Type = FTsuReflection::FindExtendedType(Function);
			if (ensure(Type != nullptr))
				TrimRedundancy(FunctionName, Type->GetName());

			TailorNameOfField(FunctionName);
		}

		CachedName = FCachedName(FunctionFName, MoveTemp(FunctionName));
	}

	return CachedName.Value;
}
