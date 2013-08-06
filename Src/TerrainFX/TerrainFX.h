//==============================================================================
//
//		TerrainFX.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		8/4/2008
//
//==============================================================================

#ifndef _TERRAIN_FX
#define _TERRAIN_FX

#include <Core.h>
#include <Renderer.h>
#include <BaseFX.h>

#if defined(_MSC_VER) && defined(_WINDOWS)
	#if defined(TERRAINFX_EXPORTS) && defined(_USRDLL)
		#define TERRAINFX_API __declspec(dllexport)
	#elif defined(TERRAINFX_IMPORTS)
		#define TERRAINFX_API  __declspec(dllimport)
	#else
		#define TERRAINFX_API
	#endif
#else
	#define TERRAINFX_API
#endif

#include "Defines.h"
#include "Interfaces.h"

#endif