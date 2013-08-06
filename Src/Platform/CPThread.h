//================================================================================
//
//		CPThread.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/6/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

class CPThread : public CRefCounted<IPThread>
{
private:
	IPRunnable* m_pRunnable;

public:
	CPThread() 
		: m_pRunnable(NULL)
	{
	}

	virtual ~CPThread()
	{
	}

protected:
	_PURE( void DoResume() )
	_PURE( void DoJoin() )
	_PURE( void DoInitialize(uint processorAffinity, void* pThreadParam) )

public:
	void Initialize(uint processorAffinity);
	
	IPRunnable* GetRunnable();

	void Resume(IPRunnable* pRunnable);
	void Join();
};

inline int ExecFunc(void* pParam)
{
	CPThread* pThread = (CPThread*) pParam;
	return pThread->GetRunnable()->Run();
}


_NAMESPACE_END