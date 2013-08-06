//==============================================================================
//
//		CommonUtiltyFns.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Saturday, April 08, 2006
//
//==============================================================================

#pragma once


_NAMESPACE_BEGIN

// Memory alignment
inline boolean
IsAlignedToXBytes(void* p, uint x)
{
	return (((size_t)p) & (x-1)) == 0;
}

// String Fns

uint 
ASCIIToUnicode(const char* pSrc, wchar* pDest);

uint
UnicodeToASCII(const wchar* pSrc, char* pDest);

const wchar*
ASCIIToUnicodeAuto(const char* pSrc, uint* pLength=NULL);

const char*
UnicodeToASCIIAuto(const wchar* pSrc, uint* pLength=NULL);

const wchar*
ToStringAuto(const wchar* pStr, ...);


// Math Fns

inline uint 
NextPowOf2(uint x)
{
	--x;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;

	return ++x;
}

inline boolean
IsPowerOf2(uint x)
{
	return ((x & (x-1)) == 0);
}

inline uint GetLog2(uint x)
{
#ifdef _MSC_VER
	unsigned long logVal;
	_BitScanReverse(&logVal, x);
	return (uint) logVal;
#else
	uint bits = 0;
	while(x > 0) 
	{
		bits++;
		x >>= 1;
	}

	return bits;
#endif
}

// ByteBuffer Fns

uint
AppendData(IByteBuffer* pBuffer, const void* pData, uint sizeToCopy);

template<typename dataType> inline uint 
AppendData(IByteBuffer* pBuffer, dataType data)
{
	return AppendData(pBuffer, &data, sizeof(dataType));	
}

// ByteBuffer copy
template<> inline uint
AppendData<const IByteBuffer*>(IByteBuffer* pBuffer, const IByteBuffer* pToCopy)
{
	return AppendData(pBuffer, pToCopy->GetData(), pToCopy->GetDataLength());
}

template<> inline uint
AppendData<IByteBuffer*>(IByteBuffer* pBuffer, IByteBuffer* pToCopy)
{
	return AppendData(pBuffer, pToCopy->GetData(), pToCopy->GetDataLength());	
}

template<> inline uint
AppendData<IByteBufferPtr>(IByteBuffer* pBuffer, IByteBufferPtr pToCopy)
{
	return AppendData(pBuffer, pToCopy.GetPtr());
}

template<> inline uint
AppendData<const IByteBufferPtr>(IByteBuffer* pBuffer, const IByteBufferPtr pToCopy)
{
	return AppendData(pBuffer, pToCopy.GetPtr());
}

// String copy
template<> inline uint
AppendData<wstring&>(IByteBuffer* pBuffer, wstring& data)
{
	uint totalBytesWritten = 0;
	uint numBytes = (uint) data.length() * sizeof(wchar);
	totalBytesWritten += AppendData(pBuffer, numBytes + sizeof(wchar));
	totalBytesWritten += AppendData(pBuffer, data.c_str(), numBytes);
	totalBytesWritten += AppendData(pBuffer, (wchar) 0);
	return totalBytesWritten;
}

template<> inline uint
AppendData<const wchar*>(IByteBuffer* pBuffer, const wchar* data)
{
	wstring str = data;
	return AppendData<wstring&>(pBuffer, str);
}

// Read
template<typename dataType> inline uint
ReadData(IByteBuffer* pBuffer, uint offset, dataType* pData)
{
	_DEBUG_ASSERT(pData);
	
	// Avoid unaligned access
	memcpy(pData, pBuffer->GetData() + offset, sizeof(dataType));
	//*pData = *((dataType*)(pBuffer->GetData() + offset));
	
	return sizeof(dataType);
}

inline uint
ReadData(IByteBuffer* pBuffer, uint offset, wstring& str)
{
	uint strDataSize = 0;
	uint headerSize = ReadData(pBuffer, offset, &strDataSize);
	const wchar* pStr = (const wchar*)(pBuffer->GetData() + offset + headerSize);
	str = pStr;
	return strDataSize + headerSize;	
}

// DLL Module loading
#if defined(_WINDOWS_)

template<typename T> HMODULE
LoadDLL(T** ppMod, const wchar* pFileName, const char* pCreateFnName)
{
	typedef T* (*CreateModFn)();
	
	HMODULE hMod = ::LoadLibrary(pFileName);
	if(hMod)
	{
		CreateModFn createModFn = NULL;
		createModFn = (CreateModFn) ::GetProcAddress(hMod, pCreateFnName); 
		if(createModFn)
		{
			*ppMod = createModFn();
		}
	}
	
	return hMod;
}

#endif

_NAMESPACE_END