#pragma once

#include "CoreMinimal.h"

#include "Engine/EngineTypes.h"

class FTsuTimer
{
public:
	FTsuTimer(TWeakObjectPtr<class UWorld> InWorld, FTimerHandle InHandle, class UTsuDelegateEvent* InEvent)
		: World(MoveTemp(InWorld))
		, Handle(MoveTemp(InHandle))
		, Event(InEvent)
	{
	}

	TWeakObjectPtr<class UWorld> World;
	FTimerHandle Handle;
	class UTsuDelegateEvent* Event;
};
