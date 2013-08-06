//==============================================================================
//
//		Defines.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		13/10/2009
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

const static uint DEFX_BRDF_TABLE_SIZE = 128;
const static uint DEFX_NUM_TABLES_PER_SIDE = 4;
const static uint DEFX_MAX_NUM_TABLES = DEFX_NUM_TABLES_PER_SIDE * DEFX_NUM_TABLES_PER_SIDE;

_DECLARE_BFX_LIB_ID(3, DEFX)

// Effect Dynamic Parameter Semantics
enum
{
	DEFX_Z_POS_ACCUM = DEFX_EFFECT_PARAM_OFFSET,
	DEFX_NORMALS_ACCUM,
	DEFX_FAR_PLANE_CORNER_VECS,
	DEFX_ALBEDO_ACCUM,
	DEFX_FAR_PLANE_OFFSETS,
	DEFX_SCREEN_UV_OFFSET,
	//DEFX_BRDF_TABLE_TEX,
	//DEFX_BRDF_PROP_TEX,
	DEFX_POINT_LIGHT,
	DEFX_VIEW_FAR_PLANE_OFFSETS,
	DEFX_DIR_LIGHT,

	DEFX_END_DYNAMIC_SEMANTICS,
};

// Effect Constant Parameter Semantics
enum
{
	DEFX_BRDF_ATLAS_TEX = DEFX_EFFECT_PARAM_OFFSET + (BFX_MAX_SEMANTICS_PER_LIB >> 1),
	DEFX_BRDF_PARAM_TEX,
	DEFX_BRDF_MAT_INDEX,

	DEFX_END_CONSTANT_SEMANTICS,
};

// Material Type ID
enum
{
	DEFX_MAT_ACCUM = DEFX_MATERIAL_TYPE_OFFSET,
	DEFX_MAT_LIGHTING,
};

_NAMESPACE_END
