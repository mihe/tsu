#include "TsuEditorCommands.h"

#include "TsuBlueprint.h"
#include "TsuBlueprintGeneratedClass.h"
#include "TsuEditorStyle.h"
#include "TsuEditorUserSettings.h"
#include "TsuPaths.h"
#include "TsuUtilities.h"

#include "EditorFramework/AssetImportData.h"
#include "Framework/Notifications/NotificationManager.h"
#include "UObject/Package.h"
#include "Widgets/Notifications/SNotificationList.h"

namespace TsuEditorCommands_Private
{

void ShowTextEditorNotification(
	const FString& Message,
	SNotificationItem::ECompletionState State)
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

TOptional<FTsuEditorCommands> FTsuEditorCommands::Instance;

FTsuEditorCommands::FTsuEditorCommands()
{
	FSourceCodeNavigation::AddNavigationHandler(this);
}

FTsuEditorCommands::~FTsuEditorCommands()
{
	FSourceCodeNavigation::RemoveNavigationHandler(this);
}

FTsuEditorCommands& FTsuEditorCommands::Get()
{
	return Instance.GetValue();
}

void FTsuEditorCommands::Register()
{
	Instance.Emplace();
}

void FTsuEditorCommands::Unregister()
{
	Instance.Reset();
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
