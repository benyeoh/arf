//==============================================================================
//
//		CoreMemory.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/8/2008
//
//==============================================================================

#ifndef _CORE_MEMORY
#define _CORE_MEMORY

#include <Core\Core.h>

#if defined(_MSC_VER) && defined(_WINDOWS)
	#if defined(COREMEMORY_EXPORTS) && defined(_USRDLL)
		#define COREMEMORY_API __declspec(dllexport)
	#elif defined(COREMEMORY_IMPORTS)
		#define COREMEMORY_API __declspec(dllimport)
	#else
		#define COREMEMORY_API
	#endif
#else
	#define COREMEMORY_API
#endif

#include "Defines.h"
#include "Interfaces.h"

#endif