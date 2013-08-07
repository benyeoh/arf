//==============================================================================
//
//		DebugFX.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/20/2008
//
//==============================================================================

#ifndef _DEBUG_FX
#define _DEBUG_FX

#include <Core\Core.h>
#include <MathUtil\MathUtil.h>
#include <Renderer\Renderer.h>

#if 0 //defined(_MSC_VER) && defined(_WINDOWS) && defined(_USRDLL)
	#ifdef DEBUGFX_EXPORTS
		#define DEBUGFX_API __declspec(dllexport)
	#else
		#define DEBUGFX_API __declspec(dllimport)
	#endif
#else
	#define DEBUGFX_API
#endif

#include "Interfaces.h"

#endif