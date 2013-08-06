//==============================================================================
//
//		Mutex.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		08/07/10
//
//==============================================================================

#include "Core.h"

_NAMESPACE_BEGIN

#ifdef _WIN32

#include "windows.h"

class Mutex::MutexImpl
{
private:
	CRITICAL_SECTION m_CritSection;

public:
	MutexImpl()
	{
		InitializeCriticalSection(&m_CritSection);
	}

	~MutexImpl()
	{
		DeleteCriticalSection(&m_CritSection);
	}

	void Lock()
	{
		EnterCriticalSection(&m_CritSection);
	}

	void Unlock()
	{
		LeaveCriticalSection(&m_CritSection);
	}
};

#endif


Mutex::Mutex()
	: m_pImpl(NULL)
{
}

Mutex::~Mutex()
{
	_DELETE(m_pImpl);
}

void
Mutex::Initialize()
{
	m_pImpl = _NEW MutexImpl();	
}

void
Mutex::Lock()
{
	m_pImpl->Lock();
}

void
Mutex::Unlock()
{
	m_pImpl->Unlock();
}

_NAMESPACE_END
