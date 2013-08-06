//==============================================================================
//
//		CIKeyboardWin32.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/19/2007
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

class CIKeyboardWin32 : public CIKeyboard
{
	const static uint MAX_MESSAGES_PER_FRAME = 8;

private: 
	Win32Msg m_CurMsg[MAX_MESSAGES_PER_FRAME];
	uint	m_NumMsgs;

public: 
	CIKeyboardWin32()
		: CIKeyboard()
		, m_NumMsgs(0)
	{
	}

	virtual ~CIKeyboardWin32()
	{
	}

protected:
	void DoUpdate();
	
public: 
	boolean HandleWindowsMsg(Win32Msg& msg);

};

_NAMESPACE_END