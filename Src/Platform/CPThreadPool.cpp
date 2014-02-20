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

const static int EMPTY		= 0x00000000;
const static int ADDING		= 0xFFFFFFFF;
const static int RESERVED	= 0xCAFECAFE;
const static int PROCESSING = 0xDEADBEEF;

const static int SPIN_LOCK_ACQUIRED	= 1;
const static int SPIN_LOCK_RELEASED = 0;

CPThreadPool::CPThreadPool()
	: m_pJobQueue(NULL)
	, m_ReservedQueue(NULL)
	, m_JobIndexAdd(0)
	, m_JobIndexProcess(0)
	, m_QueueSize(0)
	, m_NumJobsInQueue(0)
	, m_IsTerminating(FALSE)
	, m_IsAlwaysActive(FALSE)
	, m_QueueLock(SPIN_LOCK_RELEASED)
	, m_ProcessLock(SPIN_LOCK_RELEASED)
	, m_NumThreads(0)
	, m_ppThreads(NULL)
	, m_ThreadIndexCount(0)
	//, m_NumJobsPending(0)
{
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

	DoShutdown();

	_DELETE_ARRAY(m_ppThreads);
	_DELETE_ARRAY(m_pJobQueue);
	_DELETE_ARRAY(m_ReservedQueue);
}

