#pragma once

#include "CoreMinimal.h"

#include "TsuContext.h"

struct FTsuWorldScope
{
	FTsuWorldScope(FTsuContext& Context, UObject* WorldContext)
		: Context(Context)
	{
		bShouldPop = Context.PushWorldContext(WorldContext);
	}

	FTsuWorldScope(FTsuContext& Context, v8::Local<v8::Value> WorldContext)
		: Context(Context)
	{
		bShouldPop = Context.PushWorldContext(WorldContext);
	}

	~FTsuWorldScope()
	{
		if (bShouldPop)
			Context.PopWorldContext();
	}

	FTsuWorldScope(const FTsuWorldScope& Other) = delete;
	FTsuWorldScope& operator=(const FTsuWorldScope& Other) = delete;

	FTsuContext& Context;
	bool bShouldPop = false;
};
