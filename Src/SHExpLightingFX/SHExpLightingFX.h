//==============================================================================
//
//		SHExpLightingFX.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		13/10/2009
//
//==============================================================================

#ifndef _SHEXPLIGHTING_FX
#define _SHEXPLIGHTING_FX

#include <Core.h>
#include <Renderer.h>
#include <BaseFX.h>
#include <DeferredFX.h>
#include <MathUtil.h>

#if defined(_MSC_VER) && defined(_WINDOWS)
	#if defined(SHEXPLIGHTINGFX_EXPORTS) && defined(_USRDLL)
		#define SHEXPLIGHTINGFX_API __declspec(dllexport)
	#elif defined(SHEXPLIGHTINGFX_EXPORTS)
		#define SHEXPLIGHTINGFX_API  __declspec(dllimport)
	#else
		#define SHEXPLIGHTINGFX_API
	#endif
#else
	#define SHEXPLIGHTINGFX_API
#endif

#include "Defines.h"
#include "Interfaces.h"

#endif