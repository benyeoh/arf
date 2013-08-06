//================================================================================
//
//		PRTCompute.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/22/2012
//
//================================================================================

#pragma once

#if defined(_IS_USE_APPHELPER_LIB_PRTCOMPUTE)

_NAMESPACE_BEGIN

// Render libs related
_DECLARE_BFX_LIB_ID(17, AH_PRTCOMPUTE)

// Effect Constant Parameter Semantics
enum
{
	AH_PRTCOMPUTE_EFFECT_REQUIRE_PRT = AH_PRTCOMPUTE_EFFECT_PARAM_OFFSET + (BFX_MAX_SEMANTICS_PER_LIB >> 1),
};

enum
{
	AH_PRTCOMPUTE_MAT_ACCUM = AH_PRTCOMPUTE_MATERIAL_TYPE_OFFSET,
	AH_PRTCOMPUTE_MAT_BACKFACE,
	AH_PRTCOMPUTE_MAT_LIGHTING,
};

_NAMESPACE_END

#include "AHPRTComputePipeline.h"
#include "AHScenePRTComputeSetup.h"
#include "AHScenePRTComputePhase.h"
#include "AHScenePRTCompute.h"

#endif