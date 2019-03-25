#pragma once

#include "CoreMinimal.h"

class FTsuCodeGenerator
{
public:
	FTsuCodeGenerator(const FString& RootLocalPath, const FString& RootBuildPath, const FString& OutputDirectory, const FString& InIncludeBase);

	/** ... */
	void ExportClass(UClass* Class, const FString& SourceHeaderFilename, const FString& GeneratedHeaderFilename, bool bHasChanged);

	/** ... */
	void FinishExport();

private:
	/** Exports a wrapper function */
	FString ExportFunction(const FString& ClassNameCPP, UClass* Class, UFunction* Function);

	/** Exports a wrapper functions for properties */
	FString ExportProperty(const FString& ClassNameCPP, UClass* Class, UProperty* Property);

	/** @see FScriptCodeGeneratorBase::CanExportClass */
	bool CanExportClass(UClass* Class) const;

	/** Returns true if the specified function can be exported */
	static bool CanExportFunction(UClass* Class, UFunction* Function);

	/** Returns true if the specified property can be exported */
	static bool CanExportProperty(UClass* Class, UProperty* Property);

	/** Saves generated script glue heade to a temporary file if its contents is different from the eexisting one. */
	bool SaveHeaderIfChanged(const FString& HeaderPath, const FString& NewHeaderContents);

	/** Renames/replaces all existing script glue files with the temporary (new) ones */
	void RenameTempFiles();

	/** Re-bases the local path to build path */
	FString RebaseToBuildPath(const FString& FileName) const;

	/** Converts a UClass name to C++ class name (with U/A prefix) */
	FString GetClassNameCPP(UClass* Class) const;

	/** Generates a script glue header filename for the specified class */
	FString GetScriptHeaderForClass(UClass* Class);

	/** Creats a 'glue' file that merges all generated script files */
	void GlueAllGeneratedFiles();

	/** All generated script header filenames */
	TArray<FString> AllScriptHeaders;

	/** Source header filenames for all exported classes */
	TArray<FString> AllSourceClassHeaders;

	/** Path where generated script glue goes **/
	FString GeneratedCodePath;

	/** Local root path **/
	FString RootLocalPath;

	/** Build root path - may be different to RootBuildPath if we're building remotely. **/
	FString RootBuildPath;

	/** Base include directory */
	FString IncludeBase;

	/** Set of all exported class names */
	TSet<FName> ExportedClasses;

	/** List of temporary header files crated by SaveHeaderIfChanged */
	TArray<FString> TempHeaders;
};
