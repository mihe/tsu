#include "TsuAssetTypeActions.h"

#include "TsuBlueprint.h"
#include "TsuEditorCommands.h"
#include "TsuEditorStyle.h"

#include "AssetToolsModule.h"
#include "EditorFramework/AssetImportData.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "HAL/PlatformProcess.h"
#include "IAssetTools.h"
#include "Modules/ModuleManager.h"

namespace TsuAssetTypeActions_Private
{

template <typename T>
TArray<TWeakObjectPtr<T>> GetTypedWeakObjectPtrs(const TArray<UObject*>& InObjects)
{
	check(InObjects.Num() > 0);

	TArray<TWeakObjectPtr<T>> TypedObjects;
	for (UObject* Object : InObjects)
		TypedObjects.Add(CastChecked<T>(Object));

	return TypedObjects;
}

TWeakPtr<IAssetTypeActions> GetAssetTypeActionsForBlueprint()
{
	auto& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
	return AssetToolsModule.GetAssetTypeActionsForClass(UBlueprint::StaticClass());
}

void ExecuteOpenInTextEditor(TArray<TWeakObjectPtr<UTsuBlueprint>> Blueprints)
{
	for (TWeakObjectPtr<UTsuBlueprint>& Blueprint : Blueprints)
		FTsuEditorCommands::OpenGraphInTextEditor(Blueprint.Get());
}

} // namespace TsuAssetTypeActions_Private

FAssetTypeActions_TsuBlueprint::FAssetTypeActions_TsuBlueprint()
	: BlueprintActions(TsuAssetTypeActions_Private::GetAssetTypeActionsForBlueprint())
{
}

FText FAssetTypeActions_TsuBlueprint::GetName() const
{
	return FText::FromString(TEXT("TSU Class"));
}

FColor FAssetTypeActions_TsuBlueprint::GetTypeColor() const
{
	return GetBlueprintActions().GetTypeColor();
}

UClass* FAssetTypeActions_TsuBlueprint::GetSupportedClass() const
{
	return UTsuBlueprint::StaticClass();
}

bool FAssetTypeActions_TsuBlueprint::HasActions(const TArray<UObject*>& InObjects) const
{
	for (UObject* Object : InObjects)
	{
		if (Object->IsA<UTsuBlueprint>())
			return true;
	}

	return false;
}

void FAssetTypeActions_TsuBlueprint::GetActions(
	const TArray<UObject*>& InObjects,
	FMenuBuilder& MenuBuilder)
{
	GetBlueprintActions().GetActions(InObjects, MenuBuilder);

	TArray<TWeakObjectPtr<UTsuBlueprint>> Blueprints = TsuAssetTypeActions_Private::GetTypedWeakObjectPtrs<UTsuBlueprint>(InObjects);

	const FUICommandInfo& OpenInTextEditorAction = FTsuEditorCommands::Get().GetActionOpenInTextEditor();

	MenuBuilder.AddMenuEntry(
		OpenInTextEditorAction.GetLabel(),
		OpenInTextEditorAction.GetDescription(),
		OpenInTextEditorAction.GetIcon(),
		FUIAction(
			FExecuteAction::CreateStatic(
				&TsuAssetTypeActions_Private::ExecuteOpenInTextEditor,
				Blueprints),
			FCanExecuteAction()));
}

uint32 FAssetTypeActions_TsuBlueprint::GetCategories()
{
	return GetBlueprintActions().GetCategories();
}

bool FAssetTypeActions_TsuBlueprint::CanFilter()
{
	return GetBlueprintActions().CanFilter();
}

bool FAssetTypeActions_TsuBlueprint::IsImportedAsset() const
{
	return true;
}

void FAssetTypeActions_TsuBlueprint::GetResolvedSourceFilePaths(
	const TArray<UObject*>& TypeAssets,
	TArray<FString>& OutSourceFilePaths) const
{
	for (UObject* Asset : TypeAssets)
	{
		auto Blueprint = CastChecked<UTsuBlueprint>(Asset);
		Blueprint->AssetImportData->ExtractFilenames(OutSourceFilePaths);
	}
}

void FAssetTypeActions_TsuBlueprint::OpenAssetEditor(
	const TArray<UObject*>& InObjects,
	TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	return GetBlueprintActions().OpenAssetEditor(InObjects, EditWithinLevelEditor);
}

void FAssetTypeActions_TsuBlueprint::AssetsActivated(
	const TArray<UObject*>& InObjects,
	EAssetTypeActivationMethod::Type ActivationType)
{
	return GetBlueprintActions().AssetsActivated(InObjects, ActivationType);
}

bool FAssetTypeActions_TsuBlueprint::CanLocalize() const
{
	return GetBlueprintActions().CanLocalize();
}

bool FAssetTypeActions_TsuBlueprint::CanMerge() const
{
	return GetBlueprintActions().CanMerge();
}

void FAssetTypeActions_TsuBlueprint::Merge(UObject* InObject)
{
	GetBlueprintActions().Merge(InObject);
}

void FAssetTypeActions_TsuBlueprint::Merge(
	UObject* BaseAsset,
	UObject* RemoteAsset,
	UObject* LocalAsset,
	const FOnMergeResolved& ResolutionCallback)
{
	GetBlueprintActions().Merge(BaseAsset, RemoteAsset, LocalAsset, ResolutionCallback);
}

bool FAssetTypeActions_TsuBlueprint::ShouldForceWorldCentric()
{
	return GetBlueprintActions().ShouldForceWorldCentric();
}

void FAssetTypeActions_TsuBlueprint::PerformAssetDiff(
	UObject* OldAsset,
	UObject* NewAsset,
	const struct FRevisionInfo& OldRevision,
	const struct FRevisionInfo& NewRevision) const
{
	return GetBlueprintActions().PerformAssetDiff(OldAsset, NewAsset, OldRevision, NewRevision);
}

class UThumbnailInfo* FAssetTypeActions_TsuBlueprint::GetThumbnailInfo(UObject* Asset) const
{
	return GetBlueprintActions().GetThumbnailInfo(Asset);
}

EThumbnailPrimType FAssetTypeActions_TsuBlueprint::GetDefaultThumbnailPrimitiveType(UObject* Asset) const
{
	return GetBlueprintActions().GetDefaultThumbnailPrimitiveType(Asset);
}

TSharedPtr<class SWidget> FAssetTypeActions_TsuBlueprint::GetThumbnailOverlay(const struct FAssetData& AssetData) const
{
	return GetBlueprintActions().GetThumbnailOverlay(AssetData);
}

FText FAssetTypeActions_TsuBlueprint::GetAssetDescription(const struct FAssetData& AssetData) const
{
	return GetBlueprintActions().GetAssetDescription(AssetData);
}

void FAssetTypeActions_TsuBlueprint::BuildBackendFilter(struct FARFilter& InFilter)
{
	return GetBlueprintActions().BuildBackendFilter(InFilter);
}
