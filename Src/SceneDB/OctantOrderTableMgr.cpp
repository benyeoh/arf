//==============================================================================
//
//		OctantOrderTableMgr.cpp
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Monday, 24 June, 2013
//
//==============================================================================

#include "SceneDBFramework.h"

_NAMESPACE_BEGIN

const static uint OCTANT_ORDER_XYZ_BITS = 9;
const static uint OCTANT_ORDER_LEVEL_BITS = 4;

const static uint OCTANT_ORDER_VAL[OCTANT_ORDER_XYZ_BITS] =
{
	1,
	8 + 1,
	8*8 + 8 + 1,
	8*8*8 + 8*8 + 8 + 1,
	8*8*8*8 + 8*8*8 + 8*8 + 8 + 1,
	8*8*8*8*8 + 8*8*8*8 + 8*8*8 + 8*8 + 8 + 1,
	8*8*8*8*8*8 + 8*8*8*8*8 + 8*8*8*8 + 8*8*8 + 8*8 + 8 + 1,
	8*8*8*8*8*8*8 + 8*8*8*8*8*8 + 8*8*8*8*8 + 8*8*8*8 + 8*8*8 + 8*8 + 8 + 1,
	8*8*8*8*8*8*8*8 + 8*8*8*8*8*8*8 + 8*8*8*8*8*8 + 8*8*8*8*8 + 8*8*8*8 + 8*8*8 + 8*8 + 8 + 1,
};

const static _ALIGN(16) uint OCTANT_ORDER_FIRST_QUAD[4] =
{
	OCTANT_ORDER_VAL[1],
	OCTANT_ORDER_VAL[2],
	OCTANT_ORDER_VAL[3],
	OCTANT_ORDER_VAL[4],
};

const static _ALIGN(16) uint OCTANT_ORDER_SECOND_QUAD[4] =
{
	OCTANT_ORDER_VAL[5],
	OCTANT_ORDER_VAL[6],
	OCTANT_ORDER_VAL[7],
	OCTANT_ORDER_VAL[8],
};

void OctantOrderTableManager::SetGridSize(uint numLevels, float size)
{
	_DEBUG_ASSERT(numLevels > 0);
	_DEBUG_ASSERT(numLevels < OCTANT_ORDER_XYZ_BITS);

	// The actual number of levels including the root
	m_TopLevel = numLevels;
	m_Size = size;
	m_TopGridSize = size * (1 << (numLevels-1));
}

uint OctantOrderTableManager::FindLevelOffsetFromSize(float size)
{
	const static float INV_LN_2 = 1.0f / 0.693147181f;

	// Compute the level that we wish to add to
	float minAmount	= gmtl::Math::log(size / m_Size) * INV_LN_2;
	int level		= FastCeiling(minAmount);
	if(level < 0)	 
		level = 0;

	_DEBUG_ASSERT((uint)level <= m_TopLevel);

	return (uint) (m_TopLevel - level);
}

uint OctantOrderTableManager::GetOctantOrdering(const gmtl::VecA4i& gridInOctantPos)
{
	// x = (x | (x << 16)) & 0x030000FF;
	// x = (x | (x <<  8)) & 0x0300F00F;
	// x = (x | (x <<  4)) & 0x030C30C3;
	// x = (x | (x <<  2)) & 0x09249249;

	gmtl::VecA4i interleavePos;
	gmtl::VecA4i shiftPos;
	VecScalarShiftLL(&shiftPos, &gridInOctantPos, 16);
	VecVecOr(&interleavePos, &gridInOctantPos, &shiftPos);
	VecScalarAnd(&interleavePos, &interleavePos, 0x030000FF);

	VecScalarShiftLL(&shiftPos, &interleavePos, 8);
	VecVecOr(&interleavePos, &interleavePos, &shiftPos);
	VecScalarAnd(&interleavePos, &interleavePos, 0x0300F00F);

	VecScalarShiftLL(&shiftPos, &interleavePos, 4);
	VecVecOr(&interleavePos, &interleavePos, &shiftPos);
	VecScalarAnd(&interleavePos, &interleavePos, 0x030C30C3);

	VecScalarShiftLL(&shiftPos, &interleavePos, 2);
	VecVecOr(&interleavePos, &interleavePos, &shiftPos);
	VecScalarAnd(&interleavePos, &interleavePos, 0x09249249);

	// X->Z->Y order
	uint octantOrder = interleavePos[0] | (interleavePos[2] << 1) | (interleavePos[1] << 2);
	return octantOrder;
}

