#pragma once

#include "CoreMinimal.h"

#include "Engine/Blueprint.h"

#include "TsuBlueprint.generated.h"

UCLASS(BlueprintType, ClassGroup=TSU)
class TSURUNTIME_API UTsuBlueprint final
	: public UBlueprint
{
	GENERATED_BODY()

public:
	UTsuBlueprint(const FObjectInitializer& ObjectInitializer);

#if WITH_EDITOR
	UClass* GetBlueprintClass() const override;
	bool SupportedByDefaultBlueprintFactory() const override { return false; }
	bool AlwaysCompileOnLoad() const override { return true; }
	void GatherDependencies(TSet<TWeakObjectPtr<UBlueprint>>& InDependencies) const override;
	static bool ValidateGeneratedClass(const UClass* InClass);
	bool IsCodeDirty() const;
#endif // WITH_EDITOR

#if WITH_EDITORONLY_DATA
	void GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const override;

	UPROPERTY(VisibleAnywhere, Instanced, Category=ImportSettings)
	class UAssetImportData* AssetImportData = nullptr;
#endif // WITH_EDITORONLY_DATA
};
