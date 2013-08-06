//==============================================================================
//
//		CSDBSparseLooseOctTree.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/3/2009
//
//==============================================================================

#include "SceneDBFramework.h"

_NAMESPACE_BEGIN

CSDBSparseLooseOctTree::CSDBSparseLooseOctTree(uint levels, float scale, uint initSize)
	 : m_MaxGridSize(0.0f)
	 , m_ppLayers(NULL)
	 , m_Scale(scale)
	 , m_NumLayers(levels)
{
	m_Data.Initialize(initSize, 0.67f);
	
	m_ppLayers = _NEW SDBLayerPtr[levels];
	_LOOPi(levels)
	{
		m_ppLayers[i] = _NEW SDBLayer;
	}
	
	SDBLayer* pNext = NULL;
		
	_LOOPi(levels-1)
	{
		float gridSize = scale * (1 << i);
		m_ppLayers[i]->Initialize(this, gridSize, initSize, m_ppLayers[i+1], pNext);
		pNext = m_ppLayers[i];
	}
	
	float gridSize = scale * (1 << (levels-1));
	m_ppLayers[levels-1]->Initialize(this, gridSize, initSize, NULL, pNext);
	
	// Only accept objects that fit into the next level after the top
	// for performance reasons. Basically the top level search is using a basic AABB
	// test, so we want to accelerate loose bounds checking by only allowing
	// insertion of objects in children layers
	m_MaxGridSize = gridSize * 0.5f;

	m_TooLargeDataStart.data = NULL;
	//m_TooLargeDataStart.dataHash = 0;

	m_FrustumQueryLimits.min = gmtl::VecA3f(-1.0f, -1.0f, 0.0f);
	m_FrustumQueryLimits.max = gmtl::VecA3f(1.0f, 1.0f, 1.0f);
}

CSDBSparseLooseOctTree::~CSDBSparseLooseOctTree()
{
	_DELETE_ARRAY(m_ppLayers);
}

inline void		
CSDBSparseLooseOctTree::GetDataKey(void* pData, SDBDataKey* pDataKey)
{
	pDataKey->data		= (size_t) pData;
	//pDataKey->dataHash	= MurmurHashSizeT((size_t) pData);
}

inline uint
CSDBSparseLooseOctTree::FindLayerFromSize(float size)
{
	const static float INV_LN_2 = 1.0f / 0.693147181f;
		
	// Compute the level of the layer that we wish to add to
	float minAmount	= gmtl::Math::log(size / m_Scale) * INV_LN_2;
	int level		= FastCeiling(minAmount);
	if(level < 0)	 
		level = 0;
	
	_DEBUG_ASSERT((uint)level < m_NumLayers);
	
	return (uint) level;
}

boolean	CSDBSparseLooseOctTree::UpdateMove(SDBDataHandle handle, const AABox* pNewBounds)
{
	// TODO: For now
	return TRUE;
}

SDBDataHandle CSDBSparseLooseOctTree::Insert(const AABox* pBV, const OOBox* pOOBV, uint filterType, void* pData)
{	
	// Get the size of the BV
	gmtl::VecA3f dim	= pBV->max - pBV->min;
	float size			= gmtl::Math::Max(dim[0], dim[1], dim[2]);
	gmtl::VecA3f center	= pBV->min + (dim * 0.5f);
	
	// Add the data to the BV
	SDBDataKey key;
	GetDataKey(pData, &key);

	_DEBUG_ASSERT(m_Data.Get(key) == NULL);

	SDBData* pAdded = m_Data.AddDeferred(key);
	pAdded->pBV		= pBV;
	pAdded->pData	= pData;
	pAdded->pOOBV	= pOOBV;
	pAdded->filterType = filterType;

	// Ensure that the size of the BV/object is below the maximum
	if(size <= m_MaxGridSize)
	{
		uint level = FindLayerFromSize(size);
		m_ppLayers[level]->Insert(_CAST_VEC3(center), key, *pAdded);
	}
	else
	{
		if(m_TooLargeDataStart.data != NULL)
		{
			SDBData* pTooLargeData = m_Data.Get(m_TooLargeDataStart);
			_DEBUG_ASSERT(pTooLargeData != NULL);
			pTooLargeData->prev = key;
		}

		pAdded->prev.data = NULL;
		pAdded->next = m_TooLargeDataStart;
		m_TooLargeDataStart = key;
	}

	m_Data.FinishedAdding();

	return SDBDataHandle((size_t) pData);
}
	
