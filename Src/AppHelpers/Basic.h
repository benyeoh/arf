//================================================================================
//
//		Basic.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/22/2012
//
//================================================================================

#pragma once

#if defined(_IS_USE_APPHELPER_LIB_BASIC)

_NAMESPACE_BEGIN

class AHRenderComponent;
class AHPointLightComponent;
class AHSpatialComponent;

#define _CAST_COMP_SPATIAL(pEntity)		((AHSpatialComponent*) (pEntity)->GetComponent(COMP_SPATIAL))
#define _CAST_COMP_RENDER(pEntity)		((AHRenderComponent*) (pEntity)->GetComponent(COMP_RENDER))
#define _CAST_COMP_POINTLIGHT(pEntity)	((AHPointLightComponent*) (pEntity)->GetComponent(COMP_POINTLIGHT))

// Scene DB filter
enum
{
	SDB_FILTER_RENDER_COMP = 0x00000001,
	SDB_FILTER_POINTLIGHT_COMP = 0x00000002,
	SDB_FILTER_DIRLIGHT_COMP = 0x00000004,
};

// Render libs related
_DECLARE_BFX_LIB_ID(16, AH_BASIC)

//// Effect Dynamic Parameter Semantics
//enum
//{
//	AH_DYNAMIC_START = AH_EFFECT_PARAM_OFFSET - 1,
//
//#include "BasicDynParamDefines.h"
//};

// Effect Constant Parameter Semantics
enum
{
	// X: metalscale, Y: diffuseScale, Z: brdfLobeScale
	AH_BASIC_METALNESS = AH_BASIC_EFFECT_PARAM_OFFSET + (BFX_MAX_SEMANTICS_PER_LIB >> 1),

	// This must be set together with the transfer textures
	AH_BASIC_DIFFUSE_LOBE_SCALE,

	// Only used for phong lobe
	AH_BASIC_PHONG_LOBE_SCALE, 
	//AH_BASIC_PHONG_FRESNEL,

};

_NAMESPACE_END

#include "AHSpatialComponent.h"
#include "AHRenderComponent.h"
#include "AHPointLightComponent.h"
#include "AHDirLightComponent.h"
#include "AHMaterialCreateCallbackImmed.h"
#include "AHMaterialCreateCallbackDeferred.h"
#include "AHSceneRenderPhase.h"

#endif