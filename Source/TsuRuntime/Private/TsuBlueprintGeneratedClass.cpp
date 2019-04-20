#include "TsuBlueprintGeneratedClass.h"

#include "TsuBlueprint.h"
#include "TsuContext.h"
#include "TsuReflection.h"
#include "TsuRuntimeLog.h"
#include "TsuRuntimeSettings.h"
#include "TsuTypings.h"

#include "Engine/Blueprint.h"
#include "Misc/ScopeExit.h"
#include "UObject/Package.h"
#include "UObject/UObjectGlobals.h"

const FName UTsuBlueprintGeneratedClass::MetaDisplayName = TEXT("DisplayName");
const FName UTsuBlueprintGeneratedClass::MetaCategory = TEXT("Category");
const FName UTsuBlueprintGeneratedClass::MetaKeywords = TEXT("Keywords");
const FName UTsuBlueprintGeneratedClass::MetaTooltip = TEXT("Tooltip");
const FName UTsuBlueprintGeneratedClass::MetaDefaultToSelf = TEXT("DefaultToSelf");
const FName UTsuBlueprintGeneratedClass::MetaAdvancedDisplay = TEXT("AdvancedDisplay");

namespace TsuBlueprintGeneratedClass_Private
{

void LinkChild(UStruct* Parent, UField* Child)
{
	Child->Next = Parent->Children;
	Parent->Children = Child;
}

void UnlinkChild(UStruct* Parent, UField* Child)
{
	if (!Parent->Children)
		return;

	UField** CurrChild = &Parent->Children;
	while (*CurrChild)
	{
		UField*& NextChild = (*CurrChild)->Next;
		if (*CurrChild == Child)
		{
			*CurrChild = NextChild;
			break;
		}

		CurrChild = &NextChild;
	}
}

void SetCalculatedMetaDataAndFlags(UFunction* Function)
{
	// Borrowed from FKismetCompilerContext::SetCalculatedMetaDataAndFlags

	Function->ParmsSize = 0;
	Function->NumParms = 0;
	Function->ReturnValueOffset = MAX_uint16;

	for (auto Property : TFieldRange<UProperty>(Function, EFieldIteratorFlags::ExcludeSuper))
	{
		if (Property->HasAnyPropertyFlags(CPF_Parm))
		{
			++Function->NumParms;
			Function->ParmsSize = Property->GetOffset_ForUFunction() + Property->GetSize();

			if (Property->HasAnyPropertyFlags(CPF_OutParm))
				Function->FunctionFlags |= FUNC_HasOutParms;

			if (Property->HasAnyPropertyFlags(CPF_ReturnParm))
				Function->ReturnValueOffset = Property->GetOffset_ForUFunction();
		}
		else
		{
			if (!Property->HasAnyPropertyFlags(CPF_ZeroConstructor))
			{
				Function->FirstPropertyToInit = Property;
				Function->FunctionFlags |= FUNC_HasDefaults;
				break;
			}
		}
	}
}

} // namespace TsuBlueprintGeneratedClass_Private

void UTsuBlueprintGeneratedClass::FinishDestroy()
{
	UnloadModule();

	Super::FinishDestroy();
}

void UTsuBlueprintGeneratedClass::Bind()
{
	Super::Bind();

	if (Exports.IsValid())
	{
		// #todo(#mihe): Find a more appropriate place for this
		TailoredName = FTsuTypings::TailorNameOfType(this);

		for (const FTsuParsedFunction& Export : Exports.Exports)
			BindFunction(Export);
	}
}

TSharedPtr<FTsuModule> UTsuBlueprintGeneratedClass::PinModule()
{
	if (auto PinnedModule = Module.Pin())
		return PinnedModule;

	UE_LOG(LogTsuRuntime, Log, TEXT("Loading module for class '%s'..."), *TailoredName);
	Module = FTsuContext::Get().ClaimModule(*TailoredName, *Exports.Source, *Exports.Path);
	return Module.Pin();
}

void UTsuBlueprintGeneratedClass::LoadModule()
{
	PinModule();
}

void UTsuBlueprintGeneratedClass::UnloadModule()
{
	if (auto PinnedModule = Module.Pin())
		PinnedModule->Unload();
}

