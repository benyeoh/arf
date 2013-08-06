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
#include <Windows.h>

// TODO: reference additional headers your program requires here
#include <Core\Core.h>
#include <CoreMemory\CoreMemory.h>
#include <FileSystem\FileSystem.h>
#include <MaterialGen\MaterialGen.h>
#include <MaterialGenUtils\MaterialGenUtils.h>

extern _NAMESPACE IFFileSystemPtr g_pFileSystem;
extern _NAMESPACE IMGMaterialGenPtr g_pMaterialGen;

#include "AppCallback.h"