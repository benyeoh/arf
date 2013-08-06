//==============================================================================
//
//		CIInputWin32.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/19/2007
//
//==============================================================================

#include "InputWin32.h"


_NAMESPACE_BEGIN

boolean
CIInputWin32::DoInitialize()
{
	// Do nothing
	
	return TRUE;
}

boolean
CIInputWin32::HandleWindowsMsg(Win32Msg& msg)
{
	boolean isHandled = FALSE;
	
	if(m_pKeyboard)
		isHandled = ((CIKeyboardWin32*)m_pKeyboard)->HandleWindowsMsg(msg);
		
	if(!isHandled)
		if(m_pMouse)
			isHandled = ((CIMouseWin32*)m_pMouse)->HandleWindowsMsg(msg);
			
	return isHandled;
}


_NAMESPACE_END