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
	namespace ShadowFXManaged
	{
		public enum struct ParamSemantic
		{	
			// Effect Dynamic Parameter Semantics
			SHFX_DIR_LIGHT_CASCADED_PROJS	= _NAMESPACE SHFX_DIR_LIGHT_CASCADED_PROJS,
			SHFX_DIR_LIGHT_Z_SPLITS_NEAR	= _NAMESPACE SHFX_DIR_LIGHT_Z_SPLITS_NEAR,
			SHFX_DIR_LIGHT_Z_SPLITS_FAR		= _NAMESPACE SHFX_DIR_LIGHT_Z_SPLITS_FAR,
			SHFX_DIR_LIGHT_CASTER_PROJ		= _NAMESPACE SHFX_DIR_LIGHT_CASTER_PROJ,

			SHFX_DIR_LIGHT_CASCADED_RT		= _NAMESPACE SHFX_DIR_LIGHT_CASCADED_RT,

			SHFX_POINT_LIGHT_CAST_PARAMS	= _NAMESPACE SHFX_POINT_LIGHT_CAST_PARAMS,
			SHFX_POINT_LIGHT_SM_1			= _NAMESPACE SHFX_POINT_LIGHT_SM_1,
			SHFX_POINT_LIGHT_SM_2			= _NAMESPACE SHFX_POINT_LIGHT_SM_2,
			SHFX_POINT_LIGHT_SM_3			= _NAMESPACE SHFX_POINT_LIGHT_SM_3,
			SHFX_POINT_LIGHT_SM_4			= _NAMESPACE SHFX_POINT_LIGHT_SM_4,

			SHFX_CUBE_POINT_LIGHT_CAST_PARAMS	= _NAMESPACE SHFX_CUBE_POINT_LIGHT_CAST_PARAMS,
			SHFX_CUBE_POINT_LIGHT_CAST_VIEW_PROJ	= _NAMESPACE SHFX_CUBE_POINT_LIGHT_CAST_VIEW_PROJ,
			SHFX_CUBE_POINT_LIGHT_SM_1			= _NAMESPACE SHFX_CUBE_POINT_LIGHT_SM_1,
			SHFX_CUBE_POINT_LIGHT_SM_2			= _NAMESPACE SHFX_CUBE_POINT_LIGHT_SM_2,
			SHFX_CUBE_POINT_LIGHT_SM_3			= _NAMESPACE SHFX_CUBE_POINT_LIGHT_SM_3,
			SHFX_CUBE_POINT_LIGHT_SM_4			= _NAMESPACE SHFX_CUBE_POINT_LIGHT_SM_4,

			SHFX_DEBUG_OFFSET				= _NAMESPACE SHFX_DEBUG_OFFSET,
			SHFX_DEBUG_SM					= _NAMESPACE SHFX_DEBUG_SM,

			SHFX_DIR_LIGHT_Z_SPLITS_VIEW_FAR	= _NAMESPACE SHFX_DIR_LIGHT_Z_SPLITS_VIEW_FAR,
			SHFX_DIR_LIGHT_Z_SPLITS_VIEW_NEAR	= _NAMESPACE SHFX_DIR_LIGHT_Z_SPLITS_VIEW_NEAR,

			SHFX_DIR_LIGHT_TEX_OFFSETS = _NAMESPACE SHFX_DIR_LIGHT_TEX_OFFSETS,
		};

		_PROXY_CLASS_BEGIN(ShadowFX, ISHFXShadowFX)
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