#include "TsuParser.h"

#include "TsuEditorLog.h"
#include "TsuPaths.h"
#include "TsuReplProcess.h"

#include "Containers/Queue.h"
#include "HAL/PlatformProcess.h"
#include "JsonObjectConverter.h"
#include "Misc/ScopeExit.h"

bool FTsuParser::Parse(const FString& FilePath, FTsuParsedFile& Response)
{
	static TOptional<FTsuReplProcess> Process;

	if (Process && !Process->IsRunning())
	{
		UE_LOG(LogTsuEditor, Error, TEXT("Parser found to have terminated unexpectedly"));

		if (const TOptional<FString> Error = Process->ReadError())
		{
			UE_LOG(LogTsuEditor, Error, TEXT("\n%s"), *Error.GetValue());
		}

		Process.Reset();
	}

	if (!Process)
	{
		UE_LOG(LogTsuEditor, Log, TEXT("Starting parser..."));

		const FString ParserPath = FTsuPaths::ParserPath();
		const FString ParserArgs = FString::Printf(TEXT("\"%s\""), *FTsuPaths::ScriptsDir());
		Process = FTsuReplProcess::Launch(FTsuPaths::ParserPath(), ParserArgs);

		if (!Process)
		{
			UE_LOG(LogTsuEditor, Error, TEXT("Failed to launch parser"));
			return false;
		}
	}

	FTsuParserRequest Request;
	Request.File = FilePath;

	FString RequestJson;
	if (!FJsonObjectConverter::UStructToJsonObjectString(Request, RequestJson, 0, 0, 0, nullptr, false))
	{
		UE_LOG(LogTsuEditor, Error, TEXT("Failed to serialize request"));
		return false;
	}

	if (!Process->Write(RequestJson))
	{
		UE_LOG(LogTsuEditor, Error, TEXT("Failed to write parser request"));
		UE_LOG(LogTsuEditor, Log, TEXT("Terminating parser..."));

		Process.Reset();
		return false;
	}

	const TOptional<FString> ResponseJson = Process->ReadOutput();
	if (!ResponseJson)
	{
		UE_LOG(LogTsuEditor, Error, TEXT("Parser request timed out"));

		if (TOptional<FString> Error = Process->ReadError())
		{
			UE_LOG(LogTsuEditor, Error, TEXT("\n%s"), *Error.GetValue());
		}

		UE_LOG(LogTsuEditor, Log, TEXT("Terminating parser..."));

		Process.Reset();
		return false;
	}

	if (!FJsonObjectConverter::JsonObjectStringToUStruct(ResponseJson.GetValue(), &Response, 0, 0))
	{
		UE_LOG(LogTsuEditor, Error, TEXT("Failed to deserialize response: '%s'"), *ResponseJson.GetValue());
		return false;
	}

	return true;
}
