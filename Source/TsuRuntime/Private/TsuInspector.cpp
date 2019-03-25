#include "TsuInspector.h"

#include "TsuInspectorClient.h"
#include "TsuIsolate.h"

TOptional<FTsuInspectorClient> TsuInspectorClient;

FTsuInspector::FTsuInspector(v8::Platform* Platform, v8::Local<v8::Context> Context)
{
	v8::Isolate* Isolate = FTsuIsolate::Get();
	GlobalContext.Reset(Isolate, Context);

	if (!TsuInspectorClient)
		TsuInspectorClient.Emplace(Platform, Isolate);

	TsuInspectorClient->RegisterContext(Context);
}

FTsuInspector::~FTsuInspector()
{
	v8::Isolate* Isolate = FTsuIsolate::Get();
	v8::HandleScope HandleScope{Isolate};
	TsuInspectorClient->UnregisterContext(GlobalContext.Get(Isolate));
}
