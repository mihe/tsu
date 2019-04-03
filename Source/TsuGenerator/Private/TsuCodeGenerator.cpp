/**
 * #note(#mihe): This module is not currently used and is only meant to serve as a starting point for future development
 */

#include "TsuCodeGenerator.h"

#include "TsuGeneratorLog.h"
#include "TsuUtilities.h"
#include "UObject/UnrealType.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

// Can't use FRegexMatcher in UHT plugins
#include <regex>

namespace TsuCodeGenerator_Private
{

void AppendToolTip(FString& Result, UField* Field)
{
	if (!Field->HasMetaData(TEXT("ToolTip")))
		return;

	static const std::wregex NewlinePattern(L"\\r?\\n");

	const FString ToolTip = std::regex_replace(
		*Field->GetMetaData(TEXT("ToolTip")),
		NewlinePattern,
		L"\r\n * "
	).c_str();

	Result += TEXT("/**\r\n");
	Result += FString::Printf(TEXT(" * %s\r\n"), *ToolTip);
	Result += TEXT(" */\r\n");
}

} // namespace TsuCodeGenerator_Private

FTsuCodeGenerator::FTsuCodeGenerator(
	const FString& InRootLocalPath,
	const FString& InRootBuildPath,
	const FString& InOutputDirectory,
	const FString& InIncludeBase)
	: RootLocalPath(InRootLocalPath)
	, RootBuildPath(InRootBuildPath)
	, GeneratedCodePath(InOutputDirectory)
	, IncludeBase(InIncludeBase)
{
}

bool FTsuCodeGenerator::SaveHeaderIfChanged(const FString& HeaderPath, const FString& NewHeaderContents)
{
	// #hack(#mihe): Remove to make it actually run again
	if (!HeaderPath.IsEmpty())
		return false;

	FString OriginalHeaderLocal;
	FFileHelper::LoadFileToString(OriginalHeaderLocal, *HeaderPath);

	const bool bHasChanged = OriginalHeaderLocal.Len() == 0 || FCString::Strcmp(*OriginalHeaderLocal, *NewHeaderContents);
	if (bHasChanged)
	{
		// save the updated version to a tmp file so that the user can see what will be changing
		const FString TmpHeaderFilename = HeaderPath + TEXT(".tmp");

		// delete any existing temp file
		IFileManager::Get().Delete(*TmpHeaderFilename, false, true);
		if (!FFileHelper::SaveStringToFile(NewHeaderContents, *TmpHeaderFilename))
		{
			UE_LOG(LogTsuGenerator, Warning, TEXT("Failed to save header export: '%s'"), *TmpHeaderFilename);
		}
		else
		{
			TempHeaders.Add(TmpHeaderFilename);
		}
	}

	return bHasChanged;
}

void FTsuCodeGenerator::RenameTempFiles()
{
	for (FString& TempFilename : TempHeaders)
	{
		const FString Filename = TempFilename.Replace(TEXT(".tmp"), TEXT(""));
		if (!IFileManager::Get().Move(*Filename, *TempFilename, true, true))
		{
			UE_LOG(LogTsuGenerator, Error, TEXT("%s"), *FString::Printf(TEXT("Couldn't write file '%s'"), *Filename));
		}
		else
		{
			UE_LOG(LogTsuGenerator, Log, TEXT("Exported updated script header: %s"), *Filename);
		}
	}
}

FString FTsuCodeGenerator::RebaseToBuildPath(const FString& FileName) const
{
	FString NewFilename(FileName);
	FPaths::MakePathRelativeTo(NewFilename, *IncludeBase);
	return NewFilename;
}

FString FTsuCodeGenerator::GetClassNameCPP(UClass* Class) const
{
	return FString::Printf(TEXT("%s%s"), Class->GetPrefixCPP(), *Class->GetName());
}

FString FTsuCodeGenerator::GetScriptHeaderForClass(UClass* Class)
{
	return GeneratedCodePath / (Class->GetName() + TEXT(".tsu.h"));
}

bool FTsuCodeGenerator::CanExportClass(UClass* Class) const
{
	// #hack(#mihe): Remove to make it actually run again
	if (Class)
		return false;

	const bool ExportsSymbols = Class->ClassFlags & (CLASS_RequiredAPI | CLASS_MinimalAPI);
	const bool HasBeenExported = ExportedClasses.Contains(Class->GetFName());
	if (!ExportsSymbols || HasBeenExported)
		return false;

	for (auto Function : TImmediateFieldRange<UFunction>(Class))
	{
		if (CanExportFunction(Class, Function))
			return true;
	}

	for (auto Property : TImmediateFieldRange<UProperty>(Class))
	{
		if (CanExportProperty(Class, Property))
			return true;
	}

	return false;
}

