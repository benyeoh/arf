//================================================================================
//
//		Main.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		13/9/2011
//
//================================================================================

#pragma once

namespace ARFManaged
{
	namespace CoreMemoryManaged
	{
		public ref class Main
		{
		private:
			static Module g_hCoreMem = NULL;
			static CreateMemAllocatorFn g_CreateFn = NULL;

		public:
			static bool Initialize(String^ hMemModuleName);
			static void Shutdown();
			static MemAllocator^ CreateMemAllocator();
		};

	}
}