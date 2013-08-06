//==============================================================================
//
//		AppCallback.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/9/2008
//
//==============================================================================

#include "stdafx.h"

_USE_NAMESPACE

IByteBuffer* 
AppCallback::GetFileData(const wchar* pFileName)
{	
	InitializeFileSystem();
	
	if(g_pFileSystem)
	{
		wstring fileName = pFileName;//wstring fileName = _W("shaders\\");

		if(g_pFileSystem->CheckFileExists(fileName.c_str()))
		{
			IFFilePtr pFile = g_pFileSystem->GetFile(fileName.c_str());

			IByteBuffer* pBuffer = _NEW CByteBuffer(pFile->Length());
			uint bytesRead = pFile->Read(pBuffer->GetData(), pBuffer->GetBufferSize());
			pBuffer->SetDataLength(bytesRead);

			if(bytesRead == pFile->Length())
				return pBuffer;

			_DEBUG_ASSERT(FALSE);
			_DELETE( pBuffer );
		}
	}

	return NULL;
}

void 
AppCallback::Log(const wchar* pMsg)
{
	XSI::Application app;
	app.LogMessage(pMsg, XSI::siErrorMsg);
}
