#include "TsuReflection.h"

#include "TsuBlueprintGeneratedClass.h"
#include "TsuObjectLibrary.h"
#include "TsuRotatorLibrary.h"
#include "TsuTimelineLibrary.h"
#include "TsuTransformLibrary.h"
#include "TsuTypeIndex.h"
#include "TsuUtilities.h"
#include "TsuVectorLibrary.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/UObjectIterator.h"

// #todo(#mihe): Take a look at UEdGraphSchema_K2::IsAllowableBlueprintVariableType?
// #todo(#mihe): Take a look at CanBeExposed in K2Node_MakeStruct.cpp?

const FName FTsuReflection::MetaWorldContext = TEXT("WorldContext");
const FName FTsuReflection::MetaNativeMakeFunc = TEXT("NativeMakeFunc");
const FName FTsuReflection::MetaNativeBreakFunc = TEXT("NativeBreakFunc");
const FName FTsuReflection::MetaBlueprintInternalUseOnly = TEXT("BlueprintInternalUseOnly");
const FName FTsuReflection::MetaScriptMethod = TEXT("ScriptMethod");
const FName FTsuReflection::MetaScriptOperator = TEXT("ScriptOperator");
const FName FTsuReflection::MetaTsuExtension = TEXT("TsuExtension");
const FName FTsuReflection::MetaTsuStaticExtension = TEXT("TsuStaticExtension");

bool FTsuReflection::IsInternalType(UField* Type)
{
	return Type->GetBoolMetaData(MetaBlueprintInternalUseOnly);
}

bool FTsuReflection::IsInternalProperty(UProperty* Property)
{
	if (auto StructProperty = Cast<UStructProperty>(Property))
		return IsInternalType(StructProperty->Struct);
	else if (auto ObjectProperty = Cast<UObjectPropertyBase>(Property))
		return IsInternalType(ObjectProperty->PropertyClass);
	else if (auto InterfaceProperty = Cast<UInterfaceProperty>(Property))
		return IsInternalType(InterfaceProperty->InterfaceClass);

	return false;
}

bool FTsuReflection::UsesInternalTypes(UFunction* Function)
{
	bool bResult = false;

	VisitFunctionParameters([&](UProperty* Parameter)
	{
		bResult = bResult || IsInternalProperty(Parameter);
	}, Function, false, false);

	VisitFunctionReturns([&](UProperty* Return)
	{
		bResult = bResult || IsInternalProperty(Return);
	}, Function);
	
	return bResult;
}

bool FTsuReflection::IsInternalFunction(UFunction* Function)
{
	return Function->GetBoolMetaData(MetaBlueprintInternalUseOnly) || UsesInternalTypes(Function);
}

bool FTsuReflection::IsExposedFunction(UFunction* Function)
{
	return (
		Function->HasAllFunctionFlags(FUNC_Public) &&
		Function->HasAnyFunctionFlags(FUNC_BlueprintCallable | FUNC_BlueprintPure) &&
		!Function->HasAnyFunctionFlags(FUNC_EditorOnly | FUNC_Delegate) &&
		!IsInternalFunction(Function) &&
		!IsFieldDeprecated(Function)
	);
}

bool FTsuReflection::IsStaticBlueprintFunction(UFunction* Function)
{
	return IsExposedFunction(Function) && Function->HasAllFunctionFlags(FUNC_Static);
}

bool FTsuReflection::IsExplicitExtension(UClass* Class)
{
	return Class->HasMetaData(MetaTsuExtension);
}

bool FTsuReflection::IsExplicitExtension(UFunction* Function)
{
	return (
		Function->HasMetaData(MetaTsuExtension) ||
		Function->HasMetaData(MetaTsuStaticExtension) ||
		Function->HasMetaData(MetaScriptMethod) ||
		Function->HasMetaData(MetaScriptOperator) ||
		IsExplicitExtension(Function->GetOuterUClass())
	);
}

bool FTsuReflection::IsExplicitExtension(UField* Field)
{
	if (auto Function = Cast<UFunction>(Field))
		return IsExplicitExtension(Function);
	else if (auto Class = Cast<UClass>(Field))
		return IsExplicitExtension(Class);
	else
		return false;
}

bool FTsuReflection::IsExtensionFunction(UFunction* Function)
{
	return (
		IsExplicitExtension(Function) ||
		IsStaticBlueprintFunction(Function) &&
		!Function->HasMetaData(MetaWorldContext) &&
		!Function->HasMetaData(MetaNativeMakeFunc) &&
		!Function->HasMetaData(MetaNativeBreakFunc)
	);
}

