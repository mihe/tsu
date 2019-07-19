#include "TsuTypeIndex.h"

#include "TsuReflection.h"
#include "TsuRuntimeLog.h"
#include "TsuTypings.h"

#include "Editor.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Misc/PackageName.h"
#include "UObject/UObjectGlobals.h"

FTsuTypeIndex::FTsuTypeIndex()
{
	Index.Reserve(1024);

	FTsuReflection::VisitAllTypes([&](UField* Type, const FTsuTypeSet& /*References*/)
	{
		Index.Emplace(FTsuTypings::TailorNameOfType(Type), Type);
	});

#if WITH_EDITOR
	if (GEditor)
	{
		GEditor->OnBlueprintPreCompile().AddLambda([this](UBlueprint* Blueprint)
		{
			if (auto GeneratedClass = Cast<UBlueprintGeneratedClass>(Blueprint->GeneratedClass))
				Index.FindOrAdd(FTsuTypings::TailorNameOfType(GeneratedClass)) = GeneratedClass;
		});
	}
#endif // WITH_EDITOR
}

UField* FTsuTypeIndex::Find(const FString& TypeName)
{
	UField* Result = Index.FindRef(TypeName);

	// If we can't find the type, we assume that it's a not-yet-indexed blueprint
	if (!Result)
	{
		const FString ClassName = FString::Printf(TEXT("%s_C"), *TypeName);
		Result = FindObject<UBlueprintGeneratedClass>(ANY_PACKAGE, *ClassName, true);
		if (!Result)
		{
			const FString ShortPackagePath = FString::Printf(TEXT("%s.%s"), *TypeName, *ClassName);
			FString LongPackagePath;
			if (FPackageName::TryConvertShortPackagePathToLongInObjectPath(ShortPackagePath, LongPackagePath))
				Result = LoadObject<UBlueprintGeneratedClass>(nullptr, *LongPackagePath);
		}

		if (Result)
			Index.Add(TypeName) = Result;
	}

	return Result;
}
