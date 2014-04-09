//================================================================================
//
//		CPThreadPoolWin32.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/12/2012
//
//================================================================================

#include "PlatformWin32.h"

_NAMESPACE_BEGIN

const static int MAX_SEMAPHORE_COUNT = 512;
const static int MAX_RELEASE_COUNT = MAX_SEMAPHORE_COUNT >> 1;

static int s_ThreadLocalIndex = -1;
static int s_ThreadLocalRef = 0;

CPThreadPoolWin32::CPThreadPoolWin32()
	: CPThreadPool()
	, m_Semaphore(0)
{
}

CPThreadPoolWin32::~CPThreadPoolWin32()
{
	Shutdown();
}

void CPThreadPoolWin32::DoYieldThread()
{
	SwitchToThread();
}

void CPThreadPoolWin32::DoWaitThread()
{
	DWORD ret = WaitForSingleObject(m_Semaphore, INFINITE);
	_DEBUG_ASSERT(ret == WAIT_OBJECT_0);
}

void CPThreadPoolWin32::DoSignalThreads(uint numThreads)
{
	BOOL ret = ReleaseSemaphore(m_Semaphore, numThreads, NULL);
	_DEBUG_ASSERT(ret);
}

void CPThreadPoolWin32::DoSignalAllThreads()
{
	// NOTE: We just signal all threads permanently since
	// this is only used on pool shutdown
	BOOL ret = ReleaseSemaphore(m_Semaphore, MAX_RELEASE_COUNT, NULL);
	_DEBUG_ASSERT(ret);
}

void CPThreadPoolWin32::DoWaitOnFinishedEvent()
{
	DWORD ret = WaitForSingleObject(m_Event, INFINITE);
	_DEBUG_ASSERT(ret != WAIT_FAILED);
}

void CPThreadPoolWin32::DoSignalOnFinishedEvent()
{
	BOOL ret = SetEvent(m_Event);
	_DEBUG_ASSERT(ret);
}

void CPThreadPoolWin32::DoResetOnFinishedEvent()
{
	BOOL ret = ResetEvent(m_Event);
	_DEBUG_ASSERT(ret);
}

void CPThreadPoolWin32::DoInitialize(uint queueSizePow2, IPThread** pThreads, uint numThreads)
{
	_DEBUG_ASSERT(numThreads < MAX_RELEASE_COUNT);
	m_Semaphore = CreateSemaphore(NULL, 0, MAX_SEMAPHORE_COUNT, NULL);
	_DEBUG_ASSERT(m_Semaphore);

	m_Event = CreateEvent(NULL, TRUE, TRUE, NULL);
	_DEBUG_ASSERT(m_Event);

	if(s_ThreadLocalRef == 0)
	{
		s_ThreadLocalRef++;
		s_ThreadLocalIndex = TlsAlloc();

		// NOTE: Main thread will always have thread index of 0
		// (Initializing thread should always be the main thread)
		DoSetCurrentThreadIndex(0);
	}
}

void CPThreadPoolWin32::DoShutdown()
{
	s_ThreadLocalRef--;
	if(s_ThreadLocalRef == 0)
		TlsFree(s_ThreadLocalIndex);

	CloseHandle(m_Event);
	CloseHandle(m_Semaphore);
}

uint CPThreadPoolWin32::DoGetCurrentThreadIndex()
{
	return (uint) (size_t) TlsGetValue(s_ThreadLocalIndex);
}

void CPThreadPoolWin32::DoSetCurrentThreadIndex(uint curIndex)
{
	TlsSetValue(s_ThreadLocalIndex, (LPVOID) curIndex);
}

_NAMESPACE_END