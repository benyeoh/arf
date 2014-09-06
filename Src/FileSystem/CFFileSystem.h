//==============================================================================
//
//		CFFileSystem.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/23/2007
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

class CFFileSystem : public CRefCounted<IFFileSystem>
{
private:
	boolean	m_IsOpen;

protected:
	wstring m_ResourcePath;
	std::map<wstring, wstring> m_Mnemonics;
	
public:
	CFFileSystem()
		: m_IsOpen(FALSE)
	{
	}

	virtual ~CFFileSystem()
	{
	}

private:
	void ResolveMnemonics(const wchar* pPath, wstring& toStore);
	const wchar* FindMnemonic(const wchar* pMnemonic);
	
protected:
	_PURE( boolean DoInitialize() )
	
	_PURE( IFFile* DoGetFile(const wchar* pFileName) )
	_PURE( boolean DoOpen() )
	_PURE( void DoClose() )
	_PURE( boolean DoCheckFileExists(const wchar* pFileName) )
	_PURE( IFFileEnumerator* DoGetFiles(const wchar* pHierarchyPath, const wchar* pFileFilter, boolean isIncludeSubHierachies) )

	_PURE( void DoResolvePath(wstring& srcDest) )

public:
	boolean Initialize();
	
	void AddMnemonic(const wchar* pMnemonic, const wchar* pPath);
	void RemoveMnemonic(const wchar* pMnemonic);
	void ClearMnemonics();
	
	// TODO: Move this to a file utility project
	uint NormalizePath(const wchar* pFullPath, wchar* pNormPath, uint length);
	uint ResolvePath(const wchar* pResourcePath, wchar* pFullPath, uint length);

	const wchar * GetResourcePath() const;

	boolean Open(const wchar* pResourcePath);	
	IFFile* GetFile(const wchar* pFileName);
	IFFileEnumerator* GetFiles(const wchar* pHierarchyPath, const wchar* pFileFilter, boolean isIncludeSubHierachies);

	void Close();
	boolean CheckFileExists(const wchar* pFileName);
};

_NAMESPACE_END