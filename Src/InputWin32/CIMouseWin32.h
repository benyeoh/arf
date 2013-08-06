//==============================================================================
//
//		CIMouseWin32.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/19/2007
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

class CIMouseWin32 : public CIMouse
{
private: 
	boolean m_IsDeltaRecentlyUpdated;
	
public: 
	CIMouseWin32()
		: CIMouse()
		, m_IsDeltaRecentlyUpdated(FALSE)
	{
	}

	virtual ~CIMouseWin32()
	{
	}

protected:
	void DoUpdate();

public: 
	boolean HandleWindowsMsg(Win32Msg& msg);

};

_NAMESPACE_END