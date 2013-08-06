//================================================================================
//
//		CPResourceMgrWin32.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/6/2012
//
//================================================================================

#include "PlatformWin32.h"

_NAMESPACE_BEGIN

CPThread* CPResourceMgrWin32::DoCreateThread()
{
	CPThreadWin32* pThread = _NEW CPThreadWin32();
	return pThread;
}

CPThreadPool* CPResourceMgrWin32::DoCreateThreadPool()
{
	CPThreadPoolWin32* pPool = _NEW CPThreadPoolWin32();
	return pPool;
}

_NAMESPACE_END