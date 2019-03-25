#include "TsuHeapStats.h"

#include "Stats/Stats.h"

DECLARE_MEMORY_STAT(TEXT("V8 (Total Heap Size)"), STAT_V8MemoryTotalHeapSize, STATGROUP_Memory);
DECLARE_MEMORY_STAT(TEXT("V8 (Used Heap Size)"), STAT_V8MemoryUsedHeapSize, STATGROUP_Memory);
DECLARE_MEMORY_STAT(TEXT("V8 (Malloc Memory)"), STAT_V8MemoryMallocMemory, STATGROUP_Memory);
DECLARE_MEMORY_STAT(TEXT("V8 (Peak Malloc Memory)"), STAT_V8MemoryPeakMallocMemory, STATGROUP_Memory);

FTsuHeapStats::FTsuHeapStats(v8::Isolate* InIsolate)
	: FTickerObjectBase(1.f)
	, Isolate(InIsolate)
{
}

bool FTsuHeapStats::Tick(float DeltaTime)
{
	v8::HeapStatistics HeapStatistics;
	Isolate->GetHeapStatistics(&HeapStatistics);

	SET_MEMORY_STAT(STAT_V8MemoryTotalHeapSize, HeapStatistics.total_heap_size());
	SET_MEMORY_STAT(STAT_V8MemoryUsedHeapSize, HeapStatistics.used_heap_size());
	SET_MEMORY_STAT(STAT_V8MemoryMallocMemory, HeapStatistics.malloced_memory());
	SET_MEMORY_STAT(STAT_V8MemoryPeakMallocMemory, HeapStatistics.peak_malloced_memory());

	return true;
}
