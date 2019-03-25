#include "TsuEditorModule.h"

#include "TsuAssetTypeActions.h"
#include "TsuBlueprint.h"
#include "TsuBlueprintCompiler.h"
#include "TsuBlueprintGeneratedClass.h"
#include "TsuContext.h"
#include "TsuEditorCommands.h"
#include "TsuEditorStyle.h"
#include "TsuEditorUserSettings.h"
#include "TsuTypings.h"

#include "AssetToolsModule.h"
#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "IAssetTools.h"
#include "ISettingsModule.h"
#include "KismetCompiler.h"
#include "KismetCompilerModule.h"
#include "Misc/CoreDelegates.h"

namespace TsuEditorModule_Private
{

TArray<IBlueprintCompiler*>& GetKismetCompilers()
{
	auto& KismetCompilerModule = FModuleManager::LoadModuleChecked<IKismetCompilerInterface>(KISMET_COMPILER_MODULENAME);
	return KismetCompilerModule.GetCompilers();
}

} // namespace TsuEditorModule_Private

class FTsuEditorModule final
	: public ITsuEditorModule
	, public IBlueprintCompiler
{
public:
	void StartupModule() override
	{
		TsuEditorModule_Private::GetKismetCompilers().Add(this);

		FKismetCompilerContext::RegisterCompilerForBP(
			UTsuBlueprint::StaticClass(),
			&FTsuEditorModule::MakeCompiler);

		if (!IsRunningCommandlet())
		{
			FTsuEditorStyle::Initialize();
			FTsuEditorCommands::Register();
		}

		FCoreDelegates::OnPostEngineInit.AddRaw(
			this, &FTsuEditorModule::OnPostEngineInit);
	}

	void ShutdownModule() override
	{
		if (!IsRunningCommandlet())
		{
			if (GEditor)
				GEditor->OnBlueprintPreCompile().RemoveAll(this);

			UnregisterAssetTypeActions();
			FTsuEditorCommands::Unregister();
			FTsuEditorStyle::Shutdown();
		}

		TsuEditorModule_Private::GetKismetCompilers().Remove(this);
		UnregisterSettings();
	}

	bool CanCompile(const UBlueprint* Blueprint) override
	{
		return Cast<UTsuBlueprint>(Blueprint) != nullptr;
	}

	void Compile(
		UBlueprint* Blueprint,
		const FKismetCompilerOptions& CompileOptions,
		FCompilerResultsLog& Results,
		TArray<UObject*>* ObjLoaded) override
	{
		if (auto TsuBlueprint = Cast<UTsuBlueprint>(Blueprint))
		{
			FTsuBlueprintCompiler Compiler(TsuBlueprint, Results, CompileOptions, ObjLoaded);
			Compiler.Compile();
			check(Compiler.NewClass);
		}
	}

	static TSharedPtr<FKismetCompilerContext> MakeCompiler(
		UBlueprint* InBlueprint,
		FCompilerResultsLog& InMessageLog,
		const FKismetCompilerOptions& InCompileOptions)
	{
		return MakeShared<FTsuBlueprintCompiler>(
			CastChecked<UTsuBlueprint>(InBlueprint),
			InMessageLog,
			InCompileOptions,
			nullptr);
	}

private:
	void OnPostEngineInit()
	{
		FCoreDelegates::OnPostEngineInit.RemoveAll(this);

		RegisterSettings();

		if (!IsRunningCommandlet())
			RegisterAssetTypeActions();

		FTsuTypings::WriteAllTypings();

		if (GEditor)
		{
			GEditor->OnBlueprintPreCompile().AddRaw(
				this, &FTsuEditorModule::OnBlueprintPreCompile);
		}
	}

	void RegisterSettings()
	{
		if (auto SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
		{
			SettingsModule->RegisterSettings(
				TEXT("Editor"), // Container
				TEXT("Plugins"), // Category
				TEXT("TSU"), // Section
				FText::FromString(TEXT("TypeScript for Unreal")), // DisplayName
				FText::FromString(TEXT("Configure TypeScript for Unreal")), // Description
				GetMutableDefault<UTsuEditorUserSettings>());
		}
	}

	void UnregisterSettings()
	{
		if (auto SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
		{
			SettingsModule->UnregisterSettings(
				TEXT("Editor"), // Container
				TEXT("Plugins"), // Category
				TEXT("TSU")); // Section
		}
	}

	void RegisterAssetTypeActions()
	{
		AssetTypeActions = { MakeShared<FAssetTypeActions_TsuBlueprint>() };

		auto& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (const TSharedRef<IAssetTypeActions>& AssetTypeAction : AssetTypeActions)
			AssetToolsModule.RegisterAssetTypeActions(AssetTypeAction);
	}

	void UnregisterAssetTypeActions()
	{
		if (auto AssetToolsModule = FModuleManager::GetModulePtr<FAssetToolsModule>("AssetTools"))
		{
			for (const TSharedRef<IAssetTypeActions>& AssetTypeAction : AssetTypeActions)
				AssetToolsModule->Get().UnregisterAssetTypeActions(AssetTypeAction);
		}

		AssetTypeActions.Empty();
	}

	void OnBlueprintPreCompile(UBlueprint* Blueprint)
	{
		Blueprint->OnCompiled().AddRaw(this, &FTsuEditorModule::OnBlueprintCompiled);
	}

	void OnBlueprintCompiled(UBlueprint* Blueprint)
	{
		Blueprint->OnCompiled().RemoveAll(this);

		FTsuTypings::WriteTypings(Blueprint->GeneratedClass);
	}

	TArray<TSharedRef<IAssetTypeActions>> AssetTypeActions;
};

IMPLEMENT_MODULE(FTsuEditorModule, TsuEditor)
