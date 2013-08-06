//================================================================================
//
//		CPThread.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/6/2012
//
//================================================================================

#include "PlatformFramework.h"

_NAMESPACE_BEGIN

//int ExecFunc(void* pParam)
//{
//	CPThread* pThread = (CPThread*) pParam;
//	return pThread->GetRunnable()->Run();
//}

IPRunnable* CPThread::GetRunnable()
{
	return m_pRunnable;
}

void CPThread::Initialize(uint processorAffinity)
{
	DoInitialize(processorAffinity, this);
}

void CPThread::Resume(IPRunnable* pRunnable)
{
	if(!m_pRunnable)
		m_pRunnable = pRunnable;
	
	DoResume();
}

void CPThread::Join()
{
	if(m_pRunnable)
		DoJoin();
}

_NAMESPACE_END