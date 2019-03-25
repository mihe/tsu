#pragma once

#include "CoreMinimal.h"

struct TSUUTILITIES_API FTsuPaths
{
	static FString PluginDir();
	static FString BinariesDir();
	static FString ContentDir();
	static FString SourceDir();
	static FString ScriptsDir();
	static FString ScriptsSourceDir();
	static FString ParserPath();
	static FString RequirePath();
	static FString TypingsDir();
	static FString TypingPath(const TCHAR* TypeName);
};
