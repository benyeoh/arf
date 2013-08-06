//==============================================================================
//
//		CFFileEnumeratorWin32.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		6/6/2007
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CFFileEnumeratorWin32 : public CFFileEnumerator
{
private: 
	//HANDLE m_hSearch;
	//wstring m_SearchPath;
	//wstring m_DirPath;
	//
	//WIN32_FIND_DATA m_FindData;

	std::vector<wstring> m_AllFileNames;
	uint m_CurFileIndex;
	wstring m_RootPath;
	
public: 
	CFFileEnumeratorWin32(const wchar* pRootPath, const wchar* pHierachyPath, 
						  const wchar* pFileFilter, boolean isIncludeSubHierachies);
	~CFFileEnumeratorWin32();
	
private:
	void GetAllFiles(std::vector<wstring>& toStore, const wchar* pDirPath, const wchar* pFileFilter, boolean isIncludeSubHierachies);

protected:
	IFFile* DoGetNextFile();

};

_NAMESPACE_END