bool FTsuReflection::IsStaticExtension(UFunction* Function)
{
	return Function->HasMetaData(MetaTsuStaticExtension);
}

bool FTsuReflection::IsInvalidClass(UClass* Class)
{
	return (
		Class->HasAnyClassFlags(CLASS_NewerVersionExists) ||
		(Class->HasAllClassFlags(CLASS_CompiledFromBlueprint) && Class->HasAnyFlags(RF_Transient))
	);
}

bool FTsuReflection::IsValidClass(UClass* Class)
{
	return !IsInvalidClass(Class);
}

bool FTsuReflection::IsExposedProperty(UProperty* Property)
{
	return Property->HasAnyPropertyFlags(
		CPF_BlueprintVisible |
		CPF_BlueprintAssignable |
		CPF_Edit |
		CPF_EditConst
	) && !IsFieldDeprecated(Property);
}

bool FTsuReflection::IsDelegateProperty(UProperty* Property)
{
	return Property->IsA<UDelegateProperty>() || Property->IsA<UMulticastDelegateProperty>();
}

bool FTsuReflection::IsReadOnlyProperty(UProperty* Property)
{
	return (
		Property->HasAnyPropertyFlags(CPF_BlueprintReadOnly) ||
		Property->IsA<UMulticastDelegateProperty>() ||
		Property->IsA<UDelegateProperty>());
}

bool FTsuReflection::IsReferenceParameter(UProperty* Param)
{
	return Param->HasAllPropertyFlags(CPF_Parm | CPF_OutParm | CPF_ReferenceParm);
}

bool FTsuReflection::IsK2Method(const FString& FunctionName)
{
	return FunctionName.StartsWith(TEXT("K2_"));
}

bool FTsuReflection::IsK2Method(UFunction* Function)
{
	return IsK2Method(Function->GetName());
}

bool FTsuReflection::IsOutputParameter(UProperty* Param, bool bAllowReturnParam)
{
	const uint64 Mask =
		bAllowReturnParam
			? (CPF_OutParm | CPF_ConstParm | CPF_BlueprintReadOnly)
			: (CPF_OutParm | CPF_ConstParm | CPF_BlueprintReadOnly | CPF_ReturnParm);

	return (Param->PropertyFlags & Mask) == CPF_OutParm;
}

bool FTsuReflection::HasOutputParameters(UFunction* Function)
{
	if (!Function->HasAllFunctionFlags(FUNC_HasOutParms))
		return false;

	for (UProperty* Param : FParamRange{Function})
	{
		if (IsOutputParameter(Param))
			return true;
	}

	return false;
}

bool FTsuReflection::IsInputParameter(UProperty* Param)
{
	return (Param->PropertyFlags & CPF_Parm) && !IsOutputParameter(Param, true);
}

void FTsuReflection::VisitAllTypes(const TypeVisitor& Visitor)
{
	FTsuTypeSet ReferencedTypes;
	ReferencedTypes.Reserve(100);

	for (auto Class : TObjectRange<UClass>())
	{
		if (IsInternalType(Class))
			continue;

		ReferencedTypes.Reset();
		if (GetReferencesInType(Class, ReferencedTypes))
			Visitor(Class, ReferencedTypes);
	}

	for (auto Struct : TObjectRange<UScriptStruct>())
	{
		if (IsInternalType(Struct))
			continue;

		ReferencedTypes.Reset();
		if (GetReferencesInType(Struct, ReferencedTypes))
			Visitor(Struct, ReferencedTypes);
	}

	for (auto Enum : TObjectRange<UEnum>())
	{
		if (IsInternalType(Enum))
			continue;

		ReferencedTypes.Reset();
		Visitor(Enum, ReferencedTypes);
	}
}

void FTsuReflection::VisitFunctionLibraries(const LibraryVisitor& Visitor)
{
	using FCache = TArray<UClass*>;

	static const FCache Cache = []
	{
		FCache Result;

		for (auto Class : TObjectRange<UClass>())
		{
			if (!Class->IsChildOf<UBlueprintFunctionLibrary>())
				continue;

			if (Cast<UTsuBlueprintGeneratedClass>(Class))
				continue;

			Result.Add(Class);
		}

		return Result;
	}();

	for (UClass* Class : Cache)
		Visitor(Class);
}

