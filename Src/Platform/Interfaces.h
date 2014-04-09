//==============================================================================
//
//		Interfaces.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Monday, March 13, 2006
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

struct IPTimer
{
	_PURE( double GetTime() )
	
	_PURE( void BeginSample() )
	_PURE( void EndSample() )
	_PURE( double GetTimeElapsed() )
};

// App-defined
struct IPRunnable
{
	_PURE( int Run() )
};

struct IPThread : public IRefCounted
{
	_PURE( void Initialize(uint processorAffinity=0xFFFFFFFF) ) // 0xFFFFFFFF for all
	_PURE( void Resume(IPRunnable* pRunnable) )
	_PURE( void Join() )
};

struct IPThreadPool : public IRefCounted
{
	_PURE( void Initialize(uint queueSizePow2, IPThread** pThreads, uint numThreads) )
	_PURE( void Shutdown() )
	
	_PURE( void QueueJob(IPRunnable& job) )
	_PURE( void QueueJobs(IPRunnable** ppJobs, uint numJobs) )
	_PURE( void WaitUntilFinished() )

	_PURE( uint GetQueueSize() )	
	_PURE( uint GetNumJobsPending() )
	_PURE( uint GetNumThreads() )

	_PURE( uint GetCurrentThreadIndex() )

	//_PURE( uint GetNumOfJobs() )
	//_PURE( boolean IsEmpty() )

	//_PURE( void WaitForJobs() )
};

struct IPResourceMgr
{
	_PURE( IPThread* CreateThread() )
	_PURE( IPThreadPool* CreateThreadPool() )
};

struct IPPlatform : public IRefCounted
{
	_PURE( boolean Initialize() )
	
	_PURE( IPResourceMgr& GetResourceMgr() )
	_PURE( IPTimer& GetTimer() )
};

//////////////////////////////////////////////////////////////////////////
//	
//	Smart Pointers
//
//////////////////////////////////////////////////////////////////////////

_DECLARE_SMARTPTR(IPTimer)
_DECLARE_SMARTPTR(IPThread)
_DECLARE_SMARTPTR(IPThreadPool)
_DECLARE_SMARTPTR(IPPlatform)

_NAMESPACE_END

//////////////////////////////////////////////////////////////////////////
//	 
//	DLL exposed functions
//
//////////////////////////////////////////////////////////////////////////

extern "C"
{
	typedef _NAMESPACE IPPlatform* (*CreatePlatformFn)();
	_DECL_DYNAMIC_EXPORT( _NAMESPACE IPPlatform* CreatePlatform() )
}
