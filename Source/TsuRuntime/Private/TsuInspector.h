#pragma once

#include "CoreMinimal.h"

#include "TsuV8Wrapper.h"

class FTsuInspector
{
public:
	FTsuInspector(v8::Platform* Platform, v8::Local<v8::Context> Context);
	~FTsuInspector();

	FTsuInspector(const FTsuInspector& Other) = delete;
	FTsuInspector& operator=(const FTsuInspector& Other) = delete;

private:
	v8::Global<v8::Context> GlobalContext;
};
