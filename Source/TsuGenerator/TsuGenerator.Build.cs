/**
 * #note(#mihe): This module is not currently used and is only meant to serve as a starting point for future development
 */

using UnrealBuildTool;

public class TsuGenerator : ModuleRules
{
	public TsuGenerator(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivatePCHHeaderFile = "Private/TsuGeneratorPCH.h";
		bEnforceIWYU = true;

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"TsuUtilities"
			});

		PublicIncludePaths.AddRange(
			new string[]
			{					
				"Programs/UnrealHeaderTool/Public"
			});

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject"
			});

		PublicDefinitions.Add("HACK_HEADER_GENERATOR=1");
	}
}
