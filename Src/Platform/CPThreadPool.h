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
	_ALIGN(_CACHE_LINE_SIZE) uint m_ProcessIndex;
	_ALIGN(_CACHE_LINE_SIZE) uint m_AddIndex;

	_ALIGN(_CACHE_LINE_SIZE) struct
	{
		int	m_NumJobsInQueue;
		boolean m_IsWaitingLock;
	} m_AlignedVars;

	//_ALIGN(_CACHE_LINE_SIZE) int	m_NumJobsInQueue;
	//_ALIGN(_CACHE_LINE_SIZE) boolean m_IsWaitingLock;

	struct JobEntry
	{
		IPRunnable* pRunnable;
	};

    JobEntry* m_pJobQueue;
	
	//int m_NumJobsPending;
	
	IPThread**	m_ppThreads;
	uint		m_NumThreads;
	
	boolean m_IsTerminating;
	boolean m_IsAlwaysActive;

	int		m_ThreadIndexCount;
   
    uint m_QueueSize;

public:
	CPThreadPool();
	virtual ~CPThreadPool();

private:
	void WorkerProcessJobs();
	void WorkerJobInit(int numJobs, int numJobsAdded);
	void WorkerJobFinished(int numJobs);
	void WorkerWait();

protected:
	_PURE( void DoWaitThread() )
	_PURE( void DoSignalThreads(uint numThreads) )
	_PURE( void DoSignalAllThreads() )
	_PURE( void DoInitialize(uint queueSizePow2, IPThread** pThreads, uint numThreads) )
	_PURE( void DoShutdown() )
	_PURE( void DoYieldThread() )
	_PURE( uint DoGetCurrentThreadIndex() )
	_PURE( void DoSetCurrentThreadIndex(uint curIndex) )
	_PURE( void DoWaitOnFinishedEvent() )
	_PURE( void DoSignalOnFinishedEvent() )
	_PURE( void DoResetOnFinishedEvent() )

public:
	void Initialize(uint queueSizePow2, IPThread** pThreads, uint numThreads);
	void Shutdown();

	int Run();

	void QueueJobs(IPRunnable** ppJobs, uint numJobs);
	void QueueJob(IPRunnable& job);
	void WaitUntilFinished();
	//void ProcessJobs();
    //void SetAlwaysActive(boolean isEnabled);

	uint GetQueueSize();
	uint GetNumJobsPending();
	uint GetNumThreads();

	uint GetCurrentThreadIndex();

    _IMPL_ALIGNED_ALLOCS(_CACHE_LINE_SIZE)
};

_NAMESPACE_END