// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here
#include <Core/Core.h>
#include <CoreMemory/CoreMemory.h>
#include <Platform/Platform.h>

_USE_NAMESPACE

extern IPPlatformPtr g_pPlatform;

void RunThreads();
void RunThreadPool();