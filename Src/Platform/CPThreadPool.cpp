//================================================================================
//
//		CPThreadPool.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/6/2012
//
//================================================================================

#include "PlatformFramework.h"

_NAMESPACE_BEGIN

//static uint totalQueueLock = 0;
//static uint totalProcessLock = 0;

//const static int EMPTY		= 0x00000000;
//const static int ADDING		= 0xFFFFFFFF;
//const static int RESERVED	= 0xCAFECAFE;
//const static int PROCESSING = 0xDEADBEEF;
//
//const static int SPIN_LOCK_ACQUIRED	= 1;
//const static int SPIN_LOCK_RELEASED = 0;

#define InternalGetWaitingLock()		m_AlignedVars.m_IsWaitingLock
#define InternalGetNumJobsInQueue()		m_AlignedVars.m_NumJobsInQueue

CPThreadPool::CPThreadPool()
	: m_pJobQueue(NULL)
	, m_QueueSize(0)
	, m_IsTerminating(FALSE)
	, m_IsAlwaysActive(FALSE)
	, m_NumThreads(0)
	, m_ppThreads(NULL)
	, m_ThreadIndexCount(0)
{
	InternalGetWaitingLock()	= FALSE;
	m_ProcessIndex		= 0;
	m_AddIndex			= 0;
	m_ThreadIndexCount	= 0;
	InternalGetNumJobsInQueue()	= 0;
}

CPThreadPool::~CPThreadPool()
{
}

void CPThreadPool::Shutdown()
{
	// Busy wait until all threads are awake
	m_IsTerminating = TRUE;
	DoSignalAllThreads();

	// Ensure threads are terminated
	_LOOPi(m_NumThreads)
	{
		IPThread* pThread = m_ppThreads[i];
		pThread->Join();
		pThread->Release();		
	}

	DoSignalOnFinishedEvent();
	DoShutdown();

	_DELETE_ARRAY(m_ppThreads);
	_DELETE_ARRAY(m_pJobQueue);
}

void CPThreadPool::Initialize(uint queueSizePow2, IPThread** pThreads, uint numThreads)
{
	m_IsTerminating		= FALSE;
	m_ProcessIndex		= 0;
	m_AddIndex			= 0;
	m_ThreadIndexCount	= 0;
	InternalGetNumJobsInQueue()	= 0;
	InternalGetWaitingLock() = FALSE;

	m_QueueSize			= 1 << queueSizePow2;
	m_pJobQueue			= _NEW IPRunnable*[m_QueueSize];

	memset(m_pJobQueue, 0, sizeof(IPRunnable*) * m_QueueSize);

	DoInitialize(queueSizePow2, pThreads, numThreads);

	// NOTE: Main thread will always have thread index of 0
	// (Initializing thread should always be the main thread)
	//int curThreadIndex = AtomicInc(&m_ThreadIndexCount);
	//DoSetCurrentThreadIndex(curThreadIndex);

	m_ppThreads = _NEW IPThread*[numThreads];
	m_NumThreads = numThreads;
	_LOOPi(m_NumThreads)
	{
		//m_Threads.push_back(pThreads[i]);
		m_ppThreads[i] = pThreads[i];
		m_ppThreads[i]->AddRef();
		m_ppThreads[i]->Resume(this);
	}
}

void CPThreadPool::WorkerWait()
{
	//if(!m_IsAlwaysActive)
		DoWaitThread();
	//else
	//	DoYieldThread();
}

int CPThreadPool::Run()
{
	int curThreadIndex = AtomicInc(&m_ThreadIndexCount);
	DoSetCurrentThreadIndex((uint) curThreadIndex);
	WorkerWait();

	while(!m_IsTerminating)
	{
		WorkerProcessJobs();
	}

	return 0;
}

void CPThreadPool::WaitUntilFinished()
{
	// Avoid cases where we return if the lock is still waiting to be acquired
	// IE, there are jobs pending yet the wait lock is unlocked
	// The polling is not atomic but that is okay since at most
	// we yield for a few frames
	while(!InternalGetWaitingLock() && (InternalGetNumJobsInQueue() > 0))
		DoYieldThread();

	DoWaitOnFinishedEvent();
}

