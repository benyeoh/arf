//================================================================================
//
//		Main.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		13/9/2011
//
//================================================================================

#include "CoreMemoryManaged.h"

namespace ARFManaged
{
	namespace CoreMemoryManaged
	{
		bool 
		Main::Initialize(String^ hMemModuleName)
		{
			pin_ptr<const wchar> pModName = PtrToStringChars(hMemModuleName);
			g_hCoreMem = InitializeModule(pModName, NULL);

			g_CreateFn = (CreateMemAllocatorFn) GetFunction(g_hCoreMem, "CreateMemAllocator");			
			return true;
		}

		void 
		Main::Shutdown()
		{		
			if(g_hCoreMem)
			{
				ShutdownModule(g_hCoreMem);
				g_hCoreMem = NULL;
			}	
		}


		MemAllocator^ Main::CreateMemAllocator()
		{
			return gcnew MemAllocator(g_CreateFn());
		}
	}
}


#pragma managed(push, off)

BOOL APIENTRY DllMain( HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					  )
{
	return TRUE;
}

#pragma managed(pop)

