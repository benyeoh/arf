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

#include "SHPRTComputeManaged.h"

#pragma managed(push, off)

static ICMMemAllocatorPtr g_pAllocator;

_DECL_CORE_MEM_ALLOC_FNS(g_pAllocator)

#pragma managed(pop)

namespace ARFManaged
{
	namespace SHPRTComputeManaged
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

			m_CreateFn = (CreateSHPRTComputeFn) GetFunction(m_hMod, "CreateSHPRTCompute");
			//_DEBUG_ASSERT(m_CreateFn);

			if(m_CreateFn)
				return true;

			return false;
		}

		void Main::Shutdown()
		{
			ShutdownModule(m_hMod);

			g_pAllocator = NULL;
		}

		SHPRTCompute^ Main::CreateSHPRTCompute()
		{
			return gcnew SHPRTCompute(m_CreateFn());
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
