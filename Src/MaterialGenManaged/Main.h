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
	namespace MaterialGenManaged
	{

		public ref class Main
		{
		private:
			static Module m_hMod = NULL;
			static CreateMaterialGenFn m_CreateFn = NULL;

		public:
			static bool Initialize(String^ hModuleName, CoreMemoryManaged::MemAllocator^ hMemAllocator);
			static void Shutdown();
			static MaterialGen^ CreateMaterialGen();
		};

	}
}