//================================================================================
//
//		Defines.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		5/18/2012
//
//================================================================================

_NAMESPACE_BEGIN

_DECLARE_BFX_LIB_ID(4, SHPRTFX)

enum
{	
	SHPRTFX_DUMMY_PARAM = SHPRTFX_EFFECT_PARAM_OFFSET,

	SHPRTFX_END_DYNAMIC_SEMANTICS,
};

// Effect Constant Parameter Semantics
enum
{
	SHPRTFX_BRDF_TRANSFER_TEX1 = SHPRTFX_EFFECT_PARAM_OFFSET + (BFX_MAX_SEMANTICS_PER_LIB >> 1),
	SHPRTFX_BRDF_TRANSFER_TEX2,
};

//// Material Type ID
//enum
//{
//	SHPRTCOMPUTE_MAT_OCCLUSION = SHPRTCOMPUTE_MATERIAL_TYPE_OFFSET,
//};

_NAMESPACE_END