#include "TsuTypeIndex.h"

#include "TsuReflection.h"
#include "TsuRuntimeLog.h"
#include "TsuTypings.h"

#include "Editor.h"
#include "Engine/BlueprintGeneratedClass.h"
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
		if (auto Blueprint = FindObject<UBlueprint>(ANY_PACKAGE, *TypeName, true))
		{
			Result = Blueprint->GeneratedClass;
			Index.Add(TypeName) = Result;
		}
	}

	return Result;
}
