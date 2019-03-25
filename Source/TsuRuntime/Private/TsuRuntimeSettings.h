#pragma once

#include "CoreMinimal.h"

#include "TsuRuntimeSettings.generated.h"

UCLASS(config=Game, defaultconfig, ClassGroup=TSU)
class UTsuRuntimeSettings final
	: public UObject
{
	GENERATED_BODY()

public:
	/** Whether or not to use a DefaultToSelf parameter */
	UPROPERTY(EditAnywhere, Config, Category="Compilation", Meta=(ConfigRestartRequired=true))
	bool bUseSelfParameter = true;

	/** The name of the parameter that should get the 'DefaultToSelf' metadata assigned to it */
	UPROPERTY(EditAnywhere, Config, Category="Compilation", Meta=(ConfigRestartRequired=true, EditCondition="bUseSelfParameter"))
	FString SelfParameterName = TEXT("target");
};
