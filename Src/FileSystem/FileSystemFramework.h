//==============================================================================
//
//		FileSystemFramework.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/23/2007
//
//==============================================================================

#ifndef _FILESYSTEMFRAMEWORK_H
#define _FILESYSTEMFRAMEWORK_H

#include "FileSystem.h"

_NAMESPACE_BEGIN

class CFFileSystem;

extern CFFileSystem* g_pFileSystem;

_NAMESPACE_END

// Additional abstract class implementation includes
#include "CFFile.h"
#include "CFFileSystem.h"
#include "CFFileEnumerator.h"

#endif