//==============================================================================
//
//		SaveFns.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/1/2008
//
//==============================================================================

#pragma once 

namespace ARFManaged
{
	namespace MaterialGenManaged
	{
		public ref class SaveFns
		{
		public:
			static uint SaveMaterialTemplate(MaterialTemplate^ hTemplate, CoreManaged::ByteBuffer^ hBuffer)
			{
				return _NAMESPACE SaveMaterialTemplate(hTemplate->GetImpl(), hBuffer->GetImpl());
			}	
		};
		
		
	}		
}