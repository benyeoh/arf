//==============================================================================
//
//		CFFileEnumerator.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		6/6/2007
//
//==============================================================================

#include "FileSystemFramework.h"

_NAMESPACE_BEGIN

IFFile* 
CFFileEnumerator::GetNextFile()
{
	return DoGetNextFile();
}

_NAMESPACE_END