void FTsuReflection::VisitMakeFunctions(const MakeVisitor& Visitor)
{
	using FCache = TMap<UFunction*, UScriptStruct*>;

	static const FCache Cache = []
	{
		FCache Result;

		VisitFunctionLibraries([&](UClass* Library)
		{
			for (auto Function : TImmediateFieldRange<UFunction>(Library))
			{
				if (!Function->HasMetaData(MetaNativeMakeFunc))
					continue;

				UProperty* ReturnProperty = Function->GetReturnProperty();
				auto StructProperty = Cast<UStructProperty>(ReturnProperty);
				if (ensure(StructProperty))
					Result.FindOrAdd(Function) = StructProperty->Struct;
			}
		});

		return Result;
	}();

	for (const auto& Entry : Cache)
		Visitor(Entry.Key, Entry.Value);
}

void FTsuReflection::VisitBreakFunctions(const BreakVisitor& Visitor)
{
	using FCache = TMap<UFunction*, UScriptStruct*>;

	static const FCache Cache = []
	{
		FCache Result;

		VisitFunctionLibraries([&](UClass* Library)
		{
			for (auto Function : TImmediateFieldRange<UFunction>(Library))
			{
				if (!Function->HasMetaData(MetaNativeBreakFunc))
					continue;

				FParamIterator ParamIt{Function};
				if (!ensure(ParamIt))
					continue;

				auto StructProperty = Cast<UStructProperty>(*ParamIt);
				if (ensure(StructProperty))
					Result.FindOrAdd(Function) = StructProperty->Struct;
			}
		});

		return Result;
	}();

	for (const auto& Entry : Cache)
		Visitor(Entry.Key, Entry.Value);
}

void FTsuReflection::VisitProperties(
	const PropertyVisitor& Visitor,
	UStruct* Object,
	bool bIncludeDerived)
{
	const bool bIsStruct = Object->IsA<UScriptStruct>();

	if (UFunction* BreakFunction = FindBreakFunction(Object))
	{
		for (UProperty* Parameter : FParamRange{BreakFunction})
		{
			if (IsOutputParameter(Parameter))
				Visitor(Parameter, true);
		}
	}
	else
	{
		for (auto Property : TImmediateFieldRange<UProperty>(Object))
		{
			if (!IsExposedProperty(Property))
				continue;

			if (bIsStruct && IsDelegateProperty(Property))
				continue;

			Visitor(Property, IsReadOnlyProperty(Property));
		}
	}

	if (bIncludeDerived)
	{
		if (UStruct* SuperStruct = Object->GetSuperStruct())
			VisitProperties(Visitor, SuperStruct, bIncludeDerived);
	}
}

void FTsuReflection::VisitMethods(const MethodVisitor& Visitor, UStruct* Object)
{
	for (auto Function : TImmediateFieldRange<UFunction>(Object))
	{
		if (IsExposedFunction(Function) && !IsExplicitExtension(Function))
			Visitor(Function);
	}
}

void FTsuReflection::VisitExtensionMethods(const ExtensionVisitor& Visitor, UStruct* Object)
{
	// #hack(#mihe): This should be dealt with using some sort of metadata
	static const TMap<UStruct*, UStruct*> ExtensionLibraries =
		{
			{UObject::StaticClass(), UTsuObjectLibrary::StaticClass()},
			{TBaseStructure<FVector>::Get(), UTsuVectorLibrary::StaticClass()},
			{TBaseStructure<FRotator>::Get(), UTsuRotatorLibrary::StaticClass()},
			{TBaseStructure<FTransform>::Get(), UTsuTransformLibrary::StaticClass()}
		};

	using FCache = TMap<UStruct*, TArray<UFunction*>>;

	static const FCache Cache = [&]
	{
		FCache Result;

		VisitFunctionLibraries([&](UClass* Library)
		{
			for (auto Function : TImmediateFieldRange<UFunction>(Library))
			{
				if (UStruct* Type = FindExtendedTypeNonStatic(Function))
				{
					bool bIsOverridden = false;
					if (UStruct* LibraryOverride = ExtensionLibraries.FindRef(Type))
						bIsOverridden = (Library != LibraryOverride);

					if (!bIsOverridden)
						Result.FindOrAdd(Type).Add(Function);
				}
			}
		});

		return Result;
	}();

	if (const TArray<UFunction*>* Functions = Cache.Find(Object))
	{
		for (UFunction* Function : *Functions)
			Visitor(Function);
	}
}

void FTsuReflection::VisitStaticExtensionMethods(const StaticExtensionVisitor& Visitor, UStruct* Object)
{
	using FCache = TMap<UStruct*, TArray<UFunction*>>;

	static const FCache Cache = [&]
	{
		FCache Result;

		VisitFunctionLibraries([&](UClass* Library)
		{
			for (auto Function : TImmediateFieldRange<UFunction>(Library))
			{
				if (UStruct* Type = FindExtendedTypeStatic(Function))
					Result.FindOrAdd(Type).Add(Function);
			}
		});

		return Result;
	}();

	if (const TArray<UFunction*>* Functions = Cache.Find(Object))
	{
		for (UFunction* Function : *Functions)
			Visitor(Function);
	}
}

