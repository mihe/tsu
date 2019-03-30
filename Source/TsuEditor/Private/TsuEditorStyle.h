#pragma once

#include "CoreMinimal.h"

class FTsuEditorStyle
{
public:
	static void Initialize();

	static void Shutdown();

	static class ISlateStyle& Get();

private:
	static FString InContent(const FString& RelativePath, const TCHAR* Extension);

	static class FSlateStyleSet* StyleSet;
};