void UTsuBlueprintGeneratedClass::ReloadModule()
{
	UnloadModule();
	LoadModule();
}

void UTsuBlueprintGeneratedClass::RemoveNativeFunction(FName FunctionName)
{
	NativeFunctionLookupTable.RemoveAll(
		[&](const FNativeFunctionLookup& Item)
		{
			return Item.Name == FunctionName;
		});
}

void UTsuBlueprintGeneratedClass::BindFunction(const FTsuParsedFunction& Export)
{
	const FString& FunctionName = Export.Name;
	const FName FunctionFName = *FunctionName;

	if (UFunction* Function = FindFunctionByName(FunctionFName))
	{
		TsuBlueprintGeneratedClass_Private::UnlinkChild(this, Function);
		RemoveNativeFunction(FunctionFName);
		RemoveFunctionFromFunctionMap(Function);
	}

	auto Function = NewObject<UFunction>(this, FunctionFName, RF_Public | RF_MarkAsNative);
	Function->FunctionFlags |= FUNC_Public | FUNC_Static | FUNC_BlueprintCallable | FUNC_Native;

#if WITH_EDITOR
	const FString FunctionDisplayName = FString::Printf(
		TEXT("%s (%s)"),
		*FName::NameToDisplayString(FunctionName, false),
		*TailoredName);

	const FString Tooltip = FString::Printf(
		TEXT("Calls the '%s' function in the '%s' module"),
		*FunctionName,
		*TailoredName);

	Function->SetMetaData(MetaDisplayName, *FunctionDisplayName);
	Function->SetMetaData(MetaCategory, TEXT("TSU"));
	Function->SetMetaData(MetaKeywords, TEXT("tsu ts typescript js javascript"));
	Function->SetMetaData(MetaTooltip, *Tooltip);
#endif // WITH_EDITOR

	if (ensureMsgf(Export.ReturnTypes.Num() == 1, TEXT("Invalid (union) return type on function '%s'"), *FunctionName))
	{
		const FTsuParsedType& ReturnType = Export.ReturnTypes[0];

		if (ReturnType.Name != TEXT("void"))
		{
			static const FName ReturnValueName = TEXT("ReturnValue");
			if (
				UProperty* ReturnParam = NewParameterFromType(
					Function,
					ReturnValueName,
					ReturnType.Name,
					ReturnType.Dimensions))
			{
				ReturnParam->SetPropertyFlags(CPF_ReturnParm);
				TsuBlueprintGeneratedClass_Private::LinkChild(Function, ReturnParam);
			}
		}
	}

	auto Settings = GetDefault<UTsuRuntimeSettings>();

	for (int32 Index = Export.Parameters.Num() - 1; Index >= 0; --Index)
	{
		const FTsuParsedParameter& Parameter = Export.Parameters[Index];

		if (ensure(Parameter.Types.Num() == 1))
		{
			FString ParameterName = Parameter.Name;
			if (ParameterName.StartsWith(TEXT("_")))
				ParameterName = ParameterName.Mid(1);

			const FTsuParsedType& ParameterType = Parameter.Types[0];

			UProperty* Param = NewParameterFromType(
				Function,
				*ParameterName,
				ParameterType.Name,
				ParameterType.Dimensions);

			if (Param)
			{
				TsuBlueprintGeneratedClass_Private::LinkChild(Function, Param);

#if WITH_EDITOR
				if (Settings->bUseSelfParameter && ParameterName == Settings->SelfParameterName)
				{
					Function->SetMetaData(MetaDefaultToSelf, *ParameterName);
					Function->SetMetaData(MetaAdvancedDisplay, *ParameterName);
					Param->SetPropertyFlags(CPF_AdvancedDisplay);
				}
#endif // WITH_EDITOR
			}
		}
	}

	AddFunctionToFunctionMap(Function, FunctionFName);
	AddNativeFunction(*FunctionName, static_cast<FNativeFuncPtr>(&UTsuBlueprintGeneratedClass::ExecInvoke));
	TsuBlueprintGeneratedClass_Private::LinkChild(this, Function);

	Function->Bind();
	Function->StaticLink(true);

	TsuBlueprintGeneratedClass_Private::SetCalculatedMetaDataAndFlags(Function);
}

