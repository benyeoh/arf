//================================================================================
//
//		ParamSemantics.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/2/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

// Effect Dynamic Parameter Semantics
const static int SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN	= 0xDEADBEEF;
const static int SHPRTCOMPUTE_WEIGHT_TEXTURE			= SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN+1;
const static int SHPRTCOMPUTE_SRC_TEXTURE_1				= SHPRTCOMPUTE_WEIGHT_TEXTURE+1;
const static int SHPRTCOMPUTE_SRC_TEXTURE_2				= SHPRTCOMPUTE_SRC_TEXTURE_1+1;
const static int SHPRTCOMPUTE_SRC_TEXTURE_3				= SHPRTCOMPUTE_SRC_TEXTURE_2+1;
const static int SHPRTCOMPUTE_TEX_SIZE_RCP				= SHPRTCOMPUTE_SRC_TEXTURE_3+1;
const static int SHPRTCOMPUTE_PARA_VIEW					= SHPRTCOMPUTE_TEX_SIZE_RCP+1;
const static int SHPRTCOMPUTE_DYNAMIC_SEMANTIC_END		= SHPRTCOMPUTE_PARA_VIEW+1;

const static int SHPRTCOMPUTE_NUM_DYNAMIC_SEMANTIC		= SHPRTCOMPUTE_DYNAMIC_SEMANTIC_END - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1;

// Effect Constant Parameter Semantics
//const static int FFX_CONSTANT_SEMANTIC_BEGIN = FFX_DYNAMIC_SEMANTIC_END;
//const static int FFX_CONSTANT_SEMANTIC_END	= FFX_CONSTANT_SEMANTIC_BEGIN+1;


_NAMESPACE_END