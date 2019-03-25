#pragma once

#include "CoreMinimal.h"

#include "Modules/ModuleManager.h"

#define TSU_MODULE_RUNTIME TEXT("TsuRuntime")

class TSURUNTIME_API ITsuRuntimeModule
	: public IModuleInterface
{
public:
	static ITsuRuntimeModule& Get()
	{
		return FModuleManager::LoadModuleChecked<ITsuRuntimeModule>(TSU_MODULE_RUNTIME);
	}

	static bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded(TSU_MODULE_RUNTIME);
	}
};
