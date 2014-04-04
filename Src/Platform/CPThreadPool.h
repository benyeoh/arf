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
    _ALIGN(_CACHE_LINE_SIZE) struct ProcessData
    {
        int jobIndexProcess;
        int processLock;
    };

    _ALIGN(_CACHE_LINE_SIZE) struct AddData
    {
        int jobIndexAdd;
        int queueLock;
    };

    _ALIGN(_CACHE_LINE_SIZE) struct JobData
    {
        int numJobsInQueue;
    };

private:
    //_ALIGN(_CACHE_LINE_SIZE) struct  
   // {
        ProcessData m_ProcessData;
        AddData m_AddData;
        JobData m_JobData;
   // }// m_SyncData;

    IPRunnable** m_pJobQueue;
	int* m_ReservedQueue;
	
	//int m_NumJobsPending;
	
	IPThread**	m_ppThreads;
	uint		m_NumThreads;
	
	boolean m_IsTerminating;
	boolean m_IsAlwaysActive;

	int		m_ThreadIndexCount;
    int     m_NumExternalThreads;
   
    uint m_QueueSize;
    
    _ALIGN(_CACHE_LINE_SIZE) uint m_ProcessIndex;
    _ALIGN(_CACHE_LINE_SIZE) uint m_AddIndex;
    _ALIGN(_CACHE_LINE_SIZE) int m_NumIdleThreads;

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
	void ProcessJobs();
    //void SetAlwaysActive(boolean isEnabled);

	uint GetQueueSize();
	uint GetNumJobsPending();
	uint GetNumThreads();

	uint GetCurrentThreadIndex();

    _IMPL_ALIGNED_ALLOCS(_CACHE_LINE_SIZE)
};

_NAMESPACE_END