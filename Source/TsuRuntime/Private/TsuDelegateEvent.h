#pragma once

#include "CoreMinimal.h"

#include "TsuV8Wrapper.h"

#include "TsuDelegateEvent.generated.h"

UCLASS(ClassGroup=TSU)
class UTsuDelegateEvent final
	: public UObject
{
	GENERATED_BODY()

public:
	void Initialize(v8::Local<v8::Function> Callback, UFunction* Signature = nullptr);

	UFUNCTION()
	void Execute() {}

	void ProcessEvent(UFunction* Function, void* Parameters) override;

	v8::Global<v8::Value> WorldContext;
	v8::Global<v8::Function> Callback;
	UFunction* Signature = nullptr;
};
