//==============================================================================
//
//		CFFileSystemWin32.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/23/2007
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CFFileSystemWin32 : public CFFileSystem
{

public:
	CFFileSystemWin32()
		: CFFileSystem()
	{

	}

	virtual ~CFFileSystemWin32()
	{
		Close();
	}

protected:
	boolean DoInitialize();
	IFFile* DoGetFile(const wchar* pFileName);
	boolean DoOpen();
	void DoClose();
	boolean DoCheckFileExists(const wchar* pFileName);
	IFFileEnumerator* DoGetFiles(const wchar* pHierarchyPath, const wchar* pFileFilter, boolean isIncludeSubHierachies);

	void DoResolvePath(wstring& srcDest);
};

_NAMESPACE_END