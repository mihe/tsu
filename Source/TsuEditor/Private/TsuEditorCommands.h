#pragma once

#include "CoreMinimal.h"

#include "Framework/Commands/Commands.h"
#include "SourceCodeNavigation.h"

class FTsuEditorCommands final
	: public FBindingContext
	, public ISourceCodeNavigationHandler
{
public:
	FTsuEditorCommands();
	~FTsuEditorCommands();

	FTsuEditorCommands(const FTsuEditorCommands& Other) = delete;
	FTsuEditorCommands& operator=(const FTsuEditorCommands& Other) = delete;

	static FTsuEditorCommands& Get();

	static void Register();
	static void Unregister();

	static bool OpenActiveGraphInTextEditor();
	static bool OpenGraphInTextEditor(class UTsuBlueprint* Blueprint, int32 Line = 1, int32 Column = 1);
	static bool OpenFunctionInTextEditor(class UTsuBlueprint* Blueprint, const FString& FunctionName);

	const class FUICommandInfo& GetActionOpenInTextEditor() const;

private:
	void HookModules();
	void UnhookModules();

	void OnModulesChanged(FName ModuleName, EModuleChangeReason ChangeReason);
	void OnOpenInTextEditorClicked();
	void OnAddToolBarExtensions(class FToolBarBuilder& Builder);
	void OnAddMenuExtensions(class FMenuBuilder& Builder);

	bool CanNavigateToFunction(const UFunction* Function) override;
	bool NavigateToFunction(const UFunction* Function) override;

	static TSharedPtr<FTsuEditorCommands> Instance;
	
	FDelegateHandle OnModulesChangedHandle;

	// The name of this variable affects what icon style is used
	TSharedPtr<class FUICommandInfo> OpenInTextEditor;
};
