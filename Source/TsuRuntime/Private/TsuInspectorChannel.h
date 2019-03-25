#pragma once

#include "CoreMinimal.h"

#include "TsuV8Wrapper.h"

#include "Containers/Queue.h"

class FTsuInspectorChannel final
	: public v8_inspector::V8Inspector::Channel
{
	friend class FTsuInspectorChannelTask;

public:
	FTsuInspectorChannel(
		v8::Isolate* Isolate,
		v8::Platform* Platform,
		v8_inspector::V8Inspector& Inspector);

	void ReceiveMessage(const std::string& Message);

	TFunction<void(const std::string&)> OnSendMessage;

private:
	void SendMessage(v8_inspector::StringBuffer& MessageBuffer);

	void sendResponse(int CallId, std::unique_ptr<v8_inspector::StringBuffer> Message) override;
	void sendNotification(std::unique_ptr<v8_inspector::StringBuffer> Message) override;
	void flushProtocolNotifications() override {}

	v8::Isolate* Isolate = nullptr;
	v8::Platform* Platform = nullptr;
	std::unique_ptr<v8_inspector::V8InspectorSession> Inspector;
};

using FTsuInspectorChannelPtr = TSharedPtr<FTsuInspectorChannel>;
