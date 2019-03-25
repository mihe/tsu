#include "TsuEditorUserSettings.h"

#include "HAL/PlatformMisc.h"
#include "Misc/Paths.h"

#if PLATFORM_WINDOWS

#include "Windows/AllowWindowsPlatformTypes.h"

namespace TsuEditorModule_Private
{

struct FRegistryPath
{
	HKEY Key = NULL;
	const TCHAR* SubKey = nullptr;
	const TCHAR* Value = nullptr;
};

TOptional<FString> GetFirstRegistryData(const TArray<FRegistryPath>& RegistryPaths)
{
	TOptional<FString> InstallPath;

	for (const FRegistryPath& Path : RegistryPaths)
	{
		FString Value;
		if (FWindowsPlatformMisc::QueryRegKey(Path.Key, Path.SubKey, Path.Value, Value))
		{
			InstallPath = Value;
			break;
		}
	}

	return InstallPath;
}

void GetEditorDetails(ETsuTextEditor Editor, FString& Path, FString& Args)
{
	Path.Empty();
	Args.Empty();

	if (Editor == ETsuTextEditor::VisualStudioCode)
	{
		const TOptional<FString> InstallPath = GetFirstRegistryData(
			{{HKEY_CURRENT_USER,
			  TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{771FD6B0-FA20-440A-A002-3B3BAC16DC50}_is1"),
			  TEXT("InstallLocation")},
			 {HKEY_CURRENT_USER,
			  TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{D628A17A-9713-46BF-8D57-E671B46A741E}_is1"),
			  TEXT("InstallLocation")},
			 {HKEY_LOCAL_MACHINE,
			  TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{EA457B21-F73E-494C-ACAB-524FDE069978}_is1"),
			  TEXT("InstallLocation")},
			 {HKEY_LOCAL_MACHINE,
			  TEXT("Software\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{F8A2A208-72B3-4D61-95FC-8A65D340689B}_is1"),
			  TEXT("InstallLocation")}});

		if (InstallPath)
		{
			Path = InstallPath.GetValue() / TEXT("bin") / TEXT("code.cmd");
			Args = TEXT("--goto \"%f\":%l:%c \"%d\"");
		}
	}
	else if (Editor == ETsuTextEditor::VisualStudioCodeInsiders)
	{
		const TOptional<FString> InstallPath = GetFirstRegistryData(
			{{HKEY_CURRENT_USER,
			  TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{217B4C08-948D-4276-BFBB-BEE930AE5A2C}_is1"),
			  TEXT("InstallLocation")},
			 {HKEY_CURRENT_USER,
			  TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{26F4A15E-E392-4887-8C09-7BC55712FD5B}_is1"),
			  TEXT("InstallLocation")},
			 {HKEY_LOCAL_MACHINE,
			  TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{1287CAD5-7C8D-410D-88B9-0D1EE4A83FF2}_is1"),
			  TEXT("InstallLocation")},
			 {HKEY_LOCAL_MACHINE,
			  TEXT("Software\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{C26E74D1-022E-4238-8B9D-1E7564A36CC9}_is1"),
			  TEXT("InstallLocation")}});

		if (InstallPath)
		{
			Path = InstallPath.GetValue() / TEXT("bin") / TEXT("code-insiders.cmd");
			Args = TEXT("--goto \"%f\":%l:%c \"%d\"");
		}
	}
	else if (Editor == ETsuTextEditor::SublimeText3)
	{
		const TOptional<FString> InstallPath = GetFirstRegistryData(
			{{HKEY_LOCAL_MACHINE,
			  TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Sublime Text 3_is1"),
			  TEXT("InstallLocation")}});

		if (InstallPath)
		{
			Path = InstallPath.GetValue() / TEXT("subl.exe");
			Args = TEXT("\"%f\":%l:%c");
		}
	}
	else if (Editor == ETsuTextEditor::Atom)
	{
		const TOptional<FString> InstallPath = GetFirstRegistryData(
			{{HKEY_CURRENT_USER,
			  TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\atom"),
			  TEXT("InstallLocation")}});

		if (InstallPath)
		{
			Path = InstallPath.GetValue() / TEXT("atom.exe");
			Args = TEXT("\"%f\":%l:%c");
		}
	}
	else if (Editor == ETsuTextEditor::NotepadPlusPlus)
	{
		const TOptional<FString> InstallPath = GetFirstRegistryData(
			{{HKEY_LOCAL_MACHINE,
			  TEXT("Software\\Notepad++"),
			  TEXT("")}});

		if (InstallPath)
		{
			Path = InstallPath.GetValue() / TEXT("notepad++.exe");
			Args = TEXT("-n%l -c%c \"%f\"");
		}
	}
	else if (Editor == ETsuTextEditor::Notepad)
	{
		Path = TEXT("notepad.exe");
		Args = TEXT("\"%f\"");
	}

	FPaths::MakePlatformFilename(Path);
}

} // namespace TsuEditorModule_Private

#include "Windows/HideWindowsPlatformTypes.h"

#else // PLATFORM_WINDOWS
#error Not implemented
#endif // PLATFORM_WINDOWS

UTsuEditorUserSettings::UTsuEditorUserSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bUseTextEditorPreset = true;
	TextEditorPreset = ETsuTextEditor::Notepad;
	TsuEditorModule_Private::GetEditorDetails(TextEditorPreset, TextEditorPath, TextEditorArgs);
}

void UTsuEditorUserSettings::PostInitProperties()
{
	if (bUseTextEditorPreset)
		TsuEditorModule_Private::GetEditorDetails(TextEditorPreset, TextEditorPath, TextEditorArgs);

	Super::PostInitProperties();
}

#if WITH_EDITOR
void UTsuEditorUserSettings::PostEditChangeProperty(FPropertyChangedEvent& Event)
{
	static const FName Name_TextEditorPreset = GET_MEMBER_NAME_CHECKED(UTsuEditorUserSettings, TextEditorPreset);
	static const FName Name_bUseTextEditorPreset = GET_MEMBER_NAME_CHECKED(UTsuEditorUserSettings, bUseTextEditorPreset);

	const FName PropertyName = Event.GetPropertyName();

	if (PropertyName == NAME_None ||
		PropertyName == Name_TextEditorPreset ||
		PropertyName == Name_bUseTextEditorPreset)
	{
		if (bUseTextEditorPreset)
			TsuEditorModule_Private::GetEditorDetails(TextEditorPreset, TextEditorPath, TextEditorArgs);
	}

	Super::PostEditChangeProperty(Event);
}
#endif // WITH_EDITOR
