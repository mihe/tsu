#pragma once

#include "CoreMinimal.h"

#include "TsuV8Wrapper.h"

class FTsuStringConv
{
public:
	static v8::Local<v8::String> To(const TCHAR* String, int32 Length = -1);
	static v8::Local<v8::String> To(const FString& String);
	static FString From(v8::Local<v8::String> String);
};

v8::Local<v8::String> operator""_v8(const char16_t* StringPtr, size_t StringLen);

#define TCHAR_TO_V8(str) FTsuStringConv::To(str)
#define V8_TO_TCHAR(str) FTsuStringConv::From(str)
