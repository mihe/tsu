#pragma once

#include "CoreMinimal.h"

#include "TsuParsedFile.generated.h"

USTRUCT()
struct TSURUNTIME_API FTsuParsedType
{
	GENERATED_BODY()

	UPROPERTY()
	FString Name;

	UPROPERTY()
	int32 Dimensions;
};

USTRUCT()
struct TSURUNTIME_API FTsuParsedParameter
{
	GENERATED_BODY()

	UPROPERTY()
	FString Name;

	UPROPERTY()
	TArray<FTsuParsedType> Types;
};

USTRUCT()
struct TSURUNTIME_API FTsuParsedFunction
{
	GENERATED_BODY()

	UPROPERTY()
	FString Name;

	UPROPERTY()
	TArray<FTsuParsedParameter> Parameters;

	UPROPERTY()
	TArray<FTsuParsedType> ReturnTypes;

	UPROPERTY()
	int32 Line;

	UPROPERTY()
	int32 Character;
};

USTRUCT()
struct TSURUNTIME_API FTsuParsedFile
{
	GENERATED_BODY()

	UPROPERTY()
	FString Filename;

	UPROPERTY()
	FString Path;

	UPROPERTY()
	FString Source;

	UPROPERTY()
	TArray<FString> Errors;

	UPROPERTY()
	TArray<FTsuParsedFunction> Exports;

	UPROPERTY()
	TArray<FString> Dependencies;

	bool IsValid() const
	{
		return (Source.Len() > 0) && (Exports.Num() > 0);
	}
};
