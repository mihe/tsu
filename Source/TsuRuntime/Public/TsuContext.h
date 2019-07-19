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
	friend struct FTsuWorldContextScope;
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

	/** Gets the singleton, creating it if needed */
	static FTsuContext& Get();

	/** Destroys the singleton */
	static void Destroy();

	/** Returns whether the singleton exists or not */
	static bool Exists();

	/**
	 * Evaluates/runs the code of a CommonJS module inside the context
	 * 
	 * @param Code The source code of the module
	 * @param Path The absolute path to the source code
	 * 
	 * @returns The resulting `module.exports` (maybe)
	 */
	v8::MaybeLocal<v8::Value> EvalModule(const TCHAR* Code, const TCHAR* Path);

	/**
	 * Evalutes and binds the code of a CommonJS module into the context
	 * 
	 * @param Binding The name to bind the module to
	 * @param Code The source code of the module
	 * @param Path The absolute path of the source code
	 * 
	 * @returns Whether the module was successfully bound
	 */
	bool BindModule(const TCHAR* Binding, const TCHAR* Code, const TCHAR* Path);

	/**
	 * Evaluates and binds the code of a CommonJS module into the context, and then returns 
	 * a handle for said module, through which exported functions can be invoked.
	 * 
	 * @param Binding The name to bind the module to
	 * @param Code The source code of the module
	 * @param Path The absolute path of the source code
	 * 
	 * @returns A weak handle to the module
	 */
	TWeakPtr<FTsuModule> ClaimModule(const TCHAR* Binding, const TCHAR* Code, const TCHAR* Path);

