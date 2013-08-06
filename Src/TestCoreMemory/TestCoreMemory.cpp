// TestCoreMemory.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

_USE_NAMESPACE

ICMMemAllocatorPtr g_pAllocator;

_DECL_CORE_MEM_ALLOC_FNS(g_pAllocator)

int _tmain(int argc, _TCHAR* argv[])
{
	Module hCoreMem = InitializeModule(_W("CoreMemory.dll"), NULL);
	
	CreateMemAllocatorFn CallCreateMemAllocator = (CreateMemAllocatorFn) GetFunction(hCoreMem, "CreateMemAllocator");
	g_pAllocator = CallCreateMemAllocator();

	_DEBUG_ASSERT(g_pAllocator);
	MemAllocFns allocFns = GetCoreMemoryBindings();
	_BIND_TO_GLOBAL_ALLOC(allocFns);
	
	const static uint NUM_ALLOCS = 16384*2;
		
	double* pTest[NUM_ALLOCS];

	_LOOPi(NUM_ALLOCS)
	{
		pTest[i] = NULL;
	}
	
	_LOOPi(332123)
	{
		uint index = rand() % NUM_ALLOCS;
		if(pTest[index])
		{
			_FREE(pTest[index]);
			pTest[index] = NULL;
		}
		else
		{
			uint size =  rand() % 2048;
			if(size < 8) size = 8;
			pTest[index] = (double*) _MALLOC(size);
			memset(pTest[index], 0x88, size);
			pTest[index][0] = 1.0;
			_DEBUG_ASSERT( ((size_t) pTest[index]) % 8 == 0 );
			double val = *(pTest[index]);
		}		
	}

	_TRACE(_W("<<<<<<<<<<<<<<<<<<<<<<< BEFORE >>>>>>>>>>>>>>>>>>>>>>>\n"));
	CMMetrics metrics = g_pAllocator->GetMetrics();
	_TRACE(_W("metrics.numPageAllocs: %d\n"), metrics.numPageAllocs);
	_TRACE(_W("metrics.numChunkAllocs: %d\n"), metrics.numChunkAllocs);
	_TRACE(_W("metrics.numPagesFree: %d\n"), metrics.numPagesFree);
	_TRACE(_W("metrics.totalChunkSizeUsed: %d\n"), metrics.totalChunkSizeUsed);
	_TRACE(_W("metrics.totalHeapSizeUsed: %d\n"), metrics.totalHeapSizeUsed);
	_TRACE(_W("metrics.totalPageSizeUsed: %d\n"), metrics.totalPageSizeUsed);
	
	_LOOPi(NUM_ALLOCS)
	{
		if(pTest[i])
		{
			_FREE(pTest[i]);
		}
	}

	_TRACE(_W("<<<<<<<<<<<<<<<<<<<<<<< AFTER >>>>>>>>>>>>>>>>>>>>>>>\n"));
	metrics = g_pAllocator->GetMetrics();
	_TRACE(_W("metrics.numPageAllocs: %d\n"), metrics.numPageAllocs);
	_TRACE(_W("metrics.numChunkAllocs: %d\n"), metrics.numChunkAllocs);
	_TRACE(_W("metrics.numPagesFree: %d\n"), metrics.numPagesFree);
	_TRACE(_W("metrics.totalChunkSizeUsed: %d\n"), metrics.totalChunkSizeUsed);
	_TRACE(_W("metrics.totalHeapSizeUsed: %d\n"), metrics.totalHeapSizeUsed);
	_TRACE(_W("metrics.totalPageSizeUsed: %d\n"), metrics.totalPageSizeUsed);

	//getchar();
	
	g_pAllocator->DumpLeaks();
	g_pAllocator = NULL;

	ShutdownModule(hCoreMem);
	
	_DUMP_LEAKS;
	
	return 0;
}

