//==============================================================================
//
//		CFFileEnumeratorWin32.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		6/6/2007
//
//==============================================================================

#include "FileSystemWin32.h"

_NAMESPACE_BEGIN

CFFileEnumeratorWin32::CFFileEnumeratorWin32(const wchar* pRootPath, const wchar* pHierachyPath, 
											 const wchar* pFileFilter, boolean isIncludeSubHierachies)
	: CFFileEnumerator()
	, m_CurFileIndex(0)
	//, m_hSearch(NULL)
{
	m_RootPath = pRootPath;

	wstring dirPath = pHierachyPath;
	GetAllFiles(m_AllFileNames, dirPath.c_str(), pFileFilter, isIncludeSubHierachies);

	//int index = (int) m_SearchPath.rfind(_W("\\"));
	//int strSize = (int) m_SearchPath.size();
	//if(index != wstring::npos)
	//{
	//	m_DirPath = m_SearchPath.substr(0, index);
	//}
	
	// m_DirPath += _W("\\");
}

CFFileEnumeratorWin32::~CFFileEnumeratorWin32()
{
	//if(m_hSearch)
	//{
	//	FindClose(m_hSearch);
	//}
}

void CFFileEnumeratorWin32::GetAllFiles(std::vector<wstring>& toStore, const wchar* pDirPath, const wchar* pFileFilter, boolean isIncludeSubHierachies)
{
	// If we are going to recurse over all the subdirectories, first of all
	// get all the files that are in this directory that match the filter
	if(isIncludeSubHierachies)
		 GetAllFiles(toStore, pDirPath, pFileFilter, FALSE);

	WIN32_FIND_DATA findFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	wstring curDirectory = pDirPath;
	wstring curFilter = curDirectory + _W("\\") + (isIncludeSubHierachies ? _W("*") : pFileFilter);

	// Find the first file in the directory.
	hFind = FindFirstFile((m_RootPath + _W("\\") + curFilter).c_str(), &findFileData);

	if(hFind != INVALID_HANDLE_VALUE)
	{		
		// Add the first file found to the list
		if(!isIncludeSubHierachies)
		{
			toStore.push_back(curDirectory + _W("\\") + findFileData.cFileName);
		}

		// List all the other files in the directory.
		while(FindNextFile(hFind, &findFileData) != 0)
		{
			if(!isIncludeSubHierachies)
			{
				toStore.push_back(curDirectory + _W("\\") + findFileData.cFileName);
			}
			else
			{
				// If we found a directory then recurse into it
				if( ((findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) > 0) && (findFileData.cFileName[0] != _W('.')) )
				{
					GetAllFiles(toStore, (curDirectory + _W("\\") + findFileData.cFileName).c_str(), pFileFilter, TRUE);
				}
			}
		}

		FindClose(hFind);
	}
}

IFFile*
CFFileEnumeratorWin32::DoGetNextFile()
{	
	//if(m_hSearch == INVALID_HANDLE_VALUE)
	//	return NULL;

	//IFFile* pFile = NULL;
	//wstring fileName = m_RootPath;
	//fileName += _W("\\") + m_SearchPath;
	//		
	//if(!m_hSearch)
	//{
	//	m_hSearch = FindFirstFile(fileName.c_str(), &m_FindData);
	//}
	//else
	//{
	//	BOOL foundFile = FindNextFile(m_hSearch, &m_FindData);			
	//	m_hSearch = foundFile ? m_hSearch : INVALID_HANDLE_VALUE;
	//}
	//
	//if(m_hSearch == INVALID_HANDLE_VALUE)
	//	return NULL;
	//	
	//fileName = m_DirPath;
	//fileName += m_FindData.cFileName;
	//

	//// Assumes the file system is open
	//fileName = m_SearchPath + fileName;;
	
	IFFile* pFile = NULL;
	if(m_CurFileIndex < m_AllFileNames.size())
	{
		pFile = g_pFileSystem->GetFile(m_AllFileNames[m_CurFileIndex].c_str());				
		m_CurFileIndex++;
	}

	return pFile;
}

_NAMESPACE_END