// Original work - Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// Modified work - Copyright 2019 Mikael Hermansson. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "HAL/PlatformProcess.h"

class FTsuReplProcess final
{
public:
	FTsuReplProcess(const FTsuReplProcess& Other) = delete;
	FTsuReplProcess(FTsuReplProcess&& Other);

	~FTsuReplProcess();

	FTsuReplProcess& operator=(const FTsuReplProcess& Other) = delete;
	FTsuReplProcess& operator=(FTsuReplProcess&& Other) = delete;

	static TOptional<FTsuReplProcess> Launch(
		const FString& ProcessPath,
		const FString& ProcessArgs);

	bool Write(const FString& Input);

	// #todo(#mihe): Move the default values to settings
	TOptional<FString> ReadOutput(double Timeout = 5.f);
	TOptional<FString> ReadError(double Timeout = 5.f);

	bool IsRunning() const;

private:
	FTsuReplProcess(
		FProcHandle ProcessHandle,
		void* StdOutRead,
		void* StdOutWrite,
		void* StdErrRead,
		void* StdErrWrite,
		void* StdInRead,
		void* StdInWrite);

	TOptional<FString> ReadFromPipe(void* Pipe, double Timeout);

	mutable FProcHandle ProcessHandle;
	void* StdOutRead = nullptr;
	void* StdOutWrite = nullptr;
	void* StdErrRead = nullptr;
	void* StdErrWrite = nullptr;
	void* StdInRead = nullptr;
	void* StdInWrite = nullptr;
};
