#pragma once

#include "CoreMinimal.h"

#include "Factories/Factory.h"

#include "TsuFactory.generated.h"

class UTsuBlueprint;

UCLASS(ClassGroup=TSU)
class TSUEDITOR_API UTsuFactory
	: public UFactory
{
	GENERATED_BODY()

public:
	UTsuFactory(const FObjectInitializer& ObjectInitializer);

	UObject* FactoryCreateText(
		UClass* InClass,
		UObject* Parent,
		FName Name,
		EObjectFlags Flags,
		UObject* Context,
		const TCHAR* Type,
		const TCHAR*& Buffer,
		const TCHAR* BufferEnd,
		FFeedbackContext* Warn) override final;

private:
	UTsuBlueprint* CreateBlueprint(
		UClass* Class,
		UObject* Parent,
		FName Name,
		const TCHAR* Type);

	UTsuBlueprint* UpdateBlueprint(
		UTsuBlueprint* Blueprint,
		UClass* Class,
		UObject* Parent,
		FName Name,
		const TCHAR* Type);
};
