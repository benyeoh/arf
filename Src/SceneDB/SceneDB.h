//==============================================================================
//
//		SceneDB.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		12/8/2008
//
//==============================================================================

#ifndef _SCENE_DB
#define _SCENE_DB

#include <Core\Core.h>
#include <MathUtil\MathUtil.h>

#if defined(_MSC_VER) && defined(_WINDOWS)
	#if defined(SCENEDB_EXPORTS) && defined(_USRDLL)
		#define SCENEDB_API __declspec(dllexport)
	#elif defined(SCENEDB_IMPORTS)
		#define SCENEDB_API __declspec(dllimport)
	#else
		#define SCENEDB_API
	#endif
#else
	#define SCENEDB_API
#endif

#include "Interfaces.h"

#endif