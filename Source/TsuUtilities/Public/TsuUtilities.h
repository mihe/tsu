#pragma once

#include "CoreMinimal.h"

#include "UObject/UObjectIterator.h"
#include "UObject/UnrealType.h"

#define GET_TYPE_NAME_CHECKED(TypeName) \
	(sizeof(TypeName), TEXT(PREPROCESSOR_TO_STRING(TypeName)))

#if WITH_EDITOR

inline bool IsFieldDeprecated(const UField* Field)
{
	static const FName MetaDeprecatedFunction = TEXT("DeprecatedFunction");
	static const FName MetaCategory = TEXT("Category");
	static const FName MetaDisplayName = TEXT("DisplayName");

	if (Field->HasMetaData(MetaDeprecatedFunction))
		return true;

	if (Field->GetMetaData(MetaCategory) == TEXT("Deprecated"))
		return true;

	if (Field->HasMetaData(MetaDisplayName))
	{
		const FString& DisplayName = Field->GetMetaData(MetaDisplayName);
		if (DisplayName.Contains(TEXT("deprecated"), ESearchCase::IgnoreCase))
			return true;
	}

	return false;
}

#endif // WITH_EDITOR

// 32-bit FNV-1a
constexpr uint32 TsuHash(const TCHAR* Input, uint32 Hash = 0x811C9DC5U)
{
	for (; *Input; ++Input)
		Hash = (Hash ^ *Input) * 0x01000193ULL;

	return Hash;
}

#define TSU_HASH(Str) TsuHash(TEXT(Str))

template<typename T>
struct TImmediateFieldIterator
	: TFieldIterator<T>
{
	TImmediateFieldIterator(const UStruct* Struct)
		: TFieldIterator<T>(
			Struct,
			EFieldIteratorFlags::ExcludeSuper,
			EFieldIteratorFlags::ExcludeDeprecated,
			EFieldIteratorFlags::ExcludeInterfaces)
	{
	}
};

template<typename T>
struct TImmediateFieldRange
	: TFieldRange<T>
{
	TImmediateFieldRange(const UStruct* Struct)
		: TFieldRange<T>(
			Struct,
			EFieldIteratorFlags::ExcludeSuper,
			EFieldIteratorFlags::ExcludeDeprecated,
			EFieldIteratorFlags::ExcludeInterfaces)
	{
	}
};

struct FParamIterator
{
	FParamIterator(const UFunction* InFunction)
		: Field(InFunction ? InFunction->Children : nullptr)
	{
		IterateToNext();
	}

	FORCEINLINE void operator++()
	{
		Field = Field->Next;
		IterateToNext();
	}

	FORCEINLINE UProperty* operator*()
	{
		return static_cast<UProperty*>(Field);
	}

	FORCEINLINE UProperty* operator->()
	{
		return static_cast<UProperty*>(Field);
	}

	FORCEINLINE explicit operator bool() const
	{
		return Field != nullptr;
	}

	FORCEINLINE bool operator!() const
	{
		return Field == nullptr;
	}

	FORCEINLINE friend bool operator==(const FParamIterator& Lhs, const FParamIterator& Rhs)
	{
		return Lhs.Field == Rhs.Field;
	}

	FORCEINLINE friend bool operator!=(const FParamIterator& Lhs, const FParamIterator& Rhs)
	{
		return Lhs.Field != Rhs.Field;
	}

private:
	void IterateToNext()
	{
		UField* CurrentField = Field;
		while (CurrentField)
		{
			if (auto CurrentProperty = Cast<UProperty>(CurrentField))
			{
				if ((CurrentProperty->PropertyFlags & (CPF_Parm | CPF_Deprecated)) == CPF_Parm)
				{
					Field = CurrentField;
					return;
				}
			}

			CurrentField = CurrentField->Next;
		}

		Field = CurrentField;
	}

	UField* Field = nullptr;
};

struct FParamRange
{
	FParamRange(const UFunction* InFunction)
		: Begin(InFunction)
	{
	}

	FORCEINLINE friend FParamIterator begin(const FParamRange& Range)
	{
		return Range.Begin;
	}

	FORCEINLINE friend FParamIterator end(const FParamRange& Range)
	{
		return {nullptr};
	}

	FParamIterator Begin;
};
