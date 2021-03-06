// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#include <stdio.h>
#include <tchar.h>
#include <list>

#include <Windows.h>

#include <CoreMemory\CoreMemory.h>
#include <Renderer\Renderer.h>
#include <RendererUtils\RendererUtils.h>
#include <FileSystem\FileSystem.h>
#include <MaterialGen\MaterialGen.h>
#include <BaseFX\BaseFX.h>
#include <BaseFXUtils\BaseFXUtils.h>

_USE_NAMESPACE

#include "VertexCache.h"

// TODO: reference additional headers your program requires here
