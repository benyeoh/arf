//================================================================================
//
//		ObjectPoolResizable.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		10/11/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

template<typename TheObject>
class ObjectPoolResizable
{
	struct Node
	{
		Node* pPrev;
		Node* pNext;
		uint index;
	};

private:
	TheObject* m_Objects;
	Node*	m_Nodes;
	Node*	m_pHead;
	Node*	m_pTail;
	uint	m_NumObjectsAllocated;
	uint	m_PoolSize;

public: 
	ObjectPoolResizable()
		: m_pHead(NULL)
		, m_pTail(NULL)
		, m_NumObjectsAllocated(0)
		, m_PoolSize(0)
		, m_Objects(NULL)
		, m_Nodes(NULL)
	{
	}

	~ObjectPoolResizable()
	{
		if(m_Objects)
			_DELETE_ARRAY(m_Objects);

		if(m_Nodes)
			_DELETE_ARRAY(m_Nodes);
	}

private:
	inline int GetIndex(TheObject* pObj)
	{
		size_t first = (size_t) m_Objects;
		size_t diff = (((size_t) pObj) - first);	
		_DEBUG_ASSERT((diff % sizeof(TheObject)) == 0);		
		uint offset = (uint) (diff / sizeof(TheObject));	
		_DEBUG_ASSERT(offset < m_PoolSize && offset >= 0);

		return offset;
	}

	inline void MoveToAfter(Node* pNode, Node* pAfter)
	{
		if(pNode != pAfter)
		{
			Node* pPrev = pNode->pPrev;
			Node* pNext = pNode->pNext;

			pPrev->pNext = pNext;
			pNext->pPrev = pPrev;

			pNode->pNext = pAfter->pNext;
			pNode->pPrev = pAfter;

			pAfter->pNext->pPrev = pNode;
			pAfter->pNext = pNode;
		}
	}

public:
	void SetSize(uint size)
	{
		if(m_Nodes)
			_DELETE_ARRAY(m_Nodes)
		
		if(m_Objects)
			_DELETE_ARRAY(m_Objects)

		m_PoolSize = size;
		m_Nodes = _NEW Node[m_PoolSize];
		m_Objects = _NEW TheObject[m_PoolSize];

		m_pHead = &(m_Nodes[0]);
		Node* pCurNode = m_pHead;
		m_pHead->index = 0;

		_LOOPi(m_PoolSize-1)
		{
			Node* pNext = &(m_Nodes[i+1]);
			pNext->index = i+1;
			pCurNode->pNext = pNext;
			pNext->pPrev = pCurNode;
			pCurNode = pNext;			
		}

		m_pTail = pCurNode;
		m_pTail->pNext = m_pHead;
		m_pHead->pPrev = m_pTail;

		Reset();
	}

	TheObject* Get()
	{	
		TheObject* pObj = NULL;

		{
			_DEBUG_ASSERT((m_NumObjectsAllocated < m_PoolSize));

			//if(m_NumObjectsAllocated < m_PoolSize)
			{
				pObj = &(m_Objects[m_pHead->index]);

				// Push the head to the tail
				Node* pNode = m_pHead;
				m_pHead = m_pHead->pNext;
				m_pTail = pNode;

				m_NumObjectsAllocated++;
			}
		}	

		return pObj;	
	}

	boolean IsFromPool(TheObject* pObj)
	{
		size_t first = (size_t) m_Objects;
		size_t last = (size_t) &(m_Objects[m_PoolSize-1]);
		size_t addr = (size_t) pObj;

		return (addr >= first) && (addr <= last);
	}

	void Dispose(TheObject* pObj)
	{
		_DEBUG_ASSERT(IsFromPool(pObj));

		Node* pFreeNode = &(m_Nodes[GetIndex(pObj)]);

		// Put it to the head		
		MoveToAfter(pFreeNode, m_pTail);
		m_pHead = pFreeNode;
		m_pTail = pFreeNode->pPrev;

		//if(checkForOverAllocs)
		m_NumObjectsAllocated--;
	}

	void Reset()
	{
		m_NumObjectsAllocated = 0;
	}

	void Preserve(TheObject* pObj)
	{
		Node* pToLast = &(m_Nodes[GetIndex(pObj)]);

		// Move it to the tail
		MoveToAfter(pToLast, m_pTail);
		m_pTail = pToLast;
		m_pHead = pToLast->pNext;		
	}

	uint GetNumAllocated()
	{
		return m_NumObjectsAllocated;
	}

	uint GetPoolSize()
	{
		return m_PoolSize;
	}
};

_NAMESPACE_END
