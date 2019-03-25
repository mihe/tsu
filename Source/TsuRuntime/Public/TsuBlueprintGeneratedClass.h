#pragma once

#include "CoreMinimal.h"

#include "../Private/TsuModule.h"
#include "TsuParsedFile.h"

#include "Engine/BlueprintGeneratedClass.h"

#include "TsuBlueprintGeneratedClass.generated.h"

UCLASS(ClassGroup = TSU)
class TSURUNTIME_API UTsuBlueprintGeneratedClass final
	: public UBlueprintGeneratedClass
{
	GENERATED_BODY()

	static const FName MetaDisplayName;
	static const FName MetaCategory;
	static const FName MetaKeywords;
	static const FName MetaTooltip;
	static const FName MetaDefaultToSelf;
	static const FName MetaAdvancedDisplay;

public:
	void FinishDestroy() override;
	void Bind() override;

	void ReloadModule();

#if WITH_EDITOR
	void GatherDependencies(TSet<TWeakObjectPtr<class UBlueprint>>& Dependencies) const;
#endif // WITH_EDITOR

	UPROPERTY()
	FTsuParsedFile Exports;

private:
	void RemoveNativeFunction(FName FunctionName);
	void BindFunction(const FTsuParsedFunction& Export);

	TSharedPtr<FTsuModule> PinModule();
	void LoadModule();
	void UnloadModule();

	static void ExecInvoke(UObject* ExecContext, FFrame& ExecStack, RESULT_DECL);

	UProperty* NewParameterFromType(
		UObject* Outer,
		FName ParameterName,
		const FString& TypeName,
		int32 NumTypeDimensions);

	void BindDelegates();

	FString TailoredName;
	TWeakPtr<FTsuModule> Module;
};
