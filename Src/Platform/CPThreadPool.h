//================================================================================
//
//		CPThreadPool.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/6/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

class CPThreadPool : public CRefCounted<IPThreadPool>, public IPRunnable
{
private:
	IPRunnable** m_pJobQueue;
	int* m_ReservedQueue;
	
	int m_JobIndexAdd;
	int m_JobIndexProcess;
	
	int m_QueueLock;
	int m_ProcessLock;
	int m_NumJobsInQueue;
	//int m_NumJobsPending;

	uint m_QueueSize;
	
	IPThread**	m_ppThreads;
	uint		m_NumThreads;
	
	boolean m_IsTerminating;
	boolean m_IsAlwaysActive;

	int		m_ThreadIndexCount;

public:
	CPThreadPool();
	virtual ~CPThreadPool();

private:
	void WorkerProcessJobs();
	void WorkerJobInit(int numJobs);
	void WorkerJobFinished(int numJobs);
	void WorkerWait();

protected:
	_PURE( void DoWaitThread() )
	_PURE( void DoSignalThread() )
	_PURE( void DoSignalAllThreads() )
	_PURE( void DoInitialize(uint queueSizePow2, IPThread** pThreads, uint numThreads) )
	_PURE( void DoShutdown() )
	_PURE( void DoYieldThread() )
	_PURE( uint DoGetCurrentThreadIndex() )
	_PURE( void DoSetCurrentThreadIndex(uint curIndex) )

public:
	void Initialize(uint queueSizePow2, IPThread** pThreads, uint numThreads);
	void Shutdown();

	int Run();

	void QueueJobUnsafe(IPRunnable& job);
	void QueueJob(IPRunnable& job);
	void ProcessJob();
	void SetAlwaysActive(boolean isEnabled);

	uint GetQueueSize();
	uint GetNumJobsPending();
	uint GetNumThreads();

	uint GetCurrentThreadIndex();
};


_NAMESPACE_END