uint OctantOrderTableManager::GetOctantOrderingHash(uint octantOrder, uint levelFromTop)
{
	gmtl::Vec4i octantOrderVec1;	
	VecVecShiftRL(&octantOrderVec1, gmtl::Vec4i(octantOrder), gmtl::Vec4i(3, 6, 9, 12));
	VecScalarAnd(&octantOrderVec1, &octantOrderVec1, 0x7);

	gmtl::Vec4i octantOrderVec2;
	VecVecShiftRL(&octantOrderVec2, gmtl::Vec4i(octantOrder), gmtl::Vec4i(15, 18, 21, 24));
	VecScalarAnd(&octantOrderVec2, &octantOrderVec2, 0x7);

	VecVecMult(&octantOrderVec1, &octantOrderVec1, (gmtl::VecA4i*) OCTANT_ORDER_FIRST_QUAD);
	VecVecMult(&octantOrderVec2, &octantOrderVec2, (gmtl::VecA4i*) OCTANT_ORDER_SECOND_QUAD);
	VecVecAdd(&octantOrderVec1, &octantOrderVec1, &octantOrderVec2);

	return octantOrderVec1[0] + octantOrderVec1[1] + octantOrderVec1[2] + octantOrderVec1[3] + (octantOrder & 0x7) + levelFromTop;

	//_DEBUG_ASSERT(levelFromTop <= m_TopLevel);

	//// Accumulate the octant ordering starting from the top level
	//gmtl::VecA4i totalSum(0, 0, 0, 0);

	//uint curLevel = m_TopLevel-1;
	//_LOOPi(levelFromTop)
	//{
	//	// Extract bit at level
	//	gmtl::VecA4i xyzVal;
	//	VecScalarShiftRL(&xyzVal, &octantPos, curLevel);
	//	VecScalarAnd(&xyzVal, &xyzVal, 1);

	//	// Multiply by the octant value and add it to the current total
	//	// X dir first, then Z, then Y
	//	gmtl::VecA4i octantLevelVal(OCTANT_ORDER_VAL[curLevel], OCTANT_ORDER_VAL[curLevel] << 2, OCTANT_ORDER_VAL[curLevel] << 1, 0);
	//	VecVecMult(&xyzVal, &xyzVal, &octantLevelVal);
	//	VecVecAdd(&totalSum, &totalSum, &xyzVal);

	//	curLevel--;
	//}

	//// Sum total
	//uint total = totalSum[0] + totalSum[1] + totalSum[2] + levelFromTop;
	//return total;
}

uint OctantOrderTableManager::GetOctantHash(const gmtl::VecA4i& octantPos)
{
	const uint BIT_RANGE_XZ		= 12;
	const uint BIT_RANGE_Y		= 32 - (BIT_RANGE_XZ + BIT_RANGE_XZ);
	const uint EFFECTIVE_RANGE_XZ	= 1 << (BIT_RANGE_XZ - 1);
	const uint EFFECTIVE_RANGE_Y	= 1 << (BIT_RANGE_Y - 1);

	// We also have to make sure that the range is positive
	const gmtl::VecA4i midBias(EFFECTIVE_RANGE_XZ, EFFECTIVE_RANGE_Y, EFFECTIVE_RANGE_XZ, 0);
	gmtl::VecA4i octantPosBiased; 
	VecVecAdd(&octantPosBiased, &octantPos, &midBias);

	_DEBUG_ASSERT(octantPosBiased[0] >= 0);
	_DEBUG_ASSERT(octantPosBiased[1] >= 0);
	_DEBUG_ASSERT(octantPosBiased[2] >= 0);
	_DEBUG_ASSERT(octantPosBiased[0] < (1 << BIT_RANGE_XZ));
	_DEBUG_ASSERT(octantPosBiased[1] < (1 << BIT_RANGE_Y));
	_DEBUG_ASSERT(octantPosBiased[2] < (1 << BIT_RANGE_XZ));

	// Then get the octant position ID for hashing
	// We also give greater range to the horizontal directions and less on the vertical
	return (octantPosBiased[1] << (BIT_RANGE_XZ + BIT_RANGE_XZ)) | (octantPosBiased[2] << BIT_RANGE_XZ) | octantPosBiased[0];
}

