//==============================================================================
//
//		ParamSemanticDefs.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/29/2008
//
//==============================================================================

#pragma once

// Effect Dynamic Parameter Semantics
const static int DFX_DYNAMIC_SEMANTIC_BEGIN = 0xDEADBEEF;

const static int DFX_VIEW = DFX_DYNAMIC_SEMANTIC_BEGIN+1;
const static int DFX_PROJ = DFX_VIEW+1;
const static int DFX_VIEW_PROJ = DFX_PROJ+1;
const static int DFX_WORLD_BATCH = DFX_VIEW_PROJ+1;
const static int DFX_COLOR_BATCH = DFX_WORLD_BATCH+1;
const static int DFX_POS_BATCH = DFX_COLOR_BATCH+1;
const static int DFX_QUAD_SIZE_BATCH = DFX_POS_BATCH+1;
const static int DFX_QUAD_TEX_POS_BATCH = DFX_QUAD_SIZE_BATCH+1;
const static int DFX_QUAD_TEX_SIZE_BATCH = DFX_QUAD_TEX_POS_BATCH+1;
const static int DFX_QUAD_TEXTURE = DFX_QUAD_TEX_SIZE_BATCH+1;

const static int DFX_DYNAMIC_SEMANTIC_END		= DFX_QUAD_TEXTURE+1;

// Effect Constant Parameter Semantics
const static int DFX_CONSTANT_SEMANTIC_BEGIN = DFX_DYNAMIC_SEMANTIC_END;

const static int DFX_CONSTANT_SEMANTIC_END	= DFX_CONSTANT_SEMANTIC_BEGIN+1;