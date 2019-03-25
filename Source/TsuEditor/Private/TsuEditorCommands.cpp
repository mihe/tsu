#include "TsuEditorCommands.h"

#include "TsuBlueprint.h"
#include "TsuBlueprintGeneratedClass.h"
#include "TsuEditorStyle.h"
#include "TsuEditorUserSettings.h"
#include "TsuPaths.h"
#include "TsuUtilities.h"

#include "BlueprintEditor.h"
#include "BlueprintEditorModule.h"
#include "Editor.h"
#include "EditorFramework/AssetImportData.h"
#include "Framework/Commands/UICommandList.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "Framework/Notifications/NotificationManager.h"
#include "K2Node_CallFunction.h"
#include "K2Node_Event.h"
#include "Modules/ModuleManager.h"
#include "Styling/ISlateStyle.h"
#include "Toolkits/AssetEditorManager.h"
#include "Widgets/Notifications/SNotificationList.h"

#define KISMET_MODULE_NAME "Kismet"

#define LOCTEXT_NAMESPACE "TsuEditor"

namespace TsuEditorCommands_Private
{

void ShowTextEditorNotification(const FString& Message, SNotificationItem::ECompletionState State)
{
	FNotificationInfo Info(FText::FromString(Message));
	Info.Image = FTsuEditorStyle::Get().GetBrush(TEXT("TsuEditor.OpenInTextEditor"));
	Info.FadeInDuration = 0.1f;
	Info.FadeOutDuration = 0.5f;
	Info.ExpireDuration = 3.0f;
	Info.bUseThrobber = false;
	Info.bUseSuccessFailIcons = true;
	Info.bUseLargeFont = true;
	Info.bFireAndForget = true;
	Info.bAllowThrottleWhenFrameRateIsLow = false;

	TSharedPtr<SNotificationItem> NotificationItem = FSlateNotificationManager::Get().AddNotification(Info);
	NotificationItem->SetCompletionState(State);
}

bool FindActiveBlueprint(UBlueprint*& OutActiveBlueprint, FBlueprintEditor*& OutActiveEditor)
{
	UBlueprint* ActiveBlueprint = nullptr;
	IAssetEditorInstance* ActiveAssetEditor = nullptr;
	double LastActivationTime = 0.0;

	for (UObject* Asset : FAssetEditorManager::Get().GetAllEditedAssets())
	{
		IAssetEditorInstance* Editor = FAssetEditorManager::Get().FindEditorForAsset(Asset, false);
		if (Editor->GetLastActivationTime() > LastActivationTime)
		{
			LastActivationTime = Editor->GetLastActivationTime();

			if (auto Blueprint = Cast<UBlueprint>(Asset))
			{
				ActiveAssetEditor = Editor;
				ActiveBlueprint = Blueprint;
			}
			else
			{
				ActiveAssetEditor = nullptr;
				ActiveBlueprint = nullptr;
			}
		}
	}

	OutActiveBlueprint = ActiveBlueprint;
	OutActiveEditor = static_cast<FBlueprintEditor*>(ActiveAssetEditor);

	return OutActiveBlueprint != nullptr;
}

bool FindActiveTsuBlueprint(
	UTsuBlueprint*& OutActiveBlueprint,
	FString& OutFunctionName)
{
	OutActiveBlueprint = nullptr;
	OutFunctionName.Empty();

	UBlueprint* ActiveBlueprint = nullptr;
	FBlueprintEditor* ActiveEditor = nullptr;

	if (!TsuEditorCommands_Private::FindActiveBlueprint(ActiveBlueprint, ActiveEditor))
		return false;

	auto ActiveTsuBlueprint = Cast<UTsuBlueprint>(ActiveBlueprint);

	FString SelectedFunctionName;

	if (!ActiveTsuBlueprint && ActiveEditor)
	{
		for (UObject* Object : ActiveEditor->GetSelectedNodes())
		{
			if (auto CallNode = Cast<UK2Node_CallFunction>(Object))
			{
				SelectedFunctionName = CallNode->FunctionReference.GetMemberName().ToString();
				UClass* ParentClass = CallNode->FunctionReference.GetMemberParentClass();

				if (auto ParentTsuClass = Cast<UTsuBlueprintGeneratedClass>(ParentClass))
					ActiveTsuBlueprint = Cast<UTsuBlueprint>(ParentTsuClass->ClassGeneratedBy);

				break;
			}
		}
	}

	if (!ActiveTsuBlueprint)
		return false;

	OutActiveBlueprint = ActiveTsuBlueprint;
	OutFunctionName = SelectedFunctionName;

	return true;
}

bool GetFunctionLocation(
	UTsuBlueprint* Blueprint,
	const FString& FunctionName,
	int32& OutLine,
	int32& OutColumn)
{
	if (auto GeneratedClass = Cast<UTsuBlueprintGeneratedClass>(Blueprint->GeneratedClass))
	{
		for (const FTsuParsedFunction& Export : GeneratedClass->Exports.Exports)
		{
			if (Export.Name == FunctionName)
			{
				OutLine = Export.Line;
				OutColumn = Export.Character;
				return true;
			}
		}
	}

	return false;
}

} // namespace TsuEditorCommands_Private

