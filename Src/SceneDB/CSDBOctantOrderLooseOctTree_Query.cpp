//==============================================================================
//
//		CSDBOctantOrderLooseOctTree.cpp
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Monday, 24 June, 2013
//
//==============================================================================

#include "SceneDBFramework.h"
#include "OctantOrderDefines.h"

_NAMESPACE_BEGIN

uint CSDBOctantOrderLooseOctTree::Query(void** ppToStore, uint bufferSize, const AABox& queryBV, uint filterType)
{
	// Search oversized
	uint curIndex = 0;
	uint curHead = m_TooLargeDataHeadID;
	while(curHead != __INVALID)
	{	
		OctantOrderData& tooLargeData = m_TooLargeData[curHead];		
		if(filterType & tooLargeData.filterType)
		{
			if(AABoxInSphere(tooLargeData.pAABB, &queryBV))
			{
				_DEBUG_ASSERT(curIndex < bufferSize);
				ppToStore[curIndex] = tooLargeData.pData;
				curIndex++;
			}
		}

		curHead = tooLargeData.nextID;
	}

	QueryContext context;
	context.pQueryBV	= &queryBV;
	context.ppToStore	= ppToStore;
	context.bufferSize	= bufferSize;
	context.filterType	= filterType;

	return SearchOctants<QueryOctant>(context, queryBV, curIndex);
}

uint CSDBOctantOrderLooseOctTree::QuerySphere(void** ppToStore, uint bufferSize, const AABox& sphereAA, const Sphere& sphereBV, uint filterType)
{
	// Search oversized
	uint curIndex = 0;
	uint curHead = m_TooLargeDataHeadID;
	while(curHead != __INVALID)
	{	
		OctantOrderData& tooLargeData = m_TooLargeData[curHead];		
		if(filterType & tooLargeData.filterType)
		{
			if(AABoxInSphere(tooLargeData.pAABB, &sphereBV))
			{
				_DEBUG_ASSERT(curIndex < bufferSize);
				ppToStore[curIndex] = tooLargeData.pData;
				curIndex++;
			}
		}

		curHead = tooLargeData.nextID;
	}

	QueryContext context;
	context.pQueryBV	= &sphereBV;
	context.ppToStore	= ppToStore;
	context.bufferSize	= bufferSize;
	context.filterType	= filterType;

	return SearchOctants<QuerySphereOctant>(context, sphereAA, curIndex);
}

uint CSDBOctantOrderLooseOctTree::QueryInFrustumOrtho(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, AABox& frustumAA, const gmtl::MatrixA44f& viewProj, uint filterType)
{
	// Search oversized
	uint curIndex = 0;
	//uint curHead = m_TooLargeDataHeadID;
	//while(curHead != __INVALID)
	//{	
	//	OctantOrderData& tooLargeData = m_TooLargeData[curHead];		
	//	if(filterType & tooLargeData.filterType)
	//	{
	//		AABox* pCurTransformed = pScreenSpaceRes+curIndex;
	//		AABox temp;

	//		if(!pScreenSpaceRes)
	//			pCurTransformed = &temp;

	//		TransformOOBoxToAABox(&viewProj, tooLargeData.pOOBB, pCurTransformed);
	//		if(AABoxInAABox(&m_FrustumQueryLimits, pCurTransformed))
	//		{
	//			_DEBUG_ASSERT(curIndex < bufferSize);
	//			ppToStore[curIndex] = tooLargeData.pData;
	//			curIndex++;
	//		}
	//	}

	//	curHead = tooLargeData.nextID;
	//}

	QueryContext context;
	context.pQueryBV	= &viewProj;
	context.ppToStore	= ppToStore;
	context.bufferSize	= bufferSize;
	context.filterType	= filterType;
	context.pScreenSpaceToStore	= NULL;

	return SearchOctants<QueryInFrustumOrthoOctant>(context, frustumAA, curIndex);
}

