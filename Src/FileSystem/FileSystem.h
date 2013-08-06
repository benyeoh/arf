//==============================================================================
//
//		FileSystem.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Monday, March 13, 2006
//
//==============================================================================

#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include <Core\Core.h>

#include "Interfaces.h"

//////////////////////////////////////////////////////////////////////////
//	 
//	DLL exposed functions
//
//////////////////////////////////////////////////////////////////////////

extern "C"
{
	typedef _NAMESPACE IFFileSystem* (*CreateFileSystemFn)();
	_DECL_DYNAMIC_EXPORT( _NAMESPACE IFFileSystem* CreateFileSystem() )
}

#endif
