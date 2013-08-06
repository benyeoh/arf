//==============================================================================
//
//		Input.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/14/2007
//
//==============================================================================

#ifndef _INPUT_H
#define _INPUT_H

#include <Core\Core.h>
#include <MathUtil\MathUtil.h>

#if defined(_MSC_VER) && defined(_WINDOWS)
	#ifdef INPUT_EXPORTS
		#define INPUT_API __declspec(dllexport)
	#else
		#define INPUT_API __declspec(dllimport)
	#endif
#else
	#define INPUT_API
#endif

#include "Interfaces.h"


#endif
