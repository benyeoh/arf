//==============================================================================
//
//		SaveLoadHelperFns.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/29/2008
//
//==============================================================================

#include "Core.h"

_NAMESPACE_BEGIN

//void
//HashTag(MD5Digest& hash, const wchar* pName)
//{
//	uint length = ((uint) wcslen(pName)) * sizeof(wchar);
//	hash.Set(pName, length);
//}

uint
SaveData(IByteBuffer* pBuffer, uint tag, const void* pData, uint size, uint align)
{
	uint totalByteOffset = 0;
	
	size_t dataLength = ((size_t)pBuffer->GetData()) + pBuffer->GetDataLength() 
		+ sizeof(byte)	// Data chunk header
		+ sizeof(uint)	// Tag
		+ sizeof(uint);	// Size
	uint amountToSkip = align - (uint)(dataLength & (align-1));
	_LOOPi(amountToSkip)
	{
		totalByteOffset += AppendData(pBuffer, SKIP_BYTE_H);
	}
	
	totalByteOffset += AppendData(pBuffer, DATA_CHUNK_H);
	
	totalByteOffset += AppendData(pBuffer, tag);
	totalByteOffset += AppendData(pBuffer, size);
	
	_DEBUG_ASSERT( IsAlignedToXBytes(pBuffer->GetData() + pBuffer->GetDataLength(), align) );
	
	totalByteOffset += AppendData(pBuffer, pData, size);

	return totalByteOffset;
}

uint
LoadData(IByteBuffer* pBuffer, DataMap* pMap, uint offset)
{
	uint totalBytesRead = 0;
	
	byte header = 0;
	uint bytesRead = ReadData(pBuffer, offset, &header);
	if(header == DATA_CHUNK_H  || header == SKIP_BYTE_H)
	{
		totalBytesRead += bytesRead;
		
		if(header == DATA_CHUNK_H)
		{	
			uint tag;	
			totalBytesRead += ReadData(pBuffer, offset + totalBytesRead, &tag);

			uint size = 0;
			totalBytesRead += ReadData(pBuffer, offset + totalBytesRead, &size);

			byte* pData = pBuffer->GetData() + offset + totalBytesRead;
			totalBytesRead += size;

			_DEBUG_ASSERT(pMap->find(tag) == pMap->end());
			(*pMap)[tag] = pData;
		}
	}
	
	return totalBytesRead;	
}

uint
LoadAllData(IByteBuffer* pBuffer, DataMap* pMap, uint offset)
{
	uint totalBytesRead = 0;
	uint numBytesRead = 0;
	
	do
	{
		numBytesRead = LoadData(pBuffer, pMap, offset + totalBytesRead);
		totalBytesRead += numBytesRead;
	}while(numBytesRead > 0);
	
	return totalBytesRead;
}

boolean
GetData(DataMap* pMap, uint tag, void* pDest)
{
	DataMap::iterator itr = pMap->find(tag);
	if(itr == pMap->end())
	{
		//_DEBUG_ASSERT(FALSE);
		return FALSE;
	}
	
	void* pSrc = itr->second;
	uint size = 0;
	memcpy(&size, ((byte*)pSrc) - sizeof(uint), sizeof(uint));
	memcpy(pDest, itr->second, size);
	return TRUE;	
}

boolean
GetDataRaw(DataMap* pMap, uint tag, void** pDest)
{
	DataMap::iterator itr = pMap->find(tag);
	if(itr == pMap->end())
	{
		//_DEBUG_ASSERT(FALSE);
		return FALSE;
	}

	*pDest = itr->second;
	return TRUE;
}

uint
SaveDataKey(const CRCDataKey& dataKey, IByteBuffer* pBuffer)
{
	uint totalBytesWritten	= SaveData(pBuffer, _MAKE_ID('r','h','a','s'), &dataKey.hashVal);
	uint dataLength = 0;
		
	if(dataKey.pHashData)
	{
		dataLength = dataKey.pHashData->GetDataLength();
		if(dataLength > 0)
		{
			totalBytesWritten += SaveData(pBuffer, _MAKE_ID('r','d','a','t'), 
				dataKey.pHashData->GetData(), dataLength);
		}
	}
	
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('r','l','e','n'), &dataLength);
	
	return totalBytesWritten;
}

void
LoadDataKey(DataMap& store, CRCDataKey& dataKey)
{	
	boolean res = GetData(&store, _MAKE_ID('r','h','a','s'), &dataKey.hashVal);
	_DEBUG_ASSERT(res);
	
	uint length;
	res = GetData(&store, _MAKE_ID('r','l','e','n'), &length);
	_DEBUG_ASSERT(res);
	if(length > 0)
	{
		byte* pData = NULL;
		res = GetDataRaw(&store, _MAKE_ID('r','d','a','t'), (void**)&pData);
		_DEBUG_ASSERT(res);
			
		dataKey.pHashData = _NEW CByteBuffer(length);
		memcpy(dataKey.pHashData->GetData(), pData, length);
		dataKey.pHashData->SetDataLength(length);
	}
}

_NAMESPACE_END