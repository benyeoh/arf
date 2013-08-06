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
	namespace RendererManaged
	{
		public ref class Main
		{
		private:
			static Module m_hMod = NULL;
			static CreateRendererFn m_CreateFn = NULL;

		public:
			static bool Initialize(String^ hModuleName, CoreMemoryManaged::MemAllocator^ hAllocator);
			static void Shutdown();

			static Renderer^ CreateRenderer();
		};
	}
}