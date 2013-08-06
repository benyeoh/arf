//================================================================================
//
//		Interfaces.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		12/10/2011
//
//================================================================================

#pragma once 

namespace ARFManaged
{
	namespace DeferredFXManaged
	{
		public enum struct ParamSemantic
		{	
			// Effect Dynamic Parameter Semantics
			DEFX_Z_POS_ACCUM	= _NAMESPACE DEFX_Z_POS_ACCUM,
			DEFX_NORMALS_ACCUM	= _NAMESPACE DEFX_NORMALS_ACCUM,
			DEFX_FAR_PLANE_CORNER_VECS	= _NAMESPACE DEFX_FAR_PLANE_CORNER_VECS,
			DEFX_ALBEDO_ACCUM = _NAMESPACE DEFX_ALBEDO_ACCUM,
			DEFX_FAR_PLANE_OFFSETS	= _NAMESPACE DEFX_FAR_PLANE_OFFSETS,
			DEFX_SCREEN_UV_OFFSET	= _NAMESPACE DEFX_SCREEN_UV_OFFSET,
			DEFX_POINT_LIGHT		= _NAMESPACE DEFX_POINT_LIGHT,
			DEFX_VIEW_FAR_PLANE_OFFSETS	= _NAMESPACE DEFX_VIEW_FAR_PLANE_OFFSETS,
			DEFX_DIR_LIGHT			= _NAMESPACE DEFX_DIR_LIGHT,

			// Constant
			DEFX_BRDF_ATLAS_TEX = _NAMESPACE DEFX_BRDF_ATLAS_TEX,
			DEFX_BRDF_PARAM_TEX = _NAMESPACE DEFX_BRDF_PARAM_TEX,
			DEFX_BRDF_MAT_INDEX = _NAMESPACE DEFX_BRDF_MAT_INDEX,
		};

		_PROXY_CLASS_BEGIN(DeferredFX, IDEFXDeferredFX)
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