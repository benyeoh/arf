//==============================================================================
//
//		CPPlatformWin32.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, March 19, 2006
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CPPlatformWin32 : public CPPlatform
{
private:

public:
	CPPlatformWin32()
		: CPPlatform(_NEW CPTimerWin32, _NEW CPResourceMgrWin32)
	{
	}
	
	virtual ~CPPlatformWin32()
	{
	}
	
protected:
	boolean DoInitialize();
};

_NAMESPACE_END