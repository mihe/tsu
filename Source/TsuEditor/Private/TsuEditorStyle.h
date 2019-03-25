#pragma once

#include "CoreMinimal.h"

class ISlateStyle;
class FSlateStyleSet;

class FTsuEditorStyle
{
public:
	/**
	 * ...
	 */
	static void Initialize();

	/**
	 * ...
	 */
	static void Shutdown();

	/**
	 * ...
	 */
	static ISlateStyle& Get();

private:
	/**
	 * ...
	 */
	static FString InContent(const FString& RelativePath, const TCHAR* Extension);

	static FSlateStyleSet* StyleSet;
};
