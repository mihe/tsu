using System;
using System.IO;
using UnrealBuildTool;

public class TsuV8 : ModuleRules
{
	public TsuV8(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		string Platform = Target.Platform.ToString();
		string ModulePath = UnrealBuildTool.RulesCompiler.GetFileNameFromType(GetType());
		string ModuleBaseDirectory = Path.GetDirectoryName(ModulePath);
		string IncludeDirectory = Path.Combine(ModuleBaseDirectory, "include");
		string LibraryDirectory = Path.Combine(ModuleBaseDirectory, "lib", Platform);
		string BinariesDirectory = Path.Combine("$(PluginDir)", "Binaries", "ThirdParty", "V8", Platform);

		var DllNames = new string[]
		{
			"v8.dll",
			"v8_libbase.dll",
			"v8_libplatform.dll"
		};

		foreach(var DllName in DllNames)
		{
			PublicDelayLoadDLLs.Add(DllName);

			var LibName = DllName + ".lib";
			PublicAdditionalLibraries.Add(LibName);

			var DllPath = Path.Combine(BinariesDirectory, DllName);
			RuntimeDependencies.Add(DllPath);
		}

		PublicIncludePaths.Add(IncludeDirectory);
		PublicLibraryPaths.Add(LibraryDirectory);

		PublicDefinitions.AddRange(
			new string[]
			{
				"USING_V8_SHARED",
				"V8_DEPRECATION_WARNINGS",
				"V8_IMMINENT_DEPRECATION_WARNINGS"
			});
	}
}
