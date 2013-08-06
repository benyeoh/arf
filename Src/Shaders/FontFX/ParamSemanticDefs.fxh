//==============================================================================
//
//		ParamSemanticDefs.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		12/16/2007
//
//==============================================================================

#ifndef _FONTFX_PARAMSEMANTICDEFS
#define _FONTFX_PARAMSEMANTICDEFS

// Effect Dynamic Parameter Semantics
const static int FFX_DYNAMIC_SEMANTIC_BEGIN = 1000;

const static int FFX_FONT_TEXTURE = FFX_DYNAMIC_SEMANTIC_BEGIN+1;

const static int FFX_DYNAMIC_SEMANTIC_END		= FFX_FONT_TEXTURE+1;

// Effect Constant Parameter Semantics
const static int FFX_CONSTANT_SEMANTIC_BEGIN = FFX_DYNAMIC_SEMANTIC_END;

const static int FFX_CONSTANT_SEMANTIC_END	= FFX_CONSTANT_SEMANTIC_BEGIN+1;

// Effect Type IDs
const static int FFX_TYPE_FONT			= 1000;

#endif