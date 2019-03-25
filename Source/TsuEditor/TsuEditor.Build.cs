using UnrealBuildTool;

public class TsuEditor : ModuleRules
{
	public TsuEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivatePCHHeaderFile = "Private/TsuEditorPCH.h";
		bEnforceIWYU = true;

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"InputCore",
				"SlateCore",
				"Slate",
				"UnrealEd",
				"Settings",
				"AssetTools",
				"KismetCompiler",
				"Kismet",
				"BlueprintGraph",
				"Json",
				"JsonUtilities",
				"Projects",
				"SourceControl",
				"TsuUtilities",
				"TsuRuntime"
			});

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine"
			});
	}
}
