/**
 * #note(#mihe): This module is not currently used and is only meant to serve as a starting point for future development
 */

#include "TsuGeneratorModule.h"

#include "TsuGeneratorLog.h"
#include "TsuCodeGenerator.h"
#include "IProjectManager.h"
#include "Features/IModularFeatures.h"
#include "UniquePtr.h"
#include "ProjectDescriptor.h"
#include "Misc/ConfigCacheIni.h"

class FTsuGeneratorModule final
	: public ITsuGeneratorModule
{
public:
	/**
	 * @see IModuleInterface::StartupModule
	 */
	virtual void StartupModule() override;

	/**
	 * @see IModuleInterface::ShutdownModule
	 */
	virtual void ShutdownModule() override;

public:
	/**
	 * @see IScriptGeneratorPluginInterface::GetGeneratedCodeModuleName
	 */
	FString GetGeneratedCodeModuleName() const override { return TEXT("TsuRuntime"); }

	/**
	 * @see IScriptGeneratorPluginInterface::ShouldExportClassesForModule
	 */
	bool ShouldExportClassesForModule(const FString& ModuleName, EBuildModuleType::Type ModuleType, const FString& ModuleGeneratedIncludeDirectory) const override;

	/**
	 * @see IScriptGeneratorPluginInterface::SupportsTarget
	 */
	bool SupportsTarget(const FString& TargetName) const override;

	/**
	 * @see IScriptGeneratorPluginInterface::Initialize
	 */
	void Initialize(const FString& RootLocalPath, const FString& RootBuildPath, const FString& OutputDirectory, const FString& IncludeBase) override;

	/**
	 * @see IScriptGeneratorPluginInterface::ExportClass
	 */
	void ExportClass(UClass* Class, const FString& SourceHeaderFilename, const FString& GeneratedHeaderFilename, bool bHasChanged) override;

	/**
	 * @see IScriptGeneratorPluginInterface::FinishExport
	 */
	void FinishExport() override;

	/**
	 * @see IScriptGeneratorPluginInterface::GetGeneratorName
	 */
	FString GetGeneratorName() const override;

	/**
	 * @see IScriptGeneratorPluginInterface::GetGeneratorName
	 */
	void GetExternalDependencies(TArray<FString>& Dependencies) const override;

private:
	/** Specialized script code generator */
	TUniquePtr<FTsuCodeGenerator> CodeGenerator;
};

IMPLEMENT_MODULE(FTsuGeneratorModule, TsuGenerator)

void FTsuGeneratorModule::StartupModule()
{
	// Register ourselves as an editor feature
	IModularFeatures::Get().RegisterModularFeature(TEXT("ScriptGenerator"), this);
}

void FTsuGeneratorModule::ShutdownModule()
{
	CodeGenerator.Reset();

	// Unregister our editor feature
	IModularFeatures::Get().UnregisterModularFeature(TEXT("ScriptGenerator"), this);
}

FString FTsuGeneratorModule::GetGeneratorName() const
{
	return TEXT("TSU Generator");
}

void FTsuGeneratorModule::Initialize(const FString& RootLocalPath, const FString& RootBuildPath, const FString& OutputDirectory, const FString& IncludeBase)
{
	UE_LOG(LogTsuGenerator, Log, TEXT("Using TSU Generator."));
	CodeGenerator = MakeUnique<FTsuCodeGenerator>(RootLocalPath, RootBuildPath, OutputDirectory, IncludeBase);
	UE_LOG(LogTsuGenerator, Log, TEXT("Output directory: %s"), *OutputDirectory);
}

bool FTsuGeneratorModule::ShouldExportClassesForModule(const FString& ModuleName, EBuildModuleType::Type ModuleType, const FString& ModuleGeneratedIncludeDirectory) const
{
	IModuleInterface* ModuleInfo = FModuleManager::Get().GetModule(FName(*ModuleName));

	bool bCanExport = (ModuleType == EBuildModuleType::EngineRuntime || ModuleType == EBuildModuleType::GameRuntime);
	if (bCanExport)
	{
		// Only export functions from selected modules
		static TArray<FString> SupportedModules = [] {
			TArray<FString> Result;
			GConfig->GetArray(TEXT("Plugins"), TEXT("ScriptSupportedModules"), Result, GEngineIni);
			return Result;
		}();

		bCanExport = SupportedModules.Num() == 0 || SupportedModules.Contains(ModuleName);
	}
	return bCanExport;
}

void FTsuGeneratorModule::ExportClass(UClass* Class, const FString& SourceHeaderFilename, const FString& GeneratedHeaderFilename, bool bHasChanged)
{
	CodeGenerator->ExportClass(Class, SourceHeaderFilename, GeneratedHeaderFilename, bHasChanged);
}

void FTsuGeneratorModule::FinishExport()
{
	CodeGenerator->FinishExport();
}

bool FTsuGeneratorModule::SupportsTarget(const FString& TargetName) const
{
	if (FPaths::IsProjectFilePathSet())
	{
		FProjectDescriptor ProjectDescriptor;
		FText OutError;
		if (ProjectDescriptor.Load(FPaths::GetProjectFilePath(), OutError))
		{
			for (FPluginReferenceDescriptor& PluginDescriptor : ProjectDescriptor.Plugins)
			{
				// #todo(#mihe): Get the plugin name from somewhere
				if (PluginDescriptor.bEnabled && PluginDescriptor.Name == TEXT("TSU"))
					return true;
			}
		}
	}

	return false;
}

void FTsuGeneratorModule::GetExternalDependencies(TArray<FString>& Dependencies) const
{
}