// Needs to be TSharedPtr for TSharedFromThis in UI_COMMAND to work
TSharedPtr<FTsuEditorCommands> FTsuEditorCommands::Instance;

FTsuEditorCommands::FTsuEditorCommands()
	: FBindingContext(
		TEXT("TsuEditor"),
		FText::FromString(TEXT("TSU Editor")),
		NAME_None,
		FTsuEditorStyle::Get().GetStyleSetName())
{
	HookModules();
	FSourceCodeNavigation::AddNavigationHandler(this);
}

FTsuEditorCommands::~FTsuEditorCommands()
{
	FSourceCodeNavigation::RemoveNavigationHandler(this);
	UnhookModules();

	FInputBindingManager::Get().RemoveContextByName(GetContextName());
	CommandsChanged.Broadcast(*this);
}

FTsuEditorCommands& FTsuEditorCommands::Get()
{
	return *Instance;
}

void FTsuEditorCommands::Register()
{
	Instance = MakeShared<FTsuEditorCommands>();
}

void FTsuEditorCommands::Unregister()
{
	Instance.Reset();
}

void FTsuEditorCommands::HookModules()
{
	if (FModuleManager::Get().IsModuleLoaded(KISMET_MODULE_NAME))
	{
		OnModulesChanged(KISMET_MODULE_NAME, EModuleChangeReason::ModuleLoaded);
	}
	else
	{
		OnModulesChangedHandle =
			FModuleManager::Get()
				.OnModulesChanged()
				.AddRaw(this, &FTsuEditorCommands::OnModulesChanged);
	}
}

void FTsuEditorCommands::UnhookModules()
{
	if (OnModulesChangedHandle.IsValid())
	{
		FModuleManager::Get().OnModulesChanged().Remove(OnModulesChangedHandle);
		OnModulesChangedHandle.Reset();
	}
}

const FUICommandInfo& FTsuEditorCommands::GetActionOpenInTextEditor() const
{
	return *OpenInTextEditor;
}

void FTsuEditorCommands::OnModulesChanged(FName ModuleName, EModuleChangeReason ChangeReason)
{
	if (ModuleName != KISMET_MODULE_NAME || ChangeReason != EModuleChangeReason::ModuleLoaded)
		return;

	UI_COMMAND(
		OpenInTextEditor,
		"Open In Text Editor",
		"Opens the associated TypeScript file in the configured text editor",
		EUserInterfaceActionType::Button,
		FInputChord(EModifierKey::Alt | EModifierKey::Shift, EKeys::T));

	auto Extender = MakeShared<FExtender>();

	auto CommandList = MakeShared<FUICommandList>();
	CommandList->MapAction(
		OpenInTextEditor,
		FExecuteAction::CreateLambda([] { OpenActiveGraphInTextEditor(); }),
		EUIActionRepeatMode::RepeatDisabled);

	auto ToolBarExtensionsView = Extender->AddToolBarExtension(
		TEXT("Asset"),
		EExtensionHook::After,
		CommandList,
		FToolBarExtensionDelegate::CreateRaw(
			this, &FTsuEditorCommands::OnAddToolBarExtensions));

	auto MenuExtensionsView = Extender->AddMenuExtension(
		TEXT("AssetEditorActions"),
		EExtensionHook::After,
		CommandList,
		FMenuExtensionDelegate::CreateRaw(
			this, &FTsuEditorCommands::OnAddMenuExtensions));

	auto& KismetModule = FModuleManager::GetModuleChecked<FBlueprintEditorModule>(KISMET_MODULE_NAME);
	KismetModule.GetsSharedBlueprintEditorCommands()->Append(CommandList);
	KismetModule.GetMenuExtensibilityManager()->AddExtender(Extender);

	CommandsChanged.Broadcast(*this);

	UnhookModules();
}

void FTsuEditorCommands::OnOpenInTextEditorClicked()
{
	OpenActiveGraphInTextEditor();
}

void FTsuEditorCommands::OnAddToolBarExtensions(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(OpenInTextEditor);
}

void FTsuEditorCommands::OnAddMenuExtensions(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(OpenInTextEditor);
}

bool FTsuEditorCommands::CanNavigateToFunction(const UFunction* Function)
{
	UStruct* OuterClass = Function->GetOuterUClass();
	return OuterClass && OuterClass->IsA<UTsuBlueprintGeneratedClass>();
}

