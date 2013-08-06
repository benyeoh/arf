//==============================================================================
//
//		BloomFX.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/19/2008
//
//==============================================================================

#ifndef _BLOOM_FX
#define _BLOOM_FX

#include <Core.h>
#include <Renderer.h>

#if defined(_MSC_VER) && defined(_WINDOWS)
	#if defined(BLOOMFX_EXPORTS) && defined(_USRDLL)
		#define BLOOMFX_API __declspec(dllexport)
	#elif defined(BLOOMFX_IMPORTS)
		#define BLOOMFX_API __declspec(dllimport)
	#else
		#define BLOOMFX_API
	#endif
#else
	#define BLOOMFX_API
#endif

#include "Defines.h"
#include "Interfaces.h"

#endif