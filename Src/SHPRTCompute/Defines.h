//================================================================================
//
//		Defines.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/29/2012
//
//================================================================================

_NAMESPACE_BEGIN

_DECLARE_BFX_LIB_ID(2, SHPRTCOMPUTE)

const uint	NUM_SHPRT_BANDS = 3;
const uint	SHPRT_COMPUTE_OCCL_RT_LOG_SIZE = 6;
const uint	SHPRT_COMPUTE_OCCL_RT_SIZE = 1 << SHPRT_COMPUTE_OCCL_RT_LOG_SIZE;
const uint	NUM_LATENCY_BUFFERS = 3;
const uint	SHPRT_BRDF_SAMPLE_RANGE = 64;

enum
{
	SHPRTCOMPUTE_PARABOLOID_VIEW = SHPRTCOMPUTE_EFFECT_PARAM_OFFSET,

	SHPRTCOMPUTE_END_DYNAMIC_SEMANTICS
};

// Effect Constant Parameter Semantics
enum
{

};

// Material Type ID
enum
{
	SHPRTCOMPUTE_MAT_OCCLUSION = SHPRTCOMPUTE_MATERIAL_TYPE_OFFSET,
};

_NAMESPACE_END