void	
CSDBSparseLooseOctTree::Remove(SDBDataHandle handle)
{
    _DEBUG_ASSERT(handle.IsValid());

	void* pData = (void*) (size_t) handle;

	SDBDataKey key;
	GetDataKey(pData, &key);
	
	SDBData* pToRemove = m_Data.Get(key);
	if(pToRemove)
	{
		// Get the size of the BV
		gmtl::VecA3f dim	= pToRemove->pBV->max - pToRemove->pBV->min;
		float size			= gmtl::Math::Max(dim[0], dim[1], dim[2]);
		//gmtl::VecA3f center	= pBV->min + (dim * 0.5f);
	
		// Ensure that the size of the BV/object is below the maximum
		if(size <= m_MaxGridSize)
		{
			uint level = FindLayerFromSize(size);
			m_ppLayers[level]->Remove(key, *pToRemove);
		}
		else
		{
			if(m_TooLargeDataStart == key)
			{
				_DEBUG_ASSERT(pToRemove->prev.data == NULL);
				m_TooLargeDataStart = pToRemove->next;

				if(pToRemove->next.data)
				{
					// If the next in line exists then set it up
					// as the head of the chain
					SDBData* pSceneNext		= m_Data.Get(pToRemove->next);
					pSceneNext->prev.data	= NULL;
				}
			}
			else
			{
				// The data belongs somewhere in the chain of occupants
				_DEBUG_ASSERT(pToRemove->prev.data != NULL);				
				_DEBUG_ASSERT(m_TooLargeDataStart.data != NULL);

				// Repoint the prev data to the next one
				SDBData* pScenePrev = m_Data.Get(pToRemove->prev);
				_DEBUG_ASSERT(pScenePrev);
				_DEBUG_ASSERT(pScenePrev->next == key);
				pScenePrev->next = pToRemove->next;

				if(pToRemove->next.data)
				{
					// Setup the next data if it is valid
					SDBData* pSceneNext = m_Data.Get(pToRemove->next);
					pSceneNext->prev	= pToRemove->prev;
				}
			}
		}

		m_Data.Remove(key);	
	}
}
	
uint	
CSDBSparseLooseOctTree::Query(void** ppToStore, uint bufferSize, const AABox& queryBV, uint filterType)
{
	// Search oversized
	uint curIndex = 0;
	SDBDataKey* pKey = &m_TooLargeDataStart;
	while(pKey->data != NULL)
	{
		SDBData* pData = m_Data.Get(*pKey);
		_DEBUG_ASSERT(pData);
		if(filterType & pData->filterType)
		{
			if(AABoxInAABox(&queryBV, pData->pBV))
			{
				_DEBUG_ASSERT(curIndex < bufferSize);
				ppToStore[curIndex] = pData->pData;
				curIndex++;
			}
		}

		pKey = &(pData->next);
	}

	// Query from the largest layer
	return m_ppLayers[m_NumLayers-1]->QueryIncludeAdjacent(ppToStore, bufferSize, curIndex, queryBV, filterType, m_MaxGridSize * 0.5f);
}

uint	
CSDBSparseLooseOctTree::QueryCellInfo(AABox* pToStore, uint bufferSize, const AABox& queryBV)
{
	// Query from the largest layer
	return m_ppLayers[m_NumLayers-1]->QueryIncludeAdjacentCellInfo(pToStore, bufferSize, 0, queryBV, m_MaxGridSize * 0.5f);
}

