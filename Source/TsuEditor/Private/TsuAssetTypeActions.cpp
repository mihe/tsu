#include "TsuAssetTypeActions.h"

#include "TsuBlueprint.h"
#include "TsuEditorCommands.h"
#include "TsuEditorStyle.h"

#include "EditorFramework/AssetImportData.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

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

void ExecuteOpenInTextEditor(TArray<TWeakObjectPtr<UTsuBlueprint>> Blueprints)
{
	for (TWeakObjectPtr<UTsuBlueprint>& Blueprint : Blueprints)
		FTsuEditorCommands::OpenGraphInTextEditor(Blueprint.Get());
}

} // namespace TsuAssetTypeActions_Private

FText FAssetTypeActions_TsuBlueprint::GetName() const
{
	return FText::FromString(TEXT("TSU Class"));
}

FColor FAssetTypeActions_TsuBlueprint::GetTypeColor() const
{
	return FColor(63, 126, 255);
}

UClass* FAssetTypeActions_TsuBlueprint::GetSupportedClass() const
{
	return UTsuBlueprint::StaticClass();
}

bool FAssetTypeActions_TsuBlueprint::HasActions(const TArray<UObject*>& Objects) const
{
	for (UObject* Object : Objects)
	{
		if (Object->IsA<UTsuBlueprint>())
			return true;
	}

	return false;
}

void FAssetTypeActions_TsuBlueprint::GetActions(
	const TArray<UObject*>& Objects,
	FMenuBuilder& MenuBuilder)
{
	TArray<TWeakObjectPtr<UTsuBlueprint>> Blueprints =
		TsuAssetTypeActions_Private::GetTypedWeakObjectPtrs<UTsuBlueprint>(Objects);

	MenuBuilder.AddMenuEntry(
		FText::FromString("Open In Text Editor"),
		FText::FromString("Opens the associated TypeScript file in the configured text editor"),
		FSlateIcon(FTsuEditorStyle::Get().GetStyleSetName(), TEXT("TsuEditor.OpenInTextEditor")),
		FUIAction(
			FExecuteAction::CreateStatic(
				&TsuAssetTypeActions_Private::ExecuteOpenInTextEditor,
				Blueprints),
			FCanExecuteAction()));
}

uint32 FAssetTypeActions_TsuBlueprint::GetCategories()
{
	return EAssetTypeCategories::Blueprint;
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
