#include "TsuFactoryReimport.h"

#include "TsuBlueprint.h"
#include "TsuBlueprintGeneratedClass.h"
#include "TsuEditorLog.h"

#include "EditorFramework/AssetImportData.h"
#include "HAL/FileManager.h"
#include "UObject/UObjectIterator.h"

bool UTsuFactoryReimport::CanReimport(UObject* Object, TArray<FString>& OutFilenames)
{
	if (auto Blueprint = Cast<UTsuBlueprint>(Object))
	{
		Blueprint->AssetImportData->ExtractFilenames(OutFilenames);
		return true;
	}

	return false;
}

void UTsuFactoryReimport::SetReimportPaths(UObject* Object, const TArray<FString>& NewReimportPaths)
{
	auto Blueprint = Cast<UTsuBlueprint>(Object);
	if (Blueprint && ensure(NewReimportPaths.Num() == 1))
		Blueprint->AssetImportData->UpdateFilenameOnly(NewReimportPaths[0]);
}

EReimportResult::Type UTsuFactoryReimport::Reimport(UObject* Object)
{
	auto Blueprint = Cast<UTsuBlueprint>(Object);
	if (!Blueprint)
		return EReimportResult::Failed;

	const FString Filename = Blueprint->AssetImportData->GetFirstFilename();
	if (Filename.IsEmpty())
		return EReimportResult::Failed;

	UE_LOG(LogTsuEditor, Log, TEXT("Reimporting '%s'..."), *Filename);

	if (!IFileManager::Get().FileExists(*Filename))
	{
		UE_LOG(LogTsuEditor, Warning, TEXT("Reimport failed: Source file cannot be found."));
		return EReimportResult::Failed;
	}

	bool bImportCancelled = false;

	if (
		ImportObject(
			Blueprint->GetClass(),
			Blueprint->GetOuter(),
			*Blueprint->GetName(),
			RF_Public | RF_Standalone,
			Filename,
			nullptr,
			bImportCancelled) != nullptr)
	{
		UE_LOG(LogTsuEditor, Log, TEXT("Reimport succeeded"));
		return EReimportResult::Succeeded;
	}
	else if (bImportCancelled)
	{
		UE_LOG(LogTsuEditor, Warning, TEXT("Reimport canceled"));
		return EReimportResult::Failed;
	}
	else
	{
		UE_LOG(LogTsuEditor, Warning, TEXT("Reimport failed"));
		return EReimportResult::Failed;
	}
}
