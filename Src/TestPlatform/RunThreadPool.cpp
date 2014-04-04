//================================================================================
//
//		RunThreadPool.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/12/2012
//
//================================================================================

#include "stdafx.h"

const static int NUM_THREADS = 8;
const static int NUM_JOBS_SHIFT = 8;
const static int NUM_JOBS = (1 << NUM_JOBS_SHIFT) >> 0;

static int g_SharedTestVar = 0;
static boolean g_ThreadIDCheck[NUM_THREADS+1];

class SampleWorkRunnable : public IPRunnable
{
private:
	volatile int m_Member;
	int m_ID;
	IPThreadPool* m_pPool;

public:
	SampleWorkRunnable()
		: m_Member(0)
	{
	}

	void SetPool(IPThreadPool* pPool)
	{
		m_pPool = pPool;
	}

	void SetID(int id)
	{
		m_ID = id;
	}

	int Run()
	{
		_TRACE(_W("<< %d >> ThreadPool: %d\n"), m_pPool->GetCurrentThreadIndex(), m_ID); 
		_DEBUG_ASSERT( g_ThreadIDCheck[m_pPool->GetCurrentThreadIndex()] == FALSE );
		g_ThreadIDCheck[m_pPool->GetCurrentThreadIndex()] = TRUE;
		
		volatile int workIter = 250000 << 0;
		_LOOPi(workIter)
		{
			m_Member++;
			//AtomicInc(&g_SharedTestVar);
		}

		_TRACE( _W("<< %d >> Work done\n"), m_pPool->GetCurrentThreadIndex() );
		g_ThreadIDCheck[m_pPool->GetCurrentThreadIndex()] = FALSE;
		return 0;
	}
};

class SampleAddWorkRunnable : public IPRunnable
{
private:
	SampleWorkRunnable m_Work[256];
	volatile int m_Member;
	int m_ID;

	IPThreadPool* m_pPool;

public:
	SampleAddWorkRunnable()
		: m_Member(0)
	{
	}

	void SetID(int id)
	{
		m_ID = id;
	}

	void SetPool(IPThreadPool* pPool)
	{
		m_pPool = pPool;
	}

	int Run()
	{
		_TRACE(_W("<< %d >> AddWork ThreadPool: %d\n"), m_pPool->GetCurrentThreadIndex(), m_ID); 
		_DEBUG_ASSERT( g_ThreadIDCheck[m_pPool->GetCurrentThreadIndex()] == FALSE );
		g_ThreadIDCheck[m_pPool->GetCurrentThreadIndex()] = TRUE;

		_LOOPi(16)
		{
			volatile int workIter = 1000 << 0;
			_LOOPj(workIter)
			{
				m_Member++;
				//AtomicInc(&g_SharedTestVar);
			}

			m_Work[i].SetID(m_ID);
			m_Work[i].SetPool(m_pPool);
			m_pPool->QueueJob(m_Work[i]);

			//_TRACE( _W("<< %d >> AddWork: %d\n"), m_pPool->GetCurrentThreadIndex(), m_ID );
		}

		_TRACE( _W("<< %d >> Work done\n"), m_pPool->GetCurrentThreadIndex() );
		g_ThreadIDCheck[m_pPool->GetCurrentThreadIndex()] = FALSE;
		return 0;
	}
};

SampleAddWorkRunnable g_AddWorkRunnable[NUM_JOBS];

void RunThreadPool()
{
	_TRACE(_W("\n\n================================================ RunThreadPool BEGIN ==================================================\n\n"));
	
	IPThreadPoolPtr pThreadPool = g_pPlatform->GetResourceMgr().CreateThreadPool();
	IPThread* pThreads[NUM_THREADS];
	_LOOPi(NUM_THREADS)
	{
		pThreads[i] = g_pPlatform->GetResourceMgr().CreateThread();		
		pThreads[i]->Initialize(0xFFFFFFFF);

		g_ThreadIDCheck[i] = FALSE;
	}

	pThreadPool->Initialize(12, pThreads, NUM_THREADS);
	
	
	g_pPlatform->GetTimer().BeginSample();

	_LOOPk(32)
	{
		_LOOPi(NUM_JOBS)
		{
			g_AddWorkRunnable[i].SetID(i);
			g_AddWorkRunnable[i].SetPool(pThreadPool);
			pThreadPool->QueueJob(g_AddWorkRunnable[i]);
		}

		pThreadPool->ProcessJobs();
		//_TRACE(_W("ThreadPool: jobs %d, var %d\n"), pThreadPool->GetNumJobsInQueue(), g_SharedTestVar); 

		_TRACE( _W("K: %d\n"), k );
	}

	g_pPlatform->GetTimer().EndSample();
	_TRACE(_W("Time Elapsed: %f\n"), g_pPlatform->GetTimer().GetTimeElapsed());

	_TRACE(_W("\n\n================================================ RunThreadPool END ==================================================\n\n"));
}
