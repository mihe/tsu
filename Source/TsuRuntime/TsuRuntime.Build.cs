using UnrealBuildTool;

public class TsuRuntime : ModuleRules
{
	public TsuRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivatePCHHeaderFile = "Private/TsuRuntimePCH.h";
		bEnforceIWYU = true;

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"InputCore",
				"TsuUtilities",
				"TsuWebSocketPP",
				"TsuAsio"
			});

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"TsuV8"
			});

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"UnrealEd",
					"SlateCore",
					"Slate",
					"KismetCompiler",
					"Settings"
				});
		}
	}
}
