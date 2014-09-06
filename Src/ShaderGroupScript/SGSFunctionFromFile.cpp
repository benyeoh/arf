//==============================================================================
//
//		SGSFunctionFromFile.cpp
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Thursday, 21 August, 2014
//
//==============================================================================

#include "ShaderGroupScript.h"

_NAMESPACE_BEGIN

boolean SGSFunctionFromFile::Execute(SGSValue* parameters, int numParameters, SGSValue* pResults, int& numResults)
{
	_DEBUG_ASSERT(numParameters == NUM_PARAMS);

	wchar filePath[256];
	uint strLen = parameters[0].pData->GetDataLength();
	_DEBUG_ASSERT(strLen < 255);
	_LOOPi(strLen)
	{
		const char* pStr = (const char*) parameters[0].pData->GetData();
		filePath[i] = (wchar) pStr[i];
	}

	filePath[strLen] = 0;

	IByteBufferPtr pFileData = m_pAppCallback->GetFileData(filePath);
	if(pFileData && numResults >= 1)
	{
		pResults[0].SetData(pFileData);
		numResults = 1;
		return TRUE;
	}

	return FALSE;
}

_NAMESPACE_END