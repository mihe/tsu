#pragma once

#include "CoreMinimal.h"

#include "../Private/TsuContextCallback.h"
#include "../Private/TsuInspector.h"
#include "../Private/TsuModule.h"
#include "../Private/TsuTimer.h"
#include "../Private/TsuV8Wrapper.h"

#include "UObject/GCObject.h"
#include "UObject/Stack.h"
#include "UObject/WeakObjectPtrTemplates.h"

class TSURUNTIME_API FTsuContext final
	: public FGCObject
{
	friend struct TOptional<FTsuContext>;
	friend class FTsuModule;
	friend struct FTsuWorldScope;
	friend class UTsuDelegateEvent;

	using FStructKey = TTuple<void*, UScriptStruct*>;
	using FDelegateKey = TTuple<UObject*, UProperty*>;
	using FDelegateEventMap = TMap<FWeakObjectPtr, TMap<uint64, UTsuDelegateEvent*>>;

	static const FName MetaWorldContext;
	static const FName NameEventExecute;

public:
	~FTsuContext();

	FTsuContext(const FTsuContext& Other) = delete;
	FTsuContext& operator=(const FTsuContext& Other) = delete;

	static void Create();
	static void Destroy();
	static bool Exists();
	static FTsuContext& Get();

	v8::MaybeLocal<v8::Value> EvalModule(const TCHAR* Code, const TCHAR* Path);
	bool BindModule(const TCHAR* Binding, const TCHAR* Code, const TCHAR* Path);
	TWeakPtr<FTsuModule> ClaimModule(const TCHAR* Binding, const TCHAR* Code, const TCHAR* Path);

private:
	FTsuContext();

	void UnloadModule(const TCHAR* Binding);

	v8::MaybeLocal<v8::Function> GetExportedFunction(const TCHAR* Binding, const TCHAR* Name);

	bool PushWorldContext(UObject* WorldContext);
	bool PushWorldContext(v8::Local<v8::Value> WorldContext);
	void PopWorldContext();
	v8::Local<v8::Value> GetWorldContext();

	void InitializeBuiltins();
	void InitializeDelegates();
	void InitializeRequire();
	void InitializeArrayProxy();
	void InitializeStructProxy();

	v8::Local<v8::Function> ExposeObject(UStruct* Type);

	v8::Local<v8::FunctionTemplate> AddTemplate(UStruct* Type);
	v8::Local<v8::FunctionTemplate> FindOrAddTemplate(UStruct* Type);

	v8::Local<v8::Value> ReferenceStructObject(void* StructObject, UScriptStruct* StructType);
	v8::Local<v8::Value> ReferenceClassObject(UObject* ClassObject);
	v8::Local<v8::Value> ReferenceDelegate(UProperty* DelegateProperty, UObject* Parent);

	void Invoke(const TCHAR* Namespace, FFrame& Stack, RESULT_DECL);

	bool InvokeDelegateEvent(
		v8::Local<v8::Value> WorldContext,
		v8::Local<v8::Function> Callback,
		UFunction* Signature = nullptr,
		void* ParamsBuffer = nullptr);

	void AddReferencedObjects(FReferenceCollector& Collector) override;
	void OnPreGarbageCollect();
	void OnPostGarbageCollect();

	v8::Local<v8::Value> StartTimeout(v8::Local<v8::Function> Callback, float Delay, bool bLoop);

	TSU_FUNCTION_CALLBACK(OnConsoleLog);
	TSU_FUNCTION_CALLBACK(OnConsoleDisplay);
	TSU_FUNCTION_CALLBACK(OnConsoleError);
	TSU_FUNCTION_CALLBACK(OnConsoleWarning);
	TSU_FUNCTION_CALLBACK_IMPL(OnClassNew);
	TSU_FUNCTION_CALLBACK_IMPL(OnStructNew);
	TSU_FUNCTION_CALLBACK_IMPL(OnCallMethod);
	TSU_FUNCTION_CALLBACK_IMPL(OnCallStaticMethod);
	TSU_FUNCTION_CALLBACK_IMPL(OnCallExtensionMethod);
	TSU_FUNCTION_CALLBACK_IMPL(OnConsoleTimeBegin);
	TSU_FUNCTION_CALLBACK_IMPL(OnConsoleTimeEnd);
	TSU_FUNCTION_CALLBACK_IMPL(OnPropertyGet);
	TSU_FUNCTION_CALLBACK_IMPL(OnPropertySet);
	TSU_FUNCTION_CALLBACK_IMPL(OnSetTimeout);
	TSU_FUNCTION_CALLBACK_IMPL(OnSetInterval);
	TSU_FUNCTION_CALLBACK_IMPL(OnClearTimeout);
	TSU_FUNCTION_CALLBACK(OnPathJoin);
	TSU_FUNCTION_CALLBACK(OnPathResolve);
	TSU_FUNCTION_CALLBACK(OnPathDirName);
	TSU_FUNCTION_CALLBACK(OnPathParse);
	TSU_FUNCTION_CALLBACK(OnFileRead);
	TSU_FUNCTION_CALLBACK(OnFileExists);
	TSU_FUNCTION_CALLBACK_IMPL(OnRequire);
	TSU_FUNCTION_CALLBACK_IMPL(OnImport);
	TSU_FUNCTION_CALLBACK_IMPL(OnGetProperty);
	TSU_FUNCTION_CALLBACK_IMPL(OnSetProperty);
	TSU_FUNCTION_CALLBACK_IMPL(OnGetStaticClass);
	TSU_FUNCTION_CALLBACK_IMPL(OnDelegateBind);
	TSU_FUNCTION_CALLBACK_IMPL(OnDelegateUnbind);
	TSU_FUNCTION_CALLBACK_IMPL(OnDelegateExecute);
	TSU_FUNCTION_CALLBACK_IMPL(OnDelegateIsBound);
	TSU_FUNCTION_CALLBACK_IMPL(OnMulticastDelegateAdd);
	TSU_FUNCTION_CALLBACK_IMPL(OnMulticastDelegateRemove);
	TSU_FUNCTION_CALLBACK_IMPL(OnMulticastDelegateBroadcast);
	TSU_FUNCTION_CALLBACK_IMPL(OnMulticastDelegateIsBound);
	TSU_GET_INDEX_CALLBACK_IMPL(OnArrayGetIndex);
	TSU_SET_INDEX_CALLBACK_IMPL(OnArraySetIndex);
	TSU_FUNCTION_CALLBACK_IMPL(OnArrayGetLength);
	TSU_FUNCTION_CALLBACK_IMPL(OnArraySetLength);

	void CallMethod(UObject* Object, UFunction* Method, void* ParamsBuffer, v8::ReturnValue<v8::Value> ReturnValue);
	void GetMethodParams(const v8::FunctionCallbackInfo<v8::Value>& Info, UFunction* Method, void* ParamsBuffer);
	void GetExtensionMethodParams(const v8::FunctionCallbackInfo<v8::Value>& Info, UFunction* Method, void* ParamsBuffer);
	v8::Local<v8::Value> UnwrapStructProxy(const v8::Local<v8::Value>& Info);

	void PopArgumentsFromStack(FFrame& Stack, UFunction* Function, TArray<v8::Local<v8::Value>>& OutArguments);
	void PopArgumentFromStack(FFrame& Stack, UProperty* Argument, TArray<v8::Local<v8::Value>>& OutArguments);
	bool WritePropertyToContainer(UProperty* Property, v8::Local<v8::Value> Value, void* Dest);
	bool WritePropertyToBuffer(UProperty* Property, v8::Local<v8::Value> Value, void* Dest);
	v8::Local<v8::Value> ReadPropertyFromContainer(UProperty* Property, const void* Source);
	v8::Local<v8::Value> ReadPropertyFromBuffer(UProperty* Property, const void* Source);

	static TOptional<FTsuContext> Singleton;
	static v8::Isolate* Isolate;

	v8::Global<v8::Context> GlobalContext;
	static v8::Global<v8::FunctionTemplate> GlobalDelegateTemplate;
	static v8::Global<v8::FunctionTemplate> GlobalMulticastDelegateTemplate;
	static v8::Global<v8::Function> GlobalArrayHandlerConstructor;
	static v8::Global<v8::Function> GlobalArrayConstructor;
	static v8::Global<v8::Function> GlobalStructHandlerConstructor;
	TMap<FString, TSharedPtr<FTsuModule>> LoadedModules;
	TArray<v8::Global<v8::Value>> WorldContexts;
	uint64 NextDelegateHandle = 1;
	FDelegateEventMap DelegateEvents;
	TMap<FString, uint64> PendingTimeLogs;
	TMap<UStruct*, v8::Global<v8::FunctionTemplate>> Templates;
	TMap<FStructKey, v8::Global<v8::Object>> AliveStructs;
	TMap<UObject*, v8::Global<v8::Object>> AliveObjects;
	TMap<FDelegateKey, v8::Global<v8::Object>> AliveDelegates;
	TArray<FTsuTimer> AliveTimers;
	TOptional<FTsuInspector> Inspector;
};
