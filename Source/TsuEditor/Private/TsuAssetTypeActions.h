#pragma once

#include "CoreMinimal.h"

#include "AssetTypeActions_Base.h"

class FAssetTypeActions_TsuBlueprint final
	: public FAssetTypeActions_Base
{
public:
	FAssetTypeActions_TsuBlueprint();

	FText GetName() const override;
	FColor GetTypeColor() const override;
	UClass* GetSupportedClass() const override;
	bool HasActions(const TArray<UObject*>& InObjects) const override;
	void GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder) override;
	uint32 GetCategories() override;
	bool CanFilter() override;
	bool IsImportedAsset() const override;
	void GetResolvedSourceFilePaths(const TArray<UObject*>& TypeAssets, TArray<FString>& OutSourceFilePaths) const override;
	void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
	void AssetsActivated(const TArray<UObject*>& InObjects, EAssetTypeActivationMethod::Type ActivationType) override;
	bool CanLocalize() const override;
	bool CanMerge() const override;
	void Merge(UObject* InObject) override;
	void Merge(UObject* BaseAsset, UObject* RemoteAsset, UObject* LocalAsset, const FOnMergeResolved& ResolutionCallback) override;
	bool ShouldForceWorldCentric() override;
	void PerformAssetDiff(UObject* OldAsset, UObject* NewAsset, const struct FRevisionInfo& OldRevision, const struct FRevisionInfo& NewRevision) const override;
	class UThumbnailInfo* GetThumbnailInfo(UObject* Asset) const override;
	EThumbnailPrimType GetDefaultThumbnailPrimitiveType(UObject* Asset) const override;
	TSharedPtr<class SWidget> GetThumbnailOverlay(const struct FAssetData& AssetData) const override;
	FText GetAssetDescription(const struct FAssetData& AssetData) const override;
	void BuildBackendFilter(struct FARFilter& InFilter) override;

private:
	IAssetTypeActions& GetBlueprintActions() { return *BlueprintActions.Pin(); }
	const IAssetTypeActions& GetBlueprintActions() const { return *BlueprintActions.Pin(); }

	TWeakPtr<IAssetTypeActions> BlueprintActions;
};
