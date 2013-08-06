//==============================================================================
//
//		CommonUtilityFns.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/29/2007
//
//==============================================================================

#include "Core.h"

_NAMESPACE_BEGIN

// String Fns

uint 
ASCIIToUnicode(const char* pSrc, wchar* pDest)
{
	_DEBUG_ASSERT(pSrc);
	_DEBUG_ASSERT(pDest);

	uint i = 0;
	while( pSrc[i] != 0 )
	{
		pDest[i] = pSrc[i];
		++i;
	}

	pDest[i] = 0;

	return i+1;
}

uint
UnicodeToASCII(const wchar* pSrc, char* pDest)
{
	_DEBUG_ASSERT(pSrc);
	_DEBUG_ASSERT(pDest);

	uint i = 0;
	while( pSrc[i] != 0 )
	{
		pDest[i] = (char)(pSrc[i]);	// Truncate wchar
		++i;
	}

	pDest[i] = 0;

	return i+1;
}

const wchar*
ASCIIToUnicodeAuto(const char* pSrc, uint* pLength)
{
	static wchar tempWCharBuffer[1024];
	uint length = ASCIIToUnicode(pSrc, tempWCharBuffer); _DEBUG_ASSERT(length < 1024);
	if(pLength)	
		*pLength = length;

	return tempWCharBuffer;
}

const char*
UnicodeToASCIIAuto(const wchar* pSrc, uint* pLength)
{
	static char tempWCharBuffer[1024];
	uint length = UnicodeToASCII(pSrc, tempWCharBuffer); _DEBUG_ASSERT(length < 1024);
	if(pLength)	
		*pLength = length;

	return tempWCharBuffer;
}

const wchar*
ToStringAuto(const wchar* pStr, ...)
{
	va_list vl;
	static wchar sTraceString[1024];

	va_start(vl, pStr);
	vswprintf_s(sTraceString, pStr, vl);
	va_end(vl);
	_DEBUG_ASSERT(wcslen(sTraceString) < 500);

	return sTraceString;
}

// ByteBuffer Fns

uint
AppendData(IByteBuffer* pBuffer, const void* pData, uint sizeToCopy)
{
	byte* pBufData = pBuffer->GetData();
	uint bufferSize = pBuffer->GetBufferSize();
	uint dataLength = pBuffer->GetDataLength();
	uint spaceLeft = bufferSize - dataLength;
	if(sizeToCopy > spaceLeft)
	{
		uint newSize = NextPowOf2((sizeToCopy - spaceLeft) + bufferSize);
		pBuffer->ResizeBuffer(newSize);	
		pBufData = pBuffer->GetData();
	}

	memcpy(pBufData+dataLength, pData, sizeToCopy);
	pBuffer->SetDataLength(dataLength+sizeToCopy);

	return sizeToCopy;
}

_NAMESPACE_END