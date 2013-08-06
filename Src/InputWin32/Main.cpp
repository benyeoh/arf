//==============================================================================
//
//		Main.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/19/2007
//
//==============================================================================

#include "InputWin32.h"

_NAMESPACE_BEGIN

CIInput* g_pInput = NULL;
Win32Msg s_WndMsg;

_NAMESPACE_END

_USE_NAMESPACE

//HMODULE g_hCoreMem = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//if(!g_hCoreMem)
		//{
		//	g_hCoreMem = ::LoadLibrary(_W("CoreMemory.dll"));

		//	GetCoreMemoryBindingsFn GetCoreMemBindings = (GetCoreMemoryBindingsFn) ::GetProcAddress(g_hCoreMem, "GetCoreMemoryBindings");
		//	MemAllocFns allocFns = GetCoreMemBindings();
		//	_BIND_TO_GLOBAL_ALLOC(allocFns);
		//}
		break;

	case DLL_THREAD_ATTACH:	break;
	case DLL_THREAD_DETACH:	break;
	case DLL_PROCESS_DETACH:
		{
			//if(g_hCoreMem)
			//{
			//	::FreeLibrary(g_hCoreMem);
			//	g_hCoreMem = NULL;
			//}	
			break;
		}
	}
	return TRUE;
}

//------------------------------------------------------------------------------
//	CreateInput()
//		
//------------------------------------------------------------------------------
IIInput* 
CreateInput()
{
	if(!g_pInput)
		g_pInput = _NEW CIInputWin32;

	//if(!g_pWndMsg)
	//	g_pWndMsg = &s_WndMsg;
		
	return g_pInput;
}

LRESULT
FeedWindowsMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	s_WndMsg.hWnd = hWnd;
	s_WndMsg.msg = message;
	s_WndMsg.wParam = wParam;
	s_WndMsg.lParam = lParam;
	
	if(!g_pInput)
		return 1;

	return !((CIInputWin32*)g_pInput)->HandleWindowsMsg(s_WndMsg);
	
}

