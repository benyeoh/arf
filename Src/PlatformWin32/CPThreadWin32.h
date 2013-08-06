//================================================================================
//
//		CPThreadWin32.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/6/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

class CPThreadWin32 : public CPThread
{
private:
	HANDLE m_Handle;
	int m_ThreadID;

public:
	CPThreadWin32();
	virtual ~CPThreadWin32();

protected:
	void DoResume();
	void DoJoin();
	void DoInitialize(uint processorAffinity, void* pThreadParam);
};

_NAMESPACE_END