bool FTsuReflection::VisitFunctionParameters(
	const ParameterVisitor& Visitor,
	UFunction* Function,
	bool bSkipFirst,
	bool bSkipWorldContext)
{
	FName WorldContextName;
	if (Function->HasMetaData(MetaWorldContext))
		WorldContextName = *Function->GetMetaData(MetaWorldContext);

	bool bSkipNext = bSkipFirst;

	for (UProperty* Parameter : FParamRange{Function})
	{
		if (!Parameter->HasAllPropertyFlags(CPF_Parm) || Parameter->HasAnyPropertyFlags(CPF_ReturnParm))
			continue;

		check(Parameter->ArrayDim == 1);

		if (bSkipNext)
		{
			bSkipNext = false;
			continue;
		}

		if (IsOutputParameter(Parameter) && !IsReferenceParameter(Parameter))
			continue;

		if (bSkipWorldContext && Parameter->GetFName() == WorldContextName)
			continue;

		Visitor(Parameter);
	}

	return true;
}

void FTsuReflection::VisitFunctionReturns(const ReturnVisitor& Visitor, UFunction* Function)
{
	for (UProperty* Parameter : FParamRange{Function})
	{
		if (Parameter->HasAllPropertyFlags(CPF_OutParm))
			Visitor(Parameter);
	}
}

void FTsuReflection::VisitPropertyType(const PropertyTypeVisitor& Visitor, UProperty* Property)
{
	if (auto ByteProperty = Cast<UByteProperty>(Property))
	{
		if (ByteProperty->Enum)
			Visitor(ByteProperty->Enum);
	}
	else if (auto EnumProperty = Cast<UEnumProperty>(Property))
	{
		Visitor(EnumProperty->GetEnum());
	}
	else if (auto StructProperty = Cast<UStructProperty>(Property))
	{
		Visitor(StructProperty->Struct);
	}
	else if (auto ObjectProperty = Cast<UObjectPropertyBase>(Property))
	{
		Visitor(ObjectProperty->PropertyClass);
	}
	else if (auto InterfaceProperty = Cast<UInterfaceProperty>(Property))
	{
		Visitor(InterfaceProperty->InterfaceClass);
	}
	else if (auto ArrayProperty = Cast<UArrayProperty>(Property))
	{
		VisitPropertyType(Visitor, ArrayProperty->Inner);
	}
	else if (auto SetProperty = Cast<USetProperty>(Property))
	{
		VisitPropertyType(Visitor, SetProperty->ElementProp);
	}
	else if (auto MapProperty = Cast<UMapProperty>(Property))
	{
		VisitPropertyType(Visitor, MapProperty->KeyProp);
		VisitPropertyType(Visitor, MapProperty->ValueProp);
	}
	else if (auto DelegateProperty = Cast<UDelegateProperty>(Property))
	{
		VisitFunctionParameters([&](UProperty* Parameter)
		{
			VisitPropertyType(Visitor, Parameter);
		}, DelegateProperty->SignatureFunction);
	}
	else if (auto MulticastDelegateProperty = Cast<UMulticastDelegateProperty>(Property))
	{
		VisitFunctionParameters([&](UProperty* Parameter)
		{
			VisitPropertyType(Visitor, Parameter);
		}, MulticastDelegateProperty->SignatureFunction);
	}
}

bool FTsuReflection::GetReferencesInType(UField* Type, FTsuTypeSet& OutReferences)
{
	if (auto Class = Cast<UClass>(Type))
	{
		if (!IsValidClass(Class))
			return false;
	}

	if (auto ObjectType = Cast<UStruct>(Type))
	{
		VisitProperties([&](UProperty* Property, bool /*bIsReadOnly*/)
		{
			GetReferencesInProperty(Property, OutReferences);
		}, ObjectType);

		VisitMethods([&](UFunction* Function)
		{
			GetReferencesInFunction(Function, OutReferences);
		}, ObjectType);

		VisitExtensionMethods([&](UFunction* Function)
		{
			GetReferencesInFunction(Function, OutReferences);
		}, ObjectType);
	}

	OutReferences.Remove(Type);

	if (auto Struct = Cast<UStruct>(Type))
	{
		if (UStruct* SuperStruct = Struct->GetSuperStruct())
			OutReferences.Add(SuperStruct);
	}

	if (auto Class = Cast<UClass>(Type))
	{
		// For the optional 'outer' parameter in the constructor
		UClass* Class_UObject = UObject::StaticClass();
		if (Class != Class_UObject)
			OutReferences.Add(Class_UObject);

		// For the 'staticClass' member
		UClass* Class_UClass = UClass::StaticClass();
		if (Class != Class_UClass)
			OutReferences.Add(Class_UClass);

		for (FImplementedInterface& Interface : Class->Interfaces)
			OutReferences.Add(Interface.Class);
	}

	return true;
}

