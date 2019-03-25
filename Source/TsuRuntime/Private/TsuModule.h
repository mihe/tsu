#pragma once

#include "CoreMinimal.h"

#include "UObject/Script.h"

class FTsuModule
{
public:
	FTsuModule(const TCHAR* Binding);

	void Unload() const;
	void Invoke(FFrame& Stack, RESULT_DECL) const;

private:
	FString Binding;
};
