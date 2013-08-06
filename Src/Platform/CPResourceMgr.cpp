//================================================================================
//
//		CPResourceMgr.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/5/2012
//
//================================================================================

#include "PlatformFramework.h"

_NAMESPACE_BEGIN

IPThread* CPResourceMgr::CreateThread()
{
	CPThread* pThread = DoCreateThread();
	return pThread;
}

IPThreadPool* CPResourceMgr::CreateThreadPool()
{
	CPThreadPool* pPool = DoCreateThreadPool();
	return pPool;
}

_NAMESPACE_END