bool FTsuEditorCommands::NavigateToFunction(const UFunction* Function)
{
	auto OuterClass = Cast<UTsuBlueprintGeneratedClass>(Function->GetOuterUClass());
	if (!OuterClass)
		return false;

	auto Blueprint = Cast<UTsuBlueprint>(OuterClass->ClassGeneratedBy);
	if (!Blueprint)
		return false;

	return OpenFunctionInTextEditor(Blueprint, Function->GetName());
}

bool FTsuEditorCommands::OpenActiveGraphInTextEditor()
{
	UTsuBlueprint* ActiveTsuBlueprint = nullptr;
	FString SelectedFunctionName;

	if (!TsuEditorCommands_Private::FindActiveTsuBlueprint(ActiveTsuBlueprint, SelectedFunctionName))
	{
		TsuEditorCommands_Private::ShowTextEditorNotification(
			TEXT("No associated file found"),
			SNotificationItem::CS_Fail);

		return false;
	}

	if (SelectedFunctionName.IsEmpty())
		return OpenGraphInTextEditor(ActiveTsuBlueprint);
	else
		return OpenFunctionInTextEditor(ActiveTsuBlueprint, SelectedFunctionName);
}

bool FTsuEditorCommands::OpenGraphInTextEditor(UTsuBlueprint* Blueprint, int32 Line, int32 Column)
{
	const FString FilePath = Blueprint->AssetImportData->GetFirstFilename();

	FString PlatformFilePath = FilePath;
	FPaths::MakePlatformFilename(PlatformFilePath);

	FString PlatformScriptsDir = FPaths::ConvertRelativePathToFull(FTsuPaths::ScriptsDir());
	FPaths::MakePlatformFilename(PlatformScriptsDir);

	auto Settings = GetDefault<UTsuEditorUserSettings>();

	if (Settings->TextEditorPath.IsEmpty())
	{
		auto TextEditorEnum = FindObject<UEnum>(
			ANY_PACKAGE,
			GET_TYPE_NAME_CHECKED(ETsuTextEditor),
			true);

		const FString EditorPresetName = TextEditorEnum->GetDisplayNameTextByValue((int64)Settings->TextEditorPreset).ToString();

		TsuEditorCommands_Private::ShowTextEditorNotification(
			FString::Printf(
				TEXT("Unable to open %s due to empty executable path, check editor preferences"),
				*EditorPresetName),
			SNotificationItem::CS_Fail);

		return false;
	}

	FString EditorArgs = Settings->TextEditorArgs;
	EditorArgs.ReplaceInline(TEXT("%f"), *PlatformFilePath);
	EditorArgs.ReplaceInline(TEXT("%l"), *LexToString(Line));
	EditorArgs.ReplaceInline(TEXT("%c"), *LexToString(Column));
	EditorArgs.ReplaceInline(TEXT("%d"), *PlatformScriptsDir);

	FProcHandle ProcHandle = FPlatformProcess::CreateProc(
		*Settings->TextEditorPath, // URL
		*EditorArgs, // Parms
		true, // bLaunchDetected
		false, // bLaunchHidden
		false, // bLaunchReallyHidden
		nullptr, // OutProcessID
		0, // PriorityModifier
		nullptr, // OptionalWorkingDirectory
		nullptr); // PipeWriteChild

	if (!ProcHandle.IsValid())
	{
		const TCHAR* TextEditorEnumName = GET_TYPE_NAME_CHECKED(ETsuTextEditor);
		auto TextEditorEnum = FindObject<UEnum>(ANY_PACKAGE, TextEditorEnumName, true);

		const FString EditorPresetName = TextEditorEnum->GetDisplayNameTextByValue(
			(int64)Settings->TextEditorPreset).ToString();

		TsuEditorCommands_Private::ShowTextEditorNotification(
			FString::Printf(
				TEXT("Failed to open %s, see output log"),
				*EditorPresetName),
			SNotificationItem::CS_Fail);

		return false;
	}

	TsuEditorCommands_Private::ShowTextEditorNotification(
		FString::Printf(
			TEXT("Opening '%s'..."),
			*FPaths::GetCleanFilename(FilePath)),
		SNotificationItem::CS_Success);

	return true;
}

bool FTsuEditorCommands::OpenFunctionInTextEditor(class UTsuBlueprint* Blueprint, const FString& FunctionName)
{
	int32 Line = 0;
	int32 Column = 0;

	if (!TsuEditorCommands_Private::GetFunctionLocation(Blueprint, FunctionName, Line, Column))
		return false;

	if (!OpenGraphInTextEditor(Blueprint, Line, Column))
		return false;

	return true;
}

#undef LOCTEXT_NAMESPACE