void FTsuReflection::GetReferencesInFunction(UFunction* Function, FTsuTypeSet& OutReferences)
{
	VisitFunctionParameters([&](UProperty* Parameter)
	{
		GetReferencesInProperty(Parameter, OutReferences);
	}, Function);

	VisitFunctionReturns([&](UProperty* Return)
	{
		GetReferencesInProperty(Return, OutReferences);
	}, Function);
}

void FTsuReflection::GetReferencesInProperty(UProperty* Property, FTsuTypeSet& OutReferences)
{
	VisitPropertyType([&](UField* ContainedType)
	{
		OutReferences.Add(ContainedType);
	}, Property);
}

UFunction* FTsuReflection::FindMakeFunction(UStruct* Struct)
{
	auto ScriptStruct = Cast<UScriptStruct>(Struct);
	if (!ScriptStruct)
		return nullptr;

	UFunction* Result = nullptr;

	VisitMakeFunctions([&](UFunction* Function, UScriptStruct* ScriptStruct)
	{
		if (ScriptStruct == Struct)
			Result = Function;
	});

	return Result;
}

UFunction* FTsuReflection::FindBreakFunction(UStruct* Struct)
{
	auto ScriptStruct = Cast<UScriptStruct>(Struct);
	if (!ScriptStruct)
		return nullptr;

	UFunction* Result = nullptr;

	VisitBreakFunctions([&](UFunction* Function, UScriptStruct* ScriptStruct)
	{
		if (ScriptStruct == Struct)
			Result = Function;
	});

	return Result;
}

bool FTsuReflection::HasMakeFunction(UStruct* Struct)
{
	return FindMakeFunction(Struct) != nullptr;
}

bool FTsuReflection::HasBreakFunction(UStruct* Struct)
{
	return FindBreakFunction(Struct) != nullptr;
}

UProperty* FTsuReflection::GetParameter(UFunction* Function, int32 Index)
{
	FParamIterator ParamIt{Function};

	while (Index--)
		++ParamIt;

	return *ParamIt;
}

UStruct* FTsuReflection::FindExtendedTypeNonStatic(UFunction* Function)
{
	if (!IsExtensionFunction(Function) || IsStaticExtension(Function))
		return nullptr;

	FParamIterator ParamIt{Function};
	if (!ParamIt)
		return nullptr;

	UProperty* Param = *ParamIt;

	if (!IsExplicitExtension(Function) && !IsInputParameter(Param))
		return nullptr;

	UStruct* Type = nullptr;
	if (auto ObjectProp = Cast<UObjectPropertyBase>(Param))
		return ObjectProp->PropertyClass;
	else if (auto StructProp = Cast<UStructProperty>(Param))
		return StructProp->Struct;

	return nullptr;
}

UStruct* FTsuReflection::FindExtendedTypeStatic(UFunction* Function)
{
	const FString& TypeName = Function->GetMetaData(MetaTsuStaticExtension);
	if (TypeName.IsEmpty())
		return nullptr;

	UField* Type = FindTypeByName(TypeName);
	if (!Type)
	{
		UE_LOG(LogTsuRuntime, Error, TEXT("Failed to find type '%s' for extension '%s::%s'"),
			*TypeName,
			*Function->GetOuter()->GetName(),
			*Function->GetName());

		return nullptr;
	}

	UStruct* Struct = Cast<UStruct>(Type);
	if (!Struct)
	{
		UE_LOG(LogTsuRuntime, Error, TEXT("Non-object type '%s' for extension '%s::%s'"),
			*TypeName,
			*Function->GetOuter()->GetName(),
			*Function->GetName());

		return nullptr;
	}

	return Struct;
}

UStruct* FTsuReflection::FindExtendedType(UFunction* Function)
{
	if (UStruct* Type = FindExtendedTypeStatic(Function))
		return Type;

	if (UStruct* Type = FindExtendedTypeNonStatic(Function))
		return Type;

	return nullptr;
}

UField* FTsuReflection::FindTypeByName(const FString& TypeName)
{
	static FTsuTypeIndex Index;
	return Index.Find(TypeName);
}
