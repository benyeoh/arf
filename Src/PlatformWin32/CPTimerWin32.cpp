//==============================================================================
//
//		CPTimerWin32.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Monday, March 20, 2006
//
//==============================================================================

#include "PlatformWin32.h"

_NAMESPACE_BEGIN

CPTimerWin32::CPTimerWin32()
	: CPTimer()
{
	QueryPerformanceFrequency(&m_Frequency);
	QueryPerformanceCounter(&m_LastSample);
}

CPTimerWin32::~CPTimerWin32()
{
}

double
CPTimerWin32::DoGetTime()
{
	LARGE_INTEGER sample;
	QueryPerformanceCounter(&sample);
	
	return ((double) (sample.QuadPart - m_LastSample.QuadPart)) / ((double) m_Frequency.QuadPart);
}


_NAMESPACE_END