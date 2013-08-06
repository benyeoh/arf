//==============================================================================
//
//		PlatformFramework.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, March 19, 2006
//
//==============================================================================

#ifndef _PLATFORMFRAMEWORK_H
#define _PLATFORMFRAMEWORK_H

#include "Platform.h"

_NAMESPACE_BEGIN

class CPPlatform;

//extern CPPlatform* g_pPlatform;

_NAMESPACE_END

// Additional abstract class implementation includes
#include "CPTimer.h"
#include "CPThread.h"
#include "CPThreadPool.h"
#include "CPResourceMgr.h"
#include "CPPlatform.h"


#endif