#include "TsuRuntimeModule.h"

#include "TsuBlueprint.h"
#include "TsuContext.h"
#include "TsuPaths.h"
#include "TsuRuntimeBlueprintCompiler.h"

#if WITH_EDITOR
#include "TsuRuntimeSettings.h"
#endif // WITH_EDITOR

#include "Editor.h"
#include "Engine/Engine.h"
#include "GameDelegates.h"
#include "HAL/PlatformProcess.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"

#if WITH_EDITOR
#include "ISettingsModule.h"
#endif // WITH_EDITOR

class FTsuRuntimeModule final
	: public ITsuRuntimeModule
{
public:
	void StartupModule() override
	{
		const FString BinariesDir = FPaths::Combine(
			FTsuPaths::PluginDir(),
			TEXT("Binaries"),
			TEXT("ThirdParty"),
			TEXT("V8"),
#if PLATFORM_WINDOWS
#if PLATFORM_64BITS
			TEXT("Win64/")
#else // PLATFORM_64BITS
			TEXT("Win32/")
#endif // PLATFORM_64BITS
#else
#error Not implemented
#endif // PLATFORM_WINDOWS
		);

		FPlatformProcess::PushDllDirectory(*BinariesDir);

		HandleV8 = FPlatformProcess::GetDllHandle(
			*FPaths::Combine(BinariesDir, TEXT("v8.dll")));

		HandleV8LibBase = FPlatformProcess::GetDllHandle(
			*FPaths::Combine(BinariesDir, TEXT("v8_libbase.dll")));

		HandleV8LibPlatform = FPlatformProcess::GetDllHandle(
			*FPaths::Combine(BinariesDir, TEXT("v8_libplatform.dll")));

		FPlatformProcess::PopDllDirectory(*BinariesDir);

		FCoreDelegates::OnPostEngineInit.AddRaw(this, &FTsuRuntimeModule::OnPostEngineInit);
	}

	void ShutdownModule() override
	{
		RemoveCleanupDelegates();
		UnregisterSettings();

		FPlatformProcess::FreeDllHandle(HandleV8LibPlatform);
		FPlatformProcess::FreeDllHandle(HandleV8LibBase);
		FPlatformProcess::FreeDllHandle(HandleV8);
	}

private:
	void RegisterSettings()
	{
#if WITH_EDITOR
		if (auto SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
		{
			SettingsModule->RegisterSettings(
				TEXT("Project"), // Container
				TEXT("Plugins"), // Category
				TEXT("TSU"), // Section
				FText::FromString(TEXT("TypeScript for Unreal")), // DisplayName
				FText::FromString(TEXT("Configure TypeScript for Unreal")), // Description
				GetMutableDefault<UTsuRuntimeSettings>());
		}
#endif // WITH_EDITOR
	}

	void UnregisterSettings()
	{
#if WITH_EDITOR
		if (auto SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
		{
			SettingsModule->UnregisterSettings(
				TEXT("Project"), // Container
				TEXT("Plugins"), // Category
				TEXT("TSU")); // Section
		}
#endif // WITH_EDITOR
	}

	void OnPostEngineInit()
	{
		FCoreDelegates::OnPostEngineInit.RemoveAll(this);

		if (!GEditor)
		{
			FKismetCompilerContext::RegisterCompilerForBP(
				UTsuBlueprint::StaticClass(),
				&FTsuRuntimeModule::MakeCompiler);
		}

		RegisterSettings();
		FTsuContext::Create();
		AddCleanupDelegates();
	}

	static TSharedPtr<FKismetCompilerContext> MakeCompiler(
		UBlueprint* InBlueprint,
		FCompilerResultsLog& InMessageLog,
		const FKismetCompilerOptions& InCompileOptions)
	{
		return MakeShared<FTsuRuntimeBlueprintCompiler>(
			CastChecked<UTsuBlueprint>(InBlueprint),
			InMessageLog,
			InCompileOptions,
			nullptr);
	}

	void AddCleanupDelegates()
	{
#if WITH_EDITOR
		if (GEditor)
		{
			HandlePreCompile = GEditor->OnBlueprintPreCompile().AddLambda(
				[](UBlueprint* /*Blueprint*/)
				{
					FTsuContext::Destroy();
				});
		}

		HandleBeginPIE = FEditorDelegates::BeginPIE.AddLambda(
			[](bool /*bIsSimulating*/)
			{
				FTsuContext::Destroy();
			});
#endif // WITH_EDITOR

		HandleWorldDestroyed = GEngine->OnWorldDestroyed().AddLambda(
			[](UWorld* /*World*/)
			{
				FTsuContext::Destroy();
			});

		HandleEndPlayMap = FGameDelegates::Get().GetEndPlayMapDelegate().AddLambda(
			[]
			{
				FTsuContext::Destroy();
			});

		HandlePreExit = FCoreDelegates::OnPreExit.AddLambda(
			[]
			{
				FTsuContext::Destroy();
			});
	}

	void RemoveCleanupDelegates()
	{
#if WITH_EDITOR
		if (GEditor)
			GEditor->OnBlueprintPreCompile().Remove(HandlePreCompile);

		FEditorDelegates::BeginPIE.Remove(HandleBeginPIE);
#endif // WITH_EDITOR

		if (GEngine)
			GEngine->OnWorldDestroyed().Remove(HandleWorldDestroyed);

		FGameDelegates::Get().GetEndPlayMapDelegate().Remove(HandleEndPlayMap);

		FCoreDelegates::OnPreExit.Remove(HandlePreExit);
	}

	void* HandleV8 = nullptr;
	void* HandleV8LibBase = nullptr;
	void* HandleV8LibPlatform = nullptr;

	FDelegateHandle HandlePreCompile;
	FDelegateHandle HandleBeginPIE;
	FDelegateHandle HandleWorldDestroyed;
	FDelegateHandle HandleEndPlayMap;
	FDelegateHandle HandlePreExit;
};

IMPLEMENT_MODULE(FTsuRuntimeModule, TsuRuntime)