void FTsuCodeGenerator::ExportClass(UClass* Class, const FString& SourceHeaderFilename, const FString& GeneratedHeaderFilename, bool bHasChanged)
{
	if (!CanExportClass(Class))
		return;

	UE_LOG(LogTsuGenerator, Log, TEXT("Exporting class %s"), *Class->GetName());

	ExportedClasses.Add(Class->GetFName());
	AllSourceClassHeaders.Add(SourceHeaderFilename);

	const FString ClassGlueFilename = GetScriptHeaderForClass(Class);
	AllScriptHeaders.Add(ClassGlueFilename);

	const FString ClassNameCPP = GetClassNameCPP(Class);
	FString GeneratedGlue(TEXT("#pragma once\r\n\r\n"));

	for (auto Function : TImmediateFieldRange<UFunction>(Class))
	{
		if (CanExportFunction(Class, Function))
		{
			UE_LOG(LogTsuGenerator, Log, TEXT("  %s %s"), *Function->GetClass()->GetName(), *Function->GetName());
			GeneratedGlue += ExportFunction(ClassNameCPP, Class, Function);
		}
	}

	for (auto Property : TImmediateFieldRange<UProperty>(Class))
	{
		if (CanExportProperty(Class, Property))
		{
			UE_LOG(LogTsuGenerator, Log, TEXT("  %s %s"), *Property->GetClass()->GetName(), *Property->GetName());
			GeneratedGlue += ExportProperty(ClassNameCPP, Class, Property);
		}
	}

	SaveHeaderIfChanged(ClassGlueFilename, GeneratedGlue);
}

bool FTsuCodeGenerator::CanExportFunction(UClass* Class, UFunction* Function)
{
	if (Function->FunctionFlags & FUNC_Delegate)
		return false;

	if (IsFieldDeprecated(Function))
		return false;

	if (Function->HasMetaData(TEXT("BlueprintGetter")) || Function->HasMetaData("BlueprintSetter"))
		return false;

	if (Function->GetName().StartsWith(TEXT("OnRep_")))
		return false;

	for (auto It = TFieldIterator<UProperty>(Function); It; ++It)
	{
		UProperty* Param = *It;
		if (Param->IsA<UArrayProperty>() ||
			Param->ArrayDim > 1 ||
			Param->IsA<UDelegateProperty>() ||
			Param->IsA<UMulticastDelegateProperty>() ||
			Param->IsA<UWeakObjectProperty>() ||
			Param->IsA<UInterfaceProperty>())
		{
			return false;
		}
	}

	return true;
}

FString FTsuCodeGenerator::ExportFunction(const FString& ClassNameCPP, UClass* Class, UFunction* Function)
{
	FString Result;

	TsuCodeGenerator_Private::AppendToolTip(Result, Function);

	FString ResultTypeName = TEXT("void");
	if (UProperty* ResultType = Function->GetReturnProperty())
		ResultTypeName = ResultType->GetCPPType();

	Result += FString::Printf(
		TEXT("// Function: %s %s::%s("),
		*ResultTypeName,
		*ClassNameCPP,
		*Function->GetName());

	for (auto It = TFieldIterator<UProperty>(Function); It; ++It)
	{
		UProperty* Param = *It;

		if (!(Param->GetPropertyFlags() & CPF_ReturnParm))
		{
			if (Param->PassCPPArgsByRef())
				Result += FString::Printf(TEXT("const %s& %s, "), *Param->GetCPPType(), *Param->GetNameCPP());
			else
				Result += FString::Printf(TEXT("%s %s, "), *Param->GetCPPType(), *Param->GetNameCPP());
		}
	}

	Result += TEXT(")\r\n\r\n");

	return Result;
}

bool FTsuCodeGenerator::CanExportProperty(UClass* Class, UProperty* Property)
{
	// Property must be DLL exported
	if (!(Class->ClassFlags & CLASS_RequiredAPI))
		return false;

	// Only public, editable properties can be exported
	if (!Property->HasAnyFlags(RF_Public) ||
		(Property->PropertyFlags & CPF_Protected) ||
		!(Property->PropertyFlags & CPF_Edit))
	{
		return false;
	}

	if (IsFieldDeprecated(Property))
		return false;

	// Reject if it's one of the unsupported types (yet)
	if (Property->IsA<UArrayProperty>() ||
		Property->ArrayDim > 1 ||
		Property->IsA<UDelegateProperty>() ||
		Property->IsA<UMulticastDelegateProperty>() ||
		Property->IsA<UWeakObjectProperty>() ||
		Property->IsA<UInterfaceProperty>() ||
		Property->IsA<UStructProperty>())
	{
		return false;
	}

	return true;
}

FString FTsuCodeGenerator::ExportProperty(const FString& ClassNameCPP, UClass* Class, UProperty* Property)
{
	FString Result;

	TsuCodeGenerator_Private::AppendToolTip(Result, Property);

	Result += FString::Printf(
		TEXT("// Property: (%s) %s::%s\r\n\r\n"),
		*Property->GetCPPType(),
		*ClassNameCPP,
		*Property->GetName());

	return Result;
}

void FTsuCodeGenerator::FinishExport()
{
	// GlueAllGeneratedFiles();
	RenameTempFiles();
}

void FTsuCodeGenerator::GlueAllGeneratedFiles()
{
	FString Result;

	for (FString& HeaderFilename : AllSourceClassHeaders)
	{
		const FString NewFilename = RebaseToBuildPath(HeaderFilename);
		Result += FString::Printf(TEXT("#include \"%s\"\r\n"), *NewFilename);
	}

	for (FString& HeaderFilename : AllScriptHeaders)
	{
		const FString NewFilename = FPaths::GetCleanFilename(HeaderFilename);
		Result += FString::Printf(TEXT("#include \"%s\"\r\n"), *NewFilename);
	}

	SaveHeaderIfChanged(GeneratedCodePath / TEXT("GeneratedScriptLibraries.inl"), Result);
}
