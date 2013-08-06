//==============================================================================
//
//		ObjectPool.h
//
//			NOTE: Not thread safe and impl needs to be improved.				  
//				  Offers fast reset
//					
//
//		Author:		Ben Yeoh
//		Date:		11/28/2007
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

template<typename TheObject, uint poolSize, boolean checkForOverAllocs=TRUE>
class ObjectPool
{
	struct Node
	{
		Node* pPrev;
		Node* pNext;
		uint index;
	};
	
private:
	TheObject m_Objects[poolSize];
	Node	m_Nodes[poolSize];
	Node*	m_pHead;
	Node*	m_pTail;
	uint	m_NumObjectsAllocated;
	
public: 
	ObjectPool()
		: m_pHead(NULL)
		, m_pTail(NULL)
		, m_NumObjectsAllocated(0)
	{
		m_pHead = &(m_Nodes[0]);
		Node* pCurNode = m_pHead;
		m_pHead->index = 0;
		
		_LOOPi(poolSize-1)
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
	}

	~ObjectPool()
	{
	}

private:
	inline int GetIndex(TheObject* pObj)
	{
		size_t first = (size_t) m_Objects;
		size_t diff = (((size_t) pObj) - first);	
		_DEBUG_ASSERT((diff % sizeof(TheObject)) == 0);		
		uint offset = (uint) (diff / sizeof(TheObject));	
		_DEBUG_ASSERT(offset < poolSize && offset >= 0);
		
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
	TheObject* Get()
	{	
		TheObject* pObj = NULL;
		
		//if(checkForOverAllocs)
		{
			_DEBUG_ASSERT(!checkForOverAllocs || (m_NumObjectsAllocated < poolSize));

			//if(m_NumObjectsAllocated < poolSize)
			{
				pObj = &(m_Objects[m_pHead->index]);

				// Push the head to the tail
				Node* pNode = m_pHead;
				m_pHead = m_pHead->pNext;
				m_pTail = pNode;
			
				m_NumObjectsAllocated++;
			}
		}	
		//else
		//{
		//	pObj = &(m_Objects[m_pHead->index]);

		//	// Push the head to the tail
		//	Node* pNode = m_pHead;
		//	m_pHead = m_pHead->pNext;
		//	m_pTail = pNode;
		//}
		//	
		return pObj;	
	}
	
	void Dispose(TheObject* pObj)
	{
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
};

_NAMESPACE_END
