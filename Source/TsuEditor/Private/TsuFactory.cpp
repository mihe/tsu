#include "TsuFactory.h"

#include "TsuBlueprint.h"
#include "TsuBlueprintCompiler.h"
#include "TsuBlueprintGeneratedClass.h"
#include "TsuEditorLog.h"
#include "TsuEditorStyle.h"
#include "TsuHotReloadListenerInterface.h"

#include "Containers/Set.h"
#include "Editor.h"
#include "EditorFramework/AssetImportData.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Widgets/Notifications/SNotificationList.h"

namespace TsuFactory_Private
{

void ShowCompileNotification(const FString& Message, SNotificationItem::ECompletionState State)
{
	FNotificationInfo Info(FText::FromString(Message));
	Info.Image = FTsuEditorStyle::Get().GetBrush(TEXT("ClassIcon.TsuBlueprint.Large"));
	Info.FadeInDuration = 0.1f;
	Info.FadeOutDuration = 0.5f;
	Info.ExpireDuration = 10.0f;
	Info.bUseThrobber = false;
	Info.bUseSuccessFailIcons = true;
	Info.bUseLargeFont = false;
	Info.bFireAndForget = true;
	Info.bAllowThrottleWhenFrameRateIsLow = false;

	TSharedPtr<SNotificationItem> NotificationItem = FSlateNotificationManager::Get().AddNotification(Info);
	NotificationItem->SetCompletionState(State);
}

template<typename FunctionType>
void ForEachHotReloadListener(FunctionType&& Fn)
{
	UClass* ListenerInterface = UTsuHotReloadListenerInterface::StaticClass();

	for (auto Object : TObjectRange<UObject>(RF_NoFlags))
	{
		if (Object->IsPendingKill())
			continue;

		UClass* ObjectClass = Object->GetClass();
		if (ObjectClass->ImplementsInterface(ListenerInterface))
		{
			if (!Object->HasAnyFlags(RF_ClassDefaultObject) || ObjectClass->IsChildOf<UBlueprintFunctionLibrary>())
			{
				Fn(Object);
			}
		}
	}
}

void BroadcastPostHotReload()
{
	ForEachHotReloadListener([](UObject* Listener)
	{
		ITsuHotReloadListenerInterface::Execute_PostHotReload(Listener);
	});
}

} // namespace TsuFactory_Private

UTsuFactory::UTsuFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UTsuBlueprint::StaticClass();
	Formats.Add("ts;TypeScript");
	bCreateNew = false;
	bEditorImport = true;
	bText = true;
	bEditAfterNew = true;
}

UObject* UTsuFactory::FactoryCreateText(
	UClass* Class,
	UObject* Parent,
	FName Name,
	EObjectFlags /*Flags*/,
	UObject* /*Context*/,
	const TCHAR* Type,
	const TCHAR*& /*Buffer*/,
	const TCHAR* /*BufferEnd*/,
	FFeedbackContext* /*Warn*/)
{
	if (auto Blueprint = FindObject<UTsuBlueprint>(Parent, *Name.ToString()))
		return UpdateBlueprint(Blueprint, Class, Parent, Name, Type);

	return CreateBlueprint(Class, Parent, Name, Type);
}

UTsuBlueprint* UTsuFactory::CreateBlueprint(UClass* Class, UObject* Parent, FName Name, const TCHAR* Type)
{
	auto Blueprint = Cast<UTsuBlueprint>(
		FKismetEditorUtilities::CreateBlueprint(
			UBlueprintFunctionLibrary::StaticClass(),
			Parent,
			Name,
			BPTYPE_Normal,
			UTsuBlueprint::StaticClass(),
			UTsuBlueprintGeneratedClass::StaticClass(),
			TEXT("UTsuFactory")));

	FEditorDelegates::OnAssetPreImport.Broadcast(this, Class, Parent, Name, Type);

	Blueprint->AssetImportData->Update(CurrentFilename);

	FKismetEditorUtilities::CompileBlueprint(Blueprint);

	FEditorDelegates::OnAssetPostImport.Broadcast(this, Blueprint);

	return Blueprint;
}

UTsuBlueprint* UTsuFactory::UpdateBlueprint(
	UTsuBlueprint* Blueprint,
	UClass* Class,
	UObject* Parent,
	FName Name,
	const TCHAR* Type)
{
	static TSet<UTsuBlueprint*> DeferredCompilations;
	static FDelegateHandle EndPIEHandle = []
	{
		return FEditorDelegates::EndPIE.AddLambda([&](bool /*bInSimulateInEditor*/)
		{
			for (UTsuBlueprint* Blueprint : DeferredCompilations)
				FKismetEditorUtilities::CompileBlueprint(Blueprint);

			DeferredCompilations.Empty();
		});
	}();

	FEditorDelegates::OnAssetPreImport.Broadcast(this, Class, Parent, Name, Type);

	Blueprint->Modify(true);

	if (GEditor && GEditor->PlayWorld)
	{
		auto GeneratedClass = Cast<UTsuBlueprintGeneratedClass>(Blueprint->GeneratedClass);

		bool bHasErrors = false;
		bool bHasWarnings = false;

		TArray<FTsuCompilationMessage> Messages = FTsuBlueprintCompiler::CompileScript(CurrentFilename, GeneratedClass->Exports);
		for (const FTsuCompilationMessage& Message : Messages)
		{
			switch (Message.Severity)
			{
			case ETsuSeverity::Warning:
				bHasWarnings = true;
				break;
			case ETsuSeverity::Error:
				bHasErrors = true;
				break;
			}
		}

		if (!bHasErrors)
		{
			DeferredCompilations.Add(Blueprint);
			GeneratedClass->ReloadModule();
			TsuFactory_Private::BroadcastPostHotReload();
		}

		if (bHasErrors)
		{
			TsuFactory_Private::ShowCompileNotification(
				FString::Printf(
					TEXT("Error(s) compiling '%s'"),
					*GeneratedClass->ClassGeneratedBy->GetName()),
				SNotificationItem::CS_Fail);
		}
		else if (bHasWarnings)
		{
			TsuFactory_Private::ShowCompileNotification(
				FString::Printf(
					TEXT("Warning(s) compiling '%s'"),
					*GeneratedClass->ClassGeneratedBy->GetName()),
				SNotificationItem::CS_Fail);
		}
	}
	else
	{
		FKismetEditorUtilities::CompileBlueprint(Blueprint);
	}

	FEditorDelegates::OnAssetPostImport.Broadcast(this, Blueprint);

	return Blueprint;
}
