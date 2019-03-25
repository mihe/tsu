#pragma once

#include "CoreMinimal.h"

#include "IScriptGeneratorPluginInterface.h"

#include "Modules/ModuleManager.h"

#define TSU_MODULE_GENERATOR TEXT("TsuGenerator")

class TSUGENERATOR_API ITsuGeneratorModule
	: public IScriptGeneratorPluginInterface
{
public:
	static ITsuGeneratorModule& Get()
	{
		return FModuleManager::LoadModuleChecked<ITsuGeneratorModule>(TSU_MODULE_GENERATOR);
	}

	static bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded(TSU_MODULE_GENERATOR);
	}
};
