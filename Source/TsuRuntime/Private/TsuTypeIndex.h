#pragma once

#include "CoreMinimal.h"

#include "Engine/Blueprint.h"

class FTsuTypeIndex
{
public:
	FTsuTypeIndex();

	UField* Find(const FString& TypeName);

private:
	TMap<FString, UField*> Index;
};
