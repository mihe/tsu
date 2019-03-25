#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "TsuHotReloadListenerInterface.generated.h"

/** */
UINTERFACE(Blueprintable)
class TSURUNTIME_API UTsuHotReloadListenerInterface : public UInterface
{
	GENERATED_BODY()
};

/** */
class TSURUNTIME_API ITsuHotReloadListenerInterface
{
	GENERATED_BODY()

public:
	/** */
	UFUNCTION(BlueprintImplementableEvent, Category="TSU|Hot Reload")
	void PostHotReload();
};
