#pragma once

#include "CoreMinimal.h"

#include "TsuFactory.h"

#include "EditorReimportHandler.h"

#include "TsuFactoryReimport.generated.h"

UCLASS(MinimalApi, collapsecategories, ClassGroup=TSU)
class UTsuFactoryReimport final
	: public UTsuFactory
	, public FReimportHandler
{
	GENERATED_BODY()

public:
	bool CanReimport(UObject* Object, TArray<FString>& OutFilenames) override;
	void SetReimportPaths(UObject* Object, const TArray<FString>& NewReimportPaths) override;
	EReimportResult::Type Reimport(UObject* Object) override;

	UPROPERTY()
	class UTsuBlueprint* OriginalBlueprint;
};