uint CSDBOctantOrderLooseOctTree::QueryInFrustumPersp(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, AABox& frustumAA, const gmtl::MatrixA44f& viewProj, float nearZ, float farZ, uint filterType)
{
	// Search oversized
	uint curIndex = 0;
	uint curHead = m_TooLargeDataHeadID;
	while(curHead != __INVALID)
	{	
		OctantOrderData& tooLargeData = m_TooLargeData[curHead];		
		if(filterType & tooLargeData.filterType)
		{
			AABox* pCurTransformed = pScreenSpaceRes+curIndex;
			AABox temp;

			if(!pScreenSpaceRes)
				pCurTransformed = &temp;

			//if(AABoxInAABox(pData->pBV, &frustumAA))
			{
				TransformAndProjectOOBoxToAABox(&viewProj, nearZ, farZ, tooLargeData.pOOBB, pCurTransformed);
				if(AABoxInAABox(&m_FrustumQueryLimits, pCurTransformed))
				{
					_DEBUG_ASSERT(curIndex < bufferSize);
					ppToStore[curIndex] = tooLargeData.pData;
					curIndex++;
				}
			}
		}

		curHead = tooLargeData.nextID;
	}

	QueryContext context;
	context.pQueryBV	= &viewProj;
	context.ppToStore	= ppToStore;
	context.bufferSize	= bufferSize;
	context.filterType	= filterType;
	context.pScreenSpaceToStore	= pScreenSpaceRes;
	context.nearZ		= nearZ;
	context.farZ		= farZ;

	return SearchOctants<QueryInFrustumPerspOctant>(context, frustumAA, curIndex);
}	

uint CSDBOctantOrderLooseOctTree::QueryAll(void** ppToStore, uint bufferSize, uint filterType)
{
	uint curIndex = 0;
	OctantTable::Iterator octantItr = m_Octants.BeginIter();
	while(octantItr.IsValid())
	{
		OctantData* pOctant = octantItr.GetValue();
		OctantOrderDataTable::Iterator dataItr = pOctant->pDataTable->BeginIter();
		while(dataItr.IsValid())
		{
			if(dataItr.GetValue()->filterType & filterType)
			{
				_DEBUG_ASSERT(curIndex < bufferSize);
				ppToStore[curIndex] = dataItr.GetValue()->pData;
				curIndex++;
			}

			dataItr.Next();
		}

		octantItr.Next();
	}

	return curIndex;
}

uint CSDBOctantOrderLooseOctTree::QueryCellInfo(AABox* pToStore, uint bufferSize, const AABox& queryBV)
{
	uint curIndex = 0;

	QueryContext context;
	context.pQueryBV	= &queryBV;
	context.pCellInfo	= pToStore;
	context.bufferSize	= bufferSize;

	return SearchOctants<QueryCellInfo>(context, queryBV, curIndex);
}

