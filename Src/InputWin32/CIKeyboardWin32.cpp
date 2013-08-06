//==============================================================================
//
//		CIKeyboardWin32.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/19/2007
//
//==============================================================================

#include "InputWin32.h"


_NAMESPACE_BEGIN

#define _SET_KEY_STATE(msg, val) \
 \
	switch(msg) \
	{ \
		case 0x30:	SetKeyState(IKC_0, val);	break; \
		case 0x31:	SetKeyState(IKC_1, val);	break; \
		case 0x32:	SetKeyState(IKC_2, val);	break; \
		case 0x33:	SetKeyState(IKC_3, val);	break; \
		case 0x34:	SetKeyState(IKC_4, val);	break; \
		case 0x35:	SetKeyState(IKC_5, val);	break; \
		case 0x36:	SetKeyState(IKC_6, val);	break; \
		case 0x37:	SetKeyState(IKC_7, val);	break; \
		case 0x38:	SetKeyState(IKC_8, val);	break; \
		case 0x39:	SetKeyState(IKC_9, val);	break; \
 \
		case 0x41:	SetKeyState(IKC_A, val);	break; \
		case 0x42:	SetKeyState(IKC_B, val);	break; \
		case 0x43:	SetKeyState(IKC_C, val);	break; \
		case 0x44:	SetKeyState(IKC_D, val);	break; \
		case 0x45:	SetKeyState(IKC_E, val);	break; \
		case 0x46:	SetKeyState(IKC_F, val);	break; \
		case 0x47:	SetKeyState(IKC_G, val);	break; \
		case 0x48:	SetKeyState(IKC_H, val);	break; \
		case 0x49:	SetKeyState(IKC_I, val);	break; \
		case 0x4A:	SetKeyState(IKC_J, val);	break; \
		case 0x4B:	SetKeyState(IKC_K, val);	break; \
		case 0x4C:	SetKeyState(IKC_L, val);	break; \
		case 0x4D:	SetKeyState(IKC_M, val);	break; \
		case 0x4E:	SetKeyState(IKC_N, val);	break; \
		case 0x4F:	SetKeyState(IKC_O, val);	break; \
		case 0x50:	SetKeyState(IKC_P, val);	break; \
		case 0x51:	SetKeyState(IKC_Q, val);	break; \
		case 0x52:	SetKeyState(IKC_R, val);	break; \
		case 0x53:	SetKeyState(IKC_S, val);	break; \
		case 0x54:	SetKeyState(IKC_T, val);	break; \
		case 0x55:	SetKeyState(IKC_U, val);	break; \
		case 0x56:	SetKeyState(IKC_V, val);	break; \
		case 0x57:	SetKeyState(IKC_W, val);	break; \
		case 0x58:	SetKeyState(IKC_X, val);	break; \
		case 0x59:	SetKeyState(IKC_Y, val);	break; \
		case 0x5A:	SetKeyState(IKC_Z, val);	break; \
 \
		case VK_SPACE:	SetKeyState(IKC_SPACE, val); break; \
		case VK_ESCAPE:	SetKeyState(IKC_ESC, val); break; \
		case VK_TAB: SetKeyState(IKC_TAB, val); break; \
		case VK_LEFT: SetKeyState(IKC_LEFT, val); break; \
		case VK_UP: SetKeyState(IKC_UP, val); break; \
		case VK_RIGHT: SetKeyState(IKC_RIGHT, val); break; \
		case VK_DOWN: SetKeyState(IKC_DOWN, val); break; \
 \
		default:	break; \
	} \


boolean
CIKeyboardWin32::HandleWindowsMsg(Win32Msg &msg)
{
	switch(msg.msg)
	{
		case WM_KEYDOWN:
		//{
		//	_SET_KEY_STATE(msg.wParam, I_ACTIVE);
		//	return TRUE;
		//}
		case WM_KEYUP:
		{
			_DEBUG_ASSERT(m_NumMsgs < MAX_MESSAGES_PER_FRAME);
			m_CurMsg[m_NumMsgs] = msg;
			m_NumMsgs++;
			//_SET_KEY_STATE(msg.wParam, I_INACTIVE);
			return TRUE;
		}

		default: break;
	}
	
	return FALSE;
}

void
CIKeyboardWin32::DoUpdate()
{
	_LOOPi(m_NumMsgs)
	{
		switch(m_CurMsg[i].msg)
		{
		case WM_KEYDOWN:
			{
				_SET_KEY_STATE(m_CurMsg[i].wParam, I_ACTIVE);
				break;
			}

		case WM_KEYUP:
			{
				_SET_KEY_STATE(m_CurMsg[i].wParam, I_INACTIVE);
				break;
			}

		//default: break;
		}

	}

	m_NumMsgs = 0;
	//m_CurMsg.msg = WM_NULL;
}


_NAMESPACE_END