#pragma once

#include "CoreMinimal.h"

#include "TsuV8Wrapper.h"

#include "Containers/Ticker.h"

class FTsuHeapStats
	: public FTickerObjectBase
{
public:
	FTsuHeapStats(v8::Isolate* InIsolate);

	bool Tick(float DeltaTime) override;

private:
	v8::Isolate* Isolate = nullptr;
};
