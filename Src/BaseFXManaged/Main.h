//==============================================================================
//
//		Main.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/12/2009
//
//==============================================================================

#pragma once

namespace ARFManaged
{
	namespace BaseFXManaged
	{
		public ref class Main
		{
		private:
			static Module m_hMod = NULL;
			static CreateBaseFXFn m_CreateFn = NULL;

		public:
			static bool Initialize(String^ hModuleName, CoreMemoryManaged::MemAllocator^ hMemAllocator);
			static void Shutdown();
			static BaseFX^ CreateBaseFX();
		};
	}
}