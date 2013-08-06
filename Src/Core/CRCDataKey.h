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

// Useful for dense hash map
struct HashCRCDataKey
{
	inline uint operator()(const CRCDataKey& resKey) const
	{
		return resKey.hashVal;
	}
};

_NAMESPACE_END
