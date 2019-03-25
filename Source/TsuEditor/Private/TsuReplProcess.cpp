// Original work - Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// Modified work - Copyright 2019 Mikael Hermansson. All Rights Reserved.

#include "TsuReplProcess.h"

#include "TsuEditorLog.h"

#include "HAL/PlatformTime.h"
#include "HAL/RunnableThread.h"

#if PLATFORM_WINDOWS

#include "Windows/AllowWindowsPlatformTypes.h"

static bool CreateOutputPipes(void*& ReadPipe, void*& WritePipe)
{
	SECURITY_ATTRIBUTES Security = {};
	Security.nLength = sizeof(SECURITY_ATTRIBUTES);
	Security.bInheritHandle = true;

	if (!::CreatePipe(&ReadPipe, &WritePipe, &Security, 0))
		return false;

	if (!::SetHandleInformation(ReadPipe, HANDLE_FLAG_INHERIT, 0))
		return false;

	return true;
}

static bool CreateInputPipes(void*& ReadPipe, void*& WritePipe)
{
	SECURITY_ATTRIBUTES Security = {};
	Security.nLength = sizeof(SECURITY_ATTRIBUTES);
	Security.bInheritHandle = true;

	if (!::CreatePipe(&ReadPipe, &WritePipe, &Security, 0))
		return false;

	if (!::SetHandleInformation(WritePipe, HANDLE_FLAG_INHERIT, 0))
		return false;

	return true;
}

static FProcHandle CreateProc(
	const TCHAR* URL,
	const TCHAR* Parms,
	void* StdOutWrite,
	void* StdErrWrite,
	void* StdInRead)
{
	SECURITY_ATTRIBUTES Security = {};
	Security.nLength = sizeof(SECURITY_ATTRIBUTES);
	Security.bInheritHandle = true;

	STARTUPINFO Startup = {};
	Startup.cb = sizeof(STARTUPINFO);
	Startup.dwX = CW_USEDEFAULT;
	Startup.dwY = CW_USEDEFAULT;
	Startup.dwXSize = CW_USEDEFAULT;
	Startup.dwYSize = CW_USEDEFAULT;
	Startup.dwFlags = (STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES);
	Startup.wShowWindow = SW_HIDE;
	Startup.hStdInput = (::HANDLE)StdInRead;
	Startup.hStdOutput = (::HANDLE)StdOutWrite;
	Startup.hStdError = (::HANDLE)StdErrWrite;

	FString CommandLine = FString::Printf(TEXT("\"%s\" %s"), URL, Parms);

	PROCESS_INFORMATION Process = {};

	if (!CreateProcess(
			NULL, // lpApplicationName
			&CommandLine[0], // lpCommandLine
			&Security, // lpProcessAttributes
			&Security, // lpThreadAttributes
			true, // bInheritHandles
			NORMAL_PRIORITY_CLASS, // dwCreationFlags
			NULL, // lpEnvironment
			nullptr, // lpCurrentDirectory
			&Startup, // lpStartupInfo
			&Process)) // lpProcessInformation
	{
		return FProcHandle();
	}

	::CloseHandle(Process.hThread);

	return FProcHandle(Process.hProcess);
}

#include "Windows/HideWindowsPlatformTypes.h"

#else // PLATFORM_WINDOWS

static bool CreateOutputPipes(void*& ReadPipe, void*& WritePipe)
{
	return FPlatformProcess::CreatePipe(ReadPipe, WritePipe);
}

static bool CreateInputPipes(void*& ReadPipe, void*& WritePipe)
{
	return FPlatformProcess::CreatePipe(ReadPipe, WritePipe);
}

static FProcHandle CreateProc(
	const TCHAR* URL,
	const TCHAR* Parms,
	void* StdOutWrite,
	void* StdErrWrite,
	void* StdInRead)
{
#error Not implemented
}

#endif // PLATFORM_WINDOWS

FTsuReplProcess::FTsuReplProcess(
	FProcHandle InProcessHandle,
	void* InStdOutRead,
	void* InStdOutWrite,
	void* InStdErrRead,
	void* InStdErrWrite,
	void* InStdInRead,
	void* InStdInWrite)
	: ProcessHandle(InProcessHandle)
	, StdOutRead(InStdOutRead)
	, StdOutWrite(InStdOutWrite)
	, StdErrRead(InStdErrRead)
	, StdErrWrite(InStdErrWrite)
	, StdInRead(InStdInRead)
	, StdInWrite(InStdInWrite)
{
}

