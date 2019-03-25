#pragma once

#include "CoreMinimal.h"

#include "Modules/ModuleManager.h"

#define TSU_MODULE_UTILITIES TEXT("TsuUtilities")

class TSUUTILITIES_API ITsuUtilitiesModule
	: public IModuleInterface
{
public:
	static ITsuUtilitiesModule& Get()
	{
		return FModuleManager::LoadModuleChecked<ITsuUtilitiesModule>(TSU_MODULE_UTILITIES);
	}

	static bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded(TSU_MODULE_UTILITIES);
	}
};
