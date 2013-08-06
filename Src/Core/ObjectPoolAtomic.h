//================================================================================
//
//		ObjectPoolAtomic.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/13/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

template<typename TheObject, uint poolSizePow2Shift>
class ObjectPoolAtomic
{
	const static int POOL_SIZE = 1 << poolSizePow2Shift;

	const static int FREE		= 0;
	const static int IN_USE		= 0xFFFFFFFF;

private:
	TheObject	m_Objects[POOL_SIZE];
	TheObject*	m_Queue[POOL_SIZE];
	int			m_QueueState[POOL_SIZE];

	uint		m_PushIndex;
	uint		m_PopIndex;

	uint	m_NumObjectsAllocated;

public: 
	ObjectPoolAtomic()
	{
		Reset();
	}

	~ObjectPoolAtomic()
	{
	}

private:
	inline int GetIndex(TheObject* pObj)
	{
		size_t first = (size_t) m_Objects;
		size_t diff = (((size_t) pObj) - first);	
		_DEBUG_ASSERT((diff % sizeof(TheObject)) == 0);		
		uint offset = (uint) (diff / sizeof(TheObject));	
		_DEBUG_ASSERT(offset < POOL_SIZE && offset >= 0);

		return offset;
	}

public: 
	TheObject* Get()
	{	
		static int failCount = 0;

		while(m_NumObjectsAllocated < POOL_SIZE)
		{
			if( AtomicCompareAndSwap(&m_QueueState[m_PopIndex], IN_USE, FREE) == FREE )
			{
				int popIndex = m_PopIndex;
				m_PopIndex = (popIndex+1) & (POOL_SIZE-1);

				AtomicInc((int*) &m_NumObjectsAllocated);

				return m_Queue[popIndex];
			}

			failCount++;
		}

		return NULL;
	}

	void Dispose(TheObject* pObj)
	{
		_DEBUG_ASSERT(GetIndex(pObj) < POOL_SIZE);

		uint pushIndex = (uint) AtomicInc((int*) &m_PushIndex) - 1;
		pushIndex = pushIndex & (POOL_SIZE-1);

		m_Queue[pushIndex] = pObj;
		m_QueueState[pushIndex] = FREE;

		AtomicDec((int*) &m_NumObjectsAllocated);
	}

	uint GetNumObjectsAllocated()
	{
		return m_NumObjectsAllocated;
	}

	void Reset()
	{
		// NOTE: Not thread safe

		_LOOPi(POOL_SIZE)
		{
			m_Queue[i] = &m_Objects[i];
		}

		memset(m_QueueState, FREE, sizeof(m_QueueState));

		m_PushIndex = 0;
		m_PopIndex	= 0;
		m_NumObjectsAllocated = 0;
	}
};

_NAMESPACE_END
