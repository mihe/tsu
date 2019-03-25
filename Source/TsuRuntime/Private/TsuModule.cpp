#include "TsuModule.h"

#include "TsuContext.h"

FTsuModule::FTsuModule(const TCHAR* InBinding)
	: Binding(InBinding)
{
}

void FTsuModule::Unload() const
{
	FTsuContext::Get().UnloadModule(*Binding);
}

void FTsuModule::Invoke(FFrame& Stack, RESULT_DECL) const
{
	FTsuContext::Get().Invoke(*Binding, Stack, RESULT_PARAM);
}
