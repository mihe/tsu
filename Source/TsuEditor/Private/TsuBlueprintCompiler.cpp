#include "TsuBlueprintCompiler.h"

#include "TsuBlueprint.h"
#include "TsuBlueprintGeneratedClass.h"
#include "TsuEditorLog.h"
#include "TsuFactory.h"
#include "TsuParser.h"
#include "TsuPaths.h"
#include "TsuTypings.h"

#include "EditorFramework/AssetImportData.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet2/KismetReinstanceUtilities.h"
#include "KismetCompilerMisc.h"
#include "Misc/FileHelper.h"
#include "Misc/InteractiveProcess.h"
#include "Misc/ScopeExit.h"

FTsuBlueprintCompiler::FTsuBlueprintCompiler(
	UBlueprint* SourceSketch,
	FCompilerResultsLog& MessageLog,
	const FKismetCompilerOptions& CompilerOptions,
	TArray<UObject*>* ObjLoaded)
	: Super(SourceSketch, MessageLog, CompilerOptions, ObjLoaded)
{
}

void FTsuBlueprintCompiler::SpawnNewClass(const FString& NewClassName)
{
	if (auto OldClass = Cast<UTsuBlueprintGeneratedClass>(Blueprint->GeneratedClass))
		FTsuTypings::WriteDependencyTypings(OldClass);

	// First, attempt to find the class, in case it hasn't been serialized in yet
	NewClass = FindObject<UTsuBlueprintGeneratedClass>(Blueprint->GetOutermost(), *NewClassName);
	if (NewClass == nullptr)
	{
		// If the class hasn't been found, then spawn a new one
		NewClass = NewObject<UTsuBlueprintGeneratedClass>(Blueprint->GetOutermost(), FName(*NewClassName), RF_Public);
	}
	else
	{
		// Already existed, but wasn't linked in the Blueprint yet due to load ordering issues
		NewClass->ClassGeneratedBy = Blueprint;
		FBlueprintCompileReinstancer::Create(NewClass);
	}
}

void FTsuBlueprintCompiler::CreateClassVariablesFromBlueprint()
{
	UTsuBlueprintGeneratedClass* TsuClass = GetTsuClass();

	// Make sure we have (albeit empty) typings to allow self-referencing
	if (!FTsuTypings::DoTypingsExist(TsuClass))
		FTsuTypings::WriteTypings(TsuClass);

	for (const FTsuCompilationMessage& Msg : CompileScript(GetAbsoluteFilename(), TsuClass->Exports))
	{
		switch (Msg.Severity)
		{
		case ETsuSeverity::Note:
			MessageLog.Note(*Msg.Message);
			break;
		case ETsuSeverity::Warning:
			MessageLog.Warning(*Msg.Message);
			break;
		case ETsuSeverity::Error:
			MessageLog.Error(*Msg.Message);
			break;
		}
	}

	Super::CreateClassVariablesFromBlueprint();
}

TArray<FTsuCompilationMessage> FTsuBlueprintCompiler::CompileScript(const FString& FilePath, FTsuParsedFile& Result)
{
	const double StartTime = FPlatformTime::Seconds();

	TArray<FTsuCompilationMessage> Messages;

	const FString FileName = FPaths::GetCleanFilename(FilePath);

	FTsuParsedFile Exports;
	if (!FTsuParser::Parse(FilePath, Exports))
	{
		Messages.Add({ETsuSeverity::Error, TEXT("[TSU] Unexpected parsing error, see output log.")});
		return Messages;
	}

	if (Exports.Errors.Num() > 0)
	{
		for (const FString& Error : Exports.Errors)
			Messages.Add({ETsuSeverity::Error, Error});

		return Messages;
	}

	FPaths::MakeStandardFilename(Exports.Path);

	Exports.Path = FPaths::SetExtension(Exports.Path, TEXT(".js"));

	Result = MoveTemp(Exports);

	const double EndTime = FPlatformTime::Seconds();
	const double ElapsedTime = (EndTime - StartTime) * 1000.0;

	UE_LOG(LogTsuEditor, Log, TEXT("Parsing '%s' took %.1f ms"), *FileName, ElapsedTime);

	return Messages;
}

void FTsuBlueprintCompiler::EnsureProperGeneratedClass(UClass*& TargetUClass)
{
	Super::EnsureProperGeneratedClass(TargetUClass);

	auto TargetUObject = static_cast<UObject*>(TargetUClass);
	if (TargetUClass != nullptr && !TargetUObject->IsA<UTsuBlueprintGeneratedClass>())
	{
		FKismetCompilerUtilities::ConsignToOblivion(TargetUClass, Blueprint->bIsRegeneratingOnLoad);
		TargetUClass = nullptr;
	}
}

bool FTsuBlueprintCompiler::ValidateGeneratedClass(UBlueprintGeneratedClass* Class)
{
	const bool SuperResult = Super::ValidateGeneratedClass(Class);
	const bool Result = UTsuBlueprint::ValidateGeneratedClass(Class);
	return SuperResult && Result;
}

FString FTsuBlueprintCompiler::GetAbsoluteFilename() const
{
	FString Filename = GetTsuBlueprint()->AssetImportData->GetFirstFilename();
	if (Filename.IsEmpty())
		Filename = UTsuFactory::GetCurrentFilename();

	if (FPaths::IsRelative(Filename))
	{
		Filename = FPaths::ConvertRelativePathToFull(
			FPaths::ConvertRelativePathToFull(
				FTsuPaths::ScriptsSourceDir()),
			Filename);
	}

	return Filename;
}

UTsuBlueprint* FTsuBlueprintCompiler::GetTsuBlueprint() const
{
	return CastChecked<UTsuBlueprint>(Blueprint);
}

UTsuBlueprintGeneratedClass* FTsuBlueprintCompiler::GetTsuClass() const
{
	return CastChecked<UTsuBlueprintGeneratedClass>(NewClass);
}
