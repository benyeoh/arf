//==============================================================================
//
//		CPTimerWin32.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, March 19, 2006
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

class CPTimerWin32 : public CPTimer
{
private:
	LARGE_INTEGER m_Frequency;
	LARGE_INTEGER m_LastSample;

public:
	CPTimerWin32();
	virtual ~CPTimerWin32();
		
protected:
	double DoGetTime();
};

_NAMESPACE_END