//================================================================================
//
//		CPThreadWin32.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/6/2012
//
//================================================================================

#include "PlatformWin32.h"

_NAMESPACE_BEGIN

DWORD WINAPI ThreadExec(LPVOID param)
{
	return ExecFunc(param);
}

CPThreadWin32::CPThreadWin32()
	: m_Handle(0)
	, m_ThreadID(0xFFFFFFFF)
{
}

CPThreadWin32::~CPThreadWin32()
{
	if(m_Handle)
	{
		Join();

		CloseHandle(m_Handle);
		m_Handle = 0;
	}
}

void CPThreadWin32::DoResume()
{
	if(m_Handle)
	{
		DWORD prevSuspendCount = ResumeThread(m_Handle);
		_DEBUG_ASSERT(prevSuspendCount >= 0);
	}
}

void CPThreadWin32::DoJoin()
{
	if(m_Handle)
	{
		DWORD ret = WaitForSingleObject(m_Handle, INFINITE);
		_DEBUG_ASSERT(ret == WAIT_OBJECT_0);
	}
}

void CPThreadWin32::DoInitialize(uint processorAffinity, void* pThreadParam)
{
	m_Handle = CreateThread(NULL, 0, ThreadExec, pThreadParam, CREATE_SUSPENDED, (DWORD*) &m_ThreadID);
	_DEBUG_ASSERT(m_Handle);

	//DWORD_PTR affinity = 2;
	SetThreadAffinityMask(m_Handle, processorAffinity);
}

_NAMESPACE_END