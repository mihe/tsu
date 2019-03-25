#pragma once

#define _TSU_ENABLE(X) X
#define _TSU_DISABLE(X)

#define TSU_CALLBACK_ARGS_DECL(FunctionArgs) FunctionArgs(_TSU_ENABLE, _TSU_ENABLE)
#define TSU_CALLBACK_ARGS_FWD(FunctionArgs) FunctionArgs(_TSU_DISABLE, _TSU_ENABLE)

#define TSU_CALLBACK(FunctionName, FunctionArgs) \
	static void FunctionName(TSU_CALLBACK_ARGS_DECL(FunctionArgs));

#define TSU_CALLBACK_IMPL(FunctionName, FunctionArgs)                       \
	void FunctionName##Impl(TSU_CALLBACK_ARGS_DECL(FunctionArgs));          \
	static void FunctionName(TSU_CALLBACK_ARGS_DECL(FunctionArgs))          \
	{                                                                       \
		Singleton->FunctionName##Impl(TSU_CALLBACK_ARGS_FWD(FunctionArgs)); \
	}

// ----------------------- Function -----------------------

#define TSU_FUNCTION_CALLBACK_ARGS(Type, Name) \
	Type(const v8::FunctionCallbackInfo<v8::Value>&) Name(Info)

#define TSU_FUNCTION_CALLBACK(FunctionName) \
	TSU_CALLBACK(FunctionName, TSU_FUNCTION_CALLBACK_ARGS)

#define TSU_FUNCTION_CALLBACK_IMPL(FunctionName) \
	TSU_CALLBACK_IMPL(FunctionName, TSU_FUNCTION_CALLBACK_ARGS)

// ----------------------- Getter -----------------------

#define TSU_GET_INDEX_CALLBACK_ARGS(Type, Name) \
	Type(uint32_t) Name(Index),                 \
	Type(const v8::PropertyCallbackInfo<v8::Value>&) Name(Info)

#define TSU_GET_INDEX_CALLBACK(FunctionName) \
	TSU_CALLBACK(FunctionName, TSU_GET_INDEX_CALLBACK_ARGS)

#define TSU_GET_INDEX_CALLBACK_IMPL(FunctionName) \
	TSU_CALLBACK_IMPL(FunctionName, TSU_GET_INDEX_CALLBACK_ARGS)

// ----------------------- Setter -----------------------

#define TSU_SET_INDEX_CALLBACK_ARGS(Type, Name) \
	Type(uint32_t) Name(Index),                 \
	Type(v8::Local<v8::Value>) Name(Value),     \
	Type(const v8::PropertyCallbackInfo<v8::Value>&) Name(Info)

#define TSU_SET_INDEX_CALLBACK(FunctionName) \
	TSU_CALLBACK(FunctionName, TSU_SET_INDEX_CALLBACK_ARGS)

#define TSU_SET_INDEX_CALLBACK_IMPL(FunctionName) \
	TSU_CALLBACK_IMPL(FunctionName, TSU_SET_INDEX_CALLBACK_ARGS)
