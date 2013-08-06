//================================================================================
//
//		BFXDataPool.h
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
class BFXDataPool
{
private:
	ObjectPoolResizable<DataType> m_MainPool;
	ObjectPoolGrowable<DataType, 128> m_TempPool;

	Mutex	m_Mutex;
	uint	m_DirtyCount;

public:
	BFXDataPool()
		: m_DirtyCount(0)
	{
		m_Mutex.Initialize();
	}

	~BFXDataPool()
	{
	}

public:
	DataType* Alloc()
	{
		// This can be called by different threads

		MutexScopeLock lock(&m_Mutex);

		if(m_MainPool.GetNumAllocated() >= m_MainPool.GetPoolSize())
		{
			return m_TempPool.Get();
		}

		return m_MainPool.Get();
	}

	void Free(DataType* pData)
	{
		//MutexScopeLock lock(&m_Mutex);

		if(m_MainPool.IsFromPool(pData))
			m_MainPool.Dispose(pData);
	}

	void UpdateSize()
	{
		uint numObjectsInTemp = m_TempPool.GetNumAllocated();

		if(numObjectsInTemp > 0)
		{
			// We always try to allocate a contiguous buffer from the main pool
			uint newPoolSize = m_MainPool.GetPoolSize() + numObjectsInTemp;
			newPoolSize = NextPowOf2(newPoolSize);

			m_MainPool.SetSize(newPoolSize);

			// Now the temp pool has served its purpose, we deallocate it to save space
			m_TempPool.Destroy();

			// Update the dirty count to invalidate all prior allocations
			// since we've already reallocated the pool
			m_DirtyCount++;
		}
	}

	uint GetDirtyCount()
	{
		return m_DirtyCount;
	}
};

_NAMESPACE_END