//==============================================================================
//
//		CPTimer.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Monday, March 20, 2006
//
//==============================================================================

#include "PlatformFramework.h"

_NAMESPACE_BEGIN

double
CPTimer::GetTime()
{
	return DoGetTime();
}

void 
CPTimer::BeginSample()
{
	m_StartTime = GetTime();
	m_EndTime = m_StartTime;
}

void 
CPTimer::EndSample()
{
	m_EndTime = GetTime();
}

double 
CPTimer::GetTimeElapsed()
{
	return (m_EndTime - m_StartTime);
}

_NAMESPACE_END