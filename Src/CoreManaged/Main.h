//==============================================================================
//
//		Main.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/29/2008
//
//==============================================================================

#pragma once

namespace ARFManaged
{
	namespace CoreManaged
	{

		public ref class Main
		{
		private:			
		public:
			static bool Initialize(CoreMemoryManaged::MemAllocator^ hMemAllocator);
			static void Shutdown();
		};


	}
}