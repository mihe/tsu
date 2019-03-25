#pragma once

#include "CoreMinimal.h"

#include "Modules/ModuleManager.h"

#define TSU_MODULE_EDITOR TEXT("TsuEditor")

class TSUEDITOR_API ITsuEditorModule
	: public IModuleInterface
{
public:
	static ITsuEditorModule& Get()
	{
		return FModuleManager::LoadModuleChecked<ITsuEditorModule>(TSU_MODULE_EDITOR);
	}

	static bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded(TSU_MODULE_EDITOR);
	}
};
