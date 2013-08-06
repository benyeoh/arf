//==============================================================================
//
//		AppCallback.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Tuesday, January 30, 2007
//
//==============================================================================

#include "stdafx.h"

IByteBuffer* 
AppCallback::GetFileData(const wchar* pFileName)
{
	wstring fileName;// = _W("src\\shaders\\");
	fileName += pFileName;

	if(g_pFileSystem->CheckFileExists(fileName.c_str()))
	{
		IFFilePtr pFile = g_pFileSystem->GetFile(fileName.c_str());

		IByteBuffer* pBuffer = _NEW CByteBuffer();
		uint bytesRead = AppendData(pBuffer, pFile);

		if(bytesRead == pFile->Length())
			return pBuffer;

		_DEBUG_ASSERT(FALSE);
		_DELETE( pBuffer );
	}

	return NULL;
}


void 
AppCallback::Log(const wchar* pMsg)
{
	_TRACE(pMsg);
}
