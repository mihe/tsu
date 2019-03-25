#include "TsuDelegateEvent.h"

#include "TsuContext.h"
#include "TsuIsolate.h"

void UTsuDelegateEvent::Initialize(v8::Local<v8::Function> InCallback, UFunction* InSignature)
{
	WorldContext.Reset(FTsuIsolate::Get(), FTsuContext::Get().GetWorldContext());
	Callback.Reset(FTsuIsolate::Get(), InCallback);
	Signature = InSignature;
}

void UTsuDelegateEvent::ProcessEvent(UFunction* /*Function*/, void* Parameters)
{
	if (FTsuContext::Exists())
	{
		v8::Isolate* Isolate = FTsuIsolate::Get();
		v8::HandleScope HandleScope{Isolate};

		FTsuContext::Get().InvokeDelegateEvent(
			WorldContext.Get(Isolate),
			Callback.Get(Isolate),
			Signature,
			Parameters);
	}
}
