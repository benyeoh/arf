//==============================================================================
//
//		Defines.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/28/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

const static uint BFX_MAX_NUM_LIBS			= 128;

const static uint BFX_MAX_SEMANTICS_SHIFT			= 8;
const static uint BFX_MAX_SEMANTICS_PER_LIB			= 1 << BFX_MAX_SEMANTICS_SHIFT;
const static uint BFX_MAX_MATERIAL_TYPES_SHIFT		= 2;
const static uint BFX_MAX_MATERIAL_TYPES_PER_LIB	= 1 << BFX_MAX_MATERIAL_TYPES_SHIFT;

const static uint BFX_MAX_PARAM_CALLBACKS	= 128;
const static uint BFX_MAX_VOLATILE_PARAMS	= 31;
const static uint BFX_MAX_MATERIAL_TYPES	= 128;
const static uint BFX_MAX_POINT_LIGHTS		= 64;
const static uint BFX_MAX_SPOT_LIGHTS		= 64;

const static uint BFX_INVALID = 0xFFFFFFFF;

const static uint BFX_ALPHA_MODE_OPAQUE			= 0;
const static uint BFX_ALPHA_MODE_ALPHATESTED	= 1;
const static uint BFX_ALPHA_MODE_TRANSLUCENT	= 2;
const static uint BFX_ALPHA_MODE_ADDITIVE		= 3;

// Compile time checks
#define _DECLARE_BFX_LIB_ID(id, name) \
	const static uint name##_LIB_ID = id; \
	const static uint g_LibRangeOffset##id = name##_LIB_ID; \
	const static uint name##_EFFECT_PARAM_OFFSET = g_LibRangeOffset##id * BFX_MAX_SEMANTICS_PER_LIB; \
	const static uint name##_MATERIAL_TYPE_OFFSET = g_LibRangeOffset##id * BFX_MAX_MATERIAL_TYPES_PER_LIB;

// Declare BFX lib ID
_DECLARE_BFX_LIB_ID(0, BFX)

#define _GET_LIB_INDEX(semantic) (semantic >> BFX_MAX_SEMANTICS_SHIFT)
#define _GET_LIB_INDEX_FROM_MAT_TYPE(mat) (mat >> BFX_MAX_MATERIAL_TYPES_SHIFT)

// Effect Dynamic Parameter Semantics
enum
{
	BFX_WORLD = BFX_EFFECT_PARAM_OFFSET,
	BFX_VIEW,
	BFX_PROJ,
	BFX_VIEW_PROJ,
	BFX_WORLD_VIEW_PROJ,
	BFX_DIR_LIGHT,
	BFX_HEMI_LIGHT,
	BFX_POINT_LIGHT1,
	BFX_POINT_LIGHT2,
	BFX_POINT_LIGHT3,
	BFX_POINT_LIGHT4,
	BFX_SPOT_LIGHT1,
	BFX_SPOT_LIGHT2,
	BFX_SPOT_LIGHT3,
	BFX_SPOT_LIGHT4,
	BFX_CAMERA_POS_WITH_VIEW_FAR,
	BFX_WORLD_VIEW,
	BFX_CAMERA_POS_WITH_VIEW_NEAR,

	BFX_END_DYNAMIC_SEMANTICS,
};

// Effect Constant Parameter Semantics
enum
{
	BFX_DIFFUSE_TEX = BFX_EFFECT_PARAM_OFFSET + (BFX_MAX_SEMANTICS_PER_LIB >> 1),
	BFX_GLOSS_MAP_TEX,
	BFX_NORMAL_MAP_TEX,
	BFX_CUBE_ENV_MAP_TEX,
	BFX_PARALLAX_THICKNESS,
	BFX_ALPHA_MODE,
	BFX_FRESNEL,
	BFX_ENV_MAP_TINT,
	BFX_EFFECT_TEXT_DESC,
	BFX_EFFECT_VERTEX_DESC1,
	BFX_EFFECT_VERTEX_DESC2,
	BFX_EFFECT_VERTEX_DESC3,
	BFX_EFFECT_VERTEX_DESC4,
	BFX_EFFECT_VERTEX_DESC5,

	BFX_END_CONSTANT_SEMANTICS,
};

// Render Group ID
//enum
//{
//	BFX_GROUP_MAIN_SETUP = 12000,
//	BFX_GROUP_OPAQUE = 16001,
//	BFX_GROUP_ALPHA_TESTED = 16002,
//	BFX_GROUP_ADDITIVE = 16003,
//	BFX_GROUP_TRANSLUCENT = 16004,
//};

// Material Type ID
enum
{
	BFX_MAT_BASE_LIGHTING = BFX_MATERIAL_TYPE_OFFSET,
};

_NAMESPACE_END
