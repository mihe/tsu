#include "TsuBlueprint.h"

#include "TsuBlueprintGeneratedClass.h"

#if WITH_EDITORONLY_DATA
#include "EditorFramework/AssetImportData.h"
#endif // WITH_EDITORONLY_DATA

#if WITH_EDITOR
#include "HAL/FileManager.h"
#endif

UTsuBlueprint::UTsuBlueprint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
	AssetImportData = CreateEditorOnlyDefaultSubobject<UAssetImportData>(TEXT("AssetImportData"));
#endif // WITH_EDITORONLY_DATA
}

#if WITH_EDITOR

UClass* UTsuBlueprint::GetBlueprintClass() const
{
	return UTsuBlueprintGeneratedClass::StaticClass();
}

bool UTsuBlueprint::ValidateGeneratedClass(const UClass* InClass)
{
	const bool bResult = Super::ValidateGeneratedClass(InClass);

	auto GeneratedTsuClass = Cast<UTsuBlueprintGeneratedClass>(InClass);
	if (!ensure(GeneratedTsuClass))
		return false;

	auto Blueprint = Cast<UTsuBlueprint>(GetBlueprintFromClass(GeneratedTsuClass));
	if (!ensure(Blueprint))
		return false;

	return bResult;
}

bool UTsuBlueprint::SupportedByDefaultBlueprintFactory() const
{
	return false;
}

bool UTsuBlueprint::AlwaysCompileOnLoad() const
{
	return true;
}

bool UTsuBlueprint::IsCodeDirty() const
{
	if (!AssetImportData)
		return true;

	const TArray<FAssetImportInfo::FSourceFile>& SourceFiles = AssetImportData->SourceData.SourceFiles;
	if (ensure(SourceFiles.Num() == 1))
	{
		// Check the timestamp of the file as it is now, and the last imported timestamp
		if (IFileManager::Get().GetTimeStamp(*AssetImportData->GetFirstFilename()) <= SourceFiles[0].Timestamp)
			return false;
	}

	return true;
}

#endif // WITH_EDITOR

#if WITH_EDITORONLY_DATA

void UTsuBlueprint::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
{
	if (AssetImportData)
	{
		OutTags.Add(
			FAssetRegistryTag(
				SourceFileTagName(),
				AssetImportData->GetSourceData().ToJson(),
				FAssetRegistryTag::TT_Hidden));
	}

	Super::GetAssetRegistryTags(OutTags);
}

void UTsuBlueprint::GatherDependencies(TSet<TWeakObjectPtr<UBlueprint>>& Dependencies) const
{
	// #todo(#mihe): Importing new files will have a nullptr GeneratedClass
	if (auto TsuClass = Cast<UTsuBlueprintGeneratedClass>(GeneratedClass))
		TsuClass->GatherDependencies(Dependencies);
}

#endif // WITH_EDITORONLY_DATA
