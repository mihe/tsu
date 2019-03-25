#include "TsuTryCatch.h"

#include "TsuRuntimeLog.h"
#include "TsuStringConv.h"
#include "TsuUtilities.h"

#if WITH_EDITOR
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#endif // WITH_EDITOR

FTsuTryCatch::FTsuTryCatch(v8::Isolate* Isolate)
	: Isolate(Isolate)
	, Catcher(Isolate)
{
}

FTsuTryCatch::~FTsuTryCatch()
{
	Check();
}

void FTsuTryCatch::Check()
{
	if (!Catcher.HasCaught())
		return;

	v8::Local<v8::Context> Context = Isolate->GetCurrentContext();
	v8::Local<v8::Message> Exception = Catcher.Message();

	FString Message = TEXT("[V8] ") + V8_TO_TCHAR(Exception->Get());
	FString ScriptName = V8_TO_TCHAR(Exception->GetScriptResourceName().As<v8::String>());
	int32 ScriptLine = Exception->GetLineNumber(Context).ToChecked();
	int32 ScriptColumn = Exception->GetStartColumn(Context).ToChecked();

	UE_LOG(LogTsuRuntime, Error, TEXT("%s"), *Message);
	UE_LOG(LogTsuRuntime, Error, TEXT("    at %s:%d:%d"), *ScriptName, ScriptLine, ScriptColumn);

#if WITH_EDITOR
	static TMap<uint32, TWeakPtr<SNotificationItem>> Notifications;

	const uint32 NotificationHash = TsuHash(*Message);
	const FText NotificationText = FText::FromString(Message);

	TWeakPtr<SNotificationItem>& NotificationWeak = Notifications.FindOrAdd(NotificationHash);
	TSharedPtr<SNotificationItem> Notification = NotificationWeak.Pin();

	if (!Notification.IsValid() || Notification->GetCompletionState() == SNotificationItem::CS_None)
	{
		FNotificationInfo Info{NotificationText};
		Info.ExpireDuration = 10.0f;
		Info.bUseSuccessFailIcons = false;
		Info.bUseLargeFont = false;
		Info.bFireAndForget = true;

		TSharedPtr<SNotificationItem> NewNotification = FSlateNotificationManager::Get().AddNotification(Info);
		NewNotification->SetCompletionState(SNotificationItem::CS_Fail);
		NotificationWeak = NewNotification;
	}
	else
	{
		Notification->SetText(NotificationText);
		Notification->ExpireAndFadeout();
	}
#endif // WITH_EDITOR

	Catcher.Reset();
}
