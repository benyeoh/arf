//==============================================================================
//
//		CFFileSystemWin32.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/23/2007
//
//==============================================================================

#include "FileSystemWin32.h"


_NAMESPACE_BEGIN

boolean 
CFFileSystemWin32::DoInitialize()
{
	return TRUE;
}

boolean
CFFileSystemWin32::DoCheckFileExists(const wchar* pFileName)
{
	wstring fileName = m_ResourcePath;

	fileName = fileName + _W("\\") + pFileName;

	if(PathFileExists(fileName.c_str()))
		return TRUE;

	return FALSE;
}

void CFFileSystemWin32::DoResolvePath(wstring& srcDest)
{
	wstring temp = srcDest;
	srcDest = m_ResourcePath + _W("\\") + temp;
}

IFFile* 
CFFileSystemWin32::DoGetFile(const wchar* pFileName)
{
	wstring fileName = m_ResourcePath;

	fileName += _W("\\");
	fileName += pFileName;

	return _NEW CFFileWin32(fileName.c_str(), pFileName);

	// TODO: Will always return a file handle now
	// Should not return a file if it cannot find one in read mode
}

IFFileEnumerator* 
CFFileSystemWin32::DoGetFiles(const wchar* pHierarchyPath, const wchar* pFileFilter, boolean isIncludeSubHierachies)
{
	//wstring fileName = m_ResourcePath;

	//fileName = fileName + _W("\\") + pFileName;

	return _NEW CFFileEnumeratorWin32(m_ResourcePath.c_str(), pHierarchyPath, pFileFilter, isIncludeSubHierachies);

	// TODO: Will always return a file handle now
	// Should not return a file if it cannot find one in read mode
}


boolean 
CFFileSystemWin32::DoOpen()
{
	// TODO: Do nothing for now -- For compressed file system or database initialization?
	return TRUE;
}

void 
CFFileSystemWin32::DoClose()
{
	// TODO: Do nothing for now
}


_NAMESPACE_END