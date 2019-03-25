#pragma once

#include "CoreMinimal.h"

#include "TsuInspectorChannel.h"
#include "TsuV8Wrapper.h"
#include "TsuWsppWrapper.h"

#include "Containers/Ticker.h"

class FTsuInspectorClient final
	: public v8_inspector::V8InspectorClient
	, public FTickerObjectBase
{
public:
	FTsuInspectorClient(v8::Platform* Platform, v8::Isolate* Isolate);

	FTsuInspectorClient(const FTsuInspectorClient& Other) = delete;
	FTsuInspectorClient(FTsuInspectorClient&& Other) = delete;

	FTsuInspectorClient& operator=(const FTsuInspectorClient& Other) = delete;
	FTsuInspectorClient& operator=(FTsuInspectorClient&& Other) = delete;

	void RegisterContext(v8::Local<v8::Context> Context);
	void UnregisterContext(v8::Local<v8::Context> Context);

private:
	void OnSocketHttp(wspp::connection_hdl Handle);
	void OnSocketOpen(wspp::connection_hdl Handle);
	void OnSocketReceive(wspp::connection_hdl Handle, wspp_message_ptr Message);
	void OnSocketSend(wspp::connection_hdl Handle, const std::string& Message);
	void OnSocketClose(wspp::connection_hdl Handle);
	void OnSocketFail(wspp::connection_hdl Handle);

	bool Tick(float DeltaTime = 0.f) override;

	void runMessageLoopOnPause(int ContextGroupId) override;
	void quitMessageLoopOnPause() override;

	static void LogError(const TCHAR* ErrorMessage);
	static void LogError(const ANSICHAR* ErrorMessage);

	v8::Platform* Platform = nullptr;
	v8::Isolate* Isolate = nullptr;
	std::unique_ptr<v8_inspector::V8Inspector> Inspector;
	wspp_server Server;
	TOptional<FTsuInspectorChannel> Channel;
	bool bIsPaused = false;
};
