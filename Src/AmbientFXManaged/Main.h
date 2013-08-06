//================================================================================
//
//		Main.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		5/17/2012
//
//================================================================================

#pragma once

namespace ARFManaged
{
	namespace AmbientFXManaged
	{
		public ref class Main
		{
		private:
			static Module m_hMod = NULL;
			static CreateAmbientFXFn m_CreateFn = NULL;

		public:
			static bool Initialize(String^ hModuleName, CoreMemoryManaged::MemAllocator^ hMemAllocator);
			static void Shutdown();
			static AmbientFX^ CreateAmbientFX();
		};
	}
}