uint
CSDBSparseLooseOctTree::QueryInFrustumOrtho(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, 
											AABox& frustumAA, const gmtl::MatrixA44f& viewProj, uint filterType)
{
	// Search oversized
	uint curIndex = 0;
	SDBDataKey* pKey = &m_TooLargeDataStart;
	while(pKey->data != NULL)
	{
		AABox temp;

		SDBData* pData = m_Data.Get(*pKey);
		_DEBUG_ASSERT(pData);
		if(filterType & pData->filterType)
		{
			AABox* pCurTransformed;

			if(pScreenSpaceRes)
				pCurTransformed = pScreenSpaceRes+curIndex;
			else
				pCurTransformed = &temp;

			TransformOOBoxToAABox(&viewProj, pData->pOOBV, pCurTransformed);
			if(AABoxInAABox(&m_FrustumQueryLimits, pCurTransformed))
			{
				_DEBUG_ASSERT(curIndex < bufferSize);
				ppToStore[curIndex] = pData->pData;
				curIndex++;
			}
		}

		pKey = &(pData->next);
	}

	if(pScreenSpaceRes)
		return m_ppLayers[m_NumLayers-1]->QueryIncludeAdjacentFrustumOrtho(ppToStore, pScreenSpaceRes, bufferSize, curIndex, frustumAA, viewProj, filterType, m_MaxGridSize * 0.5f);
	else
		return m_ppLayers[m_NumLayers-1]->QueryIncludeAdjacentFrustumOrtho(ppToStore, bufferSize, curIndex, frustumAA, viewProj, filterType, m_MaxGridSize * 0.5f);
}

uint	
CSDBSparseLooseOctTree::QueryInFrustumPersp(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, 
											AABox& frustumAA, const gmtl::MatrixA44f& viewProj, 
											float nearZ, float farZ, uint filterType)
{
	// Search oversized
	uint curIndex = 0;
	SDBDataKey* pKey = &m_TooLargeDataStart;
	while(pKey->data != NULL)
	{
		AABox temp;

		SDBData* pData = m_Data.Get(*pKey);
		_DEBUG_ASSERT(pData);
		if(filterType & pData->filterType)
		{
			AABox* pCurTransformed;

			if(pScreenSpaceRes)
				pCurTransformed = pScreenSpaceRes+curIndex;
			else
				pCurTransformed = &temp;

			//if(AABoxInAABox(pData->pBV, &frustumAA))
			{
				TransformAndProjectOOBoxToAABox(&viewProj, nearZ, farZ, pData->pOOBV, pCurTransformed);
				if(AABoxInAABox(&m_FrustumQueryLimits, pCurTransformed))
				{
					_DEBUG_ASSERT(curIndex < bufferSize);
					ppToStore[curIndex] = pData->pData;
					curIndex++;
				}
			}
		}

		pKey = &(pData->next);
	}

	if(pScreenSpaceRes)
		return m_ppLayers[m_NumLayers-1]->QueryIncludeAdjacentFrustumPersp(ppToStore, pScreenSpaceRes, bufferSize, curIndex, frustumAA, viewProj, nearZ, farZ, filterType, m_MaxGridSize * 0.5f);
	else
		return m_ppLayers[m_NumLayers-1]->QueryIncludeAdjacentFrustumPersp(ppToStore, bufferSize, curIndex, frustumAA, viewProj, nearZ, farZ, filterType, m_MaxGridSize * 0.5f);
}	

uint	
CSDBSparseLooseOctTree::QuerySphere(void** ppToStore, uint bufferSize, const AABox& sphereAA, const Sphere& sphereBV, uint filterType)
{
	// Search oversized
	uint curIndex = 0;
	SDBDataKey* pKey = &m_TooLargeDataStart;
	while(pKey->data != NULL)
	{
		SDBData* pData = m_Data.Get(*pKey);
		_DEBUG_ASSERT(pData);
		if(filterType & pData->filterType)
		{
			if(AABoxInSphere(pData->pBV, &sphereBV))
			{
				_DEBUG_ASSERT(curIndex < bufferSize);
				ppToStore[curIndex] = pData->pData;
				curIndex++;
			}
		}

		pKey = &(pData->next);
	}

	return m_ppLayers[m_NumLayers-1]->QueryIncludeAdjacentSphere(ppToStore, bufferSize, curIndex, sphereAA, sphereBV, filterType, m_MaxGridSize * 0.5f);	
}

uint CSDBSparseLooseOctTree::QueryAll(void** ppToStore, uint bufferSize, uint filterType)
{
	uint curIndex = 0;
	DataMap::Iterator itr = m_Data.BeginIter();
	while(itr.IsValid())
	{
		if(itr.GetValue()->filterType & filterType)
		{
			_DEBUG_ASSERT(curIndex < bufferSize);
			ppToStore[curIndex] = itr.GetValue()->pData;
			curIndex++;
		}

		itr.Next();
	}

	return curIndex;
}

float CSDBSparseLooseOctTree::GetMaxGridSize()
{
	return m_MaxGridSize;
}

_NAMESPACE_END