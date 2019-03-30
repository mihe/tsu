#pragma once

#include "CoreMinimal.h"

#include "SourceCodeNavigation.h"

class FTsuEditorCommands final
	: public ISourceCodeNavigationHandler
{
public:
	FTsuEditorCommands();
	~FTsuEditorCommands();

	FTsuEditorCommands(const FTsuEditorCommands& Other) = delete;
	FTsuEditorCommands& operator=(const FTsuEditorCommands& Other) = delete;

	static FTsuEditorCommands& Get();

	static void Register();
	static void Unregister();

	static bool OpenGraphInTextEditor(class UTsuBlueprint* Blueprint, int32 Line = 1, int32 Column = 1);
	static bool OpenFunctionInTextEditor(class UTsuBlueprint* Blueprint, const FString& FunctionName);

private:
	bool CanNavigateToFunction(const UFunction* Function) override;
	bool NavigateToFunction(const UFunction* Function) override;

	static TOptional<FTsuEditorCommands> Instance;
};
