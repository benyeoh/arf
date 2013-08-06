//==============================================================================
//
//		ParamSemanticDefs.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		10/10/2007
//
//==============================================================================

#ifndef PARAMSEMANTICDEFS
#define PARAMSEMANTICDEFS

// Effect Dynamic Parameter Semantics
const static int RFX_DYNAMIC_SEMANTIC_BEGIN = -1;

const static int RFX_WORLD				= 0;
const static int RFX_VIEW				= RFX_WORLD+1;
const static int RFX_PROJ				= RFX_VIEW+1;
const static int RFX_VIEW_PROJ			= RFX_PROJ+1;
const static int RFX_WORLD_VIEW_PROJ	= RFX_VIEW_PROJ+1;
const static int RFX_DIR_LIGHT			= RFX_WORLD_VIEW_PROJ+1;
const static int RFX_HEMI_LIGHT			= RFX_DIR_LIGHT+1;
const static int RFX_POINT_LIGHT		= RFX_HEMI_LIGHT+1;
const static int RFX_CAMERA_POS			= RFX_POINT_LIGHT+1;
const static int RFX_AVERAGE_LUMINANCE	= RFX_CAMERA_POS+1;
const static int RFX_TEX_RT1	= RFX_AVERAGE_LUMINANCE+1;
const static int RFX_TEX_RT2	= RFX_TEX_RT1+1;
const static int RFX_TEX_RT3	= RFX_TEX_RT2+1;
const static int RFX_TEX_RT4	= RFX_TEX_RT3+1;

const static int RFX_DYNAMIC_SEMANTIC_END		= RFX_TEX_RT4+1;

// Effect Constant Parameter Semantics
const static int RFX_CONSTANT_SEMANTIC_BEGIN = RFX_DYNAMIC_SEMANTIC_END;

const static int RFX_DIFFUSE_TEX		= RFX_CONSTANT_SEMANTIC_BEGIN+1;
const static int RFX_GLOSS_MAP_TEX		= RFX_DIFFUSE_TEX+1;
const static int RFX_NORMAL_MAP_TEX		= RFX_GLOSS_MAP_TEX+1;
const static int RFX_CUBE_ENV_MAP_TEX	= RFX_NORMAL_MAP_TEX+1;
const static int RFX_PARALLAX_THICKNESS	= RFX_CUBE_ENV_MAP_TEX+1;
const static int RFX_ALPHA_MODE			= RFX_PARALLAX_THICKNESS+1;
const static int RFX_FILTER_TEXTURE_OFFSETS = RFX_ALPHA_MODE+1;
const static int RFX_FILTER_WEIGHTS		= RFX_FILTER_TEXTURE_OFFSETS+1;
const static int RFX_MIDDLE_GRAY_TONE	= RFX_FILTER_WEIGHTS+1;
const static int RFX_WHITE_CUTOFF		= RFX_MIDDLE_GRAY_TONE+1;
const static int RFX_BRIGHT_THRESHOLD	= RFX_WHITE_CUTOFF+1;
const static int RFX_BRIGHT_OFFSET		= RFX_BRIGHT_THRESHOLD+1;
const static int RFX_FRESNEL_EXP		= RFX_BRIGHT_OFFSET+1;
const static int RFX_FRESNEL_MIN		= RFX_FRESNEL_EXP+1;
const static int RFX_ENV_MAP_TINT		= RFX_FRESNEL_MIN+1;
const static int RFX_EFFECT_TYPE		= RFX_ENV_MAP_TINT+1;
const static int RFX_EFFECT_TEXT_DESC	= RFX_EFFECT_TYPE+1;
const static int RFX_EFFECT_VERTEX_DESC1		= RFX_EFFECT_TEXT_DESC+1;
const static int RFX_EFFECT_VERTEX_DESC2		= RFX_EFFECT_VERTEX_DESC1+1;
const static int RFX_EFFECT_VERTEX_DESC3		= RFX_EFFECT_VERTEX_DESC2+1;
const static int RFX_EFFECT_VERTEX_DESC4		= RFX_EFFECT_VERTEX_DESC3+1;
const static int RFX_EFFECT_VERTEX_DESC5		= RFX_EFFECT_VERTEX_DESC4+1;

const static int RFX_CONSTANT_SEMANTIC_END	= RFX_EFFECT_VERTEX_DESC5+1;

// Effect Type IDs
const static int RFX_TYPE_DIFFUSE		= 0;
const static int RFX_TYPE_HLDL1			= RFX_TYPE_DIFFUSE+1;
const static int RFX_TYPE_PL1			= RFX_TYPE_HLDL1+1;
const static int RFX_TYPE_POST_PROCESS	= RFX_TYPE_PL1+1;

#endif