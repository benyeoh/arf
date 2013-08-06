//==============================================================================
//
//		ScreenAmbOcclFX.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		07/12/2009
//
//==============================================================================

#ifndef _SCREENAMBOCCL_FX
#define _SCREENAMBOCCL_FX

#include <Core.h>
#include <Renderer.h>
#include <BaseFX.h>
#include <DeferredFX.h>
#include <MathUtil.h>

#if defined(_MSC_VER) && defined(_WINDOWS)
    #if defined(SCREENAMBOCCLFX_EXPORTS) && defined(_USRDLL)
        #define SCREENAMBOCCLFX_API __declspec(dllexport)
    #elif defined(SCREENAMBOCCLFX_EXPORTS)
        #define SCREENAMBOCCLFX_API  __declspec(dllimport)
    #else
        #define SCREENAMBOCCLFX_API
    #endif
#else
    #define SCREENAMBOCCLFX_API
#endif

#include "Defines.h"
#include "Interfaces.h"

#endif