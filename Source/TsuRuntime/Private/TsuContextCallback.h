#pragma once

#define TSU_CONTEXT_CALLBACK(FunctionName)                                               \
	void FunctionName(const v8::FunctionCallbackInfo<v8::Value>& Info);          \
	static void _##FunctionName(const v8::FunctionCallbackInfo<v8::Value>& Info) \
	{                                                                            \
		Singleton->FunctionName(Info);                                           \
	}
