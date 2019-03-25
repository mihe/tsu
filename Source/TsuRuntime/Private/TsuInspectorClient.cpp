#include "TsuInspectorClient.h"

#include "TsuRuntimeLog.h"

// #todo(#mihe): Should be the actual stringified port
constexpr char* JsonList = R"([{
  "description": "TSU instance",
  "id": "2e0d32cf-212c-4f25-9ec8-4896e652513a",
  "title": "TSU",
  "type": "node",
  "webSocketDebuggerUrl": "ws://127.0.0.1:19216/"
}])";

// #todo(#mihe): Should be the actual stringified version number
constexpr char* JsonVersion = R"({
  "Browser": "TSU/v0.1.0",
  "Protocol-Version": "1.1"
})";

DEFINE_LOG_CATEGORY_STATIC(LogWebSocketPP, Log, All);

FTsuInspectorClient::FTsuInspectorClient(v8::Platform* InPlatform, v8::Isolate* InIsolate)
	: FTickerObjectBase(0.f)
	, Platform(InPlatform)
	, Isolate(InIsolate)
{
	Inspector = v8_inspector::V8Inspector::create(Isolate, this);

	try
	{
		// #todo(#mihe): Should set_ostream for both of these
		Server.set_access_channels(wspp::log::alevel::none);
		Server.set_error_channels(wspp::log::elevel::none);

		Server.set_http_handler([this](wspp::connection_hdl Handle)
		{
			OnSocketHttp(Handle);
		});

		Server.set_fail_handler([this](wspp::connection_hdl Handle)
		{
			OnSocketFail(Handle);
		});

		Server.set_open_handler([this](wspp::connection_hdl Handle)
		{
			OnSocketOpen(Handle);
		});

		Server.set_close_handler([this](wspp::connection_hdl Handle)
		{
			OnSocketClose(Handle);
		});

		Server.set_message_handler([this](wspp::connection_hdl Handle, wspp_message_ptr Message)
		{
			OnSocketReceive(Handle, Message);
		});

		Server.init_asio();
		Server.listen(19216);
		Server.start_accept();
	}
	catch (const wspp::exception& Exception)
	{
		LogError(Exception.what());
	}
}

void FTsuInspectorClient::RegisterContext(v8::Local<v8::Context> Context)
{
	const uint8_t ContextName[] = "TSU";
	v8_inspector::StringView ContextNameView(ContextName, sizeof(ContextName) - 1);
	Inspector->contextCreated(v8_inspector::V8ContextInfo(Context, 1, ContextNameView));
}

void FTsuInspectorClient::UnregisterContext(v8::Local<v8::Context> Context)
{
	Inspector->contextDestroyed(Context);
}

void FTsuInspectorClient::OnSocketHttp(wspp::connection_hdl Handle)
{
	try
	{
		auto Connection = Server.get_con_from_hdl(Handle);

		const std::string& Resource = Connection->get_resource();

		if (Resource == "/json" || Resource == "/json/list")
		{
			Connection->set_body(JsonList);
			Connection->set_status(wspp::http::status_code::ok);
		}
		else if (Resource == "/json/version")
		{
			Connection->set_body(JsonVersion);
			Connection->set_status(wspp::http::status_code::ok);
		}
		else
		{
			Connection->set_body("404 Not Found");
			Connection->set_status(wspp::http::status_code::not_found);
			checkNoEntry();
		}
	}
	catch (const wspp::exception& Exception)
	{
		LogError(Exception.what());
	}
}

void FTsuInspectorClient::OnSocketOpen(wspp::connection_hdl Handle)
{
	Channel.Emplace(Isolate, Platform, *Inspector);

	Channel->OnSendMessage = [this, Handle](const std::string& Message)
	{
		OnSocketSend(Handle, Message);
	};
}

void FTsuInspectorClient::OnSocketReceive(wspp::connection_hdl Handle, wspp_message_ptr Message)
{
	Channel->ReceiveMessage(Message->get_payload());
}

void FTsuInspectorClient::OnSocketSend(wspp::connection_hdl Handle, const std::string& Message)
{
	try
	{
		Server.send(Handle, Message, wspp::frame::opcode::TEXT);
	}
	catch (const wspp::exception& Exception)
	{
		LogError(Exception.what());
	}
}

void FTsuInspectorClient::OnSocketClose(wspp::connection_hdl Handle)
{
	Channel.Reset();
}

void FTsuInspectorClient::OnSocketFail(wspp::connection_hdl /*Handle*/)
{
	checkNoEntry();
}

bool FTsuInspectorClient::Tick(float /*DeltaTime*/)
{
	try
	{
		Server.poll();
	}
	catch (const wspp::exception& Exception)
	{
		LogError(Exception.what());
	}

	return true;
}

void FTsuInspectorClient::runMessageLoopOnPause(int /*ContextGroupId*/)
{
	if (bIsPaused)
		return;

	bIsPaused = true;

	while (bIsPaused)
		Tick();
}

void FTsuInspectorClient::quitMessageLoopOnPause()
{
	bIsPaused = false;
}

void FTsuInspectorClient::LogError(const TCHAR* ErrorMessage)
{
	UE_LOG(LogWebSocketPP, Error, TEXT("%s"), ErrorMessage);
}

void FTsuInspectorClient::LogError(const ANSICHAR* ErrorMessage)
{
	LogError(UTF8_TO_TCHAR(ErrorMessage));
}
