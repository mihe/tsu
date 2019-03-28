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
	bool InvokeDelegateEvent(v8::Local<v8::Value> WorldContext, v8::Local<v8::Function> Callback, UFunction* Signature = nullptr, void* ParamsBuffer = nullptr);

	void AddReferencedObjects(FReferenceCollector& Collector) override;

	void OnPreGarbageCollect();
	void OnPostGarbageCollect();

	v8::Local<v8::Value> StartTimeout(v8::Local<v8::Function> Callback,float Delay,bool bLoop);

	TSU_CONTEXT_CALLBACK(OnConsoleLog);
	TSU_CONTEXT_CALLBACK(OnConsoleDisplay);
	TSU_CONTEXT_CALLBACK(OnConsoleError);
	TSU_CONTEXT_CALLBACK(OnConsoleWarning);
	TSU_CONTEXT_CALLBACK(OnClassNew);
	TSU_CONTEXT_CALLBACK(OnStructNew);
	TSU_CONTEXT_CALLBACK(OnCallMethod);
	TSU_CONTEXT_CALLBACK(OnCallStaticMethod);
	TSU_CONTEXT_CALLBACK(OnCallExtensionMethod);
	TSU_CONTEXT_CALLBACK(OnConsoleTimeBegin);
	TSU_CONTEXT_CALLBACK(OnConsoleTimeEnd);
	TSU_CONTEXT_CALLBACK(OnPropertyGet);
	TSU_CONTEXT_CALLBACK(OnPropertySet);
	TSU_CONTEXT_CALLBACK(OnSetTimeout);
	TSU_CONTEXT_CALLBACK(OnSetInterval);
	TSU_CONTEXT_CALLBACK(OnClearTimeout);
	TSU_CONTEXT_CALLBACK(OnPathJoin);
	TSU_CONTEXT_CALLBACK(OnPathResolve);
	TSU_CONTEXT_CALLBACK(OnPathDirName);
	TSU_CONTEXT_CALLBACK(OnFileRead);
	TSU_CONTEXT_CALLBACK(OnFileExists);
	TSU_CONTEXT_CALLBACK(OnRequire);
	TSU_CONTEXT_CALLBACK(OnImport);
	TSU_CONTEXT_CALLBACK(OnGetProperty);
	TSU_CONTEXT_CALLBACK(OnSetProperty);
	TSU_CONTEXT_CALLBACK(OnGetStaticClass);
	TSU_CONTEXT_CALLBACK(OnDelegateBind);
	TSU_CONTEXT_CALLBACK(OnDelegateUnbind);
	TSU_CONTEXT_CALLBACK(OnDelegateExecute);
	TSU_CONTEXT_CALLBACK(OnDelegateIsBound);
	TSU_CONTEXT_CALLBACK(OnMulticastDelegateAdd);
	TSU_CONTEXT_CALLBACK(OnMulticastDelegateRemove);
	TSU_CONTEXT_CALLBACK(OnMulticastDelegateBroadcast);
	TSU_CONTEXT_CALLBACK(OnMulticastDelegateIsBound);
	TSU_CONTEXT_CALLBACK(OnGetArrayElement);
	TSU_CONTEXT_CALLBACK(OnSetArrayElement);
	TSU_CONTEXT_CALLBACK(OnGetArrayLength);
	TSU_CONTEXT_CALLBACK(OnSetArrayLength);

	void CallMethod(UObject* Object, UFunction* Method, void* ParamsBuffer, v8::ReturnValue<v8::Value> ReturnValue);
	void WriteParameters(const v8::FunctionCallbackInfo<v8::Value>& Info, UFunction* Method, void* ParamsBuffer);
	void WriteExtensionParameters(const v8::FunctionCallbackInfo<v8::Value>& Info, UFunction* Method, void* ParamsBuffer);
	void PopArgumentsFromStack(FFrame& Stack, UFunction* Function, TArray<v8::Local<v8::Value>>& OutArguments);
	void PopArgumentFromStack(FFrame& Stack, UProperty* Argument, TArray<v8::Local<v8::Value>>& OutArguments);
	bool WritePropertyToContainer(UProperty* Property, v8::Local<v8::Value> Value, void* Dest);
	bool WritePropertyToBuffer(UProperty* Property, v8::Local<v8::Value> Value, void* Dest);
	v8::Local<v8::Value> ReadPropertyFromContainer(UProperty* Property, const void* Source);
	v8::Local<v8::Value> ReadPropertyFromBuffer(UProperty* Property, const void* Source);

	template<typename T>
	bool GetExternalValue(v8::Local<v8::Value> Value, T** OutData);

	template<typename T1, typename T2 = void>
	bool GetInternalFields(v8::Local<v8::Value> Value, T1** Field1, T2** Field2 = nullptr);

	TArray<v8::Local<v8::Value>> ExtractArgs(const v8::FunctionCallbackInfo<v8::Value>& Info, int32 InBegin = -1, int32 InEnd = -1);
	bool ValuesToString(const TArray<v8::Local<v8::Value>>& Values, FString& OutResult);
	bool EnsureV8(bool bCondition, const TCHAR* Expression);
	void SetProperty(v8::Local<v8::Object> Object, v8::Local<v8::String> Key, v8::Local<v8::Value> Value);
	void SetMethod(v8::Local<v8::Object> Object, v8::Local<v8::String> Key, v8::FunctionCallback Callback);
	int32 GetArrayLikeLength(v8::Local<v8::Object> Value);
	void GetCallSite(FString& OutScriptName, FString& OutFunctionName, int32& OutLineNumber);
	v8::Local<v8::Value> UnwrapStructProxy(const v8::Local<v8::Value>& Info);

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
