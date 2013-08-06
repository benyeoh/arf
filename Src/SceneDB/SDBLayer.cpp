//==============================================================================
//
//		SDBLayer.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		12/27/2008
//
//==============================================================================

#include "SceneDBFramework.h"
#include "GridHash.h"

#define __QUERY_ITERATE(query) \
	SDBGridPos gridPos; \
	uint posTempHash[3]; \
	 \
	 gmtl::VecA3f offset; \
	offset[0] = offset[1] = offset[2] = looseOffset; \
	gmtl::VecA3f minBVOffset; \
	VecVecSub(&minBVOffset, &(queryBV.min), &offset); \
	\
	gmtl::VecA3f maxBVOffset; \
	VecVecAdd(&maxBVOffset, &(queryBV.max), &offset); \
	\
	GetGridPosNoHash(minBVOffset, &gridPos, posTempHash[0], posTempHash[1], posTempHash[2]); \
	 \
	gmtl::VecA3f gridPosStart; \
	gridPosStart[0] = gridPos.pos[0] * m_GridSize; \
	gridPosStart[1] = gridPos.pos[1] * m_GridSize; \
	gridPosStart[2] = gridPos.pos[2] * m_GridSize; \
	 \
	int origGridPosX = gridPos.pos[0]; \
	float origXPos = gridPosStart[0]; \
	uint origXTempHash = posTempHash[0]; \
	 \
	int origGridPosY = gridPos.pos[1]; \
	float origYPos = gridPosStart[1]; \
	uint origYTempHash = posTempHash[1]; \
	 \
	while(gridPosStart[2] < maxBVOffset[2]) \
	{ \
		while(gridPosStart[1] < maxBVOffset[1]) \
		{ \
			while(gridPosStart[0] < maxBVOffset[0]) \
			{ \
				MurmurHashLoadGridPos(gridPos, posTempHash[0], posTempHash[1], posTempHash[2]); \
				\
				SDBOctCell* pCell = m_Grid.Get(gridPos); \
				if(pCell) \
					curIndex = query; \
				\
				gridPosStart[0] += m_GridSize;\
				gridPos.pos[0]++; \
				MurmurHashSaveVal(posTempHash[0], gridPos.pos[0]); \
			} \
			\
			gridPos.pos[0] = origGridPosX; \
			gridPosStart[0] = origXPos; \
			posTempHash[0] = origXTempHash; \
			\
			gridPosStart[1] += m_GridSize; \
			gridPos.pos[1]++; \
			MurmurHashSaveVal(posTempHash[1], gridPos.pos[1]); \
		} \
		\
		gridPos.pos[1] = origGridPosY; \
		gridPosStart[1] = origYPos; \
		posTempHash[1] = origYTempHash; \
		\
		gridPosStart[2] += m_GridSize; \
		gridPos.pos[2]++; \
		MurmurHashSaveVal(posTempHash[2], gridPos.pos[2]); \
	} \
	 \
	return curIndex;