private:
	FTsuContext();

	/**
	 * Unloads a module by simply unbinding it from the global object, meaning it'll get disposed of
	 * once the GC does its thing.
	 * 
	 * @param Binding The name which the module was bound to
	 */
	void UnloadModule(const TCHAR* Binding);

	/**
	 * Gets the V8 value of a specified function from a specified module.
	 * 
	 * @param Binding The name which the parent module was bound to
	 * @param Name The name of the function
	 * @returns The V8 value (maybe)
	 */
	v8::MaybeLocal<v8::Function> GetExportedFunction(const TCHAR* ModuleBinding, const TCHAR* Name);

	/**
	 * Pushes an object onto the top of the world context stack.
	 * 
	 * @see FTsuWorldContextScope
	 * @param WorldContext The object to push onto to the stack
	 */
	void PushWorldContext(v8::Local<v8::Object> WorldContext);

	/**
	 * Pushes an object onto the top of the world context stack. This overload will implicitly create a
	 * V8 reference to the object.
	 * 
	 * @see FTsuWorldContextScope
	 * @param WorldContext The object to push onto the stack
	 * @returns Whether it was successfully pushed or not
	 */
	bool PushWorldContext(UObject* WorldContext);

	/**
	 * Pops the top-most object from the world context stack.
	 *
	 * @see FTsuWorldContextScope
	 */
	void PopWorldContext();

	/** Gets the top-most object from the world context stack */
	v8::Local<v8::Object> GetWorldContext();

	/** Binds all the core stuff to the global object, like `console.log`, etc. */
	void InitializeBuiltins();

	/** Creates and stores the templates for regular and multicast delegates */
	void InitializeDelegates();

	/** Creates and stores the `EKeys` object */
	void InitializeKeys();

	/** Loads and binds the code for `require` */
	void InitializeRequire();

	/** Loads, creates and stores the constructor for the array proxy handler */
	void InitializeArrayProxy();

	/** Loads, creates and stores the constructor for the struct proxy handler */
	void InitializeStructProxy();

	/** Finds the constructor for a given type. Creates and caches it if it isn't already. */
	v8::Local<v8::Function> FindOrAddConstructor(UStruct* Type);

	/** Creates, caches and returns a template for a given type */
	v8::Local<v8::FunctionTemplate> AddTemplate(UStruct* Type);

	/** Finds the template for a given type. Creates and caches it if it isn't already. */
	v8::Local<v8::FunctionTemplate> FindOrAddTemplate(UStruct* Type);

	/**
	 * Creates a V8 instance of a given struct instance and adds it to the list of alive structs.
	 * 
	 * @param StructObject The given struct object
	 * @param StructType The type of the given struct object
	 * @returns The resulting V8 object
	 */
	v8::Local<v8::Object> ReferenceStructObject(void* StructObject, UScriptStruct* StructType);

	/**
	 * Creates a V8 instance of a given UObject and adds it to the list of alive objects.
	 * 
	 * @note Will return a null value if a `nullptr` is passed to it.
	 * @param ClassObject The given class object
	 * @returns The resulting V8 object, or null
	 */
	v8::Local<v8::Value> ReferenceClassObject(UObject* ClassObject);

	/**
	 * Creates a V8 instance of a regular or multicast delegate and adds it to the list of alive delegates.
	 * 
	 * @param ParentProperty The delegate property of the parent UObject
	 * @param Parent Pointer to the parent UObject
	 * @returns The resulting V8 object
	 */
	v8::Local<v8::Object> ReferenceDelegate(UProperty* ParentProperty, UObject* Parent);

	/** The native function callback for exported TSU functions */
	void Invoke(const TCHAR* Namespace, FFrame& Stack, RESULT_DECL);

	/**
	 * Callback for UTsuDelegateEvent when a delegate event is called/broadcast.
	 * 
	 * @param WorldContext The world context at the time of event creation
	 * @param Callback The callback bound to the event
	 * @param Signature Optional signature of the delegate, if the delegate expects parameters
	 * @param ParamsBuffer Optional parameter buffer, if the delegate expects parameters
	 * @returns Whether the call was successful or not
	 */
	bool InvokeDelegateEvent(
		v8::Local<v8::Object> WorldContext,
		v8::Local<v8::Function> Callback,
		UFunction* Signature = nullptr,
		void* ParamsBuffer = nullptr);

	 /** Override of FGCObject::AddReferencedObjects */
	void AddReferencedObjects(FReferenceCollector& Collector) override;

	/** Callback for pre UObject GC */
	void OnPreGarbageCollect();

	/** Callback for post UObject GC */
	void OnPostGarbageCollect();

	/**
	 * Creates and stores a callback to be invoked after a specified delay, using `FTimerManager`.
	 * 
	 * @param Callback The callback to execute
	 * @param Delay The time to wait before executing the callback
	 * @param bLoop Whether to queue another timeout after this one is done
	 * @returns The V8 instance of the resulting `FTimerHandle`
	 */
	v8::Local<v8::Value> StartTimeout(v8::Local<v8::Function> Callback, float Delay, bool bLoop);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnConsoleLog);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnConsoleDisplay);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnConsoleError);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnConsoleWarning);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnConsoleTrace);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnConsoleTimeBegin);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnConsoleTimeEnd);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnClassNew);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnStructNew);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnCallMethod);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnCallStaticMethod);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnCallExtensionMethod);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnPropertyGet);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnPropertySet);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnGetArrayElement);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnSetArrayElement);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnGetArrayLength);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnSetArrayLength);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnSetTimeout);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnSetInterval);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnClearTimeout);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnPathJoin);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnPathResolve);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnPathDirName);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnFileRead);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnFileExists);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnRequire);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnImport);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnGetProperty);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnSetProperty);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnGetStaticClass);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnActorSpawn);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnActorComponentAddTo);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnDelegateBind);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnDelegateUnbind);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnDelegateExecute);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnDelegateIsBound);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnMulticastDelegateAdd);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnMulticastDelegateRemove);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnMulticastDelegateBroadcast);

	/** ... */
	TSU_CONTEXT_CALLBACK(OnMulticastDelegateIsBound);

	/** ... */
	void CallMethod(
		UObject* Object,
		UFunction* Method,
		void* ParamsBuffer,
		v8::ReturnValue<v8::Value> ReturnValue);

	/** ... */
	void WriteParameters(
		const v8::FunctionCallbackInfo<v8::Value>& Info,
		UFunction* Method,
		void* ParamsBuffer);

	/** ... */
	void WriteExtensionParameters(
		const v8::FunctionCallbackInfo<v8::Value>& Info,
		UFunction* Method,
		void* ParamsBuffer);

	/** ... */
	void PopArgumentsFromStack(
		FFrame& Stack,
		UFunction* Function,
		TArray<v8::Local<v8::Value>>& OutArguments);

	/** ... */
	void PopArgumentFromStack(
		FFrame& Stack,
		UProperty* Argument,
		TArray<v8::Local<v8::Value>>& OutArguments);

	/** ... */
	bool WritePropertyToContainer(
		UProperty* Property,
		v8::Local<v8::Value> Value,
		void* Dest);

	/** ... */
	bool WritePropertyToBuffer(
		UProperty* Property,
		v8::Local<v8::Value> Value,
		void* Dest);

	/**
	 * Checks to see if the supplied parameter has a default value associated with it and writes said value to
	 * it. If no default value is found it will simply call `UProperty::InitializeValue`.
	 * 
	 * @param Method The method to which the parameter belongs
	 * @param Param The parameter to initialize
	 * @param Buffer The buffer in which the value is to be stored
	 */
	void WriteDefaultValue(UFunction* Method, UProperty* Param, void* Buffer);

	/** ... */
	v8::Local<v8::Value> ReadPropertyFromContainer(UProperty* Property, const void* Source);

	/** ... */
	v8::Local<v8::Value> ReadPropertyFromBuffer(UProperty* Property, const void* Source);

	/** ... */
	template<typename T>
	bool GetExternalValue(v8::Local<v8::Value> Value, T** OutData);

	/** ... */
	template<typename T1, typename T2 = void>
	bool GetInternalFields(v8::Local<v8::Value> Value, T1** Field1, T2** Field2 = nullptr);

	/** ... */
	TArray<v8::Local<v8::Value>> ExtractArgs(
		const v8::FunctionCallbackInfo<v8::Value>& Info,
		int32 InBegin = -1,
		int32 InEnd = -1);

	/** ... */
	bool ValuesToString(const TArray<v8::Local<v8::Value>>& Values, FString& OutResult);

	/** ... */
	bool EnsureV8(bool bCondition, const TCHAR* Expression);

	/** ... */
	void DefineProperty(
		v8::Local<v8::Object> Object,
		v8::Local<v8::String> Key,
		v8::Local<v8::Value> Value);

	/** ... */
	void DefineMethod(
		v8::Local<v8::Object> Object,
		v8::Local<v8::String> Key,
		v8::FunctionCallback Callback);

	/** ... */
	int32 GetArrayLikeLength(v8::Local<v8::Object> Value);

	/** ... */
	void GetCallSite(
		FString& OutScriptName,
		FString& OutFunctionName,
		int32& OutLineNumber);

	/** ... */
	v8::Local<v8::Value> UnwrapStructProxy(const v8::Local<v8::Value>& Info);

	/** ... */
	static TOptional<FTsuContext> Singleton;

	/** ... */
	static v8::Isolate* Isolate;

	/** ... */
	v8::Global<v8::Context> GlobalContext;

	/** ... */
	static v8::Global<v8::FunctionTemplate> GlobalDelegateTemplate;

	/** ... */
	static v8::Global<v8::FunctionTemplate> GlobalMulticastDelegateTemplate;

	/** ... */
	static v8::Global<v8::Function> GlobalArrayHandlerConstructor;

	/** ... */
	static v8::Global<v8::Function> GlobalArrayConstructor;

	/** ... */
	static v8::Global<v8::Function> GlobalStructHandlerConstructor;

	/** ... */
	v8::Global<v8::Object> GlobalKeys;

	/** ... */
	TMap<FString, TSharedPtr<FTsuModule>> LoadedModules;

	/** ... */
	TArray<v8::Global<v8::Object>> WorldContexts;

	/** ... */
	uint64 NextDelegateHandle = 1;

	/** ... */
	FDelegateEventMap DelegateEvents;

	/** ... */
	TMap<FString, uint64> PendingTimeLogs;

	/** ... */
	TMap<UStruct*, v8::Global<v8::FunctionTemplate>> Templates;

	/** ... */
	TMap<FStructKey, v8::Global<v8::Object>> AliveStructs;

	/** ... */
	TMap<UObject*, v8::Global<v8::Object>> AliveObjects;

	/** ... */
	TMap<FDelegateKey, v8::Global<v8::Object>> AliveDelegates;

	/** ... */
	TArray<FTsuTimer> AliveTimers;

	/** ... */
	TOptional<FTsuInspector> Inspector;
};
