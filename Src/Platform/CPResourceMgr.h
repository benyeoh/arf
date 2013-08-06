//================================================================================
//
//		CPResourceMgr.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/5/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

class CPResourceMgr : public IPResourceMgr
{
private:

public:
	CPResourceMgr()
	{
	}
	
	~CPResourceMgr()
	{
	}

protected:
	_PURE( CPThread* DoCreateThread() )
	_PURE( CPThreadPool* DoCreateThreadPool() )

public:
	IPThread* CreateThread();
	IPThreadPool* CreateThreadPool();
};

_NAMESPACE_END