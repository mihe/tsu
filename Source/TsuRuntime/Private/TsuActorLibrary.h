#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "TsuActorLibrary.generated.h"

UCLASS(ClassGroup=TSU)
class UTsuActorLibrary final
	: public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Puts a component in to the OwnedComponents array of the Actor.
	 * The Component must be owned by the Actor or else it will assert
	 * In general this should not need to be called directly by anything other than UActorComponent functions
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static void AddOwnedComponent(AActor* Actor, UActorComponent* Component)
	{
		Actor->AddOwnedComponent(Component);
	}

	/**
	 * Removes a component from the OwnedComponents array of the Actor.
	 * In general this should not need to be called directly by anything other than UActorComponent functions
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static void RemoveOwnedComponent(AActor* Actor, UActorComponent* Component)
	{
		Actor->RemoveOwnedComponent(Component);
	}

	/** Adds a component to the instance components array */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static void AddInstancedComponent(AActor* Actor, UActorComponent* Component)
	{
		Actor->AddInstanceComponent(Component);
	}

	/** Register this component, creating any rendering/physics state. Will also adds to outer Actor's Components array, if not already present. */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, Meta=(TsuExtensionLibrary))
	static void RegisterComponent(UActorComponent* Component)
	{
		Component->RegisterComponent();
	}
};
