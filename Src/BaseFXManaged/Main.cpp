//==============================================================================
//
//		Main.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/12/2009
//
//==============================================================================

#include "BaseFXManaged.h"

#pragma managed(push, off)

static ICMMemAllocatorPtr g_pAllocator;

_DECL_CORE_MEM_ALLOC_FNS(g_pAllocator)

#pragma managed(pop)

namespace ARFManaged
{
	namespace BaseFXManaged
	{
		bool Main::Initialize(String^ hModuleName, CoreMemoryManaged::MemAllocator^ hMemAllocator)
		{
			pin_ptr<const wchar> pModName = PtrToStringChars(hModuleName);

			if(hMemAllocator)
			{
				g_pAllocator = hMemAllocator->GetImpl();
				MemAllocFns allocFns = GetCoreMemoryBindings();
				_BIND_TO_GLOBAL_ALLOC(allocFns);

				m_hMod = InitializeModule(pModName, &allocFns);
			}
			else
			{
				m_hMod = InitializeModule(pModName, NULL);
			}

			m_CreateFn = (CreateBaseFXFn) GetFunction(m_hMod, "CreateBaseFX");
			//_DEBUG_ASSERT(m_CreateFn);

			if(m_CreateFn)
				return true;

			return false;
		}

		void 
		Main::Shutdown()
		{
			ShutdownModule(m_hMod);
			
			g_pAllocator = NULL;
		}

		BaseFX^ 
		Main::CreateBaseFX()
		{
			return gcnew BaseFX(m_CreateFn());
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




