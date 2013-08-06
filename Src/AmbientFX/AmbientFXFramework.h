//==============================================================================
//
//		AmbientFXFramework.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		5/31/2009
//
//==============================================================================

#pragma once

#include "AmbientFX.h"

#include <CoreMemory/CoreMemory.h>
#include <RendererUtils/RendererUtils.h>
#include <BaseFXUtils/BaseFXUtils.h>
#include <MathUtil/MathUtil.h>

_NAMESPACE_BEGIN

class CAFXAmbientFX;

_NAMESPACE_END

// Compress ambient cube map to 3 bands
const static uint NUM_SH_BANDS = 3;

//// TODO: Refactor this heavily
//#include <d3d9.h>
//#include <d3dx9.h>

#include "Defines.h"
#include "CAFXParamPool.h"
#include "CAFXSHLightToolKit.h"
#include "CAFXResourceMgr.h"
#include "CAFXAmbientFX.h"

