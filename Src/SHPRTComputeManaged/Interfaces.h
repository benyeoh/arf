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
	namespace SHPRTComputeManaged
	{
		public enum struct ParamSemantic
		{	
			// Effect Dynamic Parameter Semantics
			SHPRTCOMPUTE_PARABOLOID_VIEW = _NAMESPACE SHPRTCOMPUTE_PARABOLOID_VIEW,
		};

		_PROXY_CLASS_BEGIN(SHPRTCompute, ISHPRTCompute)
		private:
			//ResourceMgr^	m_hResource;

		public:
			//ResourceMgr^	GetResourceMgr()	{ return m_hResource; }

			//boolean Initialize(RendererManaged::Renderer^ hRenderer, 
			//	MaterialGenManaged::MaterialGen^ hMatGen, 
			//	CoreManaged::AppCallback^ hCallback);

			//array<uint>^ Render(Mesh^ hMesh, float detailLevel, uint matType, 
			//	ParamContainer^ hParamContainer, RenderContainer^ hRenderContainer);

		_PROXY_CLASS_END		
	}
}