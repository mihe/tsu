#include "TsuPaths.h"

#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"

FString FTsuPaths::PluginDir()
{
	static const FString Result = []
	{
		TSharedPtr<IPlugin> ThisPlugin = IPluginManager::Get().FindPlugin(TEXT("TSU"));
		verify(ThisPlugin.IsValid());
		return ThisPlugin->GetBaseDir();
	}();

	return Result;
}

FString FTsuPaths::BinariesDir()
{
	return FPaths::Combine(
		PluginDir(),
		TEXT("Binaries"),
#if PLATFORM_WINDOWS
		TEXT("Win64/")
#else
#error Not implemented
#endif
	);
}

FString FTsuPaths::ContentDir()
{
	return FPaths::Combine(PluginDir(), TEXT("Content/"));
}

FString FTsuPaths::SourceDir()
{
	return FPaths::Combine(PluginDir(), TEXT("Source/"));
}

FString FTsuPaths::ScriptsDir()
{
	return FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Scripts/"));
}

FString FTsuPaths::ScriptsSourceDir()
{
	return FPaths::Combine(ScriptsDir(), TEXT("Source/"));
}

FString FTsuPaths::ParserPath()
{
	return FPaths::Combine(BinariesDir(), TEXT("TsuParser.exe"));
}

FString FTsuPaths::BootstrapPath()
{
	return FPaths::Combine(SourceDir(), TEXT("TsuBootstrap"), TEXT("dist"));
}

FString FTsuPaths::TypingsDir()
{
	return FPaths::Combine(FPaths::ProjectIntermediateDir(), TEXT("Typings/"));
}

FString FTsuPaths::TypingPath(const TCHAR* TypeName)
{
	return FPaths::Combine(
		FPaths::ProjectIntermediateDir(),
		TEXT("Typings"),
		TypeName,
		TEXT("index.d.ts"));
}