FTsuReplProcess::FTsuReplProcess(FTsuReplProcess&& Other)
	: ProcessHandle(Other.ProcessHandle)
	, StdOutRead(Other.StdOutRead)
	, StdOutWrite(Other.StdOutWrite)
	, StdErrRead(Other.StdErrRead)
	, StdErrWrite(Other.StdErrWrite)
	, StdInRead(Other.StdInRead)
	, StdInWrite(Other.StdInWrite)
{
	Other.ProcessHandle.Reset();
}

FTsuReplProcess::~FTsuReplProcess()
{
	if (ProcessHandle.IsValid())
	{
		FPlatformProcess::ClosePipe(StdOutRead, StdOutWrite);
		FPlatformProcess::ClosePipe(StdErrRead, StdErrWrite);
		FPlatformProcess::ClosePipe(StdInRead, StdInWrite);

		FPlatformProcess::TerminateProc(ProcessHandle);
		FPlatformProcess::CloseProc(ProcessHandle);
	}
}

TOptional<FTsuReplProcess> FTsuReplProcess::Launch(
	const FString& ProcessPath,
	const FString& ProcessArgs)
{
	// #todo(#mihe): This function lacks proper cleanup on failure

	void* StdOutRead = nullptr;
	void* StdOutWrite = nullptr;
	if (!CreateOutputPipes(StdOutRead, StdOutWrite))
	{
		UE_LOG(LogTsuEditor, Error, TEXT("Failed to create output pipes"));
		return {};
	}

	void* StdErrRead = nullptr;
	void* StdErrWrite = nullptr;
	if (!CreateOutputPipes(StdErrRead, StdErrWrite))
	{
		UE_LOG(LogTsuEditor, Error, TEXT("Failed to create error pipes"));
		return {};
	}

	void* StdInRead = nullptr;
	void* StdInWrite = nullptr;
	if (!CreateInputPipes(StdInRead, StdInWrite))
	{
		UE_LOG(LogTsuEditor, Error, TEXT("Failed to create input pipes"));
		return {};
	}

	FProcHandle ProcessHandle = CreateProc(
		*ProcessPath,
		*ProcessArgs,
		StdOutWrite,
		StdErrWrite,
		StdInRead);

	if (!ProcessHandle.IsValid())
	{
		UE_LOG(LogTsuEditor, Error, TEXT("Failed to create process"));
		return {};
	}

#if PLATFORM_WINDOWS
	static HANDLE JobHandle = []
	{
		HANDLE Result = CreateJobObject(NULL, NULL);
		check(Result != NULL);
		JOBOBJECT_EXTENDED_LIMIT_INFORMATION LimitInformation = {};
		LimitInformation.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
		verify(SetInformationJobObject(Result, JobObjectExtendedLimitInformation, &LimitInformation, sizeof(LimitInformation)));
		return Result;
	}();

	if (!AssignProcessToJobObject(JobHandle, ProcessHandle.Get()))
	{
		UE_LOG(LogTsuEditor, Error, TEXT("Failed to tie process to editor lifetime"));
		return {};
	}
#else // PLATFORM_WINDOWS
#error Not implemented
#endif // PLATFORM_WINDOWS

	return FTsuReplProcess(
		ProcessHandle,
		StdOutRead,
		StdOutWrite,
		StdErrRead,
		StdErrWrite,
		StdInRead,
		StdInWrite);
}

bool FTsuReplProcess::Write(const FString& Input)
{
	return FPlatformProcess::WritePipe(StdInWrite, Input);
}

TOptional<FString> FTsuReplProcess::ReadOutput(double Timeout)
{
	return ReadFromPipe(StdOutRead, Timeout);
}

TOptional<FString> FTsuReplProcess::ReadError(double Timeout)
{
	return ReadFromPipe(StdErrRead, Timeout);
}

TOptional<FString> FTsuReplProcess::ReadFromPipe(void* Pipe, double Timeout)
{
	const double StartTime = FPlatformTime::Seconds();

	FString Output;

	do
	{
		const double ElapsedTime = FPlatformTime::Seconds() - StartTime;
		if (ElapsedTime >= Timeout)
			return {};

		Output = FPlatformProcess::ReadPipe(Pipe);
	} while (Output.IsEmpty());

	FString Remainder;

	do
	{
		Remainder = FPlatformProcess::ReadPipe(Pipe);
		Output += Remainder;
	} while (!Remainder.IsEmpty());

	return Output;
}

bool FTsuReplProcess::IsRunning() const
{
	return FPlatformProcess::IsProcRunning(ProcessHandle);
}