void OctantOrderTableManager::GetOctantAndGridPosFromWorld(const gmtl::VecA3f& pos, gmtl::VecA4i& outOctantPos, gmtl::VecA4i& outGridPosInOctant)
{
	// Scale position wrt to TopGridSize
	gmtl::VecA3f gridPos;
	VecScalarMult(&gridPos, &pos, 1.0f / m_TopGridSize);

	// Get the index of the grid pos
	gmtl::VecA4i gridPosFloorInt; 
	gmtl::VecA4f gridPosFloorFloat;
	VecFloor(&gridPosFloorInt, &gridPos);
	VecIntToFloat(&gridPosFloorFloat, &gridPosFloorInt);

	outOctantPos = gridPosFloorInt;

	// Find the octant grid pos within the octant

	// Find the offset within the octant
	gmtl::VecA4f posInOctant;
	VecVecSub(&posInOctant, &gridPos, &gridPosFloorFloat);

	// Scale it to grid sized
	VecScalarMult(&posInOctant, &posInOctant, m_TopGridSize / m_Size);

	// Then get the truncated coordinates
	gmtl::VecA4i posInOctantInt;
	VecFloatToInt(&posInOctantInt, &posInOctant);

	_DEBUG_ASSERT(posInOctantInt[0] >= 0);
	_DEBUG_ASSERT(posInOctantInt[1] >= 0);
	_DEBUG_ASSERT(posInOctantInt[2] >= 0);
	_DEBUG_ASSERT(posInOctantInt[0] < (1 << OCTANT_ORDER_XYZ_BITS));
	_DEBUG_ASSERT(posInOctantInt[1] < (1 << OCTANT_ORDER_XYZ_BITS));
	_DEBUG_ASSERT(posInOctantInt[2] < (1 << OCTANT_ORDER_XYZ_BITS));

	outGridPosInOctant = posInOctantInt;
}

void OctantOrderTableManager::InsertParentCells(OctantData* pOctant, uint octantOrderHash, uint octantOrdering, uint parentOffset, uint levelOffset)
{
	// Compute hash value at parent cell by subtracting from sum total

	// Start from current level up to the root and insert missing parent cells
	uint curLevel = m_TopLevel - levelOffset;
	
	_LOOPi(levelOffset)
	{
		//uint curLevelOffset = levelOffset - i;
		//uint curLevel = m_TopLevel - curLevelOffset;
		//uint parentOffset = (octantOrdering >> (curLevel * 3)) & 0x7;

		// We find the hash for the parent cell by subtracting from current cell relative offset
		octantOrderHash -= (parentOffset * OCTANT_ORDER_VAL[curLevel] + 1);

		OctantOrderCellKey parentCellKey;
		parentCellKey.gridUniqueHash = octantOrderHash;

		OctantOrderCell* pParentCell = pOctant->pCellTable->Get(toInsertCellKey);

		uint childFlag = (1 << parentOffset);

		// If we find a parent cell, it means that all parents are already inserted
		if(pParentCell)
		{
			// Add the child and then return
			pParentCell->children |= childFlag;
			break;
		}

		// We add the new parent cell now
		// Compute the level offset of the parent cell
		uint curLevelOffset = levelOffset - i - 1;
		curLevel = m_TopLevel - curLevelOffset;
		
		// Then find the offset relative to its parent
		parentOffset = (octantOrdering >> (curLevel * 3)) & 0x7;

		pParentCell					= pOctant->pCellTable->AddDeferred(parentCellKey);
		pParentCell->children		= (curLevelOffset << (8+3)) | (parentOffset << 8) | childFlag;
		pParentCell->firstDataID	= NULL;
		pOctant->pCellTable->FinishedAdding();
	}
}

boolean OctantOrderTableManager::InsertInOctant(OctantData* pOctant, uint octantOrderHash, uint parentOrderOffset, const AABox* pAABox, const OOBox* pOOBV, uint filterType, void* pData, SDBDataHandle* pOutHandle)
{
	OctantOrderCellKey toInsertCellKey;
	toInsertCellKey.gridUniqueHash = octantOrderHash;
	
	OctantOrderDataKey toInsertDataKey;
	toInsertDataKey.dataID	= (size_t) pData;
	toInsertDataKey.cellKey = toInsertCellKey;

	OctantOrderData* pToInsertData = pOctant->pDataTable->AddDeferred(toInsertDataKey);
	pToInsertData->flag		= filterType;
	pToInsertData->pOOBB	= pOOBV;
	pToInsertData->pAABB	= pAABox;
	pToInsertData->prevID	= NULL;

	boolean isNewCellAdded	= FALSE;
	OctantOrderCell* pCell	= pOctant->pCellTable->Get(toInsertCellKey);
	if(pCell)
	{
		// Set the data to point to the current head
		pToInsertData->nextID = pCell->firstDataID;

		// Add the data to the head of the chain of occupants
		if(pCell->firstDataID)
		{
			// Set the current head's prev to the newly inserted data
			OctantOrderDataKey firstDataKey;
			firstDataKey.dataID		= pCell->firstDataID;
			firstDataKey.cellKey	= toInsertCellKey;
			OctantOrderData* pFirstData = pOctant->pDataTable->Get(firstDataKey);
			_DEBUG_ASSERT(pFirstData);
			_DEBUG_ASSERT(pFirstData->prevID == NULL);
			pFirstData->prevID = toInsertDataKey.dataID;
		}

		pCell->firstDataID = toInsertDataKey.dataID;
	}
	else
	{
		// Set the data as the only occupant in the chain
		pToInsertData->nextID = NULL;

		// Add a new cell
		pCell = pOctant->pCellTable->AddDeferred(toInsertCellKey);
		pCell->children		= (parentOrderOffset << 8);	// 8 bits for children flag, 3 bits + 4 bits for parent order offset and level offset
		pCell->firstDataID	= toInsertDataKey.dataID;
		pOctant->pCellTable->FinishedAdding();

		isNewCellAdded = TRUE;
	}

	// Finish adding data
	pOctant->pDataTable->FinishedAdding();

	// Set handle for this data
	pOutHandle->data[0] = toInsertDataKey.dataID;
	pOutHandle->data[1] = toInsertDataKey.cellKey.gridUniqueHash;

	return isNewCellAdded;
}

