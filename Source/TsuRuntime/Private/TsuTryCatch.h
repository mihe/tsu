#pragma once

#include "CoreMinimal.h"

#include "TsuV8Wrapper.h"

class TSURUNTIME_API FTsuTryCatch
{
public:
	FTsuTryCatch(v8::Isolate* Isolate);
	~FTsuTryCatch();

	FTsuTryCatch(const FTsuTryCatch& Other) = delete;
	FTsuTryCatch& operator=(const FTsuTryCatch& Other) = delete;

	void Check();

private:
	v8::Isolate* Isolate = nullptr;
	v8::TryCatch Catcher;
};