void CPThreadPool::WorkerJobFinished(int numJobs)
{
	if(numJobs == 0)
	{
		// Last thread
		// Wake any threads waiting for jobs to finish

		// First check to see if race condition involving main thread
		// resetting the event exists

		// We wait for the job-adding-thread to finish locking
		// NOTE: There can only be one job-adder thread at one time
		// due to the atomic counter for num of jobs and the fact that we increment
		// the semaphore after the wait lock
		while(!InternalGetWaitingLock())
			DoYieldThread();

		// Example InternalGetWaitingLock() states
		// State: Unlocked	Adders: 1-2-3-4-5-6-7-8	Workers:
		// State: Locked	Adders:				Workers: 5-6-7
		// State: Locked	Adders: 1-2			Workers: 0-1-2-3-4
		// State: Unlocked	Adders: 1 (still) 	Workers: 1
		// State: Unlocked	Adders: 1 (still)	Workers: 
		// State: Locked	Adders:				Workers: 0
		// State: Unlocked	Adders:				Workers:

		// Wake all threads waiting for jobs to finish
		DoSignalOnFinishedEvent();

		InternalGetWaitingLock() = FALSE;
	}

	if(numJobs < (int) m_NumThreads)
	{
		//AtomicDec(&m_SignalCount);
		WorkerWait();
	}
}

void CPThreadPool::WorkerJobInit(int numJobs, int numJobsAdded)
{
	if((numJobs - numJobsAdded) == 0)
	{
		// First work
		
		// Wait until thread sync lock is unlocked
		while(InternalGetWaitingLock())
			DoYieldThread();

		// Then lock the sync point lock
		DoResetOnFinishedEvent();
		InternalGetWaitingLock() = TRUE;
	}

	int numThreadsToWake = numJobsAdded;
	if(numJobs > (int) m_NumThreads)
		numThreadsToWake -= (numJobs - m_NumThreads);

	if(numThreadsToWake > 0)
	{
		DoSignalThreads(numThreadsToWake);
	}
}

void CPThreadPool::WorkerProcessJobs()
{ 
    uint processIndex = AtomicInc((int*) &m_ProcessIndex) - 1;
    int jobIndexToProcess = processIndex & (m_QueueSize-1);

    IPRunnable* pRunnable = m_pJobQueue[jobIndexToProcess];
    _DEBUG_ASSERT(pRunnable);
    m_pJobQueue[jobIndexToProcess] = NULL;

    pRunnable->Run();

	int numJobs = AtomicDec(&InternalGetNumJobsInQueue());
 
    WorkerJobFinished(numJobs);

    // Just awoke so go to main loop
    return;
}

void CPThreadPool::QueueJobs(IPRunnable** ppJobs, uint numJobs)
{
	// WARNING: Threadpool must a buffer greater than the number of concurrent
	// threads adding to the pool

	uint addIndex = AtomicAdd((int*) &m_AddIndex, (int) numJobs) - numJobs;

	_LOOPi(numJobs)
	{
		int jobIndexToAdd = (int) (addIndex & (m_QueueSize-1));
		IPRunnable** ppState = m_pJobQueue + jobIndexToAdd;
		while (*ppState)
			DoYieldThread();

		m_pJobQueue[jobIndexToAdd] = ppJobs[i];
		addIndex++;
	}


	//AtomicInc(&m_NumJobsPending);
	int numJobsInQueue = AtomicAdd(&InternalGetNumJobsInQueue(), numJobs);

	// Setup new job incoming
	WorkerJobInit(numJobsInQueue, numJobs);
}

void CPThreadPool::QueueJob(IPRunnable& job)
{
	IPRunnable* pJob = &job;
	QueueJobs(&pJob, 1);
}

uint CPThreadPool::GetQueueSize()
{
	return m_QueueSize;
}

uint CPThreadPool::GetNumJobsPending()
{
	return (uint) InternalGetNumJobsInQueue();
}

//void CPThreadPool::SetAlwaysActive(boolean isEnabled)
//{
//	m_IsAlwaysActive = isEnabled;
//}

uint CPThreadPool::GetNumThreads()
{
	return m_NumThreads;
}

uint CPThreadPool::GetCurrentThreadIndex()
{
	return DoGetCurrentThreadIndex();
}

_NAMESPACE_END