void UTsuBlueprintGeneratedClass::ExecInvoke(UObject* /*Context*/, FFrame& Stack, RESULT_DECL)
{
	ON_SCOPE_EXIT { P_FINISH; };

	auto Outer = Stack.CurrentNativeFunction->GetOuterUClass();
	auto This = static_cast<UTsuBlueprintGeneratedClass*>(Outer);
	This->PinModule()->Invoke(Stack, RESULT_PARAM);
}

UProperty* UTsuBlueprintGeneratedClass::NewParameterFromType(
	UObject* Outer,
	FName ParameterName,
	const FString& TypeName,
	int32 NumTypeDimensions)
{
	constexpr EObjectFlags NewObjectFlags = RF_Public;

	UProperty* Parameter = nullptr;

	if (NumTypeDimensions > 0)
	{
		check(NumTypeDimensions == 1);

		auto ArrayProperty = NewObject<UArrayProperty>(Outer, ParameterName, NewObjectFlags);
		UProperty* InnerProperty = NewParameterFromType(ArrayProperty, ParameterName, TypeName, 0);
		if (!InnerProperty)
			return nullptr;

		ArrayProperty->Inner = InnerProperty;
		Parameter = ArrayProperty;
	}
	else if (TypeName == TEXT("String"))
	{
		Parameter = NewObject<UStrProperty>(Outer, ParameterName, NewObjectFlags);
	}
	else if (TypeName == TEXT("boolean"))
	{
		Parameter = NewObject<UBoolProperty>(Outer, ParameterName, NewObjectFlags);
	}
	else if (TypeName == TEXT("number"))
	{
		Parameter = NewObject<UFloatProperty>(Outer, ParameterName, NewObjectFlags);
	}
	else
	{
		UField* Type = FTsuReflection::FindTypeByName(TypeName);
		if (!Type)
		{
			UE_LOG(LogTsuRuntime, Warning, TEXT("Failed to resolve object type '%s', falling back to 'UObject'..."), *TypeName);
			Type = UObject::StaticClass();
		}

		if (Type == UClass::StaticClass())
		{
			auto ClassProperty = NewObject<UClassProperty>(Outer, ParameterName, NewObjectFlags);
			ClassProperty->SetPropertyClass(UClass::StaticClass());
			ClassProperty->SetMetaClass(UObject::StaticClass());
			Parameter = ClassProperty;
		}
		else if (auto Struct = Cast<UScriptStruct>(Type))
		{
			auto StructProperty = NewObject<UStructProperty>(Outer, ParameterName, NewObjectFlags);
			StructProperty->Struct = Struct;
			Parameter = StructProperty;
		}
		else if (auto Class = Cast<UClass>(Type))
		{
			auto ObjectProperty = NewObject<UObjectProperty>(Outer, ParameterName, NewObjectFlags);
			ObjectProperty->SetPropertyClass(Class);
			Parameter = ObjectProperty;
		}
		else if (auto Enum = Cast<UEnum>(Type))
		{
			auto EnumProperty = NewObject<UEnumProperty>(Outer, ParameterName, NewObjectFlags);
			auto UnderlyingProp = NewObject<UByteProperty>(EnumProperty, TEXT("UnderlyingType"), NewObjectFlags);

			EnumProperty->SetEnum(Enum);
			EnumProperty->AddCppProperty(UnderlyingProp);

			Parameter = EnumProperty;
		}
	}

	if (!ensureMsgf(Parameter != nullptr, TEXT("Failed to resolve parameter type: %s"), *TypeName))
		return nullptr;

	Parameter->SetPropertyFlags(CPF_Parm);

	return Parameter;
}

#if WITH_EDITOR

void UTsuBlueprintGeneratedClass::GatherDependencies(TSet<TWeakObjectPtr<UBlueprint>>& Dependencies) const
{
	for (const FString& DependencyName : Exports.Dependencies)
	{
		if (auto Dependency = FindObject<UBlueprint>(ANY_PACKAGE, *DependencyName, true))
			Dependencies.Add(Dependency);
	}
}

#endif // WITH_EDITOR
