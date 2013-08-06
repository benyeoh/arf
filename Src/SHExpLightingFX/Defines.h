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

// Effect Dynamic Parameter Semantics
enum
{
	SEFX_DYNAMIC_SEMANTIC_BEGIN = 260,

	SEFX_TEX_R1 = 260,
	SEFX_TEX_R2 = 261,
	SEFX_TEX_R3 = 262,
	SEFX_TEX_R4 = 263,
	SEFX_TEX_G1 = 264,
	SEFX_TEX_G2 = 265,
	SEFX_TEX_G3 = 266,
	SEFX_TEX_G4 = 267,
	SEFX_TEX_B1 = 268,
	SEFX_TEX_B2 = 269,
	SEFX_TEX_B3 = 270,
	SEFX_TEX_B4 = 271,

	SEFX_UNIT_COEFFS		= 272,
	SEFX_VIS_SPHERES_32		= 273,	// Currently unused
	SEFX_VIS_SPHERES_64		= 274,	// Currently unused
	SEFX_VIS_SPHERES_128	= 275,	// Currently unused

	SEFX_TEX_VIS_SPHERES	= 276,
	SEFX_TEX_YLM_1			= 277,
	SEFX_TEX_YLM_2			= 278,
	SEFX_TEX_YLM_3			= 279,
	SEFX_TEX_YLM_4			= 280,
	SEFX_TEX_AB_TABLE		= 281,
	
	SEFX_TEX_VIS_SPHERES_ATTR	= 282,
	SEFX_NUM_VIS_SPHERES		= 283,
	SEFX_COVERAGE_SPHERE		= 284,

	SEFX_DYNAMIC_SEMANTIC_END = 300,
};

// Effect Constant Parameter Semantics
enum
{
	SEFX_CONSTANT_SEMANTIC_BEGIN = 5200,

	SEFX_OCCL_TEX_SIZE	= 5200,
	SEFX_TEX_OCCL_1		= 5201,
	SEFX_TEX_OCCL_2		= 5202,
	SEFX_TEX_OCCL_3		= 5203,
	SEFX_TEX_OCCL_4		= 5204,

	SEFX_CONSTANT_SEMANTIC_END = 5250,
};

// Render Group ID
enum
{
	SEFX_GROUP_DEPTH_COPY	  = 13000,
	SEFX_GROUP_OCCLUDER_ACCUM = 13001,
	SEFX_GROUP_COLLAPSE_LIGHT = 13002,
};

// Material Type ID
enum
{
	SEFX_MAT_COLLAPSE_LIGHT = 32,
	SEFX_MAT_OCCLUDER_ACCUM = 33,
	SEFX_MAT_STENCIL_VOLUME = 34,
};

const static uint SEFX_MAX_SPHERE_GROUPS		= 128;
const static uint SEFX_MAX_SPHERES_PER_GROUP	= 128;

const static uint SEFX_MIN_SH_BANDS = 2;
const static uint SEFX_MAX_SH_BANDS = 4;

_NAMESPACE_END