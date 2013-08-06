//================================================================================
//
//		Interfaces.h
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
		public value struct Metrics
		{
			uint numPagesFree;
			uint numPageAllocs;
			uint numChunkAllocs;
			uint totalPageSizeUsed;
			uint totalChunkSizeUsed;
			uint totalHeapSizeUsed;			
		};

		_PROXY_CLASS_BEGIN(MemAllocator, ICMMemAllocator)
			//Metrics GetMetrics() 
			void	DumpLeaks() { m_pImpl->DumpLeaks(); }
		_PROXY_CLASS_END
	}
}