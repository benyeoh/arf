//==============================================================================
//
//		Mutex.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		08/07/10
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class Mutex
{
private:
	class MutexImpl;
	MutexImpl* m_pImpl;

public:
	Mutex();
	~Mutex();

public:
	void	Initialize();
	void	Lock();
	void	Unlock();
};

class MutexScopeLock
{
private:
	Mutex* m_pMutex;

public:
	MutexScopeLock(Mutex* pMutex)
		: m_pMutex(pMutex)
	{
		m_pMutex->Lock();
	}

	~MutexScopeLock()
	{
		m_pMutex->Unlock();
	}
};

_NAMESPACE_END
