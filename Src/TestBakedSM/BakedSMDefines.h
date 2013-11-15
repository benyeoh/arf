//================================================================================
//
//		BakedSMDefines.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/29/2012
//
//================================================================================

_DECLARE_BFX_LIB_ID(8, BAKEDSMCOMPUTE)

const uint  BAKED_SM_NUM_SH_BANDS = 4;
const uint	BAKED_SM_COMPUTE_SM_RT_LOG_SIZE = 6;
const uint	BAKED_SM_COMPUTE_SM_RT_SIZE = 1 << BAKED_SM_COMPUTE_SM_RT_LOG_SIZE;
const uint	BAKED_SM_NUM_LATENCY_BUFFERS = 3;
const float BAKED_SM_EXP_K_VAL = 10.0f;
const float BAKED_SM_MAX_FAR_PLANE = 30.0f;

enum
{
    BAKEDSMCOMPUTE_PARABOLOID_VIEW = BAKEDSMCOMPUTE_EFFECT_PARAM_OFFSET,
    BAKEDSMCOMPUTE_FAR_PLANE,
    
    BAKEDSMCOMPUTE_BAKED_SM_EXP,    // Obviously this should just be a fixed constant

    BAKEDSMCOMPUTE_END_DYNAMIC_SEMANTICS
};

// Effect Constant Parameter Semantics
enum
{
    BAKEDSMCOMPUTE_BAKED_SM_TEX1 = BAKEDSMCOMPUTE_EFFECT_PARAM_OFFSET + (BFX_MAX_SEMANTICS_PER_LIB >> 1),
    BAKEDSMCOMPUTE_BAKED_SM_TEX2,
    BAKEDSMCOMPUTE_BAKED_SM_TEX3,
    BAKEDSMCOMPUTE_BAKED_SM_TEX4,
};


// Material Type ID
enum
{
    BAKEDSMCOMPUTE_MAT_OCCLUSION = BAKEDSMCOMPUTE_MATERIAL_TYPE_OFFSET,
};
