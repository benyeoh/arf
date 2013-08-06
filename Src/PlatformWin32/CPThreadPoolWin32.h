//================================================================================
//
//		CPThreadPoolWin32.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/12/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

class CPThreadPoolWin32 : public CPThreadPool
{
private:
	HANDLE m_Semaphore;

public:
	CPThreadPoolWin32();
	virtual ~CPThreadPoolWin32();

protected:
	void DoWaitThread();
	void DoSignalThread();
	void DoSignalAllThreads();
	void DoInitialize(uint queueSizePow2, IPThread** pThreads, uint numThreads);
	void DoShutdown();
	void DoYieldThread();

	uint DoGetCurrentThreadIndex();
	void DoSetCurrentThreadIndex(uint curIndex);
};

_NAMESPACE_END