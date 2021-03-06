#pragma once

#include "CoreMinimal.h"

using FTsuTypeSet = TSet<UField*>;

class TSURUNTIME_API FTsuTypings
{
	static const TCHAR* MetaHidden;
	static const TCHAR* MetaDisplayName;
	static const FName MetaScriptName;
	static const FName MetaScriptMethod;
	static const FName MetaScriptOperator;
	static const FName MetaScriptConstant;

public:
	static void WriteCoreTypings();
	static void WriteGlobalTypings();
	static void WriteKeyTypings();
	static void WriteAllTypings();
	static void WriteTypings(UField* Type);
	static void WriteDependencyTypings(class UTsuBlueprintGeneratedClass* Class);
	static bool DoTypingsExist(UField* Type);
	static bool DoTypingsExist(const TCHAR* TypeName);
	static void WriteEnum(FString& Output, UEnum* Enum);
	static void WriteObject(FString& Output, UStruct* Type);
	static void WriteToolTip(FString& Output, UFunction* Function, bool bSkipFirst, bool bIndent);
	static void WriteToolTip(FString& Output, UField* Field, bool bIndent);
	static void WriteToolTip(FString& Output, FString ToolTip, bool bIndent);
	static void WriteMethod(FString& Output, UFunction* Function);
	static void WriteExtensionMethod(FString& Output, UFunction* Function);
	static void WriteStaticExtensionMethod(FString& Output, UFunction* Function);
	static void WriteParameters(FString& Output, UFunction* Function, bool bSkipFirst = false);
	static void WriteReturns(FString& Output, UFunction* Function, bool bSkipFirst = false);
	static void WriteProperty(FString& Output, UProperty* Property, bool bIsReadOnly);
	static bool SaveTypings(const FString& TypeName, const FString& Output);

	static FString GetPropertyType(UProperty* Property, bool bIsReadOnly = false);
	static FString GetPropertyTypeForSuffix(UProperty* Property);

	static TOptional<FString> GetExplicitScriptName(UField* Field);
	static TOptional<FString> GetExplicitScriptMethodName(UFunction* Function);

	static const FString& TailorNameOfType(UField* Type);
	static const FString& TailorNameOfField(UField* Field);
	static const FString& TailorNameOfExtension(UFunction* Function);

private:
	static FString& ResetPersistentOutputBuffer();
	static bool IsReservedIdentifier(const FString& Word);
	static bool WriteTypings(UField* Type, const FTsuTypeSet& References);
	static FString& Deduplicate(FString& Name);
	static FString& CamelCase(FString& Name);
	static FString CamelCase(FString&& Name);
	static FString& AlphaNumerize(FString& Name);
	static FString& TrimRedundancy(FString& Name, const FString& ParentName);
	static FString& TailorNameOfField(FString& Name);
	static FString TailorNameOfField(FString&& Name);
};
