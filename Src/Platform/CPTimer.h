//==============================================================================
//
//		CPTimer.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Monday, March 20, 2006
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

class CPTimer : public IPTimer
{
private:
	double m_StartTime;
	double m_EndTime;
	
public:
	CPTimer()
		: m_StartTime(0.0)
		, m_EndTime(0.0)
	{
	}
	
	virtual ~CPTimer()
	{
	}
	
protected:
	virtual double DoGetTime() = 0;
	
public:
	double GetTime();
	
	void BeginSample();
	void EndSample();
	double GetTimeElapsed();
};

_NAMESPACE_END