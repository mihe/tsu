#pragma once

#include "CoreMinimal.h"

#include "TsuContext.h"

struct FTsuWorldContextScope
{
	FTsuWorldContextScope(FTsuContext& Context, UObject* WorldContext)
		: Context(Context)
	{
		bShouldPop = Context.PushWorldContext(WorldContext);
	}

	FTsuWorldContextScope(FTsuContext& Context, v8::Local<v8::Object> WorldContext)
		: Context(Context)
		, bShouldPop(true)
	{
		Context.PushWorldContext(WorldContext);
	}

	~FTsuWorldContextScope()
	{
		if (bShouldPop)
			Context.PopWorldContext();
	}

	FTsuWorldContextScope(const FTsuWorldContextScope& Other) = delete;
	FTsuWorldContextScope& operator=(const FTsuWorldContextScope& Other) = delete;

	FTsuContext& Context;
	bool bShouldPop = false;
};
