#pragma once

#include "CoreMinimal.h"

#include "KismetCompiler.h"

class UTsuBlueprint;
class UTsuBlueprintGeneratedClass;

class FTsuRuntimeBlueprintCompiler final
	: public FKismetCompilerContext
{
	using Super = FKismetCompilerContext;

public:
	FTsuRuntimeBlueprintCompiler(
		UBlueprint* SourceSketch,
		FCompilerResultsLog& MessageLog,
		const FKismetCompilerOptions& CompilerOptions,
		TArray<UObject*>* ObjLoaded);

private:
	void SpawnNewClass(const FString& NewClassName) override;
	void CreateClassVariablesFromBlueprint() override;
	void EnsureProperGeneratedClass(UClass*& TargetUClass) override;

	UTsuBlueprintGeneratedClass* GetTsuClass() const;
};
