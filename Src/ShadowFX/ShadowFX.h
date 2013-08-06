//==============================================================================
//
//		ShadowFX.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		8/4/2008
//
//==============================================================================

#ifndef _SHADOW_FX
#define _SHADOW_FX

#include <Core\Core.h>
#include <Renderer\Renderer.h>
#include <BaseFX\BaseFX.h>
#include <BaseFXUtils\BaseFXUtils.h>

#if defined(_MSC_VER) && defined(_WINDOWS)
	#if defined(SHADOWFX_EXPORTS) && defined(_USRDLL)
		#define SHADOWFX_API __declspec(dllexport)
	#elif defined(SHADOWFX_IMPORTS)
		#define SHADOWFX_API  __declspec(dllimport)
	#else
		#define SHADOWFX_API
	#endif
#else
	#define SHADOWFX_API
#endif

#include "Defines.h"
#include "Interfaces.h"

#endif