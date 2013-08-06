//==============================================================================
//
//		CFFileWin32.cpp
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
CFFileWin32::DoOpen(eFileOpen flag)
{
	const static wchar* s_pModes[3] =
	{
		_W("rb"),
		_W("wb"),
		_W("r+b"),
	};

	_DEBUG_ASSERT( m_pFileHandle == NULL );

	_wfopen_s(&m_pFileHandle, m_AbsolutePath.c_str(), s_pModes[flag]);

	return m_pFileHandle != NULL;
}

void 
CFFileWin32::DoClose()
{
	_DEBUG_ASSERT( m_pFileHandle );

	fclose( m_pFileHandle );

	m_pFileHandle = NULL;
}

uint 
CFFileWin32::DoRead(void* dst, uint length)
{
	return (uint)(fread( dst, sizeof(char), length, m_pFileHandle ));
}

uint 
CFFileWin32::DoWrite(const void* src, uint length)
{
	return (uint)(fwrite( src, sizeof(char), length, m_pFileHandle));
}

boolean 
CFFileWin32::DoSeek(uint offset, eFileSeek origin)
{
	uint seekType;

	switch(origin)
	{
	case FS_BEGIN:	seekType = SEEK_SET; break;
	case FS_END: seekType = SEEK_END; break;
	case FS_CUR: seekType = SEEK_CUR; break;
	}

	return ( fseek(m_pFileHandle, offset, seekType) == 0 );
}

uint 
CFFileWin32::DoTell()
{
	return (uint)( ftell(m_pFileHandle) );
}


_NAMESPACE_END