#define __QUERY_ADJACENT(queryNeighbourMinus1, queryNeighbourPlus1) \
\
	SDBGridPos gridPos; \
	GetGridPos(_CAST_VEC3(queryBV.min), &gridPos); \
 \
	float gridPosXQuarter = gridPos.pos[0] * m_GridSize + m_GridSize * 0.25f; \
	float gridPosYQuarter = gridPos.pos[1] * m_GridSize + m_GridSize * 0.25f; \
	float gridPosZQuarter = gridPos.pos[2] * m_GridSize + m_GridSize * 0.25f; \
 \
	uint adjacentCheck = 0; \
	if(queryBV.min[0] > gridPosXQuarter) \
		adjacentCheck |= 0x1; \
	if(queryBV.min[1] > gridPosYQuarter) \
		adjacentCheck |= 0x2; \
	if(queryBV.min[2] > gridPosZQuarter) \
		adjacentCheck |= 0x4; \
 \
	switch(adjacentCheck) \
	{ \
		case 0: \
		{ \
			curIndex = queryNeighbourMinus1; \
 \
			gridPos.pos[1] -= 1; \
			curIndex = queryNeighbourMinus1; \
			gridPos.pos/*[1] += 1; \
	\
	gridPos.pos[0] -= 1; \
	curIndex = queryNeighbourMinus1; \
	\
	gridPos.pos[1] -= 1; \
	curIndex = queryNeighbourMinus1; \
	break; \
	} \
	\
	case 1: \
	{ \
	curIndex = queryNeighbourMinus1; \
	\
	gridPos.pos[1] -= 1; \
	curIndex = queryNeighbourMinus1; \
	gridPos.pos[1] += 1; \
	\
	gridPos.pos[0] += 1; \
	curIndex = queryNeighbourMinus1; \
	\
	gridPos.pos[1] -= 1; \
	curIndex = queryNeighbourMinus1; \
	break; \
	} \
	\
	case 2: \
	{ \
	curIndex = queryNeighbourMinus1; \
	\
	gridPos.pos[1] += 1; \
	curIndex = queryNeighbourMinus1; \
	gridPos.pos[1] -= 1; \
	\
	gridPos.pos[0] -= 1; \
	curIndex = queryNeighbourMinus1; \
	\
	gridPos.pos[1] += 1; \
	curIndex = queryNeighbourMinus1; \
	break; \
	} \
	\
	case 3: \
	{ \
	curIndex = queryNeighbourMinus1; \
	\
	gridPos.pos[1] += 1; \
	curIndex = queryNeighbourMinus1; \
	gridPos.pos[1] -= 1; \
	\
	gridPos.pos[0] += 1; \
	curIndex = queryNeighbourMinus1; \
	\
	gridPos.pos[1] += 1; \
	curIndex = queryNeighbourMinus1; \
	break; \
	} \
	\
	case 4: \
	{ \
	curIndex = queryNeighbourPlus1; \
	\
	gridPos.pos[1] -= 1; \
	curIndex = queryNeighbourPlus1; \
	gridPos.pos[1] += 1; \
	\
	gridPos.pos[0] -= 1; \
	curIndex = queryNeighbourPlus1; \
	\
	gridPos.pos[1] -= 1; \
	curIndex = queryNeighbourPlus1; \
	break; \
	} \
	\
	case 5: \
	{ \
	curIndex = queryNeighbourPlus1; \
	\
	gridPos.pos[1] -= 1; \
	curIndex = queryNeighbourPlus1; \
	gridPos.pos[1] += 1; \
	\
	gridPos.pos[0] += 1; \
	curIndex = queryNeighbourPlus1; \
	\
	gridPos.pos[1] -= 1; \
	curIndex = queryNeighbourPlus1; \
	break; \
	} \
	\
	case 6: \
	{ \
	curIndex = queryNeighbourPlus1; \
	\
	gridPos.pos[1] += 1;  \
	curIndex = queryNeighbourPlus1; \
	gridPos.pos[1] -= 1; \
	\
	gridPos.pos[0] -= 1; \
	curIndex = queryNeighbourPlus1; \
	\
	gridPos.pos[1] += 1; \
	curIndex = queryNeighbourPlus1; \
	break; \
	} \
	\
	case 7: \
	{ \
	curIndex = queryNeighbourPlus1; \
	\
	gridPos.pos[1] += 1; \
	curIndex = queryNeighbourPlus1; \
	gridPos.pos[1] -= 1; \
	\
	gridPos.pos[0] += 1; \
	curIndex = queryNeighbourPlus1; \
	\
	gridPos.pos[1] += 1; \
	curIndex = queryNeighbourPlus1; \
	break; \
	} \
	} \
	\
	return curIndex; \


	#define __QUERY_NEIGHBOUR_PAIR(query) \
	SDBOctCell* pCell = NULL; \
	uint intermediateH = 0; \
	\
	MurmurHashPosSave(gridPos, intermediateH); \
	pCell = m_Grid.Get(gridPos); \
	if(pCell) { curIndex = query; } \
	\
	gridPos.pos[2] += zInc; \
	MurmurHashPosLoad(gridPos, intermediateH); \
	pCell = m_Grid.Get(gridPos); \
	if(pCell) { curIndex = query; } \
	gridPos.pos[2] -= zInc; \
	\
	return curIndex;*/

	
_NAMESPACE_BEGIN

inline void	
SDBLayer::GetGridPos(const gmtl::Vec3f& pos, SDBGridPos* pGridPos)
{
	float scaledPosX	= pos[0] * m_InvScale;
	float scaledPosY	= pos[1] * m_InvScale;
	float scaledPosZ	= pos[2] * m_InvScale;

	pGridPos->pos[0]	= FastFloor(scaledPosX);
	pGridPos->pos[1]	= FastFloor(scaledPosY);
	pGridPos->pos[2]	= FastFloor(scaledPosZ);
	
	//if(pos[0] >= 0.0f)
	//	pGridPos->pos[0] = (int) scaledPosX;
	//else
	//	pGridPos->pos[0] = (int) (scaledPosX-1);
	//
	//if(pos[1] >= 0.0f)
	//	pGridPos->pos[1] = (int) scaledPosY;
	//else
	//	pGridPos->pos[1] = (int) (scaledPosY-1);
	//
	//if(pos[2] >= 0.0f)
	//	pGridPos->pos[2] = (int) scaledPosZ;
	//else
	//	pGridPos->pos[2] = (int) (scaledPosZ-1);
		
	// Calculate hash value
	MurmurHashPos(*pGridPos);
}

inline void SDBLayer::GetGridPosNoHash(const gmtl::VecA3f& pos, SDBGridPos* pGridPos, uint& xHash, uint& yHash, uint& zHash)
{
	gmtl::VecA3f posScaled;
	VecScalarMult(&posScaled, &pos, m_InvScale);
	
	//float scaledPosX	= pos[0] * m_InvScale;
	//float scaledPosY	= pos[1] * m_InvScale;
	//float scaledPosZ	= pos[2] * m_InvScale;

	pGridPos->pos[0]	= FastFloor(posScaled[0]);
	pGridPos->pos[1]	= FastFloor(posScaled[1]);
	pGridPos->pos[2]	= FastFloor(posScaled[2]);

	// Calculate hash value
	MurmurHashSaveVal(xHash, pGridPos->pos[0]);
	MurmurHashSaveVal(yHash, pGridPos->pos[1]);
	MurmurHashSaveVal(zHash, pGridPos->pos[2]);
	//MurmurHashLoadGridPos(*pGridPos, xHash, yHash, zHash);
}

inline uint	
SDBLayer::GetOctCellPos(const SDBGridPos& gridPos)
{
	uint flagResult = 0;
	flagResult |= (gridPos.pos[0] & 1);
	flagResult |= ((gridPos.pos[1] & 1) << 1);
	flagResult |= ((gridPos.pos[2] & 1) << 2);
	
	return flagResult;
}
	
void	
SDBLayer::Initialize(CSDBSparseLooseOctTree* pMain, 
					float gridSize,
					uint initSize, 
					SDBLayer* pParent, 
					SDBLayer* pChild)
{	
	m_pMain		= pMain;
	m_pParent	= pParent;
	m_pChild	= pChild;
	
	m_GridSize		= gridSize;
	m_InvScale		= 1.0f / m_GridSize;
	m_HalfGridSize	= 0.5f * m_GridSize;
	
	m_GridSizeVec.set(m_GridSize, m_GridSize, m_GridSize);
	m_HalfGridSizeVec.set(m_HalfGridSize, m_HalfGridSize, m_HalfGridSize);
	m_CullGridSizeVec.set(2.0f * m_GridSize, 2.0f * m_GridSize, 2.0f * m_GridSize);
	
	m_Grid.Initialize(initSize, 0.667f);
}
	
void	
SDBLayer::Insert(const gmtl::Vec3f& pos, SDBDataKey& dataKey, SDBData& data)
{
	SDBGridPos gridPosKey;
	GetGridPos(pos, &gridPosKey);

	data.gridPos	= gridPosKey;

	// The data is always added to the head of the chain
	// so there is no previous
	data.prev.data	= NULL;
		
	SDBOctCell* pCell = m_Grid.Get(gridPosKey);
	if(pCell)
	{
		// Set the data to point to the current head
		data.next = pCell->firstData;

		// Add the data to the head of the chain of occupants
		if(pCell->firstData.data)
		{
			SDBData* pPrevFirst = m_pMain->GetDataMap().Get(pCell->firstData);
			_DEBUG_ASSERT(pPrevFirst);
			_DEBUG_ASSERT(pPrevFirst->prev.data == NULL);
			pPrevFirst->prev = dataKey;
		}
		
		pCell->firstData = dataKey;
	}
	else
	{
		// Set the data as the only occupant in the chain
		data.next.data	= NULL;

		// Add a new cell
		SDBOctCell* pNewCell = m_Grid.AddDeferred(gridPosKey);
		pNewCell->childFlag	= 0;
		pNewCell->firstData	= dataKey;
		m_Grid.FinishedAdding();
	}
	
	if(m_pParent)
		m_pParent->NotifyCreated(gridPosKey);
}
	
void	
SDBLayer::Remove(SDBDataKey& dataKey, SDBData& data)
{	
	SDBOctCell* pCell = m_Grid.Get(data.gridPos);
	_DEBUG_ASSERT(pCell);
	
	if(pCell->firstData == dataKey)
	{
		// The data is first in the chain of occupants
		_DEBUG_ASSERT(data.prev.data == NULL);
		
		// Set the first data in the oct cell to the next in line
		pCell->firstData = data.next;
		
		if(data.next.data)
		{
			// If the next in line exists then set it up
			// as the head of the chain
			SDBData* pSceneNext		= m_pMain->GetDataMap().Get(data.next);
			pSceneNext->prev.data	= NULL;
		}
		else
		{
			// Next in line does not exist			
			// Remove the cell if it has no children
			if(!(pCell->childFlag))
			{
				m_Grid.Remove(data.gridPos);
				if(m_pParent)
					m_pParent->NotifyDestroyed(data.gridPos);
			}
		}
	}
	else
	{
		// The data belongs somewhere in the chain of occupants
		_DEBUG_ASSERT(data.prev.data != NULL);				
		_DEBUG_ASSERT(pCell->firstData.data != NULL);

		// Repoint the prev data to the next one
		SDBData* pScenePrev = m_pMain->GetDataMap().Get(data.prev);
		_DEBUG_ASSERT(pScenePrev);
		_DEBUG_ASSERT(pScenePrev->next == dataKey);
		pScenePrev->next = data.next;
		
		if(data.next.data)
		{
			// Setup the next data if it is valid
			SDBData* pSceneNext = m_pMain->GetDataMap().Get(data.next);
			pSceneNext->prev	= data.prev;
		}
	}		
}		

void	
SDBLayer::NotifyCreated(const SDBGridPos& childPos)
{
	uint childIndex = GetOctCellPos(childPos);

	// This is equivalent to a div by 2 and a floor
	SDBGridPos gridPos;
	gridPos.pos[0] = childPos.pos[0] >> 1;
	gridPos.pos[1] = childPos.pos[1] >> 1;
	gridPos.pos[2] = childPos.pos[2] >> 1;
		
	//SDBGridPos gridPos;
	//gridPos.pos[0] = childPos[0] >> 1;
	//	childPos.pos[0] < 0 ? (childPos.pos[0]-1) / 2 : childPos.pos[0] / 2;
	// 
	//gridPos.pos[1] = 
	//	childPos.pos[1] < 0 ? (childPos.pos[1]-1) / 2 : childPos.pos[1] / 2;
	// 
	//gridPos.pos[2] = 
	//	childPos.pos[2] < 0 ? (childPos.pos[2]-1) / 2  : childPos.pos[2] / 2;
	 
	MurmurHashPos(gridPos);
	
	SDBOctCell* pCell = m_Grid.Get(gridPos);
	if(!pCell)
	{
		// Create a new oct cell
		pCell = m_Grid.AddDeferred(gridPos);
		pCell->childFlag			= (1 << childIndex);
		pCell->children[childIndex]	= childPos;
		pCell->firstData.data = NULL;
		m_Grid.FinishedAdding();
		
		if(m_pParent)
			m_pParent->NotifyCreated(gridPos);
	}
	else
	{
		// Add the child
		pCell->childFlag |= (1 << childIndex);
		pCell->children[childIndex] = childPos;
	}
}

void	
SDBLayer::NotifyDestroyed(const SDBGridPos& childPos)
{
	uint childIndex = GetOctCellPos(childPos);

	SDBGridPos gridPos;
	gridPos.pos[0] = childPos.pos[0] >> 1;
	gridPos.pos[1] = childPos.pos[1] >> 1;
	gridPos.pos[2] = childPos.pos[2] >> 1;
	MurmurHashPos(gridPos);
	
	SDBOctCell* pCell = m_Grid.Get(gridPos);
	_DEBUG_ASSERT(pCell);
	
	// Remove the child from the cell
	pCell->childFlag &= ~(1 << childIndex);
	
	// Remove the oct cell if there are 
	// no children and no occupants
	if(pCell->childFlag == 0 &&
		pCell->firstData.data == NULL)
	{
		m_Grid.Remove(gridPos);
		if(m_pParent)
			m_pParent->NotifyDestroyed(gridPos);
	}
}

//////////////////////////////////////////////////////////////////////////
//	 Query Impl Contained 
//
//////////////////////////////////////////////////////////////////////////

inline uint	
SDBLayer::QueryFrustumPerspContained(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, uint curIndex, 
									const gmtl::MatrixA44f& viewProj, float nearZ, float farZ, 
									uint filterType, SDBGridPos& cellPos, SDBOctCell* pCell)
{
	// Add each occupant
	SDBDataKey* pCurDataKey = &(pCell->firstData);
	while(pCurDataKey->data)
	{
		SDBData* pData = m_pMain->GetDataMap().Get(*pCurDataKey);
		_DEBUG_ASSERT(pData);
		
		if(pData->filterType & filterType)
		{
			TransformAndProjectOOBoxToAABox(&viewProj, nearZ, farZ, pData->pOOBV, pScreenSpaceRes+curIndex);
			_DEBUG_ASSERT(curIndex < bufferSize);
			ppToStore[curIndex] = pData->pData;
			curIndex++;
		}
			
		pCurDataKey = &(pData->next);
	}
	
	if(m_pChild)
	{
		if(pCell->childFlag & 0x01)
			curIndex = m_pChild->QueryFrustumPerspContained(ppToStore, pScreenSpaceRes, bufferSize, curIndex, 
				viewProj, nearZ, farZ, filterType, pCell->children[0]);

		if(pCell->childFlag & 0x02)
			curIndex = m_pChild->QueryFrustumPerspContained(ppToStore, pScreenSpaceRes, bufferSize, curIndex, 
				viewProj, nearZ, farZ, filterType, pCell->children[1]);

		if(pCell->childFlag & 0x04)
			curIndex = m_pChild->QueryFrustumPerspContained(ppToStore, pScreenSpaceRes, bufferSize, curIndex, 
				viewProj, nearZ, farZ, filterType, pCell->children[2]);

		if(pCell->childFlag & 0x08)
			curIndex = m_pChild->QueryFrustumPerspContained(ppToStore, pScreenSpaceRes, bufferSize, curIndex, 
				viewProj, nearZ, farZ, filterType, pCell->children[3]);

		if(pCell->childFlag & 0x10)
			curIndex = m_pChild->QueryFrustumPerspContained(ppToStore, pScreenSpaceRes, bufferSize, curIndex, 
				viewProj, nearZ, farZ, filterType, pCell->children[4]);

		if(pCell->childFlag & 0x20)
			curIndex = m_pChild->QueryFrustumPerspContained(ppToStore, pScreenSpaceRes, bufferSize, curIndex, 
				viewProj, nearZ, farZ, filterType, pCell->children[5]);

		if(pCell->childFlag & 0x40)
			curIndex = m_pChild->QueryFrustumPerspContained(ppToStore, pScreenSpaceRes, bufferSize, curIndex, 
				viewProj, nearZ, farZ, filterType, pCell->children[6]);

		if(pCell->childFlag & 0x80)
			curIndex = m_pChild->QueryFrustumPerspContained(ppToStore, pScreenSpaceRes, bufferSize, curIndex, 
				viewProj, nearZ, farZ, filterType, pCell->children[7]);
	}
	
	return curIndex;
}

inline uint	
SDBLayer::QueryFrustumOrthoContained(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, uint curIndex, 
									const gmtl::MatrixA44f& viewProj, uint filterType, SDBGridPos& cellPos, SDBOctCell* pCell)
{
	// Add each occupant
	SDBDataKey* pCurDataKey = &(pCell->firstData);
	while(pCurDataKey->data)
	{
		SDBData* pData = m_pMain->GetDataMap().Get(*pCurDataKey);
		_DEBUG_ASSERT(pData);
		
		if(pData->filterType & filterType)
		{
			TransformOOBoxToAABox(&viewProj, pData->pOOBV, pScreenSpaceRes+curIndex);
			_DEBUG_ASSERT(curIndex < bufferSize);
			ppToStore[curIndex] = pData->pData;
			curIndex++;
		}
			
		pCurDataKey = &(pData->next);
	}
	
	if(m_pChild)
	{
		if(pCell->childFlag & 0x01)
			curIndex = m_pChild->QueryFrustumOrthoContained(ppToStore, pScreenSpaceRes, bufferSize, curIndex, 
				viewProj, filterType, pCell->children[0]);

		if(pCell->childFlag & 0x02)
			curIndex = m_pChild->QueryFrustumOrthoContained(ppToStore, pScreenSpaceRes, bufferSize, curIndex, 
				viewProj, filterType, pCell->children[1]);

		if(pCell->childFlag & 0x04)
			curIndex = m_pChild->QueryFrustumOrthoContained(ppToStore, pScreenSpaceRes, bufferSize, curIndex, 
				viewProj, filterType, pCell->children[2]);

		if(pCell->childFlag & 0x08)
			curIndex = m_pChild->QueryFrustumOrthoContained(ppToStore, pScreenSpaceRes, bufferSize, curIndex, 
				viewProj, filterType, pCell->children[3]);

		if(pCell->childFlag & 0x10)
			curIndex = m_pChild->QueryFrustumOrthoContained(ppToStore, pScreenSpaceRes, bufferSize, curIndex, 
				viewProj, filterType, pCell->children[4]);

		if(pCell->childFlag & 0x20)
			curIndex = m_pChild->QueryFrustumOrthoContained(ppToStore, pScreenSpaceRes, bufferSize, curIndex, 
				viewProj, filterType, pCell->children[5]);

		if(pCell->childFlag & 0x40)
			curIndex = m_pChild->QueryFrustumOrthoContained(ppToStore, pScreenSpaceRes, bufferSize, curIndex, 
			viewProj, filterType, pCell->children[6]);

		if(pCell->childFlag & 0x80)
			curIndex = m_pChild->QueryFrustumOrthoContained(ppToStore, pScreenSpaceRes, bufferSize, curIndex, 
				viewProj, filterType, pCell->children[7]);
	}
	
	return curIndex;
}
							
inline uint
SDBLayer::QueryCellInfoContained(AABox* pToStore, uint bufferSize, uint curIndex, SDBGridPos& cellPos, SDBOctCell* pCell)
{	
	AABox octCellBV;
	octCellBV.min[0] = (float) cellPos.pos[0];
	octCellBV.min[1] = (float) cellPos.pos[1];
	octCellBV.min[2] = (float) cellPos.pos[2];
	
	VecVecMult((gmtl::VecA4f*) &octCellBV.min, (gmtl::VecA4f*)&octCellBV.min, (gmtl::VecA4f*)&m_GridSizeVec);
	VecVecAdd((gmtl::VecA4f*)&octCellBV.max, (gmtl::VecA4f*)&octCellBV.min, (gmtl::VecA4f*)&m_GridSizeVec);

	_DEBUG_ASSERT(curIndex < bufferSize);
	pToStore[curIndex]	= octCellBV;
	curIndex++;
	
	if(m_pChild)
	{
		if(pCell->childFlag & 0x01)
			curIndex = m_pChild->QueryCellInfoContained(pToStore, bufferSize, curIndex, pCell->children[0]);

		if(pCell->childFlag & 0x02)
			curIndex = m_pChild->QueryCellInfoContained(pToStore, bufferSize, curIndex, pCell->children[1]);

		if(pCell->childFlag & 0x04)
			curIndex = m_pChild->QueryCellInfoContained(pToStore, bufferSize, curIndex, pCell->children[2]);

		if(pCell->childFlag & 0x08)
			curIndex = m_pChild->QueryCellInfoContained(pToStore, bufferSize, curIndex, pCell->children[3]);

		if(pCell->childFlag & 0x10)
			curIndex = m_pChild->QueryCellInfoContained(pToStore, bufferSize, curIndex, pCell->children[4]);

		if(pCell->childFlag & 0x20)
			curIndex = m_pChild->QueryCellInfoContained(pToStore, bufferSize, curIndex, pCell->children[5]);

		if(pCell->childFlag & 0x40)
			curIndex = m_pChild->QueryCellInfoContained(pToStore, bufferSize, curIndex, pCell->children[6]);

		if(pCell->childFlag & 0x80)
			curIndex = m_pChild->QueryCellInfoContained(pToStore, bufferSize, curIndex, pCell->children[7]);
	}
	
	return curIndex;
}

inline uint
SDBLayer::QueryContained(void** ppToStore, uint bufferSize, uint curIndex, uint filterType, SDBGridPos& cellPos, SDBOctCell* pCell)
{
	// Add each occupant
	SDBDataKey* pCurDataKey = &(pCell->firstData);
	while(pCurDataKey->data)
	{
		SDBData* pData = m_pMain->GetDataMap().Get(*pCurDataKey);
		_DEBUG_ASSERT(pData);
		
		if(pData->filterType & filterType)
		{
			_DEBUG_ASSERT(curIndex < bufferSize);
			ppToStore[curIndex] = pData->pData;
			curIndex++;
		}
				
		pCurDataKey = &(pData->next);
	}
	
	if(m_pChild)
	{
		if(pCell->childFlag & 0x01)
			curIndex = m_pChild->QueryContained(ppToStore, bufferSize, curIndex, filterType, pCell->children[0]);

		if(pCell->childFlag & 0x02)
			curIndex = m_pChild->QueryContained(ppToStore, bufferSize, curIndex, filterType, pCell->children[1]);

		if(pCell->childFlag & 0x04)
			curIndex = m_pChild->QueryContained(ppToStore, bufferSize, curIndex, filterType, pCell->children[2]);

		if(pCell->childFlag & 0x08)
			curIndex = m_pChild->QueryContained(ppToStore, bufferSize, curIndex, filterType, pCell->children[3]);

		if(pCell->childFlag & 0x10)
			curIndex = m_pChild->QueryContained(ppToStore, bufferSize, curIndex, filterType, pCell->children[4]);

		if(pCell->childFlag & 0x20)
			curIndex = m_pChild->QueryContained(ppToStore, bufferSize, curIndex, filterType, pCell->children[5]);

		if(pCell->childFlag & 0x40)
			curIndex = m_pChild->QueryContained(ppToStore, bufferSize, curIndex, filterType, pCell->children[6]);

		if(pCell->childFlag & 0x80)
			curIndex = m_pChild->QueryContained(ppToStore, bufferSize, curIndex, filterType, pCell->children[7]);
	}
	
	return curIndex;
}


//////////////////////////////////////////////////////////////////////////
//	Query Impl
//
//////////////////////////////////////////////////////////////////////////

inline uint
SDBLayer::QuerySphere(void** ppToStore, uint bufferSize, uint curIndex, const Sphere& sphere, uint filterType, SDBGridPos& cellPos, SDBOctCell* pCell)
{
	_DEBUG_ASSERT(pCell);
			
	// Cull against a looser bounding volume
	//float cullGridSize	= 2.0f * m_GridSize;
	AABox octCellBV;
	octCellBV.min[0] = (float) cellPos.pos[0];
	octCellBV.min[1] = (float) cellPos.pos[1];
	octCellBV.min[2] = (float) cellPos.pos[2];
	
	VecVecMult((gmtl::VecA4f*) &octCellBV.min, (gmtl::VecA4f*)&octCellBV.min, (gmtl::VecA4f*)&m_GridSizeVec);
	VecVecSub((gmtl::VecA4f*)&octCellBV.min, (gmtl::VecA4f*)&octCellBV.min, (gmtl::VecA4f*)&m_HalfGridSizeVec);
	VecVecAdd((gmtl::VecA4f*)&octCellBV.max, (gmtl::VecA4f*)&octCellBV.min, (gmtl::VecA4f*)&m_CullGridSizeVec);
	
	if(AABoxInSphere(&octCellBV, &sphere))
	{
		// Add each occupant if the bounding volumes intersect
		SDBDataKey* pCurDataKey = &(pCell->firstData);
		while(pCurDataKey->data)
		{
			SDBData* pData = m_pMain->GetDataMap().Get(*pCurDataKey);
			_DEBUG_ASSERT(pData);
			
			if(pData->filterType & filterType)
			{
				if(AABoxInSphere(pData->pBV, &sphere))
				{
					_DEBUG_ASSERT(curIndex < bufferSize);
					ppToStore[curIndex] = pData->pData;
					curIndex++;
				}
			}
						
			pCurDataKey = &(pData->next);
		}
		
		// Query each oct child if it exists
		if(m_pChild)
		{
			if(pCell->childFlag & 0x01)
				curIndex = m_pChild->QuerySphere(ppToStore, bufferSize, curIndex, sphere, filterType, pCell->children[0]);

			if(pCell->childFlag & 0x02)
				curIndex = m_pChild->QuerySphere(ppToStore, bufferSize, curIndex, sphere, filterType, pCell->children[1]);

			if(pCell->childFlag & 0x04)
				curIndex = m_pChild->QuerySphere(ppToStore, bufferSize, curIndex, sphere, filterType, pCell->children[2]);

			if(pCell->childFlag & 0x08)
				curIndex = m_pChild->QuerySphere(ppToStore, bufferSize, curIndex, sphere, filterType, pCell->children[3]);

			if(pCell->childFlag & 0x10)
				curIndex = m_pChild->QuerySphere(ppToStore, bufferSize, curIndex, sphere, filterType, pCell->children[4]);

			if(pCell->childFlag & 0x20)
				curIndex = m_pChild->QuerySphere(ppToStore, bufferSize, curIndex, sphere, filterType, pCell->children[5]);

			if(pCell->childFlag & 0x40)
				curIndex = m_pChild->QuerySphere(ppToStore, bufferSize, curIndex, sphere, filterType, pCell->children[6]);

			if(pCell->childFlag & 0x80)
				curIndex = m_pChild->QuerySphere(ppToStore, bufferSize, curIndex, sphere, filterType, pCell->children[7]);
		}
	}

	return curIndex;
}

inline uint
SDBLayer::QueryFrustumOrtho(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, uint curIndex, const gmtl::MatrixA44f& viewProj, 
							uint filterType, SDBGridPos& cellPos, SDBOctCell* pCell)
{
	_DEBUG_ASSERT(pCell);
			
	// Cull against a looser bounding volume
	//float cullGridSize	= 2.0f * m_GridSize;
	AABox octCellBV;
	octCellBV.min[0] = (float) cellPos.pos[0];
	octCellBV.min[1] = (float) cellPos.pos[1];
	octCellBV.min[2] = (float) cellPos.pos[2];
	
	VecVecMult((gmtl::VecA4f*) &octCellBV.min, (gmtl::VecA4f*)&octCellBV.min, (gmtl::VecA4f*)&m_GridSizeVec);
	VecVecSub((gmtl::VecA4f*)&octCellBV.min, (gmtl::VecA4f*)&octCellBV.min, (gmtl::VecA4f*)&m_HalfGridSizeVec);
	VecVecAdd((gmtl::VecA4f*)&octCellBV.max, (gmtl::VecA4f*)&octCellBV.min, (gmtl::VecA4f*)&m_CullGridSizeVec);
	
	AABox transformedAA;
	TransformAABoxToAABox(&viewProj, &octCellBV, &transformedAA);

	if(AABoxInAABox(&m_FrustumQueryLimits, &transformedAA))
	{
		if(AABoxContainsAABox(&m_FrustumQueryLimits, &transformedAA))
		{
			return QueryFrustumOrthoContained(ppToStore, pScreenSpaceRes, bufferSize, curIndex, 
				viewProj, filterType, cellPos, pCell);
		}
		
		// Add each occupant if the bounding volumes intersect
		SDBDataKey* pCurDataKey = &(pCell->firstData);
		while(pCurDataKey->data)
		{
			SDBData* pData = m_pMain->GetDataMap().Get(*pCurDataKey);
			_DEBUG_ASSERT(pData);

			if(pData->filterType & filterType)
			{
				TransformOOBoxToAABox(&viewProj, pData->pOOBV, &transformedAA);
				if(AABoxInAABox(&m_FrustumQueryLimits, &transformedAA))
				{
					_DEBUG_ASSERT(curIndex < bufferSize);
					ppToStore[curIndex] = pData->pData;
					*(pScreenSpaceRes+curIndex) = transformedAA;
					curIndex++;
				}
			}
			
			pCurDataKey = &(pData->next);
		}
		
		// Query each oct child if it exists
		if(m_pChild)
		{
			if(pCell->childFlag & 0x01)
				curIndex = m_pChild->QueryFrustumOrtho(ppToStore, pScreenSpaceRes, bufferSize, curIndex, viewProj, filterType, pCell->children[0]);

			if(pCell->childFlag & 0x02)
				curIndex = m_pChild->QueryFrustumOrtho(ppToStore, pScreenSpaceRes, bufferSize, curIndex, viewProj, filterType, pCell->children[1]);

			if(pCell->childFlag & 0x04)
				curIndex = m_pChild->QueryFrustumOrtho(ppToStore, pScreenSpaceRes, bufferSize, curIndex, viewProj, filterType, pCell->children[2]);

			if(pCell->childFlag & 0x08)
				curIndex = m_pChild->QueryFrustumOrtho(ppToStore, pScreenSpaceRes, bufferSize, curIndex, viewProj, filterType, pCell->children[3]);

			if(pCell->childFlag & 0x10)
				curIndex = m_pChild->QueryFrustumOrtho(ppToStore, pScreenSpaceRes, bufferSize, curIndex, viewProj, filterType, pCell->children[4]);

			if(pCell->childFlag & 0x20)
				curIndex = m_pChild->QueryFrustumOrtho(ppToStore, pScreenSpaceRes, bufferSize, curIndex, viewProj, filterType, pCell->children[5]);

			if(pCell->childFlag & 0x40)
				curIndex = m_pChild->QueryFrustumOrtho(ppToStore, pScreenSpaceRes, bufferSize, curIndex, viewProj, filterType, pCell->children[6]);

			if(pCell->childFlag & 0x80)
				curIndex = m_pChild->QueryFrustumOrtho(ppToStore, pScreenSpaceRes, bufferSize, curIndex, viewProj, filterType, pCell->children[7]);
		}
	}

	return curIndex;	
}
	
inline uint
SDBLayer::QueryFrustumOrtho(void** ppToStore, uint bufferSize, uint curIndex, const gmtl::MatrixA44f& viewProj, 
							uint filterType, SDBGridPos& cellPos, SDBOctCell* pCell)
{
	_DEBUG_ASSERT(pCell);
			
	// Cull against a looser bounding volume
	//float cullGridSize	= 2.0f * m_GridSize;
	AABox octCellBV;
	octCellBV.min[0] = (float) cellPos.pos[0];
	octCellBV.min[1] = (float) cellPos.pos[1];
	octCellBV.min[2] = (float) cellPos.pos[2];
	
	VecVecMult((gmtl::VecA4f*) &octCellBV.min, (gmtl::VecA4f*)&octCellBV.min, (gmtl::VecA4f*)&m_GridSizeVec);
	VecVecSub((gmtl::VecA4f*)&octCellBV.min, (gmtl::VecA4f*)&octCellBV.min, (gmtl::VecA4f*)&m_HalfGridSizeVec);
	VecVecAdd((gmtl::VecA4f*)&octCellBV.max, (gmtl::VecA4f*)&octCellBV.min, (gmtl::VecA4f*)&m_CullGridSizeVec);
	
	AABox transformedAA;
	TransformAABoxToAABox(&viewProj, &octCellBV, &transformedAA);

	if(AABoxInAABox(&m_FrustumQueryLimits, &transformedAA))
	{
		if(AABoxContainsAABox(&m_FrustumQueryLimits, &transformedAA))
		{
			return QueryContained(ppToStore, bufferSize, curIndex, filterType, cellPos, pCell);
		}
		
		// Add each occupant if the bounding volumes intersect
		SDBDataKey* pCurDataKey = &(pCell->firstData);
		while(pCurDataKey->data)
		{
			SDBData* pData = m_pMain->GetDataMap().Get(*pCurDataKey);
			_DEBUG_ASSERT(pData);
			
			if(pData->filterType & filterType)
			{
				TransformOOBoxToAABox(&viewProj, pData->pOOBV, &transformedAA);
				if(AABoxInAABox(&m_FrustumQueryLimits, &transformedAA))
				{
					_DEBUG_ASSERT(curIndex < bufferSize);
					ppToStore[curIndex] = pData->pData;
					curIndex++;
				}
			}
						
			pCurDataKey = &(pData->next);
		}
		
		// Query each oct child if it exists
		if(m_pChild)
		{
			if(pCell->childFlag & 0x01)
				curIndex = m_pChild->QueryFrustumOrtho(ppToStore, bufferSize, curIndex, viewProj, filterType, pCell->children[0]);

			if(pCell->childFlag & 0x02)
				curIndex = m_pChild->QueryFrustumOrtho(ppToStore, bufferSize, curIndex, viewProj, filterType, pCell->children[1]);

			if(pCell->childFlag & 0x04)
				curIndex = m_pChild->QueryFrustumOrtho(ppToStore, bufferSize, curIndex, viewProj, filterType, pCell->children[2]);

			if(pCell->childFlag & 0x08)
				curIndex = m_pChild->QueryFrustumOrtho(ppToStore, bufferSize, curIndex, viewProj, filterType, pCell->children[3]);

			if(pCell->childFlag & 0x10)
				curIndex = m_pChild->QueryFrustumOrtho(ppToStore, bufferSize, curIndex, viewProj, filterType, pCell->children[4]);

			if(pCell->childFlag & 0x20)
				curIndex = m_pChild->QueryFrustumOrtho(ppToStore, bufferSize, curIndex, viewProj, filterType, pCell->children[5]);

			if(pCell->childFlag & 0x40)
				curIndex = m_pChild->QueryFrustumOrtho(ppToStore, bufferSize, curIndex, viewProj, filterType, pCell->children[6]);

			if(pCell->childFlag & 0x80)
				curIndex = m_pChild->QueryFrustumOrtho(ppToStore, bufferSize, curIndex, viewProj, filterType, pCell->children[7]);
		}
	}

	return curIndex;	
}

inline uint	
SDBLayer::QueryFrustumPersp(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, uint curIndex, const gmtl::MatrixA44f& viewProj, 
							float nearZ, float farZ, uint filterType, SDBGridPos& cellPos, SDBOctCell* pCell)
{
	_DEBUG_ASSERT(pCell);
			
	// Cull against a looser bounding volume
	//float cullGridSize	= 2.0f * m_GridSize;
	AABox octCellBV;
	octCellBV.min[0] = (float) cellPos.pos[0];
	octCellBV.min[1] = (float) cellPos.pos[1];
	octCellBV.min[2] = (float) cellPos.pos[2];
	
	VecVecMult((gmtl::VecA4f*) &octCellBV.min, (gmtl::VecA4f*)&octCellBV.min, (gmtl::VecA4f*)&m_GridSizeVec);
	VecVecSub((gmtl::VecA4f*)&octCellBV.min, (gmtl::VecA4f*)&octCellBV.min, (gmtl::VecA4f*)&m_HalfGridSizeVec);
	VecVecAdd((gmtl::VecA4f*)&octCellBV.max, (gmtl::VecA4f*)&octCellBV.min, (gmtl::VecA4f*)&m_CullGridSizeVec);
	
	AABox transformedAA;
	TransformAndProjectAABoxToAABox(&viewProj, nearZ, farZ, &octCellBV, &transformedAA);

	if(AABoxInAABox(&m_FrustumQueryLimits, &transformedAA))
	{
		if(AABoxContainsAABox(&m_FrustumQueryLimits, &transformedAA))
		{
			return QueryFrustumPerspContained(ppToStore, pScreenSpaceRes, bufferSize, curIndex, viewProj, 
											nearZ, farZ, filterType, cellPos, pCell);
		}
		
		// Add each occupant if the bounding volumes intersect
		SDBDataKey* pCurDataKey = &(pCell->firstData);
		while(pCurDataKey->data)
		{
			SDBData* pData = m_pMain->GetDataMap().Get(*pCurDataKey);
			_DEBUG_ASSERT(pData);
		
			if(pData->filterType & filterType)
			{	
				TransformAndProjectOOBoxToAABox(&viewProj, nearZ, farZ, pData->pOOBV, &transformedAA);
				if(AABoxInAABox(&m_FrustumQueryLimits, &transformedAA))
				{
					_DEBUG_ASSERT(curIndex < bufferSize);
					ppToStore[curIndex] = pData->pData;
					*(pScreenSpaceRes+curIndex) = transformedAA;
					curIndex++;
				}
			}
						
			pCurDataKey = &(pData->next);
		}
		
		// Query each oct child if it exists
		if(m_pChild)
		{
			if(pCell->childFlag & 0x01)
				curIndex = m_pChild->QueryFrustumPersp(ppToStore, pScreenSpaceRes, bufferSize, curIndex, viewProj, nearZ, farZ, filterType, pCell->children[0]);

			if(pCell->childFlag & 0x02)
				curIndex = m_pChild->QueryFrustumPersp(ppToStore, pScreenSpaceRes, bufferSize, curIndex, viewProj, nearZ, farZ, filterType, pCell->children[1]);

			if(pCell->childFlag & 0x04)
				curIndex = m_pChild->QueryFrustumPersp(ppToStore, pScreenSpaceRes, bufferSize, curIndex, viewProj, nearZ, farZ, filterType, pCell->children[2]);

			if(pCell->childFlag & 0x08)
				curIndex = m_pChild->QueryFrustumPersp(ppToStore, pScreenSpaceRes, bufferSize, curIndex, viewProj, nearZ, farZ, filterType, pCell->children[3]);

			if(pCell->childFlag & 0x10)
				curIndex = m_pChild->QueryFrustumPersp(ppToStore, pScreenSpaceRes, bufferSize, curIndex, viewProj, nearZ, farZ, filterType, pCell->children[4]);

			if(pCell->childFlag & 0x20)
				curIndex = m_pChild->QueryFrustumPersp(ppToStore, pScreenSpaceRes, bufferSize, curIndex, viewProj, nearZ, farZ, filterType, pCell->children[5]);

			if(pCell->childFlag & 0x40)
				curIndex = m_pChild->QueryFrustumPersp(ppToStore, pScreenSpaceRes, bufferSize, curIndex, viewProj, nearZ, farZ, filterType, pCell->children[6]);

			if(pCell->childFlag & 0x80)
				curIndex = m_pChild->QueryFrustumPersp(ppToStore, pScreenSpaceRes, bufferSize, curIndex, viewProj, nearZ, farZ, filterType, pCell->children[7]);
		}
	}

	return curIndex;
}

inline uint	
SDBLayer::QueryFrustumPersp(void** ppToStore, uint bufferSize, uint curIndex, const gmtl::MatrixA44f& viewProj, 
							float nearZ, float farZ, uint filterType, SDBGridPos& cellPos, SDBOctCell* pCell)
{
	_DEBUG_ASSERT(pCell);
			
	// Cull against a looser bounding volume
	//float cullGridSize	= 2.0f * m_GridSize;
	AABox octCellBV;
	octCellBV.min[0] = (float) cellPos.pos[0];
	octCellBV.min[1] = (float) cellPos.pos[1];
	octCellBV.min[2] = (float) cellPos.pos[2];
	
	VecVecMult((gmtl::VecA4f*) &octCellBV.min, (gmtl::VecA4f*)&octCellBV.min, (gmtl::VecA4f*)&m_GridSizeVec);
	VecVecSub((gmtl::VecA4f*)&octCellBV.min, (gmtl::VecA4f*)&octCellBV.min, (gmtl::VecA4f*)&m_HalfGridSizeVec);
	VecVecAdd((gmtl::VecA4f*)&octCellBV.max, (gmtl::VecA4f*)&octCellBV.min, (gmtl::VecA4f*)&m_CullGridSizeVec);
	
	AABox transformedAA;
	TransformAndProjectAABoxToAABox(&viewProj, nearZ, farZ, &octCellBV, &transformedAA);

	if(AABoxInAABox(&m_FrustumQueryLimits, &transformedAA))
	{
		if(AABoxContainsAABox(&m_FrustumQueryLimits, &transformedAA))
		{
			return QueryContained(ppToStore, bufferSize, curIndex, filterType, cellPos, pCell);
		}
		
		// Add each occupant if the bounding volumes intersect
		SDBDataKey* pCurDataKey = &(pCell->firstData);
		while(pCurDataKey->data)
		{
			SDBData* pData = m_pMain->GetDataMap().Get(*pCurDataKey);
			_DEBUG_ASSERT(pData);
			
			if(pData->filterType & filterType)
			{
				TransformAndProjectOOBoxToAABox(&viewProj, nearZ, farZ, pData->pOOBV, &transformedAA);
				if(AABoxInAABox(&m_FrustumQueryLimits, &transformedAA))
				{
					_DEBUG_ASSERT(curIndex < bufferSize);
					ppToStore[curIndex] = pData->pData;
					curIndex++;
				}
			}
			
			pCurDataKey = &(pData->next);
		}
		
		// Query each oct child if it exists
		if(m_pChild)
		{
			if(pCell->childFlag & 0x01)
				curIndex = m_pChild->QueryFrustumPersp(ppToStore, bufferSize, curIndex, viewProj, nearZ, farZ, filterType, pCell->children[0]);

			if(pCell->childFlag & 0x02)
				curIndex = m_pChild->QueryFrustumPersp(ppToStore, bufferSize, curIndex, viewProj, nearZ, farZ, filterType, pCell->children[1]);

			if(pCell->childFlag & 0x04)
				curIndex = m_pChild->QueryFrustumPersp(ppToStore, bufferSize, curIndex, viewProj, nearZ, farZ, filterType, pCell->children[2]);

			if(pCell->childFlag & 0x08)
				curIndex = m_pChild->QueryFrustumPersp(ppToStore, bufferSize, curIndex, viewProj, nearZ, farZ, filterType, pCell->children[3]);

			if(pCell->childFlag & 0x10)
				curIndex = m_pChild->QueryFrustumPersp(ppToStore, bufferSize, curIndex, viewProj, nearZ, farZ, filterType, pCell->children[4]);

			if(pCell->childFlag & 0x20)
				curIndex = m_pChild->QueryFrustumPersp(ppToStore, bufferSize, curIndex, viewProj, nearZ, farZ, filterType, pCell->children[5]);

			if(pCell->childFlag & 0x40)
				curIndex = m_pChild->QueryFrustumPersp(ppToStore, bufferSize, curIndex, viewProj, nearZ, farZ, filterType, pCell->children[6]);

			if(pCell->childFlag & 0x80)
				curIndex = m_pChild->QueryFrustumPersp(ppToStore, bufferSize, curIndex, viewProj, nearZ, farZ, filterType, pCell->children[7]);
		}
	}

	return curIndex;
}

inline uint	
SDBLayer::QueryCellInfo(AABox* pToStore, uint bufferSize, uint curIndex, const AABox& queryBV, SDBGridPos& cellPos, SDBOctCell* pCell)
{
	_DEBUG_ASSERT(pCell);
			
	// Cull against a looser bounding volume
	//float cullGridSize	= 2.0f * m_GridSize;
	AABox octCellBV;
	octCellBV.min[0] = (float) cellPos.pos[0];
	octCellBV.min[1] = (float) cellPos.pos[1];
	octCellBV.min[2] = (float) cellPos.pos[2];
	
	VecVecMult((gmtl::VecA4f*) &octCellBV.min, (gmtl::VecA4f*)&octCellBV.min, (gmtl::VecA4f*)&m_GridSizeVec);
	VecVecAdd((gmtl::VecA4f*)&octCellBV.max, (gmtl::VecA4f*)&octCellBV.min, (gmtl::VecA4f*)&m_GridSizeVec);

	_DEBUG_ASSERT(curIndex < bufferSize);
	pToStore[curIndex]	= octCellBV;		
	curIndex++;
	
	VecVecSub((gmtl::VecA4f*)&octCellBV.min, (gmtl::VecA4f*)&octCellBV.min, (gmtl::VecA4f*)&m_HalfGridSizeVec);
	VecVecAdd((gmtl::VecA4f*)&octCellBV.max, (gmtl::VecA4f*)&octCellBV.max, (gmtl::VecA4f*)&m_HalfGridSizeVec);
		
	if(AABoxInAABox(&queryBV, &octCellBV))
	{	
		if(AABoxContainsAABox(&queryBV, &octCellBV))
		{
			return QueryCellInfoContained(pToStore, bufferSize, curIndex, cellPos, pCell);
		}
		
		// Query each oct child if it exists
		if(m_pChild)
		{
			if(pCell->childFlag & 0x01)
				curIndex = m_pChild->QueryCellInfo(pToStore, bufferSize, curIndex, queryBV, pCell->children[0]);

			if(pCell->childFlag & 0x02)
				curIndex = m_pChild->QueryCellInfo(pToStore, bufferSize, curIndex, queryBV, pCell->children[1]);

			if(pCell->childFlag & 0x04)
				curIndex = m_pChild->QueryCellInfo(pToStore, bufferSize, curIndex, queryBV, pCell->children[2]);

			if(pCell->childFlag & 0x08)
				curIndex = m_pChild->QueryCellInfo(pToStore, bufferSize, curIndex, queryBV, pCell->children[3]);

			if(pCell->childFlag & 0x10)
				curIndex = m_pChild->QueryCellInfo(pToStore, bufferSize, curIndex, queryBV, pCell->children[4]);

			if(pCell->childFlag & 0x20)
				curIndex = m_pChild->QueryCellInfo(pToStore, bufferSize, curIndex, queryBV, pCell->children[5]);

			if(pCell->childFlag & 0x40)
				curIndex = m_pChild->QueryCellInfo(pToStore, bufferSize, curIndex, queryBV, pCell->children[6]);

			if(pCell->childFlag & 0x80)
				curIndex = m_pChild->QueryCellInfo(pToStore, bufferSize, curIndex, queryBV, pCell->children[7]);
		}
	}

	return curIndex;	
}

inline uint	
SDBLayer::Query(void** ppToStore, uint bufferSize, uint curIndex, const AABox& queryBV, uint filterType, SDBGridPos& cellPos, SDBOctCell* pCell)
{
	_DEBUG_ASSERT(pCell);
			
	// Cull against a looser bounding volume
	//float cullGridSize	= 2.0f * m_GridSize;
	AABox octCellBV;
	octCellBV.min[0] = (float) cellPos.pos[0];
	octCellBV.min[1] = (float) cellPos.pos[1];
	octCellBV.min[2] = (float) cellPos.pos[2];
	
	VecVecMult((gmtl::VecA4f*) &octCellBV.min, (gmtl::VecA4f*)&octCellBV.min, (gmtl::VecA4f*)&m_GridSizeVec);
	VecVecSub((gmtl::VecA4f*)&octCellBV.min, (gmtl::VecA4f*)&octCellBV.min, (gmtl::VecA4f*)&m_HalfGridSizeVec);
	VecVecAdd((gmtl::VecA4f*)&octCellBV.max, (gmtl::VecA4f*)&octCellBV.min, (gmtl::VecA4f*)&m_CullGridSizeVec);
	
	if(AABoxInAABox(&queryBV, &octCellBV))
	{
		if(AABoxContainsAABox(&queryBV, &octCellBV))
		{
			return QueryContained(ppToStore, bufferSize, curIndex, filterType, cellPos, pCell);
		}
		
		// Add each occupant if the bounding volumes intersect
		SDBDataKey* pCurDataKey = &(pCell->firstData);
		while(pCurDataKey->data)
		{
			SDBData* pData = m_pMain->GetDataMap().Get(*pCurDataKey);
			_DEBUG_ASSERT(pData);
			if((pData->filterType & filterType) && AABoxInAABox(&queryBV, pData->pBV))
			{
				_DEBUG_ASSERT(curIndex < bufferSize);
				ppToStore[curIndex] = pData->pData;
				curIndex++;
			}
			
			pCurDataKey = &(pData->next);
		}
		
		// Query each oct child if it exists
		if(m_pChild)
		{
			if(pCell->childFlag & 0x01)
				curIndex = m_pChild->Query(ppToStore, bufferSize, curIndex, queryBV, filterType, pCell->children[0]);

			if(pCell->childFlag & 0x02)
				curIndex = m_pChild->Query(ppToStore, bufferSize, curIndex, queryBV, filterType, pCell->children[1]);

			if(pCell->childFlag & 0x04)
				curIndex = m_pChild->Query(ppToStore, bufferSize, curIndex, queryBV, filterType, pCell->children[2]);

			if(pCell->childFlag & 0x08)
				curIndex = m_pChild->Query(ppToStore, bufferSize, curIndex, queryBV, filterType, pCell->children[3]);

			if(pCell->childFlag & 0x10)
				curIndex = m_pChild->Query(ppToStore, bufferSize, curIndex, queryBV, filterType, pCell->children[4]);

			if(pCell->childFlag & 0x20)
				curIndex = m_pChild->Query(ppToStore, bufferSize, curIndex, queryBV, filterType, pCell->children[5]);

			if(pCell->childFlag & 0x40)
				curIndex = m_pChild->Query(ppToStore, bufferSize, curIndex, queryBV, filterType, pCell->children[6]);

			if(pCell->childFlag & 0x80)
				curIndex = m_pChild->Query(ppToStore, bufferSize, curIndex, queryBV, filterType, pCell->children[7]);
		}
	}

	return curIndex;	
}

//////////////////////////////////////////////////////////////////////////
//	Query Contained
//
//////////////////////////////////////////////////////////////////////////
inline uint
SDBLayer::QueryContained(void** ppToStore, uint bufferSize, uint curIndex, uint filterType, SDBGridPos& cellPos)
{
	SDBOctCell* pCell = m_Grid.Get(cellPos);
	return QueryContained(ppToStore, bufferSize, curIndex, filterType, cellPos, pCell);
}

inline uint
SDBLayer::QueryCellInfoContained(AABox* pToStore, uint bufferSize, uint curIndex, SDBGridPos& cellPos)
{
	SDBOctCell* pCell = m_Grid.Get(cellPos);
	return QueryCellInfoContained(pToStore, bufferSize, curIndex, cellPos, pCell);
}

inline uint	
SDBLayer::QueryFrustumPerspContained(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, uint curIndex, 
									const gmtl::MatrixA44f& viewProj, float nearZ, float farZ, 
									uint filterType, SDBGridPos& cellPos)
{
	SDBOctCell* pCell = m_Grid.Get(cellPos);
	return QueryFrustumPerspContained(ppToStore, pScreenSpaceRes, bufferSize, curIndex, viewProj, nearZ, farZ, filterType, cellPos, pCell);	
}
									
inline uint	
SDBLayer::QueryFrustumOrthoContained(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, uint curIndex, 
									const gmtl::MatrixA44f& viewProj, uint filterType, SDBGridPos& cellPos)
{
	SDBOctCell* pCell = m_Grid.Get(cellPos);
	return QueryFrustumOrthoContained(ppToStore, pScreenSpaceRes, bufferSize, curIndex, viewProj, filterType, cellPos, pCell);	
}

//////////////////////////////////////////////////////////////////////////
//	Query
//
//////////////////////////////////////////////////////////////////////////

uint
SDBLayer::QuerySphere(void** ppToStore, uint bufferSize, uint curIndex, const Sphere& sphere, uint filterType, SDBGridPos& cellPos)
{
	SDBOctCell* pCell = m_Grid.Get(cellPos);
	return QuerySphere(ppToStore, bufferSize, curIndex, sphere, filterType, cellPos, pCell);
}

uint	
SDBLayer::QueryFrustumOrtho(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, uint curIndex,
							const gmtl::MatrixA44f& viewProj, uint filterType, SDBGridPos& cellPos)
{
	SDBOctCell* pCell = m_Grid.Get(cellPos);
	return QueryFrustumOrtho(ppToStore, pScreenSpaceRes, bufferSize, curIndex, viewProj, filterType, cellPos, pCell);
}

uint	
SDBLayer::QueryFrustumOrtho(void** ppToStore, uint bufferSize, uint curIndex, const gmtl::MatrixA44f& viewProj, uint filterType, SDBGridPos& cellPos)
{
	SDBOctCell* pCell = m_Grid.Get(cellPos);
	return QueryFrustumOrtho(ppToStore, bufferSize, curIndex, viewProj, filterType, cellPos, pCell);
}

uint	
SDBLayer::QueryFrustumPersp(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, uint curIndex, const gmtl::MatrixA44f& viewProj, 
							float nearZ, float farZ, uint filterType, SDBGridPos& cellPos)
{
	SDBOctCell* pCell = m_Grid.Get(cellPos);
	return QueryFrustumPersp(ppToStore, pScreenSpaceRes, bufferSize, curIndex, viewProj, nearZ, farZ, filterType, cellPos, pCell);
}

uint	
SDBLayer::QueryFrustumPersp(void** ppToStore, uint bufferSize, uint curIndex, const gmtl::MatrixA44f& viewProj, 
							float nearZ, float farZ, uint filterType, SDBGridPos& cellPos)
{
	SDBOctCell* pCell = m_Grid.Get(cellPos);
	return QueryFrustumPersp(ppToStore, bufferSize, curIndex, viewProj, nearZ, farZ, filterType, cellPos, pCell);
}

uint	
SDBLayer::Query(void** ppToStore, uint bufferSize, uint curIndex, const AABox& queryBV, uint filterType, SDBGridPos& cellPos)
{
	SDBOctCell* pCell = m_Grid.Get(cellPos);
	return Query(ppToStore, bufferSize, curIndex, queryBV, filterType, cellPos, pCell);
}

uint	
SDBLayer::QueryCellInfo(AABox* pToStore, uint bufferSize, uint curIndex, const AABox& queryBV, SDBGridPos& cellPos)
{
	SDBOctCell* pCell = m_Grid.Get(cellPos);
	return QueryCellInfo(pToStore, bufferSize, curIndex, queryBV, cellPos, pCell);
}

//////////////////////////////////////////////////////////////////////////
//	Top Layer Query
//
//////////////////////////////////////////////////////////////////////////

//inline uint
//SDBLayer::QueryNeighbourPairSphere(SDBGridPos& gridPos, void** ppToStore, uint bufferSize, uint curIndex, 
//									const Sphere& sphere, uint filterType, int zInc)
//{
//	__QUERY_NEIGHBOUR_PAIR(QuerySphere(ppToStore, bufferSize, curIndex, sphere, filterType, gridPos, pCell))	
//}
//
//inline uint
//SDBLayer::QueryNeighbourPairFrustumOrtho(SDBGridPos& gridPos, void** ppToStore, uint bufferSize, uint curIndex, 
//									const gmtl::MatrixA44f& viewProj, uint filterType, int zInc)
//{
//	__QUERY_NEIGHBOUR_PAIR(QueryFrustumOrtho(ppToStore, bufferSize, curIndex, viewProj, filterType, gridPos, pCell))	
//}
//
//inline uint
//SDBLayer::QueryNeighbourPairFrustumOrtho(SDBGridPos& gridPos, void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, uint curIndex, 
//									const gmtl::MatrixA44f& viewProj, uint filterType, int zInc)
//{
//	__QUERY_NEIGHBOUR_PAIR(QueryFrustumOrtho(ppToStore, pScreenSpaceRes, bufferSize, curIndex, viewProj, filterType, gridPos, pCell))	
//}
//
//
//inline uint
//SDBLayer::QueryNeighbourPairFrustumPersp(SDBGridPos& gridPos, void** ppToStore, uint bufferSize, uint curIndex, 
//									const gmtl::MatrixA44f& viewProj, float nearZ, float farZ, uint filterType, int zInc)
//{
//	__QUERY_NEIGHBOUR_PAIR(QueryFrustumPersp(ppToStore, bufferSize, curIndex, viewProj, nearZ, farZ, filterType, gridPos, pCell))	
//}
//
//inline uint
//SDBLayer::QueryNeighbourPairFrustumPersp(SDBGridPos& gridPos, void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, uint curIndex, 
//									const gmtl::MatrixA44f& viewProj, float nearZ, float farZ, uint filterType, int zInc)
//{
//	__QUERY_NEIGHBOUR_PAIR(QueryFrustumPersp(ppToStore, pScreenSpaceRes, bufferSize, curIndex, viewProj, nearZ, farZ, filterType, gridPos, pCell))	
//}
//
//inline uint
//SDBLayer::QueryNeighbourPair(SDBGridPos& gridPos, void** ppToStore, uint bufferSize, uint curIndex, const AABox& queryBV, uint filterType, int zInc)
//{
//	__QUERY_NEIGHBOUR_PAIR(Query(ppToStore, bufferSize, curIndex, queryBV, filterType, gridPos, pCell))	
//}
//
//inline uint
//SDBLayer::QueryNeighbourPairCellInfo(SDBGridPos& gridPos, AABox* pToStore, uint bufferSize, uint curIndex, const AABox& queryBV, int zInc)
//{
//	__QUERY_NEIGHBOUR_PAIR(QueryCellInfo(pToStore, bufferSize, curIndex, queryBV, gridPos, pCell))	
//}

uint
SDBLayer::QueryIncludeAdjacentSphere(void** ppToStore, uint bufferSize, uint curIndex, const AABox& sphereAA, const Sphere& sphere, uint filterType, float looseOffset)
{
	const AABox& queryBV = sphereAA;
	//queryBV.min.set(sphere.center[0] - sphere.radius, sphere.center[1] - sphere.radius, sphere.center[2] - sphere.radius);
	//queryBV.max.set(sphere.center[0] + sphere.radius, sphere.center[1] + sphere.radius, sphere.center[2] + sphere.radius);
	
	__QUERY_ITERATE(QuerySphere(ppToStore, bufferSize, curIndex, sphere, filterType, gridPos, pCell))	

	//__QUERY_ADJACENT(QueryNeighbourPairSphere(gridPos, ppToStore, bufferSize, curIndex, sphere, filterType, -1), 
	//				QueryNeighbourPairSphere(gridPos, ppToStore, bufferSize, curIndex, sphere, filterType, 1))
}

uint
SDBLayer::QueryIncludeAdjacentFrustumPersp(void** ppToStore, uint bufferSize, uint curIndex, 
										const AABox& queryBV, const gmtl::MatrixA44f& viewProj, 
										float nearZ, float farZ, uint filterType, float looseOffset)
{
	__QUERY_ITERATE(QueryFrustumPersp(ppToStore, bufferSize, curIndex, viewProj, nearZ, farZ, filterType, gridPos, pCell))	

	//__QUERY_ADJACENT(QueryNeighbourPairFrustumPersp(gridPos, ppToStore, bufferSize, curIndex, viewProj, nearZ, farZ, filterType, -1), 
	//				QueryNeighbourPairFrustumPersp(gridPos, ppToStore, bufferSize, curIndex, viewProj, nearZ, farZ, filterType, 1))
}

uint SDBLayer::QueryIncludeAdjacentFrustumPersp(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, uint curIndex, 
												const AABox& queryBV, const gmtl::MatrixA44f& viewProj, 
												float nearZ, float farZ, uint filterType, float looseOffset)
{	
	__QUERY_ITERATE(QueryFrustumPersp(ppToStore, pScreenSpaceRes, bufferSize, curIndex, viewProj, nearZ, farZ, filterType, gridPos, pCell))	
}


uint
SDBLayer::QueryIncludeAdjacentFrustumOrtho(void** ppToStore, uint bufferSize, uint curIndex, 
										const AABox& queryBV, const gmtl::MatrixA44f& viewProj, uint filterType, float looseOffset)
{
	__QUERY_ITERATE(QueryFrustumOrtho(ppToStore, bufferSize, curIndex, viewProj, filterType, gridPos, pCell))

	//__QUERY_ADJACENT(QueryNeighbourPairFrustumOrtho(gridPos, ppToStore, bufferSize, curIndex, viewProj, filterType, -1),
	//				QueryNeighbourPairFrustumOrtho(gridPos, ppToStore, bufferSize, curIndex, viewProj, filterType, 1))
}

uint
SDBLayer::QueryIncludeAdjacentFrustumOrtho(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, uint curIndex, 
										const AABox& queryBV, const gmtl::MatrixA44f& viewProj, uint filterType, float looseOffset)
{
	__QUERY_ITERATE(QueryFrustumOrtho(ppToStore, pScreenSpaceRes, bufferSize, curIndex, viewProj, filterType, gridPos, pCell))
}

uint
SDBLayer::QueryIncludeAdjacent(void** ppToStore, uint bufferSize, uint curIndex, const AABox& queryBV, uint filterType, float looseOffset)
{
	__QUERY_ITERATE(Query(ppToStore, bufferSize, curIndex, queryBV, filterType, gridPos, pCell))	

	//__QUERY_ADJACENT(QueryNeighbourPair(gridPos, ppToStore, bufferSize, curIndex, queryBV, filterType, -1),
	//				QueryNeighbourPair(gridPos, ppToStore, bufferSize, curIndex, queryBV, filterType, 1))
}

uint
SDBLayer::QueryIncludeAdjacentCellInfo(AABox* pToStore, uint bufferSize, uint curIndex, const AABox& queryBV, float looseOffset)
{
	__QUERY_ITERATE(QueryCellInfo(pToStore, bufferSize, curIndex, queryBV, gridPos, pCell))
	//__QUERY_ADJACENT(QueryNeighbourPairCellInfo(gridPos, pToStore, bufferSize, curIndex, queryBV, -1), 
	//				QueryNeighbourPairCellInfo(gridPos, pToStore, bufferSize, curIndex, queryBV, 1))
}

_NAMESPACE_END