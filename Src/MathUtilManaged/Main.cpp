//==============================================================================
//
//		Main.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/26/2009
//
//==============================================================================

#include "MathUtilManaged.h"

#pragma managed(push, off)

static ICMMemAllocator* g_pAllocator = NULL;

_DECL_CORE_MEM_ALLOC_FNS(g_pAllocator)

#pragma managed(pop)

namespace ARFManaged
{
	namespace MathUtilManaged
	{
		bool 
		Main::Initialize(CoreMemoryManaged::MemAllocator^ hMemAllocator)
		{
			if(hMemAllocator)
			{
				g_pAllocator = hMemAllocator->GetImpl();
				g_pAllocator->AddRef();

				MemAllocFns allocFns = GetCoreMemoryBindings();
				_BIND_TO_GLOBAL_ALLOC(allocFns);
			}

			return true;
		}

		void 
		Main::Shutdown()
		{
			_RELEASE(g_pAllocator);
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




