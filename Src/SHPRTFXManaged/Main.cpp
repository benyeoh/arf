//================================================================================
//
//		Main.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		5/14/2012
//
//================================================================================

#include "SHPRTFXManaged.h"

#pragma managed(push, off)

static ICMMemAllocatorPtr g_pAllocator;

_DECL_CORE_MEM_ALLOC_FNS(g_pAllocator)

#pragma managed(pop)

namespace ARFManaged
{
	namespace SHPRTFXManaged
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

			//m_CreateFn = (CreateSHPRTFXFn) GetFunction(m_hMod, "CreateSHPRTFX");
			////_DEBUG_ASSERT(m_CreateFn);

			//if(m_CreateFn)
			//	return true;

			return true;
		}

		void Main::Shutdown()
		{
			ShutdownModule(m_hMod);

			g_pAllocator = NULL;
		}

		//SHPRTFX^ Main::CreateSHPRTFX()
		//{
		//	return gcnew SHPRTFX(m_CreateFn());
		//}
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
