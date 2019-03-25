#include "TsuInspectorChannel.h"

FTsuInspectorChannel::FTsuInspectorChannel(
	v8::Isolate* InIsolate,
	v8::Platform* InPlatform,
	v8_inspector::V8Inspector& InInspector)
	: Isolate(InIsolate)
	, Platform(InPlatform)
	, Inspector(InInspector.connect(1, this, {}))
{
}

void FTsuInspectorChannel::ReceiveMessage(const std::string& MessageUtf8)
{
	static_assert(sizeof(TCHAR) == sizeof(uint16_t), "Character size mismatch");

	const FString MessageUtf16 = UTF8_TO_TCHAR(MessageUtf8.c_str());
	const auto MessagePtr = reinterpret_cast<const uint16_t*>(MessageUtf16.GetCharArray().GetData());
	const auto MessageLen = (size_t)MessageUtf16.Len();

	Inspector->dispatchProtocolMessage({MessagePtr, MessageLen});
}

void FTsuInspectorChannel::SendMessage(v8_inspector::StringBuffer& MessageBuffer)
{
	v8_inspector::StringView MessageView = MessageBuffer.string();

	std::string Message;

	if (MessageView.is8Bit())
		Message = reinterpret_cast<const char*>(MessageView.characters8());
	else
		Message = TCHAR_TO_UTF8(MessageView.characters16());

	if (OnSendMessage)
		OnSendMessage(Message);
}

void FTsuInspectorChannel::sendResponse(int /*CallId*/, std::unique_ptr<v8_inspector::StringBuffer> Message)
{
	SendMessage(*Message);
}

void FTsuInspectorChannel::sendNotification(std::unique_ptr<v8_inspector::StringBuffer> Message)
{
	SendMessage(*Message);
}
