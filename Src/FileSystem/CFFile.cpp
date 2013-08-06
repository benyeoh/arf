//==============================================================================
//
//		CFFile.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/23/2007
//
//==============================================================================

#include "FileSystemFramework.h"

_NAMESPACE_BEGIN

boolean 
CFFile::Open(eFileOpen flag)
{
	if(!m_IsOpen)
		m_IsOpen = DoOpen(flag);

	return m_IsOpen;
}

uint
CFFile::Tell()
{
	uint bytesRead = 0;

	if(!m_IsOpen)
	{
		if(Open(FO_READ_ONLY))
		{
			bytesRead = DoTell();
			Close();
		}
	}
	else
		bytesRead = DoTell();

	return bytesRead;
}

boolean
CFFile::Seek(uint offset, eFileSeek origin)
{
	boolean success = FALSE;

	if(!m_IsOpen)
	{
		if(Open(FO_READ_ONLY))
		{
			success = DoSeek(offset, origin);
			Close();
		}
	}
	else
		success = DoSeek(offset, origin);

	return success;
}

uint
CFFile::Length()
{
	uint size = 0;

	if(!m_IsOpen)
	{
		Open(FO_READ_ONLY);

		uint prevPos = Tell();
		Seek(0, FS_END);
		size = Tell();
		Seek(prevPos, FS_BEGIN);

		Close();
	}	
	else
	{
		uint prevPos = Tell();
		Seek(0, FS_END);
		size = Tell();
		Seek(prevPos, FS_BEGIN);
	}

	return size;
}

uint 
CFFile::Read(void* dst, uint length)
{
	uint bytesRead = 0;

	if(!m_IsOpen)
	{
		if(Open(FO_READ_ONLY))
		{
			bytesRead = DoRead(dst, length);
			Close();
		}
	}
	else
		bytesRead  = DoRead(dst, length);

	return bytesRead;
}

uint 
CFFile::Write(const void* src, uint length)
{
	uint bytesWritten = 0;

	if(!m_IsOpen)
	{
		if(Open(FO_WRITE_ONLY))
		{
			bytesWritten = DoWrite(src, length);
			Close();
		}
	}
	else
		bytesWritten  = DoWrite(src, length);

	return bytesWritten;
}

void 
CFFile::Close()
{
	if(m_IsOpen)
	{
		DoClose();
		m_IsOpen = FALSE;
	}
}

const wchar* 
CFFile::GetFileName()
{
	return m_FileName.c_str();
}

_NAMESPACE_END