#pragma once

#include "CoreMinimal.h"

#include "KismetCompiler.h"

class UTsuBlueprint;
class UTsuBlueprintGeneratedClass;

enum class ETsuSeverity
{
	Note,
	Warning,
	Error
};

struct FTsuCompilationMessage
{
	ETsuSeverity Severity;
	FString Message;
};

class FTsuBlueprintCompiler final
	: public FKismetCompilerContext
{
	using Super = FKismetCompilerContext;

public:
	FTsuBlueprintCompiler(
		UBlueprint* SourceSketch,
		FCompilerResultsLog& MessageLog,
		const FKismetCompilerOptions& CompilerOptions,
		TArray<UObject*>* ObjLoaded);

	static TArray<FTsuCompilationMessage> CompileScript(const FString& Filename, struct FTsuParsedFile& Result);

private:
	void SpawnNewClass(const FString& NewClassName) override;
	void CreateClassVariablesFromBlueprint() override;
	void EnsureProperGeneratedClass(UClass*& TargetUClass) override;
	bool ValidateGeneratedClass(UBlueprintGeneratedClass* Class) override;

	FString GetAbsoluteFilename() const;

	UTsuBlueprint* GetTsuBlueprint() const;
	UTsuBlueprintGeneratedClass* GetTsuClass() const;
};
