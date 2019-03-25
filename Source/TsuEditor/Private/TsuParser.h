#pragma once

#include "CoreMinimal.h"

#include "TsuParsedFile.h"

#include "TsuParser.generated.h"

USTRUCT()
struct TSUEDITOR_API FTsuParserRequest
{
	GENERATED_BODY()

	UPROPERTY()
	FString File;
};

struct TSUEDITOR_API FTsuParser
{
	static bool Parse(const FString& FilePath, FTsuParsedFile& Response);
};
