//==============================================================================
//
//		SaveLoadHelperFns.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/29/2008
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

const static byte DATA_CHUNK_H = 0xFF;
const static byte SKIP_BYTE_H = 0xFD;
const static byte END_CHUNK_H = 0xFE;

class DataMap : public dense_hash_map<uint, byte*>
{
public:
	DataMap()
	{
		set_empty_key(0x00000000);
		set_deleted_key(0xFFFFFFFF);
	}
	
	~DataMap()
	{
	}
};

uint
SaveData(IByteBuffer* pBuffer, uint tag, const void* pData, uint size, uint align=1);

template<typename dataType> uint
SaveData(IByteBuffer* pBuffer, uint tag, dataType* pData)
{
	return SaveData(pBuffer, tag, pData, sizeof(dataType), GetStride(pData));
}

inline uint
SaveDataString(IByteBuffer* pBuffer, uint tag, const wchar* pStr)
{
	uint length = (uint) (wcslen(pStr) + 1);
	return SaveData(pBuffer, tag, (void*) pStr, length * sizeof(wchar), strideof(wchar));
}

uint
LoadData(IByteBuffer* pBuffer, DataMap* pMap, uint offset);

uint
LoadAllData(IByteBuffer* pBuffer, DataMap* pMap, uint offset);

boolean
GetData(DataMap* pMap, uint tag, void* pDest);

boolean
GetDataRaw(DataMap* pMap, uint tag, void** pDest);

uint
SaveDataKey(const CRCDataKey& dataKey, IByteBuffer* pBuffer);

void
LoadDataKey(DataMap& store, CRCDataKey& dataKey);

_NAMESPACE_END