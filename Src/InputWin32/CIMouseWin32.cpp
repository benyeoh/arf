//==============================================================================
//
//		CIMouseWin32.cpp
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
CIMouseWin32::HandleWindowsMsg(Win32Msg& msg)
{
	switch(msg.msg)
	{
		case WM_MOUSEMOVE:
		{
			DWORD x = LOWORD(msg.lParam);
			DWORD y = HIWORD(msg.lParam);
			
			gmtl::Vec3f newPos;
			newPos[0] = (float) x;
			newPos[1] = (float)	y;
			newPos[2] = 0.0f;
			
			const gmtl::Vec3f& pos = GetPos();
			
			gmtl::Vec3f newPosDelta = newPos - pos;
			
			//_DEBUG_TRACE(_W("pos: %f, %f, %f\n"), newPosDelta[0], newPosDelta[1], newPosDelta[2]);
			SetPos(newPos);
			SetPosDelta(newPosDelta);
			
			m_IsDeltaRecentlyUpdated = TRUE;
			
			return TRUE;
		}

		case WM_LBUTTONDOWN:
		{
			//_DEBUG_TRACE(_W("Left Button Down\n"));
			SetButtonState(IMC_LEFTBUTTON, I_ACTIVE);
			return TRUE;
		}		

		case WM_RBUTTONDOWN:
		{
			//_DEBUG_TRACE(_W("Right Button Down\n"));
			SetButtonState(IMC_RIGHTBUTTON, I_ACTIVE);
			return TRUE;
		}
		
		case WM_LBUTTONUP:
		{
			//_DEBUG_TRACE(_W("Left Button Up\n"));
			SetButtonState(IMC_LEFTBUTTON, I_INACTIVE);
			return TRUE;
		}
		
		case WM_RBUTTONUP:
		{
			//_DEBUG_TRACE(_W("Right Button Up\n"));
			SetButtonState(IMC_RIGHTBUTTON, I_INACTIVE);
			return TRUE;
		}
	}
	
	return FALSE;
}

void
CIMouseWin32::DoUpdate()
{
	if(m_IsDeltaRecentlyUpdated)
		m_IsDeltaRecentlyUpdated = FALSE;
	else	
		SetPosDelta(gmtl::Vec3f(0.0f, 0.0f, 0.0f));
}


_NAMESPACE_END