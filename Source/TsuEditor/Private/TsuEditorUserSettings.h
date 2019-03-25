#pragma once

#include "CoreMinimal.h"

#include "TsuEditorUserSettings.generated.h"

UENUM()
enum class ETsuTextEditor : uint8
{
	VisualStudioCode,
	VisualStudioCodeInsiders,
	Atom,
	SublimeText3,
	NotepadPlusPlus UMETA(DisplayName="Notepad++"),
	Notepad,

	Max UMETA(Hidden)
};

UCLASS(config=EditorPerProjectUserSettings, ClassGroup=TSU)
class UTsuEditorUserSettings final
	: public UObject
{
	GENERATED_BODY()

public:
	UTsuEditorUserSettings(const FObjectInitializer& ObjectInitializer);

	void PostInitProperties() override;

#if WITH_EDITOR
	void PostEditChangeProperty(struct FPropertyChangedEvent& Event) override;
#endif // WITH_EDITOR

	/** Whether to use the preconfigured editor path and arguments when opening associated TypeScript files */
	UPROPERTY(EditAnywhere, Config, Category="Text Editor", meta=(DisplayName="Use Editor Preset"))
	bool bUseTextEditorPreset;

	/** Editor preset to use when opening associated TypeScript files */
	UPROPERTY(EditAnywhere, Config, Category="Text Editor", meta=(DisplayName="Editor Preset", EditCondition="bUseTextEditorPreset"))
	ETsuTextEditor TextEditorPreset;

	/** Editor executable to run when opening associated TypeScript files */
	UPROPERTY(EditAnywhere, Config, Category="Text Editor", meta=(DisplayName="Editor Path", EditCondition="!bUseTextEditorPreset"))
	FString TextEditorPath;

	/** Arguments to provide the editor executable when opening associated TypeScript files */
	UPROPERTY(EditAnywhere, Config, Category="Text Editor", meta=(DisplayName="Editor Arguments", EditCondition="!bUseTextEditorPreset"))
	FString TextEditorArgs;
};
