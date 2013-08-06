//==============================================================================
//
//		AppHelpers.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/16/2007
//
//==============================================================================

#pragma once 

#include <Core/Core.h>
#include <FileSystem/FileSystem.h>
#include <Platform/Platform.h>
#include <MathUtil/MathUtil.h>
#include <Renderer/Renderer.h>
#include <RendererUtils/RendererUtils.h>
#include <BaseFX/BaseFX.h>
#include <BaseFXUtils/BaseFXUtils.h>
#include <SceneDB/SceneDB.h>
#include <ShadowFX/ShadowFX.h>
#include <DeferredFX/DeferredFX.h>
#include <SHPRTCompute/SHPRTCompute.h>
#include <SHPRTFX/SHPRTFX.h>

//=============================================================================
//	Apphelper library usage defines
//		Comment out these to remove lib from AppHelper module
//=============================================================================
#define _IS_USE_APPHELPER_LIB_BASIC
#define _IS_USE_APPHELPER_LIB_PRTCOMPUTE


_NAMESPACE_BEGIN

class AHEntityManager;
class AHEntity;

_NAMESPACE_END

#include "Defines.h"
#include "AHComponent.h"
#include "AHEntity.h"
#include "AHEntityManager.h"
#include "AHScreenSpaceFrustumCuller.h"
#include "LoadSaveFns.h"
#include "ComputeBounds.h"

// Libs
#include "Basic.h"
#include "PRTCompute.h"