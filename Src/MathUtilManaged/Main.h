//==============================================================================
//
//		Main.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		5/3/2009
//
//==============================================================================

#pragma once

namespace ARFManaged
{
	namespace MathUtilManaged
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