using System;
using System.IO;
using UnrealBuildTool;

public class TsuWebSocketPP : ModuleRules
{
	public TsuWebSocketPP(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		string Platform = Target.Platform.ToString();
		string ModulePath = UnrealBuildTool.RulesCompiler.GetFileNameFromType(GetType());
		string ModuleBaseDirectory = Path.GetDirectoryName(ModulePath);
		string IncludeDirectory = Path.Combine(ModuleBaseDirectory, "include");

		PublicIncludePaths.Add(IncludeDirectory);
	}
}
