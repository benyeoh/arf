//==============================================================================
//
//		CoreMemoryFramework.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/8/2008
//
//==============================================================================

#ifdef _WINDOWS
	#include <windows.h>
#endif

#ifndef _NO_CORE_MEMORY_METRICS
	#define _INC_METRIC_NUM(metric, num) m_Metrics.metric+=num
	#define _DEC_METRIC_NUM(metric, num) m_Metrics.metric-=num
#else
	#define _INC_METRIC_NUM(metric, num) (void*)0
	#define _DEC_METRIC_NUM(metric, num) (void*)0
#endif

#define _INC_METRIC(metric) _INC_METRIC_NUM(metric, 1)

#include "CoreMemory.h"

#include "Defines.h"
#include "CMSegregatedListAllocator.h"

_NAMESPACE_BEGIN

extern CMSegregatedListAllocator* g_pSharedAllocator;

void* CoreMemoryAlloc(size_t size, const wchar* pFile, uint lineNum);
void CoreMemoryFree(void* p);
void* CoreMemoryRealloc(void* p, size_t size, const wchar* pFile, uint lineNum);
void* CoreMemoryCalloc(uint count, size_t size, const wchar* pFile, uint lineNum);

_NAMESPACE_END
