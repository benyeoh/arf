//================================================================================
//
//		Interfaces.h
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
	namespace SHPRTFXManaged
	{
		public enum struct ParamSemantic
		{	
			// Effect Dynamic Parameter Semantics
			SHPRTFX_BRDF_TRANSFER_TEX1 = _NAMESPACE SHPRTFX_BRDF_TRANSFER_TEX1,
			SHPRTFX_BRDF_TRANSFER_TEX2 = _NAMESPACE SHPRTFX_BRDF_TRANSFER_TEX2,
		};

		//_PROXY_CLASS_BEGIN(SHPRTFX, ISHPRTFX)
		//private:
		//	//ResourceMgr^	m_hResource;

		//public:
		//	//ResourceMgr^	GetResourceMgr()	{ return m_hResource; }

		//	//boolean Initialize(RendererManaged::Renderer^ hRenderer, 
		//	//	MaterialGenManaged::MaterialGen^ hMatGen, 
		//	//	CoreManaged::AppCallback^ hCallback);

		//	//array<uint>^ Render(Mesh^ hMesh, float detailLevel, uint matType, 
		//	//	ParamContainer^ hParamContainer, RenderContainer^ hRenderContainer);

		//_PROXY_CLASS_END		
	}
}