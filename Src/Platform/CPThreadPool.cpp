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
	, m_QueueSize(0)
	, m_IsTerminating(FALSE)
	, m_IsAlwaysActive(FALSE)
	, m_NumThreads(0)
	, m_ppThreads(NULL)
	, m_ThreadIndexCount(0)
    , m_NumIdleThreads(0)
	//, m_NumJobsPending(0)
{
    m_ProcessData.jobIndexProcess = 0;
    m_ProcessData.processLock = SPIN_LOCK_RELEASED;
    m_AddData.jobIndexAdd = 0;
    m_AddData.queueLock = SPIN_LOCK_RELEASED;
    m_JobData.numJobsInQueue = 0;
    m_NumExternalThreads = 0;
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
	m_ProcessData.processLock	= SPIN_LOCK_RELEASED;
    m_ProcessData.jobIndexProcess = 0;
    m_AddData.queueLock         = SPIN_LOCK_RELEASED;
    m_AddData.jobIndexAdd       = 0;
    m_JobData.numJobsInQueue	= 0;
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
    AtomicInc(&m_NumIdleThreads);

	//if(!m_IsAlwaysActive)
		DoWaitThread();
	//else
	//	DoYieldThread();

    AtomicDec(&m_NumIdleThreads);
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
    //uint processIndex = AtomicInc((int*) &m_ProcessIndex) - 1;
    //int jobIndexToProcess = (processIndex+1) & (m_QueueSize-1);

    //IPRunnable* pRunnable = m_pJobQueue[jobIndexToProcess];
    //_DEBUG_ASSERT(m_ReservedQueue[jobIndexProcess] != EMPTY);
    //m_ReservedQueue[jobIndexToProcess] = EMPTY;

    //pRunnable->Run();

    //int numJobs;
    //uint addIndex = m_AddIndex;
    //if(addIndex >= processIndex)
    //    numJobs = addIndex - processIndex;
    //else
    //    numJobs = (0xFFFFFFFF - processIndex + 1) + addIndex;
 
    //// TODO: Need to sync here with main thread
    //// Only the main thread may do AtomicAdd(&m_NumThreads)
    //// and needs to sync here
    //// ---------------------
    //WorkerJobFinished(numJobs);
    //// ---------------------


    //// Just awoke so go to main loop
    //return;

    // Keep trying to get a lock as long as there are jobs
	// This is to support threads not in the threadpool to also process jobs in the queue
	while( m_JobData.numJobsInQueue > 0 )
	{
        if( AtomicCompareAndSwap(&m_ProcessData.processLock, SPIN_LOCK_ACQUIRED, SPIN_LOCK_RELEASED) == SPIN_LOCK_RELEASED )
        {
            int jobIndexProcess = m_ProcessData.jobIndexProcess;
            if(m_ReservedQueue[jobIndexProcess] == RESERVED)
            {
                m_ReservedQueue[jobIndexProcess] = PROCESSING;
                m_ProcessData.jobIndexProcess = (jobIndexProcess+1) & (m_QueueSize-1);

                // Release lock
                //AtomicSwap(&m_ProcessData.processLock, SPIN_LOCK_RELEASED);
                m_ProcessData.processLock = SPIN_LOCK_RELEASED;

                // Run the process
                IPRunnable* pRunnable = m_pJobQueue[jobIndexProcess];
                m_ReservedQueue[jobIndexProcess] = EMPTY;

                int numJobs = AtomicDec(&m_JobData.numJobsInQueue);
                pRunnable->Run();	
                
                //AtomicDec(&m_NumJobsPending);

                WorkerJobFinished(numJobs);

                // Just awoke so go to main loop
                return;
            }
            else
            {
                m_ProcessData.processLock = SPIN_LOCK_RELEASED;
            }
        }

		//if( AtomicCompareAndSwap(&m_ProcessData.processLock, SPIN_LOCK_ACQUIRED, SPIN_LOCK_RELEASED) == SPIN_LOCK_RELEASED )
		//{
		//	int jobIndexProcess = m_ProcessData.jobIndexProcess;
		//	if(m_ReservedQueue[jobIndexProcess] == RESERVED)
		//	{
		//		m_ReservedQueue[jobIndexProcess] = PROCESSING;
		//		m_ProcessData.jobIndexProcess = (jobIndexProcess+1) & (m_QueueSize-1);

		//		// Release lock
		//		//AtomicSwap(&m_ProcessData.processLock, SPIN_LOCK_RELEASED);
		//		m_ProcessData.processLock = SPIN_LOCK_RELEASED;

		//		// Run the process
		//		IPRunnable* pRunnable = m_pJobQueue[jobIndexProcess];
		//		m_ReservedQueue[jobIndexProcess] = EMPTY;

		//		pRunnable->Run();	
		//		int numJobs = AtomicDec(&m_JobData.numJobsInQueue);

		//		//AtomicDec(&m_NumJobsPending);

		//		WorkerJobFinished(numJobs);

		//		// Just awoke so go to main loop
		//		return;
		//	}
		//	else
		//	{
		//		m_ProcessData.processLock = SPIN_LOCK_RELEASED;
		//	}
		//}

		//int jobIndexProcess = m_ProcessData.jobIndexProcess ;//(m_ProcessData.jobIndexProcess) & (m_QueueSize-1);//m_ProcessData.jobIndexProcess 

		//// We try to get a short spin lock on the current job
		//if( AtomicCompareAndSwap(&(m_ReservedQueue[jobIndexProcess]), PROCESSING, RESERVED) == RESERVED )
		//{
		//	// Successfully swapped
		//	// So let other threads process available jobs
		//	int nextIndexToProcess = (jobIndexProcess+1) & (m_QueueSize-1);
		//	next = nextIndexToProcess;
		//	//AtomicInc(&m_ProcessData.jobIndexProcess);
		//	last = AtomicCompareAndSwap(&m_ProcessData.jobIndexProcess, nextIndexToProcess, jobIndexProcess);

		//	// Run the process
		//	IPRunnable* pRunnable = m_pJobQueue[jobIndexProcess];
		//	//m_pJobQueue[jobIndexProcess]->Run();
		//	
		//	// Reset to empty when finished
		//	m_ReservedQueue[jobIndexProcess] = EMPTY;
		//	//AtomicSwap(&m_ReservedQueue[jobIndexProcess], EMPTY);

		//	//int numJobs = AtomicDec(&m_JobData.numJobsInQueue);
		//	
		//	pRunnable->Run();	
		//	int numJobs = AtomicDec(&m_JobData.numJobsInQueue);

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

void CPThreadPool::ProcessJobs()
{
	// NOTE: Called by external main thread to also share in processing jobs in the queue
	while(true)
	{
        if( m_JobData.numJobsInQueue > 0)
        {
		    if( AtomicCompareAndSwap(&m_ProcessData.processLock, SPIN_LOCK_ACQUIRED, SPIN_LOCK_RELEASED) == SPIN_LOCK_RELEASED )
		    {
			    int jobIndexProcess = m_ProcessData.jobIndexProcess;
			    if(m_ReservedQueue[jobIndexProcess] == RESERVED)
			    {
				    m_ReservedQueue[jobIndexProcess] = PROCESSING;
				    m_ProcessData.jobIndexProcess = (jobIndexProcess+1) & (m_QueueSize-1);

				    // Release lock
				    //AtomicSwap(&m_ProcessData.processLock, SPIN_LOCK_RELEASED);
				    m_ProcessData.processLock = SPIN_LOCK_RELEASED;

				    // Run the process
				    IPRunnable* pRunnable = m_pJobQueue[jobIndexProcess];
				    m_ReservedQueue[jobIndexProcess] = EMPTY;
                
                    // Lock shared count
                    AtomicDec(&m_JobData.numJobsInQueue);

				    pRunnable->Run();								
			    }
			    else
			    {
				    m_ProcessData.processLock = SPIN_LOCK_RELEASED;
			    }
		    }
        }
        else if(m_NumIdleThreads != m_NumThreads)
        {
            DoYieldThread();
        }
        else
            break;
	}
}

void CPThreadPool::QueueJobUnsafe(IPRunnable& job)
{
    volatile int* pState = m_ReservedQueue + m_AddData.jobIndexAdd;
    while (*pState != EMPTY);

	//while(m_ReservedQueue[m_AddData.jobIndexAdd] != EMPTY);
	int jobIndex = m_AddData.jobIndexAdd;
	m_AddData.jobIndexAdd = (jobIndex+1) & (m_QueueSize-1);

	m_pJobQueue[jobIndex] = &job;
	
	m_ReservedQueue[jobIndex] = RESERVED;

	//AtomicInc(&m_NumJobsPending);
	int numJobs = AtomicInc(&m_JobData.numJobsInQueue);
	
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
		if( AtomicCompareAndSwap(&m_AddData.queueLock, SPIN_LOCK_ACQUIRED, SPIN_LOCK_RELEASED) == SPIN_LOCK_RELEASED )
		{
			jobIndexAdd = m_AddData.jobIndexAdd;
			if(m_ReservedQueue[jobIndexAdd] == EMPTY)
			{
				m_ReservedQueue[jobIndexAdd] = ADDING;
				break;
			}
			else
				m_AddData.queueLock = SPIN_LOCK_RELEASED;
		}
		//AtomicInc((int*)&totalQueueLock);

	} while (true);

	m_AddData.jobIndexAdd = (jobIndexAdd+1) & (m_QueueSize-1);
	//AtomicSwap(&m_AddData.queueLock, SPIN_LOCK_RELEASED);
	m_AddData.queueLock = SPIN_LOCK_RELEASED;

	m_pJobQueue[jobIndexAdd] = &job;

	// Set the reserved flag to show that buffer hole is reserved
	m_ReservedQueue[jobIndexAdd] = RESERVED;
	//AtomicInc(&m_NumJobsPending);
	int numJobs = AtomicInc(&m_JobData.numJobsInQueue);
	
	// Setup new job incoming
	WorkerJobInit(numJobs);
}

//uint CPThreadPool::GetNumJobsInQueue()
//{
//	return (uint) m_JobData.numJobsInQueue;
//}

uint CPThreadPool::GetQueueSize()
{
	return m_QueueSize;
}

uint CPThreadPool::GetNumJobsPending()
{
	return (uint) m_JobData.numJobsInQueue;
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