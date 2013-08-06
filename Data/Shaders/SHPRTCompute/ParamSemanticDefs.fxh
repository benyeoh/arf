//================================================================================
//
//		ParamSemanticDefs.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/2/2012
//
//================================================================================

#ifndef _SHPRTCOMPUTE_PARAMSEMANTICDEFS
#define _SHPRTCOMPUTE_PARAMSEMANTICDEFS

const static int SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN	= 0xDEADBEEF;
const static int SHPRTCOMPUTE_WEIGHT_TEXTURE			= SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN+1;
const static int SHPRTCOMPUTE_SRC_TEXTURE_1				= SHPRTCOMPUTE_WEIGHT_TEXTURE+1;
const static int SHPRTCOMPUTE_SRC_TEXTURE_2				= SHPRTCOMPUTE_SRC_TEXTURE_1+1;
const static int SHPRTCOMPUTE_SRC_TEXTURE_3				= SHPRTCOMPUTE_SRC_TEXTURE_2+1;
const static int SHPRTCOMPUTE_TEX_SIZE_RCP				= SHPRTCOMPUTE_SRC_TEXTURE_3+1;
const static int SHPRTCOMPUTE_PARA_VIEW					= SHPRTCOMPUTE_TEX_SIZE_RCP+1;
const static int SHPRTCOMPUTE_DYNAMIC_SEMANTIC_END		= SHPRTCOMPUTE_PARA_VIEW+1;

#endif