using System;
using System.IO;
using UnrealBuildTool;

public class TsuAsio : ModuleRules
{
	public TsuAsio(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		string Platform = Target.Platform.ToString();
		string ModulePath = UnrealBuildTool.RulesCompiler.GetFileNameFromType(GetType());
		string ModuleBaseDirectory = Path.GetDirectoryName(ModulePath);
		string IncludeDirectory = Path.Combine(ModuleBaseDirectory, "include");

		PublicIncludePaths.Add(IncludeDirectory);
		PublicDefinitions.AddRange(
			new string[]
			{
				"_WEBSOCKETPP_CPP11_STL_",
				"ASIO_STANDALONE"
			});
	}
}
