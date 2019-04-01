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

	const FString Message = TEXT("[V8] ") + V8_TO_TCHAR(Exception->Get());

	UE_LOG(LogTsuRuntime, Error, TEXT("%s"), *Message);

	v8::Local<v8::StackTrace> StackTrace = Exception->GetStackTrace();
	const int32 NumFrames = StackTrace->GetFrameCount();

	for (int32 Index = 0; Index < NumFrames; ++Index)
	{
		v8::Local<v8::StackFrame> StackFrame = StackTrace->GetFrame(Isolate, Index);

		const FString ScriptName = V8_TO_TCHAR(StackFrame->GetScriptName());
		const FString FunctionName = V8_TO_TCHAR(StackFrame->GetFunctionName());
		const int32 Line = StackFrame->GetLineNumber();
		const int32 Column = StackFrame->GetColumn();

		UE_LOG(LogTsuRuntime, Error, TEXT("    at %s:%s:%d:%d"), *ScriptName, *FunctionName, Line, Column);
	}

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
