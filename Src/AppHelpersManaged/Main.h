//================================================================================
//
//		Main.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/21/2012
//
//================================================================================

#pragma once

namespace ARFManaged
{
	namespace AppHelpersManaged
	{
		public ref class Main
		{
		private:
			static Module m_hMod = NULL;
			//static CreateSHPRTFXFn m_CreateFn = NULL;

		public:
			static bool Initialize(String^ hModuleName, CoreMemoryManaged::MemAllocator^ hMemAllocator);
			static void Shutdown();
			//static SHPRTFX^ CreateSHPRTFX();
		};
	}
}