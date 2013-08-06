//================================================================================
//
//		BFXDataContext.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		10/11/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

template<typename DataType>
class BFXDataContext : public CRefCounted<IRefCounted>
{
private:
	BFXDataPool<DataType>* m_pGlobalPool;

	DataType**	m_pData;

	uint		m_BufferSize;
	uint		m_DataSize;
	uint		m_NumUsed;
	uint		m_DirtyCount;

public:
	BFXDataContext(BFXDataPool<DataType>* pGlobalPool)
		: m_pData(NULL)
		, m_BufferSize(0)
		, m_DataSize(0)
		, m_NumUsed(0)
		, m_DirtyCount(0)
		, m_pGlobalPool(pGlobalPool)
	{
	}

	~BFXDataContext()
	{
		if(m_pData)
			_FREE(m_pData);
	}

public:
	DataType* Alloc()
	{
		if(m_NumUsed < m_DataSize)
		{
			// This is the normal case where there is unused data available in the buffer
			_DEBUG_ASSERT(m_DirtyCount == m_pGlobalPool->GetDirtyCount());

			m_NumUsed++;
			return m_pData[m_NumUsed-1];
		}

		// Reallocate the buffer bounds if we need more
		if(m_DataSize >= m_BufferSize)
		{
			uint newBufferSize = m_BufferSize * 16;
			if(newBufferSize <= 0)
				newBufferSize = 16;

			DataType** ppNewData = (DataType**) _MALLOC(sizeof(DataType*) * newBufferSize);
		
			if(m_pData)
			{
				memcpy(ppNewData, m_pData, sizeof(DataType*) * m_DataSize);
				_FREE(m_pData);
			}

			m_BufferSize = newBufferSize;
			m_pData = ppNewData;
		}

		// We have to allocate more data space from the pool to fill our buffer
		DataType* pData = m_pGlobalPool->Alloc();
		m_pData[m_DataSize] = pData;
		m_DataSize++;

		m_NumUsed++;

		return pData;
	}

	void Dispose()
	{
		uint poolDirtyCount = m_pGlobalPool->GetDirtyCount();

		// Only dispose the unused data if the allocated data buffer is still valid
		if(poolDirtyCount == m_DirtyCount)
		{
			_LOOPi(m_DataSize)
			{
				m_pGlobalPool->Free(m_pData[i]);
			}
		}

		m_DataSize = 0;
		m_NumUsed = 0;
	}

	void UpdateAndReset()
	{
		uint poolDirtyCount = m_pGlobalPool->GetDirtyCount();
		if(poolDirtyCount != m_DirtyCount)
		{
			// Invalidate the data buffer if the pool has been reallocated
			m_DataSize = 0;
			m_DirtyCount = poolDirtyCount;
		}

		m_NumUsed = 0;
	}

	DataType** GetDataBuffer()
	{
		return m_pData;
	}

	uint GetNumUsedData()
	{
		return m_NumUsed;
	}

	// Align to cache line to prevent false sharing
	_IMPL_ALIGNED_ALLOCS(_CACHE_LINE_SIZE)
};

_NAMESPACE_END