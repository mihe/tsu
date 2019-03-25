#include "TsuIsolate.h"

#include "TsuHeapStats.h"
#include "TsuRuntimeLog.h"

v8::Isolate* FTsuIsolate::Get()
{
	static v8::Isolate* Isolate = []
	{
		v8::V8::SetDcheckErrorHandler(
			[](const char* File, int Line, const char* Message)
			{
				UE_LOG(LogTsuRuntime,
					Fatal,
					TEXT("%s(%d): %s"),
					UTF8_TO_TCHAR(File),
					Line,
					UTF8_TO_TCHAR(Message));
			});

		v8::V8::InitializePlatform(GetPlatform());
		v8::V8::Initialize();

		v8::ResourceConstraints Constraints;
		Constraints.ConfigureDefaults(
			FPlatformMemory::GetConstants().TotalPhysical,
			FPlatformMemory::GetConstants().TotalVirtual);

		v8::Isolate::CreateParams Params;
		Params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
		Params.constraints = Constraints;

		v8::Isolate* Result = v8::Isolate::New(Params);
		Result->SetFatalErrorHandler(
			[](const char* Location, const char* Message)
			{
				UE_LOG(LogTsuRuntime,
					Fatal,
					TEXT("%s: %s"),
					UTF8_TO_TCHAR(Location),
					UTF8_TO_TCHAR(Message));
			});

		Result->Enter();

		static FTsuHeapStats HeapStats{Result};

		return Result;
	}();

	return Isolate;
}

v8::Platform* FTsuIsolate::GetPlatform()
{
	static std::unique_ptr<v8::Platform> Instance = v8::platform::NewDefaultPlatform();
	return Instance.get();
}
