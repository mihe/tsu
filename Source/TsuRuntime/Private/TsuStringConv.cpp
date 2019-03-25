#include "TsuStringConv.h"

#include "TsuIsolate.h"

v8::Local<v8::String> FTsuStringConv::To(const FString& String)
{
	return To(*String, String.Len());
}

v8::Local<v8::String> FTsuStringConv::To(const TCHAR* String, int32 Length)
{
	static_assert(sizeof(TCHAR) == sizeof(uint16_t), "Character size mismatch");

	return v8::String::NewFromTwoByte(
		FTsuIsolate::Get(),
		reinterpret_cast<const uint16_t*>(String),
		v8::NewStringType::kNormal,
		Length
	).ToLocalChecked();
}

FString FTsuStringConv::From(v8::Local<v8::String> String)
{
	static_assert(sizeof(uint16_t) == sizeof(TCHAR), "Character size mismatch");

	v8::Isolate* Isolate = FTsuIsolate::Get();
	v8::String::Value StringValue{Isolate, String};
	auto Length = (int32)StringValue.length();
	auto Ptr = reinterpret_cast<const TCHAR*>(*StringValue);
	return FString(Length, Ptr);
}

v8::Local<v8::String> operator""_v8(const char16_t* StringPtr, size_t StringLen)
{
	return v8::String::NewFromTwoByte(
		FTsuIsolate::Get(),
		reinterpret_cast<const uint16_t*>(StringPtr),
		v8::NewStringType::kNormal,
		(int)StringLen
	).ToLocalChecked();
}
