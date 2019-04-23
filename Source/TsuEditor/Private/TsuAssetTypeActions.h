#pragma once

#include "CoreMinimal.h"

#include "AssetTypeActions_Base.h"

class FAssetTypeActions_TsuBlueprint final
	: public FAssetTypeActions_Base
{
public:
	FText GetName() const override;
	FColor GetTypeColor() const override;
	UClass* GetSupportedClass() const override;
	bool HasActions(const TArray<UObject*>& Objects) const override;
	void GetActions(const TArray<UObject*>& Objects, FMenuBuilder& MenuBuilder) override;
	uint32 GetCategories() override;
	bool IsImportedAsset() const override;
	void GetResolvedSourceFilePaths(const TArray<UObject*>& TypeAssets, TArray<FString>& OutSourceFilePaths) const override;
};