void OctantOrderTableManager::Insert(const AABox* pAABox, const OOBox* pOOBV, uint filterType, void* pData, SDBDataHandle* pOutHandle)
{
	gmtl::VecA3f diff;
	VecVecSub(&diff, &(pAABox->max), &(pAABox->min));
	float size = diff[0] > diff[1] ? diff[0] : diff[1];
	size = diff[2] > size ? diff[2] : size;

	gmtl::VecA3f center;
	VecVecAdd(&center, &(pAABox->max), &(pAABox->min));
	VecScalarMult(&center, &center, 0.5f);

	if(size <= m_TopGridSize)
	{
		// Find an octant and get the grid position within the octant
		gmtl::VecA4i octantPos;
		gmtl::VecA4i gridPosInOctant;
		GetOctantAndGridPosFromWorld(center, octantPos, gridPosInOctant);

		// Compute octant hash
		uint octantHash			= GetOctantHash(octantPos);

		// Compute ordering within octant
		uint octantOrdering		= GetOctantOrdering(gridPosInOctant);
		uint levelOffset		= FindLevelOffsetFromSize(size);

		// We only want the relevant bits from the level offset
		octantOrdering			= octantOrdering & (0xFFFFFFFF << ((m_TopLevel - levelOffset) * 3));

		// This is the offset from the parent and the level offset from the root
		uint parentOrderOffset	= (levelOffset << 3) | (octantOrdering >> ((m_TopLevel - levelOffset) * 3)) & 0x7;

		// Get the octant ordering hash
		uint octantOrderHash	= GetOctantOrderingHash(octantOrdering, levelOffset);

		OctantKey octantKey;
		octantKey.octantPos = octantHash;
		OctantData* pOctant = m_Octants.Get(octantKey);
		if(!pOctant)
		{
			// Octant does not exist, so add
			OctantData* pJustAdded = m_Octants.AddDeferred(key);
			if(m_FreeOctantData.size() > 0)
			{
				OctantData& lastFree = m_FreeOctantData.back();
				pJustAdded->pCellTable	= lastFree.pCellTable;
				pJustAdded->pDataTable	= lastFree.pDataTable;
			}
			else
			{
				OctantData newAlloc;
				newAlloc.pCellTable = _NEW OctantCellTable();
				newAlloc.pDataTable	= _NEW OctantDataTable();
				newAlloc.pCellTable->Initialize(32, 0.67f);
				newAlloc.pDataTable->Initialize(32, 0.67f);

				m_AllocatedOctantData.push_back(newAlloc);
				pJustAdded->pCellTable = newAlloc.pCellTable;
				pJustAdded->pDataTable = newAlloc.pDataTable;
			}

			m_Octants.FinishedAdding();
			pOctant = m_Octants.Get(key);

			_DEBUG_ASSERT(pOctant);
		}

		// Then insert into octant order table
		if(InsertInOctant(pOctant, octantOrderHash, parentOrderOffset, pAABox, pOOBV, filterType, pData, pOutHandle))
			InsertParentCells(pOctant, octantOrderHash, octantOrdering, (parentOrderOffset & 0x7), levelOffset);
	}
	else
	{
		// Insert to TooLargeList

	}

}

void OctantOrderTableManager::Remove(SDBDataHandle* pHandle)
{

}

uint OctantOrderTableManager::Query(void** ppToStore, uint bufferSize, const AABox& queryBV, uint filterType)
{

}

uint OctantOrderTableManager::QuerySphere(void** ppToStore, uint bufferSize, const Sphere& sphereBV, uint filterType)
{

}

uint OctantOrderTableManager::QueryInFrustumOrtho(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, AABox& frustumAA, const gmtl::MatrixA44f& viewProj, uint filterType)
{

}

uint OctantOrderTableManager::QueryInFrustumPersp(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, AABox& frustumAA, const gmtl::MatrixA44f& viewProj, float nearZ, float farZ, uint filterType)
{

}	

uint OctantOrderTableManager::QueryAll(void** ppToStore, uint bufferSize, uint filterType)
{

}

uint OctantOrderTableManager::QueryCellInfo(AABox* pToStore, uint bufferSize, const AABox& queryBV)
{

}

_NAMESPACE_END