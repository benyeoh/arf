//================================================================================
//
//		Main.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		5/14/2012
//
//================================================================================

#pragma once

namespace ARFManaged
{
	namespace SHPRTComputeManaged
	{
		public ref class Main
		{
		private:
			static Module m_hMod = NULL;
			static CreateSHPRTComputeFn m_CreateFn = NULL;

		public:
			static bool Initialize(String^ hModuleName, CoreMemoryManaged::MemAllocator^ hMemAllocator);
			static void Shutdown();
			static SHPRTCompute^ CreateSHPRTCompute();
		};
	}
}