// Octant queries
uint CSDBOctantOrderLooseOctTree::QueryOctantContained(const QueryContext& context, OctantOrderCell* pCell, uint octantOrderHash, uint level, uint curIndex)
{
	_DEBUG_ASSERT(pCell);

	uint dataID = pCell->firstDataID;
	while(dataID != __INVALID)
	{
		OctantOrderDataKey dataKey;
		dataKey.cellKey.gridUniqueHash	= octantOrderHash;
		dataKey.dataID					= dataID;
		OctantOrderData* pData = context.octant.pDataTable->Get(dataKey);
		_DEBUG_ASSERT(pData);

		if(pData->filterType & context.filterType)
		{
			_DEBUG_ASSERT(curIndex < context.bufferSize);
			context.ppToStore[curIndex] = pData->pData;
			curIndex++;
		}

		dataID = pData->nextID;
	}

	uint childrenFlag = pCell->children & 0xFF;
	if(childrenFlag)
	{
		_DEBUG_ASSERT(level > 0);
		_DEBUG_ASSERT( level == (m_TopLevel - ((pCell->children >> (8+3)) & 0xF)) );

		float childGridSize = gridSize * 0.5f;
		uint childLevel		= level-1;

		if(childrenFlag & 0x01)
		{
			// (0, 0, 0)
			uint childOctantOrderHash = octantOrderHash + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = QueryOctantContained(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x02)
		{
			// (1, 0, 0)
			uint childOctantOrderHash = octantOrderHash + OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = QueryOctantContained(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x04)
		{
			// (0, 0, 1)
			uint childOctantOrderHash = octantOrderHash + 2 * OCTANT_ORDER_VAL[childLevel] + 1;

			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = QueryOctantContained(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x08)
		{
			// (1, 0, 1)
			uint childOctantOrderHash = octantOrderHash + 3 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = QueryOctantContained(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x10)
		{
			// (0, 1, 0)
			uint childOctantOrderHash = octantOrderHash + 4 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = QueryOctantContained(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x20)
		{
			// (1, 1, 0)
			uint childOctantOrderHash = octantOrderHash + 5 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = QueryOctantContained(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x40)
		{
			// (0, 1, 1)
			uint childOctantOrderHash = octantOrderHash + 6 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = QueryOctantContained(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x80)
		{
			// (1, 1, 1)
			uint childOctantOrderHash = octantOrderHash + 7 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = QueryOctantContained(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}
	}

	return curIndex;
}

uint CSDBOctantOrderLooseOctTree::QueryOctant(const QueryContext& context, OctantOrderCell* pCell, const gmtl::VecA3f& cellPos, float gridSize, uint octantOrderHash, uint level, uint curIndex)
{
	_DEBUG_ASSERT(pCell);

	const AABox* pQueryBV = (const AABox*) context.pQueryBV;

	// Set loose bounds
	float looseOffset = gmtl::Math::Min(m_MaxGridSizeForData, gridSize) * 0.5f;

	AABox cellBounds;
	VecScalarAdd(&cellBounds.min, &cellPos, -looseOffset);
	VecScalarAdd(&cellBounds.max, &cellPos, looseOffset + gridSize);

	if(AABoxInAABox(pQueryBV, &cellBounds))
	{
		if(AABoxContainsAABox(pQueryBV, &cellBounds))
		{
			return QueryOctantContained(context, pCell, octantOrderHash, level, curIndex);
		}

		uint dataID = pCell->firstDataID;
		while(dataID != __INVALID)
		{
			OctantOrderDataKey dataKey;
			dataKey.cellKey.gridUniqueHash	= octantOrderHash;
			dataKey.dataID					= dataID;
			OctantOrderData* pData = context.octant.pDataTable->Get(dataKey);
			_DEBUG_ASSERT(pData);

			if(pData->filterType & context.filterType)
			{
				if(AABoxInAABox(&queryBV, pData->pAABB))
				{
					_DEBUG_ASSERT(curIndex < context.bufferSize);
					context.ppToStore[curIndex] = pData->pData;
					curIndex++;
				}
			}

			dataID = pData->nextID;
		}

		curIndex = QueryChildren<QueryOctant>(context, pCell, cellPos, gridSize, octantOrderHash, level, curIndex);
	}

	return curIndex;
}

uint CSDBOctantOrderLooseOctTree::QuerySphereOctant(const QueryContext& context, OctantOrderCell* pCell, const gmtl::VecA3f& cellPos, float gridSize, uint octantOrderHash, uint level, uint curIndex)
{
	_DEBUG_ASSERT(pCell);

	const Sphere* pQueryBV = (const Sphere*) context.pQueryBV;

	// Set loose bounds
	float looseOffset = gmtl::Math::Min(m_MaxGridSizeForData, gridSize) * 0.5f;

	AABox cellBounds;
	VecScalarAdd(&cellBounds.min, &cellPos, -looseOffset);
	VecScalarAdd(&cellBounds.max, &cellPos, looseOffset + gridSize);

	if(SphereInAABox(pQueryBV, &cellBounds))
	{
		// TODO: We probably want a special case where we do not do these containment tests
		float sphereRadius	= sphereBV.center[3];
		float looseGridSize = gridSize + 2.0f * looseOffset;
		if(sphereRadius >= (looseGridSize * 1.4142136f) && SphereContainsAABox(pQueryBV, &cellBounds))
		{
			return QueryOctantContained(context, pCell, octantOrderHash, level, curIndex);
		}

		uint dataID = pCell->firstDataID;
		while(dataID != __INVALID)
		{
			OctantOrderDataKey dataKey;
			dataKey.cellKey.gridUniqueHash	= octantOrderHash;
			dataKey.dataID					= dataID;
			OctantOrderData* pData = context.octant.pDataTable->Get(dataKey);
			_DEBUG_ASSERT(pData);

			if(pData->filterType & context.filterType)
			{
				if(SphereInAABox(pQueryBV, pData->pAABB))
				{
					_DEBUG_ASSERT(curIndex < context.bufferSize);
					context.ppToStore[curIndex] = pData->pData;
					curIndex++;
				}
			}

			dataID = pData->nextID;
		}

		curIndex = QueryChildren<QuerySphereOctant>(context, pCell, cellPos, gridSize, octantOrderHash, level, curIndex);
	}

	return curIndex;
}

uint CSDBOctantOrderLooseOctTree::QueryInFrustumOrthoOctantContained(const QueryContext& context, OctantOrderCell* pCell, uint octantOrderHash, uint level, uint curIndex)
{
	_DEBUG_ASSERT(pCell);

	uint dataID = pCell->firstDataID;
	while(dataID != __INVALID)
	{
		OctantOrderDataKey dataKey;
		dataKey.cellKey.gridUniqueHash	= octantOrderHash;
		dataKey.dataID					= dataID;
		OctantOrderData* pData = context.octant.pDataTable->Get(dataKey);
		_DEBUG_ASSERT(pData);

		if(pData->filterType & context.filterType)
		{
			AABox transformedAA;
			const gmtl::MatrixA44f* pQueryProj = (const gmtl::MatrixA44f*) context.pQueryBV;

			TransformOOBoxToAABox(pQueryProj, pData->pOOBB, &transformedAA);
			if(AABoxInAABox(&m_FrustumQueryLimits, &transformedAA))
			{
				_DEBUG_ASSERT(curIndex < bufferSize);
				ppToStore[curIndex] = pData->pData;
				if(pScreenSpaceRes)
					*(pScreenSpaceRes+curIndex) = transformedAA;

				curIndex++;
			}
		}

		dataID = pData->nextID;
	}

	uint childrenFlag = pCell->children & 0xFF;
	if(childrenFlag)
	{
		_DEBUG_ASSERT(level > 0);
		_DEBUG_ASSERT( level == (m_TopLevel - ((pCell->children >> (8+3)) & 0xF)) );

		float childGridSize = gridSize * 0.5f;
		uint childLevel		= level-1;

		if(childrenFlag & 0x01)
		{
			// (0, 0, 0)
			uint childOctantOrderHash = octantOrderHash + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = QueryInFrustumOrthoOctantContained(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x02)
		{
			// (1, 0, 0)
			uint childOctantOrderHash = octantOrderHash + OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = QueryInFrustumOrthoOctantContained(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x04)
		{
			// (0, 0, 1)
			uint childOctantOrderHash = octantOrderHash + 2 * OCTANT_ORDER_VAL[childLevel] + 1;

			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = QueryInFrustumOrthoOctantContained(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x08)
		{
			// (1, 0, 1)
			uint childOctantOrderHash = octantOrderHash + 3 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = QueryInFrustumOrthoOctantContained(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x10)
		{
			// (0, 1, 0)
			uint childOctantOrderHash = octantOrderHash + 4 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = QueryInFrustumOrthoOctantContained(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x20)
		{
			// (1, 1, 0)
			uint childOctantOrderHash = octantOrderHash + 5 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = QueryInFrustumOrthoOctantContained(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x40)
		{
			// (0, 1, 1)
			uint childOctantOrderHash = octantOrderHash + 6 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = QueryInFrustumOrthoOctantContained(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x80)
		{
			// (1, 1, 1)
			uint childOctantOrderHash = octantOrderHash + 7 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = QueryInFrustumOrthoOctantContained(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}
	}

	return curIndex;
}

uint CSDBOctantOrderLooseOctTree::QueryInFrustumOrthoOctant(const QueryContext& context, OctantOrderCell* pCell, const gmtl::VecA3f& cellPos, float gridSize, uint octantOrderHash, uint level, uint curIndex)
{
	_DEBUG_ASSERT(pCell);

	const gmtl::MatrixA44f* pQueryProj = (const gmtl::MatrixA44f*) context.pQueryBV;

	// Set loose bounds
	float looseOffset = gmtl::Math::Min(m_MaxGridSizeForData, gridSize) * 0.5f;

	AABox cellBounds;
	VecScalarAdd(&cellBounds.min, &cellPos, -looseOffset);
	VecScalarAdd(&cellBounds.max, &cellPos, looseOffset + gridSize);

	AABox transformedAA;
	TransformAABoxToAABox(pQueryProj, &cellBounds, &transformedAA);

	if(AABoxInAABox(&m_FrustumQueryLimits, &transformedAA))
	{
		if(AABoxContainsAABox(&m_FrustumQueryLimits, &transformedAA))
		{
			return QueryInFrustumOrthoOctantContained(context, pCell, octantOrderHash, level, curIndex);
		}

		uint dataID = pCell->firstDataID;
		while(dataID != __INVALID)
		{
			OctantOrderDataKey dataKey;
			dataKey.cellKey.gridUniqueHash	= octantOrderHash;
			dataKey.dataID					= dataID;
			OctantOrderData* pData = context.octant.pDataTable->Get(dataKey);
			_DEBUG_ASSERT(pData);

			if(pData->filterType & context.filterType)
			{
				TransformOOBoxToAABox(pQueryProj, pData->pOOBB, &transformedAA);
				if(AABoxInAABox(&m_FrustumQueryLimits, &transformedAA))
				{
					_DEBUG_ASSERT(curIndex < bufferSize);
					ppToStore[curIndex] = pData->pData;
					if(pScreenSpaceRes)
						*(pScreenSpaceRes+curIndex) = transformedAA;

					curIndex++;
				}
			}

			dataID = pData->nextID;
		}

		curIndex = QueryChildren<QueryInFrustumOrthoOctant>(context, pCell, cellPos, gridSize, octantOrderHash, level, curIndex);
	}

	return curIndex;
}

uint CSDBOctantOrderLooseOctTree::QueryInFrustumPerspOctantContained(const QueryContext& context, OctantOrderCell* pCell, uint octantOrderHash, uint level, uint curIndex)
{
	_DEBUG_ASSERT(pCell);

	uint dataID = pCell->firstDataID;
	while(dataID != __INVALID)
	{
		OctantOrderDataKey dataKey;
		dataKey.cellKey.gridUniqueHash	= octantOrderHash;
		dataKey.dataID					= dataID;
		OctantOrderData* pData = context.octant.pDataTable->Get(dataKey);
		_DEBUG_ASSERT(pData);

		if(pData->filterType & context.filterType)
		{
			AABox transformedAA;
			const gmtl::MatrixA44f* pQueryProj = (const gmtl::MatrixA44f*) context.pQueryBV;

			TransformAndProjectOOBoxToAABox(pQueryProj, context.nearZ, context.farZ, pData->pOOBB, &transformedAA);
			if(AABoxInAABox(&m_FrustumQueryLimits, &transformedAA))
			{
				_DEBUG_ASSERT(curIndex < bufferSize);
				ppToStore[curIndex] = pData->pData;
				if(pScreenSpaceRes)
					*(pScreenSpaceRes+curIndex) = transformedAA;

				curIndex++;
			}
		}

		dataID = pData->nextID;
	}

	uint childrenFlag = pCell->children & 0xFF;
	if(childrenFlag)
	{
		_DEBUG_ASSERT(level > 0);
		_DEBUG_ASSERT( level == (m_TopLevel - ((pCell->children >> (8+3)) & 0xF)) );

		float childGridSize = gridSize * 0.5f;
		uint childLevel		= level-1;

		if(childrenFlag & 0x01)
		{
			// (0, 0, 0)
			uint childOctantOrderHash = octantOrderHash + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = QueryInFrustumPerspOctantContained(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x02)
		{
			// (1, 0, 0)
			uint childOctantOrderHash = octantOrderHash + OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = QueryInFrustumPerspOctantContained(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x04)
		{
			// (0, 0, 1)
			uint childOctantOrderHash = octantOrderHash + 2 * OCTANT_ORDER_VAL[childLevel] + 1;

			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = QueryInFrustumPerspOctantContained(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x08)
		{
			// (1, 0, 1)
			uint childOctantOrderHash = octantOrderHash + 3 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = QueryInFrustumPerspOctantContained(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x10)
		{
			// (0, 1, 0)
			uint childOctantOrderHash = octantOrderHash + 4 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = QueryInFrustumPerspOctantContained(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x20)
		{
			// (1, 1, 0)
			uint childOctantOrderHash = octantOrderHash + 5 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = QueryInFrustumPerspOctantContained(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x40)
		{
			// (0, 1, 1)
			uint childOctantOrderHash = octantOrderHash + 6 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = QueryInFrustumPerspOctantContained(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x80)
		{
			// (1, 1, 1)
			uint childOctantOrderHash = octantOrderHash + 7 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = QueryInFrustumPerspOctantContained(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}
	}

	return curIndex;
}

uint CSDBOctantOrderLooseOctTree::QueryInFrustumPerspOctant(const QueryContext& context, OctantOrderCell* pCell, const gmtl::VecA3f& cellPos, float gridSize, uint octantOrderHash, uint level, uint curIndex)
{
	_DEBUG_ASSERT(pCell);

	const gmtl::MatrixA44f* pQueryProj = (const gmtl::MatrixA44f*) context.pQueryBV;

	// Set loose bounds
	float looseOffset = gmtl::Math::Min(m_MaxGridSizeForData, gridSize) * 0.5f;

	AABox cellBounds;
	VecScalarAdd(&cellBounds.min, &cellPos, -looseOffset);
	VecScalarAdd(&cellBounds.max, &cellPos, looseOffset + gridSize);

	AABox transformedAA;
	TransformAndProjectAABoxToAABox(pQueryProj, context.nearZ, context.farZ, &cellBounds, &transformedAA);

	if(AABoxInAABox(&m_FrustumQueryLimits, &transformedAA))
	{
		if(AABoxContainsAABox(&m_FrustumQueryLimits, &transformedAA))
		{
			return QueryInFrustumPerspOctantContained(context, pCell, octantOrderHash, level, curIndex);
		}

		uint dataID = pCell->firstDataID;
		while(dataID != __INVALID)
		{
			OctantOrderDataKey dataKey;
			dataKey.cellKey.gridUniqueHash	= octantOrderHash;
			dataKey.dataID					= dataID;
			OctantOrderData* pData = context.octant.pDataTable->Get(dataKey);
			_DEBUG_ASSERT(pData);

			if(pData->filterType & context.filterType)
			{
				TransformAndProjectOOBoxToAABox(pQueryProj, context.nearZ, context.farZ, pData->pOOBB, &transformedAA);
				if(AABoxInAABox(&m_FrustumQueryLimits, &transformedAA))
				{
					_DEBUG_ASSERT(curIndex < bufferSize);
					ppToStore[curIndex] = pData->pData;
					if(pScreenSpaceRes)
						*(pScreenSpaceRes+curIndex) = transformedAA;

					curIndex++;
				}
			}

			dataID = pData->nextID;
		}

		curIndex = QueryChildren<QueryInFrustumPerspOctant>(context, pCell, cellPos, gridSize, octantOrderHash, level, curIndex);
	}

	return curIndex;
}

uint CSDBOctantOrderLooseOctTree::QueryOctantCellInfoContained(const QueryContext& context, OctantOrderCell* pCell, const gmtl::VecA3f& cellPos, float gridSize, uint octantOrderHash, uint level, uint curIndex)
{
	_DEBUG_ASSERT(pCell);

	AABox cellBounds;
	cellBounds.min = cellPos;
	VecScalarAdd(&cellBounds.max, &cellPos, gridSize);

	_DEBUG_ASSERT(curIndex < context.bufferSize);
	context.pCellInfo[curIndex]	= cellBounds;		
	curIndex++;

	uint childrenFlag = pCell->children & 0xFF;
	if(childrenFlag)
	{
		_DEBUG_ASSERT(level > 0);
		_DEBUG_ASSERT( level == (m_TopLevel - ((pCell->children >> (8+3)) & 0xF)) );

		float childGridSize = gridSize * 0.5f;
		uint childLevel		= level-1;

		if(childrenFlag & 0x01)
		{
			// (0, 0, 0)
			uint childOctantOrderHash = octantOrderHash + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			gmtl::VecA3f childPos = cellPos;
			curIndex = QueryOctantCellInfoContained(context, pChild, childPos, childGridSize, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x02)
		{
			// (1, 0, 0)
			uint childOctantOrderHash = octantOrderHash + OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			gmtl::VecA3f childPos = cellPos;
			childPos[0] += childGridSize;
			curIndex = QueryOctantCellInfoContained(context, pChild, childPos, childGridSize, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x04)
		{
			// (0, 0, 1)
			uint childOctantOrderHash = octantOrderHash + 2 * OCTANT_ORDER_VAL[childLevel] + 1;

			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			gmtl::VecA3f childPos = cellPos;
			childPos[2] += childGridSize;
			curIndex = QueryOctantCellInfoContained(context, pChild, childPos, childGridSize, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x08)
		{
			// (1, 0, 1)
			uint childOctantOrderHash = octantOrderHash + 3 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			gmtl::VecA3f childPos;
			VecVecAdd(&childPos, &cellPos, gmtl::VecA3f(childGridSize, 0.0f, childGridSize));
			curIndex = QueryOctantCellInfoContained(context, pChild, childPos, childGridSize, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x10)
		{
			// (0, 1, 0)
			uint childOctantOrderHash = octantOrderHash + 4 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			gmtl::VecA3f childPos = cellPos;
			childPos[1] += childGridSize;
			curIndex = QueryOctantCellInfoContained(context, pChild, childPos, childGridSize, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x20)
		{
			// (1, 1, 0)
			uint childOctantOrderHash = octantOrderHash + 5 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			gmtl::VecA3f childPos;
			VecVecAdd(&childPos, &cellPos, gmtl::VecA3f(childGridSize, childGridSize, 0.0f));
			curIndex = QueryOctantCellInfoContained(context, pChild, childPos, childGridSize, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x40)
		{
			// (0, 1, 1)
			uint childOctantOrderHash = octantOrderHash + 6 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			gmtl::VecA3f childPos;
			VecVecAdd(&childPos, &cellPos, gmtl::VecA3f(0.0f, childGridSize, childGridSize));
			curIndex = QueryOctantCellInfoContained(context, pChild, childPos, childGridSize, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x80)
		{
			// (1, 1, 1)
			uint childOctantOrderHash = octantOrderHash + 7 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			gmtl::VecA3f childPos;
			VecVecAdd(&childPos, &cellPos, gmtl::VecA3f(childGridSize, childGridSize, childGridSize));
			curIndex = QueryOctantCellInfoContained(context, pChild, childPos, childGridSize, childOctantOrderHash, childLevel, curIndex);
		}
	}

	return curIndex;
}

uint CSDBOctantOrderLooseOctTree::QueryCellInfoOctant(const QueryContext& context, OctantOrderCell* pCell, const gmtl::VecA3f& cellPos, float gridSize, uint octantOrderHash, uint level, uint curIndex)
{
	_DEBUG_ASSERT(pCell);

	const AABox* pQueryBV = (const AABox*) context.pQueryBV;

	AABox cellBounds;
	cellBounds.min = cellPos;
	VecScalarAdd(&cellBounds.max, &cellPos, gridSize);
	
	_DEBUG_ASSERT(curIndex < context.bufferSize);
	context.pCellInfo[curIndex]	= cellBounds;		
	curIndex++;

	if(AABoxInAABox(pQueryBV, &cellBounds))
	{
		if(AABoxContainsAABox(pQueryBV, &cellBounds))
		{
			return QueryCellInfoContained(context, pCell, octantOrderHash, level, curIndex);
		}

		uint childrenFlag = pCell->children & 0xFF;
		if(childrenFlag)
		{
			_DEBUG_ASSERT(level > 0);
			_DEBUG_ASSERT( level == (m_TopLevel - ((pCell->children >> (8+3)) & 0xF)) );

			float childGridSize = gridSize * 0.5f;
			uint childLevel		= level-1;

			if(childrenFlag & 0x01)
			{
				// (0, 0, 0)
				uint childOctantOrderHash = octantOrderHash + 1;

				OctantOrderCellKey childKey;
				childKey.gridUniqueHash = childOctantOrderHash;
				OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

				gmtl::VecA3f childPos = cellPos;
				curIndex = QueryCellInfoOctant(context, pChild, childPos, childGridSize, childOctantOrderHash, childLevel, curIndex);
			}

			if(childrenFlag & 0x02)
			{
				// (1, 0, 0)
				uint childOctantOrderHash = octantOrderHash + OCTANT_ORDER_VAL[childLevel] + 1;

				OctantOrderCellKey childKey;
				childKey.gridUniqueHash = childOctantOrderHash;
				OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

				gmtl::VecA3f childPos = cellPos;
				childPos[0] += childGridSize;
				curIndex = QueryCellInfoOctant(context, pChild, childPos, childGridSize, childOctantOrderHash, childLevel, curIndex);
			}

			if(childrenFlag & 0x04)
			{
				// (0, 0, 1)
				uint childOctantOrderHash = octantOrderHash + 2 * OCTANT_ORDER_VAL[childLevel] + 1;

				childKey.gridUniqueHash = childOctantOrderHash;
				OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

				gmtl::VecA3f childPos = cellPos;
				childPos[2] += childGridSize;
				curIndex = QueryCellInfoOctant(context, pChild, childPos, childGridSize, childOctantOrderHash, childLevel, curIndex);
			}

			if(childrenFlag & 0x08)
			{
				// (1, 0, 1)
				uint childOctantOrderHash = octantOrderHash + 3 * OCTANT_ORDER_VAL[childLevel] + 1;

				OctantOrderCellKey childKey;
				childKey.gridUniqueHash = childOctantOrderHash;
				OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

				gmtl::VecA3f childPos;
				VecVecAdd(&childPos, &cellPos, gmtl::VecA3f(childGridSize, 0.0f, childGridSize));
				curIndex = QueryCellInfoOctant(context, pChild, childPos, childGridSize, childOctantOrderHash, childLevel, curIndex);
			}

			if(childrenFlag & 0x10)
			{
				// (0, 1, 0)
				uint childOctantOrderHash = octantOrderHash + 4 * OCTANT_ORDER_VAL[childLevel] + 1;

				OctantOrderCellKey childKey;
				childKey.gridUniqueHash = childOctantOrderHash;
				OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

				gmtl::VecA3f childPos = cellPos;
				childPos[1] += childGridSize;
				curIndex = QueryCellInfoOctant(context, pChild, childPos, childGridSize, childOctantOrderHash, childLevel, curIndex);
			}

			if(childrenFlag & 0x20)
			{
				// (1, 1, 0)
				uint childOctantOrderHash = octantOrderHash + 5 * OCTANT_ORDER_VAL[childLevel] + 1;

				OctantOrderCellKey childKey;
				childKey.gridUniqueHash = childOctantOrderHash;
				OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

				gmtl::VecA3f childPos;
				VecVecAdd(&childPos, &cellPos, gmtl::VecA3f(childGridSize, childGridSize, 0.0f));
				curIndex = QueryCellInfoOctant(context, pChild, childPos, childGridSize, childOctantOrderHash, childLevel, curIndex);
			}

			if(childrenFlag & 0x40)
			{
				// (0, 1, 1)
				uint childOctantOrderHash = octantOrderHash + 6 * OCTANT_ORDER_VAL[childLevel] + 1;

				OctantOrderCellKey childKey;
				childKey.gridUniqueHash = childOctantOrderHash;
				OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

				gmtl::VecA3f childPos;
				VecVecAdd(&childPos, &cellPos, gmtl::VecA3f(0.0f, childGridSize, childGridSize));
				curIndex = QueryCellInfoOctant(context, pChild, childPos, childGridSize, childOctantOrderHash, childLevel, curIndex);
			}

			if(childrenFlag & 0x80)
			{
				// (1, 1, 1)
				uint childOctantOrderHash = octantOrderHash + 7 * OCTANT_ORDER_VAL[childLevel] + 1;

				OctantOrderCellKey childKey;
				childKey.gridUniqueHash = childOctantOrderHash;
				OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

				gmtl::VecA3f childPos;
				VecVecAdd(&childPos, &cellPos, gmtl::VecA3f(childGridSize, childGridSize, childGridSize));
				curIndex = QueryCellInfoOctant(context, pChild, childPos, childGridSize, childOctantOrderHash, childLevel, curIndex);
			}
		}
	}

	return curIndex;
}

_NAMESPACE_END