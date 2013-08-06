//==============================================================================
//
//		RendererFeedback.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Wednesday, May 31, 2006
//
//==============================================================================

#include "stdafx.h"
#include "ShaderVerifier.h"

//boolean
//RendererFeedback::GetParameter(uint semantic, REffectParam& toFill)
//{
//	return TRUE;
//}

//uint 
//RendererFeedback::GetParamSemanticID(const wchar* pSemantic)
//{
//	return 0;
//}

_NAMESPACE_BEGIN

IByteBuffer* 
RendererFeedback::GetFileData(const wchar* pFileName)
{
	if(g_pFileSystem)
	{
		wstring fileName;//wstring fileName = _W("shaders\\");
		fileName += pFileName;
		
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
RendererFeedback::Log(const wchar* pMsg)
{
	wprintf(_W("%s\n"), pMsg);
}

_NAMESPACE_END