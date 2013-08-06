//==============================================================================
//
//		Main.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/29/2008
//
//==============================================================================

#include "RendererManaged.h"

#pragma managed(push, off)

static ICMMemAllocatorPtr g_pAllocator;

_DECL_CORE_MEM_ALLOC_FNS(g_pAllocator)

#pragma managed(pop)

namespace ARFManaged
{
	namespace RendererManaged
	{
		bool
		Main::Initialize(String^ hModuleName, CoreMemoryManaged::MemAllocator^ hMemAllocator)
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

			m_CreateFn = (CreateRendererFn) GetFunction(m_hMod, "CreateRenderer");
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

		Renderer^
		Main::CreateRenderer()
		{
			return gcnew Renderer(m_CreateFn());
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




