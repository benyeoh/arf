//================================================================================
//
//		CPResourceMgrWin32.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/6/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

class CPResourceMgrWin32 : public CPResourceMgr
{
private:

public:
	CPResourceMgrWin32()
	{
	}

	virtual ~CPResourceMgrWin32()
	{
	}

protected:
	CPThread* DoCreateThread();
	CPThreadPool* DoCreateThreadPool();
};

_NAMESPACE_END