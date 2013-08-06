//==============================================================================
//
//		Shaders.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Monday, June 05, 2006
//
//==============================================================================

#pragma once 

// Effect Parameter Semantics
const static int RFX_SEMANTICS_BEGIN	= -1;

const static int RFX_WORLD	= RFX_SEMANTICS_BEGIN+1;
const static int RFX_VIEW	= RFX_WORLD+1;
const static int RFX_PROJ	= RFX_VIEW+1;
const static int RFX_VIEW_PROJ = RFX_PROJ+1;
const static int RFX_WORLD_VIEW_PROJ = RFX_VIEW_PROJ+1;
const static int RFX_DIR_LIGHT = RFX_WORLD_VIEW_PROJ+1;

const static int RFX_SEMANTICS_END		= RFX_DIR_LIGHT+1;

// Effect Type IDs
const static int RFX_EFFECT_TYPE_BEGIN	= -1;

const static int RFX_DIFFUSE = RFX_EFFECT_TYPE_BEGIN+1;

const static int RFX_EFFECT_TYPE_END = RFX_DIFFUSE+1;


