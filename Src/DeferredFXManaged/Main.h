//================================================================================
//
//		Main.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		12/10/2011
//
//================================================================================

#pragma once

namespace ARFManaged
{
	namespace DeferredFXManaged
	{
		public ref class Main
		{
		private:
			static Module m_hMod = NULL;
			static CreateDeferredFXFn m_CreateFn = NULL;

		public:
			static bool Initialize(String^ hModuleName, CoreMemoryManaged::MemAllocator^ hMemAllocator);
			static void Shutdown();
			static DeferredFX^ CreateDeferredFX();
		};
	}
}