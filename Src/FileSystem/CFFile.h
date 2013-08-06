//==============================================================================
//
//		CFFile.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/23/2007
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

class CFFile : public CRefCounted<IFFile>
{
private:
	boolean m_IsOpen;
	
protected:	
	wstring m_FileName;
	
public:
	CFFile(const wchar* pFileName)
		: m_IsOpen(FALSE)
	{
		m_FileName = pFileName;
	}

	virtual ~CFFile()
	{
	}

protected:
	virtual boolean DoOpen(eFileOpen flag) = 0;
	virtual void DoClose() = 0;
	virtual uint DoRead(void* dst, uint length) = 0;
	virtual uint DoWrite(const void* src, uint length) = 0;
	virtual boolean DoSeek(uint offset, eFileSeek origin) = 0;
	virtual uint DoTell() = 0;

public:
	boolean Open(eFileOpen flag);
	uint Read(void* dst, uint length);
	uint Write(const void* src, uint length);
	boolean Seek(uint offset, eFileSeek origin);
	uint Tell();
	uint Length();
	void Close();	
	const wchar* GetFileName();
};

_NAMESPACE_END