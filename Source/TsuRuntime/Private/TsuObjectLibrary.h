#pragma once

#include "CoreMinimal.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#include "TsuObjectLibrary.generated.h"

UCLASS(ClassGroup=TSU, BlueprintInternalUseOnly, Meta=(TsuExtension))
class UTsuObjectLibrary final
	: public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Returns true if the object is usable (non-null and not pending kill). */
	UFUNCTION(BlueprintPure)
	static bool IsValid(const UObject* Object)
	{
		return UKismetSystemLibrary::IsValid(Object);
	}

	/** Returns the actual object name. */
	UFUNCTION(BlueprintPure)
	static FString GetName(const UObject* Object)
	{
		return UKismetSystemLibrary::GetObjectName(Object);
	}

	/** Returns the full path to the specified object. */
	UFUNCTION(BlueprintPure)
	static FString GetPathName(const UObject* Object)
	{
		return UKismetSystemLibrary::GetPathName(Object);
	}

	/**
	 * Returns the display name (or actor label), for displaying as a debugging aid.
	 * Note: In editor builds, this is the actor label. 
	 * In non-editor builds, this is the actual object name.
	 * This function should not be used to uniquely identify actors!
	 * It is not localized and should not be used for display to an end user of a game.
	 */
	UFUNCTION(BlueprintPure)
	static FString GetDisplayName(const UObject* Object)
	{
		return UKismetSystemLibrary::GetDisplayName(Object);
	}

	/** Returns the class of the object, will always be valid if object is not null */
	UFUNCTION(BlueprintPure)
	static UClass* GetClass(const UObject* Object)
	{
		return UGameplayStatics::GetObjectClass(Object);
	}

	/** Returns true if the two objects are equal */
	UFUNCTION(BlueprintPure)
	static bool Equals(UObject* Object, UObject* Other)
	{
		return UKismetMathLibrary::EqualEqual_ObjectObject(Object, Other);
	}
};
