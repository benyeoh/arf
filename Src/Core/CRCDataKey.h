//==============================================================================
//
//		CRCDataKey.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		2/25/2009
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

struct CRCDataKey
{
	uint hashVal;
	IByteBufferPtr pHashData;

	inline bool operator==(const CRCDataKey& other) const
	{
		if(IsValid() && other.IsValid())
			if(hashVal == other.hashVal && pHashData->GetDataLength() == other.pHashData->GetDataLength())
				return memcmp(pHashData->GetData(), other.pHashData->GetData(), pHashData->GetDataLength()) == 0;
			
		return false;
	}
	
	inline bool operator !=(const CRCDataKey& rhs) const
	{
		return !(operator==(rhs));
	}
	
	inline bool IsValid() const
	{
		return pHashData != NULL;
	}
	
	inline void Set(const void* pData, uint length)
	{
		pHashData = _NEW CByteBuffer(length);
		memcpy(pHashData->GetData(), pData, length);
		pHashData->SetDataLength(length);
		
		hashVal = CRCCalcCharLength((char*)pData, length);
	}
};

struct CRCDataPtrKey
{
	uint hashVal;
	const void* pHashData;
	uint length;

	inline bool operator==(const CRCDataPtrKey& other) const
	{
		if(IsValid() && other.IsValid())
			if(hashVal == other.hashVal && length == other.length)
				return memcmp(pHashData, other.pHashData, length) == 0;

		return false;
	}

	inline bool operator !=(const CRCDataPtrKey& rhs) const
	{
		return !(operator==(rhs));
	}

	inline bool IsValid() const
	{
		return pHashData != NULL;
	}

	inline void Set(const void* pData, uint length)
	{
		this->pHashData = pData;
		this->length = length;
		this->hashVal = CRCCalcCharLength((char*)pData, length);
	}
};


// Useful for dense hash map
struct HashCRCDataKey
{
	inline uint operator()(const CRCDataKey& resKey) const
	{
		return resKey.hashVal;
	}
};

struct HashCRCDataPtrKey
{
	inline uint operator()(const CRCDataPtrKey& resKey) const
	{
		return resKey.hashVal;
	}
};

_NAMESPACE_END