void CPThreadPool::Initialize(uint queueSizePow2, IPThread** pThreads, uint numThreads)
{
	m_IsTerminating		= FALSE;
	m_IsAlwaysActive	= FALSE;

	m_ThreadIndexCount	= 0;
	m_ProcessLock		= SPIN_LOCK_RELEASED;
	m_QueueLock			= SPIN_LOCK_RELEASED;
	m_JobIndexAdd		= 0;
	m_JobIndexProcess	= 0;
	m_NumJobsInQueue	= 0;
	//m_NumJobsPending	= 0;

	m_QueueSize			= 1 << queueSizePow2;
	m_pJobQueue			= _NEW IPRunnable*[m_QueueSize];
	m_ReservedQueue		= _NEW int[m_QueueSize];

	memset(m_pJobQueue, 0, sizeof(IPRunnable*) * m_QueueSize);
	memset(m_ReservedQueue, 0, sizeof(int) * m_QueueSize);

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
	if(!m_IsAlwaysActive)
		DoWaitThread();
	else
		DoYieldThread();
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

void CPThreadPool::WorkerJobFinished(int numJobs)
{
	if(numJobs < (int) m_NumThreads)
	{
		//AtomicDec(&m_SignalCount);
		WorkerWait();
	}
}

void CPThreadPool::WorkerJobInit(int numJobs)
{
	if(numJobs <= (int) m_NumThreads)
	{
		//AtomicInc(&m_SignalCount);
		DoSignalThread();
	}
}

void CPThreadPool::WorkerProcessJobs()
{
	// Keep trying to get a lock as long as there are jobs
	// This is to support threads not in the threadpool to also process jobs in the queue
	while( m_NumJobsInQueue > 0 )
	{
		if( AtomicCompareAndSwap(&m_ProcessLock, SPIN_LOCK_ACQUIRED, SPIN_LOCK_RELEASED) == SPIN_LOCK_RELEASED )
		{
			int jobIndexProcess = m_JobIndexProcess;
			if(m_ReservedQueue[jobIndexProcess] == RESERVED)
			{
				m_ReservedQueue[jobIndexProcess] = PROCESSING;
				m_JobIndexProcess = (jobIndexProcess+1) & (m_QueueSize-1);

				// Release lock
				//AtomicSwap(&m_ProcessLock, SPIN_LOCK_RELEASED);
				m_ProcessLock = SPIN_LOCK_RELEASED;

				// Run the process
				IPRunnable* pRunnable = m_pJobQueue[jobIndexProcess];
				m_ReservedQueue[jobIndexProcess] = EMPTY;

				pRunnable->Run();	
				int numJobs = AtomicDec(&m_NumJobsInQueue);

				//AtomicDec(&m_NumJobsPending);

				WorkerJobFinished(numJobs);

				// Just awoke so go to main loop
				return;
			}
			else
			{
				m_ProcessLock = SPIN_LOCK_RELEASED;
			}
		}

		//int jobIndexProcess = m_JobIndexProcess ;//(m_JobIndexProcess) & (m_QueueSize-1);//m_JobIndexProcess 

		//// We try to get a short spin lock on the current job
		//if( AtomicCompareAndSwap(&(m_ReservedQueue[jobIndexProcess]), PROCESSING, RESERVED) == RESERVED )
		//{
		//	// Successfully swapped
		//	// So let other threads process available jobs
		//	int nextIndexToProcess = (jobIndexProcess+1) & (m_QueueSize-1);
		//	next = nextIndexToProcess;
		//	//AtomicInc(&m_JobIndexProcess);
		//	last = AtomicCompareAndSwap(&m_JobIndexProcess, nextIndexToProcess, jobIndexProcess);

		//	// Run the process
		//	IPRunnable* pRunnable = m_pJobQueue[jobIndexProcess];
		//	//m_pJobQueue[jobIndexProcess]->Run();
		//	
		//	// Reset to empty when finished
		//	m_ReservedQueue[jobIndexProcess] = EMPTY;
		//	//AtomicSwap(&m_ReservedQueue[jobIndexProcess], EMPTY);

		//	//int numJobs = AtomicDec(&m_NumJobsInQueue);
		//	
		//	pRunnable->Run();	
		//	int numJobs = AtomicDec(&m_NumJobsInQueue);

		//	//AtomicDec(&m_NumJobsPending);

		//	WorkerJobFinished(numJobs);

		//	// Just awoke so go to main loop
		//	return;
		//}

		//AtomicInc((int*)&totalProcessLock);
	}

	// If other threads (external to the threadpool) have processed jobs 
	// while we were trying to get a lock then just wait
	WorkerWait();
}

void CPThreadPool::ProcessJob()
{
	// NOTE: Called by external threads to also share in processing jobs in the queue

	while( m_NumJobsInQueue > 0 )
	{
		if( AtomicCompareAndSwap(&m_ProcessLock, SPIN_LOCK_ACQUIRED, SPIN_LOCK_RELEASED) == SPIN_LOCK_RELEASED )
		{
			int jobIndexProcess = m_JobIndexProcess;
			if(m_ReservedQueue[jobIndexProcess] == RESERVED)
			{
				m_ReservedQueue[jobIndexProcess] = PROCESSING;
				m_JobIndexProcess = (jobIndexProcess+1) & (m_QueueSize-1);

				// Release lock
				//AtomicSwap(&m_ProcessLock, SPIN_LOCK_RELEASED);
				m_ProcessLock = SPIN_LOCK_RELEASED;

				// Run the process
				IPRunnable* pRunnable = m_pJobQueue[jobIndexProcess];
				m_ReservedQueue[jobIndexProcess] = EMPTY;

				pRunnable->Run();	
				
				// Lock shared count
				AtomicDec(&m_NumJobsInQueue);
				return;
			}
			else
			{
				m_ProcessLock = SPIN_LOCK_RELEASED;
			}
		}
	}
}

void CPThreadPool::QueueJobUnsafe(IPRunnable& job)
{
    volatile int* pState = m_ReservedQueue + m_JobIndexAdd;
    while (*pState != EMPTY);

	//while(m_ReservedQueue[m_JobIndexAdd] != EMPTY);
	int jobIndex = m_JobIndexAdd;
	m_JobIndexAdd = (jobIndex+1) & (m_QueueSize-1);

	m_pJobQueue[jobIndex] = &job;
	
	m_ReservedQueue[jobIndex] = RESERVED;

	//AtomicInc(&m_NumJobsPending);
	int numJobs = AtomicInc(&m_NumJobsInQueue);
	
	// Setup new job incoming
	WorkerJobInit(numJobs);
}

void CPThreadPool::QueueJob(IPRunnable& job)
{
	// Thread safe
	// NOTE: QueueJob will block until there is space in the buffer
	int jobIndexAdd;

	do 
	{
		if( AtomicCompareAndSwap(&m_QueueLock, SPIN_LOCK_ACQUIRED, SPIN_LOCK_RELEASED) == SPIN_LOCK_RELEASED )
		{
			jobIndexAdd = m_JobIndexAdd;
			if(m_ReservedQueue[jobIndexAdd] == EMPTY)
			{
				m_ReservedQueue[jobIndexAdd] = ADDING;
				break;
			}
			else
				m_QueueLock = SPIN_LOCK_RELEASED;
		}
		//AtomicInc((int*)&totalQueueLock);

	} while (true);

	m_JobIndexAdd = (jobIndexAdd+1) & (m_QueueSize-1);
	//AtomicSwap(&m_QueueLock, SPIN_LOCK_RELEASED);
	m_QueueLock = SPIN_LOCK_RELEASED;

	m_pJobQueue[jobIndexAdd] = &job;

	// Set the reserved flag to show that buffer hole is reserved
	m_ReservedQueue[jobIndexAdd] = RESERVED;
	//AtomicInc(&m_NumJobsPending);
	int numJobs = AtomicInc(&m_NumJobsInQueue);
	
	// Setup new job incoming
	WorkerJobInit(numJobs);
}

//uint CPThreadPool::GetNumJobsInQueue()
//{
//	return (uint) m_NumJobsInQueue;
//}

uint CPThreadPool::GetQueueSize()
{
	return m_QueueSize;
}

uint CPThreadPool::GetNumJobsPending()
{
	return (uint) m_NumJobsInQueue;
}

void CPThreadPool::SetAlwaysActive(boolean isEnabled)
{
	m_IsAlwaysActive = isEnabled;
}

uint CPThreadPool::GetNumThreads()
{
	return m_NumThreads;
}

uint CPThreadPool::GetCurrentThreadIndex()
{
	return DoGetCurrentThreadIndex();
}

_NAMESPACE_END