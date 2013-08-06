//==============================================================================
//
//		ObjectPoolGrowable.h
//
//			Specifically retains previously allocated buffers when growing.
//			Unlike something like a vector, which reallocs a main buffer.
//			
//			Warning: No destructor/threading support yet
//
//		Author:		Ben Yeoh
//		Date:		29/11/09
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

template<typename TheObject, uint chunkSize>
class ObjectPoolGrowable
{
	struct Chunk
	{
		Chunk* pNext;
	};

private:
	byte*	m_pHead;
	byte*	m_pCurrent;

	uint	m_NumObjectsAllocatedCurChunk;
	uint	m_TotalNumObjectsAllocated;

public: 
	ObjectPoolGrowable()
		: m_pHead(NULL)
		, m_pCurrent(NULL)
		, m_NumObjectsAllocatedCurChunk(0)
		, m_TotalNumObjectsAllocated(0)
	{
		//Initialize();
	}

	~ObjectPoolGrowable()
	{
		Destroy();
	}

private:
	inline Chunk* AllocOneChunk()
	{
		// Warning: Ignoring special alignment for now
		Chunk* pNewChunk = (Chunk*) _MALLOC( chunkSize * sizeof(TheObject) + sizeof(Chunk) );
		pNewChunk->pNext = NULL;
		return pNewChunk;
	}

	inline TheObject* GetNext()
	{
		TheObject* pObj = (TheObject*) (m_pCurrent + sizeof(Chunk) + m_NumObjectsAllocatedCurChunk * sizeof(TheObject));
		m_NumObjectsAllocatedCurChunk++;
		m_TotalNumObjectsAllocated++;
		return pObj;
	}

public:
	void Destroy()
	{
		Chunk* pCurrent = (Chunk*) m_pHead;
		while(pCurrent)
		{
			Chunk* pNext = pCurrent->pNext;
			_FREE(pCurrent);
			pCurrent = pNext;
		}

		m_pHead		= NULL;
		
		Reset();
	}

	void Initialize()
	{
		Destroy();

		m_pHead		= (byte*) AllocOneChunk();
		m_pCurrent	= m_pHead;
	}

	inline TheObject* GetLast()
	{
		_DEBUG_ASSERT(m_NumObjectsAllocatedCurChunk > 0);
		TheObject* pObj = (TheObject*) (m_pCurrent + sizeof(Chunk*) + (m_NumObjectsAllocatedCurChunk-1) * sizeof(TheObject));
		return pObj;
	}

	inline TheObject* Get()
	{	
		if(!m_pCurrent)
			Initialize();

		if(m_NumObjectsAllocatedCurChunk >= chunkSize)
		{
			if( !((Chunk*)m_pCurrent)->pNext )
			{
				Chunk* pNewChunk	= AllocOneChunk();
				((Chunk*)m_pCurrent)->pNext	= pNewChunk;
			}

			m_pCurrent = (byte*) ((Chunk*)m_pCurrent)->pNext;
			m_NumObjectsAllocatedCurChunk = 0;
		}

		return GetNext();	
	}
	
	inline uint GetNumAllocated()
	{
		return m_TotalNumObjectsAllocated;
	}

	void Reset()
	{
		m_pCurrent = m_pHead;
		m_NumObjectsAllocatedCurChunk = 0;
		m_TotalNumObjectsAllocated = 0;
	}
};

_NAMESPACE_END
