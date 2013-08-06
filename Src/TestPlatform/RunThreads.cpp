//================================================================================
//
//		RunThreads.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/12/2012
//
//================================================================================

#include "stdafx.h"

static int g_SharedTestVarThread = 0;

class SampleWorkThreadRunnable : public IPRunnable
{
private:
	
public:
	SampleWorkThreadRunnable()
	{
	}

	int Run()
	{
		int workIter = 1250000;
		_LOOPi(workIter)
		{
			AtomicInc(&g_SharedTestVarThread);
		}

		return 0;
	}
};

void RunThreads()
{
	_TRACE(_W("\n\n================================================ RunThreads BEGIN ==================================================\n\n"));

	IPThreadPtr pThreads[8];
	SampleWorkThreadRunnable runnable[8];

	_LOOPi(8)
	{
		pThreads[i] = g_pPlatform->GetResourceMgr().CreateThread();
		pThreads[i]->Initialize(0xFFFFFFFF);
		pThreads[i]->Resume(&runnable[i]);
	}

	_LOOPi(8)
	{
		pThreads[i]->Join();
		_TRACE(_W("Thread %d: %d\n"), i, g_SharedTestVarThread);
	}

	_TRACE(_W("\n\n================================================ RunThreads END ==================================================\n\n"));
}
