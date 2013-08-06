//==============================================================================
//
//		CIInputWin32.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/19/2007
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

class CIInputWin32 : public CIInput
{
private: 

public: 
	CIInputWin32()
		: CIInput(_NEW CIMouseWin32, _NEW CIKeyboardWin32)
	{
	}

	virtual ~CIInputWin32()
	{
	}

protected: 
	boolean DoInitialize();

public:
	boolean HandleWindowsMsg(Win32Msg& msg);
};

_NAMESPACE_END