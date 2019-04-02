#include "TsuRuntimeBlueprintCompiler.h"

#include "TsuBlueprint.h"
#include "TsuBlueprintGeneratedClass.h"

#include "Kismet2/KismetReinstanceUtilities.h"
#include "KismetCompilerMisc.h"

FTsuRuntimeBlueprintCompiler::FTsuRuntimeBlueprintCompiler(
	UBlueprint* SourceSketch,
	FCompilerResultsLog& MessageLog,
	const FKismetCompilerOptions& CompilerOptions)
	: Super(SourceSketch, MessageLog, CompilerOptions)
{
}

void FTsuRuntimeBlueprintCompiler::SpawnNewClass(const FString& NewClassName)
{
	// #hack(#mihe): If this is anything other than the skeleton class, we will
	// most likely have trouble stealing the exports
	check(NewClassName.StartsWith(TEXT("SKEL_")));

	auto OldClass = CastChecked<UTsuBlueprintGeneratedClass>(Blueprint->GeneratedClass);

	// First, attempt to find the class, in case it hasn't been serialized in yet
	NewClass = FindObject<UTsuBlueprintGeneratedClass>(Blueprint->GetOutermost(), *NewClassName);
	if (NewClass == NULL)
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

void FTsuRuntimeBlueprintCompiler::CreateClassVariablesFromBlueprint()
{
	// #hack(#mihe): Steal exports from the non-transient proper class
	if (NewClass->GetName().StartsWith(TEXT("SKEL_")))
	{
		auto SkeletonClass = CastChecked<UTsuBlueprintGeneratedClass>(NewClass);
		auto GeneratedBy = CastChecked<UTsuBlueprint>(SkeletonClass->ClassGeneratedBy);
		auto ActualClass = CastChecked<UTsuBlueprintGeneratedClass>(GeneratedBy->GeneratedClass);
		SkeletonClass->Exports = ActualClass->Exports;
	}

	Super::CreateClassVariablesFromBlueprint();
}

void FTsuRuntimeBlueprintCompiler::EnsureProperGeneratedClass(UClass*& TargetUClass)
{
	Super::EnsureProperGeneratedClass(TargetUClass);

	auto TargetUObject = static_cast<UObject*>(TargetUClass);
	if (TargetUClass != nullptr && !TargetUObject->IsA<UTsuBlueprintGeneratedClass>())
	{
		FKismetCompilerUtilities::ConsignToOblivion(TargetUClass, Blueprint->bIsRegeneratingOnLoad);
		TargetUClass = nullptr;
	}
}

UTsuBlueprintGeneratedClass* FTsuRuntimeBlueprintCompiler::GetTsuClass() const
{
	return CastChecked<UTsuBlueprintGeneratedClass>(NewClass);
}
