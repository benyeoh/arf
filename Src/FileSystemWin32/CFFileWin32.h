//==============================================================================
//
//		CFFileWin32.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, March 19, 2006
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CFFileWin32 : public CFFile
{
private:
	FILE*		m_pFileHandle;
	wstring		m_AbsolutePath;
	
public:
	CFFileWin32(const wchar* pAbsolutePath, const wchar* pRelativePath)
		: CFFile(pRelativePath)
		, m_pFileHandle(NULL)
	{
		m_AbsolutePath = pAbsolutePath;
	}

	virtual ~CFFileWin32()
	{
		Close();
	}

protected:
	boolean DoOpen(eFileOpen flag);
	void DoClose();
	uint DoRead(void* dst, uint length);
	uint DoWrite(const void* src, uint length);
	boolean DoSeek(uint offset, eFileSeek origin);
	uint DoTell();
};

_NAMESPACE_END