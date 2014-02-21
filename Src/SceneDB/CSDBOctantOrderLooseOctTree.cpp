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

_NAMESPACE_BEGIN

CSDBOctantOrderLooseOctTree::CSDBOctantOrderLooseOctTree(uint numLevels, float size)
	: m_FreeTooLargeDataHeadID(__INVALID)
	, m_TooLargeDataHeadID(__INVALID)
	, m_FreeAppHandleHeadID(__INVALID)
{
	const static int MIN_LEVEL_OFFSET_FOR_DATA = 1;

	_DEBUG_ASSERT(numLevels >= MIN_LEVEL_OFFSET_FOR_DATA);
	_DEBUG_ASSERT(numLevels <= OCTANT_ORDER_XYZ_BITS);

	// The actual number of levels excluding the root
	m_TopLevel = numLevels;
	m_Size = size;
	m_TopGridSize = size * (1 << numLevels);

    m_OctantNumGridsPerSide = (m_TopGridSize / m_Size);
    m_OctantMaxGridPerSide  = ((int) m_OctantNumGridsPerSide) - 1;

	// Only accept objects that fit into the next level after the top
	// for performance reasons. Basically the top level search is using a basic AABB
	// test, so we want to accelerate loose bounds checking by only allowing
	// insertion of objects in children layers
	m_MaxGridSizeForData = size * (1 << (numLevels - MIN_LEVEL_OFFSET_FOR_DATA));

	m_FrustumQueryLimits.min = gmtl::VecA3f(-1.0f, -1.0f, 0.0f);
	m_FrustumQueryLimits.max = gmtl::VecA3f(1.0f, 1.0f, 1.0f);

    m_Octants.Initialize(32, 0.67f);
}

CSDBOctantOrderLooseOctTree::~CSDBOctantOrderLooseOctTree()
{
	_LOOPi(m_AllocatedOctantData.size())
	{
		_DELETE(m_AllocatedOctantData[i].pCellTable);
		_DELETE(m_AllocatedOctantData[i].pDataTable);
	}
}

inline uint CSDBOctantOrderLooseOctTree::FindLevelOffsetFromSize(float size)
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

inline uint CSDBOctantOrderLooseOctTree::GetOctantOrdering(const gmtl::VecA3i& gridInOctantPos)
{
	// We interleave XYZ in one 32 bit integer, 3-bit each
	// x = (x | (x << 16)) & 0x030000FF;
	// x = (x | (x <<  8)) & 0x0300F00F;
	// x = (x | (x <<  4)) & 0x030C30C3;
	// x = (x | (x <<  2)) & 0x09249249;

	gmtl::VecA3i interleavePos;
	gmtl::VecA3i shiftPos;
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

inline uint CSDBOctantOrderLooseOctTree::GetOctantOrderingHash(uint octantOrder, uint levelFromTop)
{
	gmtl::VecA4i octantOrderVec1;
    octantOrderVec1[0] = octantOrder >> 3;
    octantOrderVec1[1] = octantOrder >> 6;
    octantOrderVec1[2] = octantOrder >> 9;
    octantOrderVec1[3] = octantOrder >> 12;
	//VecVecShiftRL(&octantOrderVec1, &gmtl::VecA4i(octantOrder, octantOrder, octantOrder, octantOrder), &gmtl::VecA4i(3, 6, 9, 12));
	VecScalarAnd(&octantOrderVec1, &octantOrderVec1, 0x7);

	gmtl::VecA4i octantOrderVec2;
    octantOrderVec2[0] = octantOrder >> 15;
    octantOrderVec2[1] = octantOrder >> 18;
    octantOrderVec2[2] = octantOrder >> 21;
    octantOrderVec2[3] = octantOrder >> 24;
	//VecVecShiftRL(&octantOrderVec2, &gmtl::VecA4i(octantOrder, octantOrder, octantOrder, octantOrder), &gmtl::VecA4i(15, 18, 21, 24));
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

inline uint CSDBOctantOrderLooseOctTree::GetOctantHash(const gmtl::VecA3i& octantPos)
{
	_DEBUG_ASSERT(octantPos[0] >= -EFFECTIVE_RANGE_XZ);
	_DEBUG_ASSERT(octantPos[1] >= -EFFECTIVE_RANGE_Y);
	_DEBUG_ASSERT(octantPos[2] >= -EFFECTIVE_RANGE_XZ);
	_DEBUG_ASSERT(octantPos[0] < EFFECTIVE_RANGE_XZ);
	_DEBUG_ASSERT(octantPos[1] < EFFECTIVE_RANGE_Y);
	_DEBUG_ASSERT(octantPos[2] < EFFECTIVE_RANGE_XZ);

	// We also have to make sure that the range is positive
	const gmtl::VecA3i midBias(EFFECTIVE_RANGE_XZ, EFFECTIVE_RANGE_Y, EFFECTIVE_RANGE_XZ);
	gmtl::VecA3i octantPosBiased; 
	VecVecAdd(&octantPosBiased, &octantPos, &midBias);

	_DEBUG_ASSERT(octantPosBiased[0] >= 0);
	_DEBUG_ASSERT(octantPosBiased[1] >= 0);
	_DEBUG_ASSERT(octantPosBiased[2] >= 0);
	_DEBUG_ASSERT(octantPosBiased[0] < (1 << NUM_BITS_XZ));
	_DEBUG_ASSERT(octantPosBiased[1] < (1 << NUM_BITS_Y));
	_DEBUG_ASSERT(octantPosBiased[2] < (1 << NUM_BITS_XZ));

	// Then get the octant position ID for hashing
	// We also give greater range to the horizontal directions and less on the vertical
	return (octantPosBiased[1] << (NUM_BITS_XZ + NUM_BITS_XZ)) | (octantPosBiased[2] << NUM_BITS_XZ) | octantPosBiased[0];
}

inline void CSDBOctantOrderLooseOctTree::GetOctantPosFromWorld(const gmtl::VecA3f& pos, gmtl::VecA3i& outOctantPos)
{
	// Scale position wrt to TopGridSize
	gmtl::VecA3f gridPos;
	VecScalarMult(&gridPos, &pos, 1.0f / m_TopGridSize);

	// Get the index of the grid pos
	gmtl::VecA3i gridPosFloorInt; 
	VecFloor(&gridPosFloorInt, &gridPos);

	outOctantPos = gridPosFloorInt;
}

inline void CSDBOctantOrderLooseOctTree::GetOctantAndGridPosFromWorld(const gmtl::VecA3f& pos, gmtl::VecA3i& outOctantPos, gmtl::VecA3i& outGridPosInOctant)
{
	// Scale position wrt to TopGridSize
	gmtl::VecA3f gridPos;
	VecScalarMult(&gridPos, &pos, 1.0f / m_TopGridSize);

	// Get the index of the grid pos
	gmtl::VecA3i gridPosFloorInt; 
	gmtl::VecA3f gridPosFloorFloat;
	VecFloor(&gridPosFloorInt, &gridPos);
	VecIntToFloat(&gridPosFloorFloat, &gridPosFloorInt);
    //_DEBUG_ASSERT((gridPos[0] - gridPosFloorFloat[0]) < 1.0f);
    //_DEBUG_ASSERT((gridPos[1] - gridPosFloorFloat[1]) < 1.0f);
    //_DEBUG_ASSERT((gridPos[2] - gridPosFloorFloat[2]) < 1.0f);

	outOctantPos = gridPosFloorInt;

	// Find the octant grid pos within the octant

	// Find the offset within the octant
	gmtl::VecA3f posInOctant;
	VecVecSub(&posInOctant, &gridPos, &gridPosFloorFloat);
	_DEBUG_ASSERT(posInOctant[0] <= 1.0f);
	_DEBUG_ASSERT(posInOctant[1] <= 1.0f);
	_DEBUG_ASSERT(posInOctant[2] <= 1.0f);
   
	// Scale it to grid sized
	VecScalarMult(&posInOctant, &posInOctant, m_OctantNumGridsPerSide);

	// Then get the truncated coordinates
	gmtl::VecA3i posInOctantInt;
	VecFloatToInt(&posInOctantInt, &posInOctant);
    VecVecMin(&posInOctantInt, &posInOctantInt, &gmtl::VecA3i(m_OctantMaxGridPerSide, m_OctantMaxGridPerSide, m_OctantMaxGridPerSide));

	_DEBUG_ASSERT(posInOctantInt[0] >= 0);
	_DEBUG_ASSERT(posInOctantInt[1] >= 0);
	_DEBUG_ASSERT(posInOctantInt[2] >= 0);

    _DEBUG_ASSERT(posInOctantInt[0] <= m_OctantMaxGridPerSide);
    _DEBUG_ASSERT(posInOctantInt[1] <= m_OctantMaxGridPerSide);
    _DEBUG_ASSERT(posInOctantInt[2] <= m_OctantMaxGridPerSide);

	outGridPosInOctant = posInOctantInt;
}

void CSDBOctantOrderLooseOctTree::InsertParentCells(OctantData* pOctant, uint octantOrderHash, uint octantOrdering, uint parentOffset, uint levelOffset)
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

		OctantOrderCell* pParentCell = pOctant->pCellTable->Get(parentCellKey);

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
		pParentCell->firstDataID	= __INVALID;
		pOctant->pCellTable->FinishedAdding();
	}
}

boolean CSDBOctantOrderLooseOctTree::InsertInOctant(OctantData* pOctant, uint octantOrderHash, uint octantOrdering, const AABox* pAABox, const OOBox* pOOBV, uint filterType, void* pData, uint levelOffset)
{
	OctantOrderCellKey toInsertCellKey;
	toInsertCellKey.gridUniqueHash = octantOrderHash;
	
	OctantOrderDataKey toInsertDataKey;
	toInsertDataKey.dataID	= (size_t) pData;
	toInsertDataKey.cellKey = toInsertCellKey;

	// Check to make sure that the same object is not already added (but only checks in the current cell for now)
	_DEBUG_ASSERT(pOctant->pDataTable->Get(toInsertDataKey) == NULL);
	
	OctantOrderData* pToInsertData = pOctant->pDataTable->AddDeferred(toInsertDataKey);
	pToInsertData->filterType	= filterType;
	pToInsertData->pOOBB		= pOOBV;
	pToInsertData->pAABB		= pAABox;
	pToInsertData->prevID		= __INVALID;
	pToInsertData->pData		= pData;

	boolean isNewCellAdded	= FALSE;
	OctantOrderCell* pCell	= pOctant->pCellTable->Get(toInsertCellKey);
	if(pCell)
	{
		// Set the data to point to the current head
		pToInsertData->nextID = pCell->firstDataID;

		// Add the data to the head of the chain of occupants
		if(pCell->firstDataID != __INVALID)
		{
			// Set the current head's prev to the newly inserted data
			OctantOrderDataKey firstDataKey;
			firstDataKey.dataID		= pCell->firstDataID;
			firstDataKey.cellKey	= toInsertCellKey;
			OctantOrderData* pFirstData = pOctant->pDataTable->Get(firstDataKey);
			_DEBUG_ASSERT(pFirstData);
			_DEBUG_ASSERT(pFirstData->prevID == __INVALID);
			pFirstData->prevID = toInsertDataKey.dataID;
		}

		pCell->firstDataID = toInsertDataKey.dataID;
	}
	else
	{
		// Set the data as the only occupant in the chain
		pToInsertData->nextID = __INVALID;

		// Add a new cell

		// This is the offset from the parent and the level offset from the root
		uint parentOrderOffset	= (levelOffset << 3) | (octantOrdering >> ((m_TopLevel - levelOffset) * 3)) & 0x7;

		pCell = pOctant->pCellTable->AddDeferred(toInsertCellKey);
		pCell->children		= (parentOrderOffset << 8);	// 8 bits for children flag, 3 bits + 4 bits for parent order offset and level offset
		pCell->firstDataID	= toInsertDataKey.dataID;
		pOctant->pCellTable->FinishedAdding();

		isNewCellAdded = TRUE;
	}

	// Finish adding data
	pOctant->pDataTable->FinishedAdding();

	return isNewCellAdded;
}

inline size_t CSDBOctantOrderLooseOctTree::AllocHandle()
{
	size_t newHandle;

	if(m_FreeAppHandleHeadID == __INVALID)
	{
		newHandle = m_AppHandles.size(); 
		m_AppHandles.push_back(AppHandle());
	}
	else
	{
		newHandle = m_FreeAppHandleHeadID;
		size_t nextFreeHandle = m_AppHandles[m_FreeAppHandleHeadID].dataID; 
		m_FreeAppHandleHeadID = nextFreeHandle;
	}

	return newHandle;
}

inline void CSDBOctantOrderLooseOctTree::FreeHandle(size_t handle)
{
	m_AppHandles[handle].dataID = m_FreeAppHandleHeadID;
	m_FreeAppHandleHeadID = handle;
}

boolean	CSDBOctantOrderLooseOctTree::UpdateMove(SDBDataHandle handle, const AABox* pNewBounds)
{
	_DEBUG_ASSERT(handle.IsValid());

	// TODO: Do the updates in cache
	// Thread friendly
	
	AppHandle& appHandle = m_AppHandles[(size_t)handle];

	if(appHandle.gridInOctantHash != (uint)__INVALID)
	{
		gmtl::VecA3f center;
		VecVecAdd(&center, &(pNewBounds->max), &(pNewBounds->min));
		VecScalarMult(&center, &center, 0.5f);

		gmtl::VecA3i octantPos;
		gmtl::VecA3i gridPosInOctant;
		GetOctantAndGridPosFromWorld(center, octantPos, gridPosInOctant);

		uint octantHash			= GetOctantHash(octantPos);
		if(octantHash == appHandle.octantPosHash)
		{
            gmtl::VecA3f diff;
            VecVecSub(&diff, &(pNewBounds->max), &(pNewBounds->min));
            float size = diff[0] > diff[1] ? diff[0] : diff[1];
            size = diff[2] > size ? diff[2] : size;

			// Compute ordering within octant
			uint octantOrdering		= GetOctantOrdering(gridPosInOctant);
			uint levelOffset		= FindLevelOffsetFromSize(size);
			_DEBUG_ASSERT(levelOffset <= 0xF);

			// We only want the relevant bits from the level offset
			octantOrdering			= octantOrdering & (0xFFFFFFFF << ((m_TopLevel - levelOffset) * 3));

			// Get the octant ordering hash
			uint octantOrderHash	= GetOctantOrderingHash(octantOrdering, levelOffset);
			if(octantOrderHash == appHandle.gridInOctantHash)
				return FALSE;
		}
	}
	else
	{
        gmtl::VecA3f diff;
        VecVecSub(&diff, &(pNewBounds->max), &(pNewBounds->min));
        float size = diff[0] > diff[1] ? diff[0] : diff[1];
        size = diff[2] > size ? diff[2] : size;

		// Check size
		// If it is greater than the max grid size than it will remain the too large list
		if(size > m_MaxGridSizeForData)
			return FALSE;
	}

	// Will require remove/insert
	return TRUE;
}

SDBDataHandle CSDBOctantOrderLooseOctTree::Insert(const AABox* pAABox, const OOBox* pOOBV, uint filterType, void* pData)
{
    _DEBUG_ASSERT((size_t) pData != __INVALID);

	gmtl::VecA3f diff;
	VecVecSub(&diff, &(pAABox->max), &(pAABox->min));
	float size = diff[0] > diff[1] ? diff[0] : diff[1];
	size = diff[2] > size ? diff[2] : size;

	gmtl::VecA3f center;
	VecVecAdd(&center, &(pAABox->max), &(pAABox->min));
	VecScalarMult(&center, &center, 0.5f);

	// Get free handle
	size_t handleIndex = AllocHandle();
	AppHandle& appHandle = m_AppHandles[handleIndex];

	// Add to an octant if it fits within one
	if(size <= m_MaxGridSizeForData)
	{
		// Find an octant and get the grid position within the octant
		gmtl::VecA3i octantPos;
		gmtl::VecA3i gridPosInOctant;
		GetOctantAndGridPosFromWorld(center, octantPos, gridPosInOctant);

		// Compute octant hash
		uint octantHash			= GetOctantHash(octantPos);
		appHandle.octantPosHash = octantHash;

		// Compute ordering within octant
		uint octantOrdering		= GetOctantOrdering(gridPosInOctant);
		uint levelOffset		= FindLevelOffsetFromSize(size);
		_DEBUG_ASSERT(levelOffset <= 0xF);

		// We only want the relevant bits from the level offset
		octantOrdering			= octantOrdering & (0xFFFFFFFF << ((m_TopLevel - levelOffset) * 3));

		// Get the octant ordering hash
		uint octantOrderHash	= GetOctantOrderingHash(octantOrdering, levelOffset);

		OctantKey octantKey;
		octantKey.octantPos = octantHash;
		OctantData* pOctant = m_Octants.Get(octantKey);
		if(!pOctant)
		{
			// Octant does not exist, so add
			OctantData* pJustAdded = m_Octants.AddDeferred(octantKey);
			if(m_FreeOctantData.size() > 0)
			{
				OctantData& lastFree = m_FreeOctantData.back();
				pJustAdded->pCellTable	= lastFree.pCellTable;
				pJustAdded->pDataTable	= lastFree.pDataTable;
                m_FreeOctantData.pop_back();
			}
			else
			{
				const static float GROWTH_RATE = 0.67f;
				const static uint INIT_SIZE = 32;

				OctantData newAlloc;
				newAlloc.pCellTable = _NEW OctantOrderCellTable();
				newAlloc.pDataTable	= _NEW OctantOrderDataTable();
				newAlloc.pCellTable->Initialize(INIT_SIZE, GROWTH_RATE);
				newAlloc.pDataTable->Initialize(INIT_SIZE, GROWTH_RATE);

				m_AllocatedOctantData.push_back(newAlloc);
				pJustAdded->pCellTable = newAlloc.pCellTable;
				pJustAdded->pDataTable = newAlloc.pDataTable;
			}

			m_Octants.FinishedAdding();
			pOctant = m_Octants.Get(octantKey);

			_DEBUG_ASSERT(pOctant);
		}

		// Set handle for this data
		appHandle.dataID			= (size_t) pData;
		appHandle.gridInOctantHash	= octantOrderHash;

		_DEBUG_ASSERT(appHandle.gridInOctantHash != (uint) __INVALID);

		// Then insert into octant order table
		if(InsertInOctant(pOctant, octantOrderHash, octantOrdering, pAABox, pOOBV, filterType, pData, levelOffset))
		{
			uint parentOffset	= (octantOrdering >> ((m_TopLevel - levelOffset) * 3)) & 0x7;

			// A new cell was added, so we add the parents too if they do not exist
			InsertParentCells(pOctant, octantOrderHash, octantOrdering, parentOffset, levelOffset);
		}

        _DEBUG_ASSERT(pOctant->pCellTable->GetNumOfElements() > 0);
        _DEBUG_ASSERT(pOctant->pDataTable->GetNumOfElements() > 0);
	}
	else
	{
		// Insert to TooLargeList
		appHandle.octantPosHash		= (uint) __INVALID;
		appHandle.gridInOctantHash	= (uint) __INVALID;

		OctantOrderData* pToInsert;
		if(m_FreeTooLargeDataHeadID == __INVALID)
		{
			// Resize the data pool
			appHandle.dataID = m_TooLargeData.size();

			m_TooLargeData.push_back(OctantOrderData());
			pToInsert = &(m_TooLargeData.back());
		}
		else
		{
			// Otherwise get from the free list
			appHandle.dataID = m_FreeTooLargeDataHeadID;

			pToInsert = &(m_TooLargeData[m_FreeTooLargeDataHeadID]);
			m_FreeTooLargeDataHeadID = pToInsert->nextID;

			#ifdef _DEBUG
				_DEBUG_ASSERT(((size_t)pToInsert->pData) == __INVALID);
			#endif
		}

		// Fill data and set list pointer
		pToInsert->filterType	= filterType;
		pToInsert->pAABB		= pAABox;
		pToInsert->pOOBB		= pOOBV;
		pToInsert->pData		= pData;
		pToInsert->nextID		= m_TooLargeDataHeadID;
		pToInsert->prevID		= __INVALID;

		// Set the prev head pointers
		if(m_TooLargeDataHeadID != __INVALID)
		{
			m_TooLargeData[m_TooLargeDataHeadID].prevID = appHandle.dataID;
		}

		// Set the current head pointer
		m_TooLargeDataHeadID = appHandle.dataID;
	}

	return SDBDataHandle(handleIndex);
}

void CSDBOctantOrderLooseOctTree::Remove(SDBDataHandle handle)
{
    _DEBUG_ASSERT(handle.IsValid());

	AppHandle& appHandle = m_AppHandles[(size_t)handle];

	if(appHandle.gridInOctantHash != (uint) __INVALID)
	{
		// Get the octant
		OctantKey octantKey;
		octantKey.octantPos = appHandle.octantPosHash;
		OctantData* pOctant = m_Octants.Get(octantKey);
		_DEBUG_ASSERT(pOctant);

		// Get the cell
		OctantOrderCellKey cellKey;
		cellKey.gridUniqueHash = appHandle.gridInOctantHash;
		OctantOrderCell* pCell = pOctant->pCellTable->Get(cellKey);
		_DEBUG_ASSERT(pCell);

		// Get the data
		OctantOrderDataKey dataKey;
		dataKey.cellKey.gridUniqueHash	= appHandle.gridInOctantHash;
		dataKey.dataID					= appHandle.dataID;

		OctantOrderData* pToRemove = pOctant->pDataTable->Get(dataKey);
		_DEBUG_ASSERT(pToRemove);

		if(pCell->firstDataID == appHandle.dataID)
		{
			// The data to remove is the head of the cell
			_DEBUG_ASSERT(pToRemove->prevID == __INVALID);
			
			// Set the head of the cell to the next dataID
			pCell->firstDataID = pToRemove->nextID;

			if(pToRemove->nextID != __INVALID)
			{
				// Then set the next dataID to invalid if it exists
				OctantOrderDataKey nextDataKey;
				nextDataKey.cellKey.gridUniqueHash	= appHandle.gridInOctantHash;
				nextDataKey.dataID					= pToRemove->nextID;
				OctantOrderData* pNextData = pOctant->pDataTable->Get(nextDataKey);
				_DEBUG_ASSERT(pNextData);

				pNextData->prevID = __INVALID;
				pOctant->pDataTable->Remove(dataKey);		
			}
			else
			{
				pOctant->pDataTable->Remove(dataKey);		

				// If next data does not exist, then remove this cell if it also has no children
				if((pCell->children & 0xFF) == 0)
				{
					uint parentOffset	= (pCell->children >> 8) & 0x7;
					uint levelOffset	= (pCell->children >> (8+3)) & 0xF;
	
					pOctant->pCellTable->Remove(cellKey);

					// Remove parent cells if they have no more children (and data)
					if(RemoveParentCells(pOctant, appHandle.gridInOctantHash, parentOffset, levelOffset))
					{
						// We also free the octant if there are no more cells
						_DEBUG_ASSERT(pOctant->pCellTable->GetNumOfElements() == 0);
						_DEBUG_ASSERT(pOctant->pDataTable->GetNumOfElements() == 0);
                        m_FreeOctantData.push_back(*pOctant);
                        m_Octants.Remove(octantKey);
					}
                    #ifdef _DEBUG
                    else
                    {
                        _DEBUG_ASSERT(pOctant->pCellTable->GetNumOfElements() > 0);
                        _DEBUG_ASSERT(pOctant->pDataTable->GetNumOfElements() > 0);
                    }
                    #endif
				}
			}
		}
		else
		{
			// The data belongs somewhere in the chain of occupants
			_DEBUG_ASSERT(pToRemove->prevID != __INVALID);				
			_DEBUG_ASSERT(pCell->firstDataID != __INVALID);

			// Repoint the prev data to the next one
			OctantOrderDataKey prevKey;
			prevKey.cellKey.gridUniqueHash	= appHandle.gridInOctantHash;
			prevKey.dataID					= pToRemove->prevID;
			OctantOrderData* pPrev = pOctant->pDataTable->Get(prevKey);
			_DEBUG_ASSERT(pPrev);
			_DEBUG_ASSERT(pPrev->nextID == appHandle.dataID);
			pPrev->nextID = pToRemove->nextID;

			if(pToRemove->nextID != __INVALID)
			{
				// Setup the next data if it is valid
				OctantOrderDataKey nextKey;
				nextKey.cellKey.gridUniqueHash	= appHandle.gridInOctantHash;
				nextKey.dataID					= pToRemove->nextID;

				OctantOrderData* pNext = pOctant->pDataTable->Get(nextKey);
				pNext->prevID	= pToRemove->prevID;
			}

			pOctant->pDataTable->Remove(dataKey);
		}
	}
	else
	{
		// Remove from TooLargeList
		#ifdef _DEBUG
			_DEBUG_ASSERT(((size_t)m_TooLargeData[appHandle.dataID].pData) != __INVALID);
		#endif

		// Link prev and next pointers
		uint nextID = m_TooLargeData[appHandle.dataID].nextID;
		uint prevID = m_TooLargeData[appHandle.dataID].prevID;
		if(nextID != __INVALID)
		{
			m_TooLargeData[nextID].prevID = prevID;
		}

		if(prevID != __INVALID)
		{
			m_TooLargeData[prevID].nextID = nextID;
		}
		else
		{
			_DEBUG_ASSERT(appHandle.dataID == m_TooLargeDataHeadID);
			m_TooLargeDataHeadID = nextID;
		}

		// Then set the current data to the head of the free list
		m_TooLargeData[appHandle.dataID].nextID = m_FreeTooLargeDataHeadID;
		m_FreeTooLargeDataHeadID = appHandle.dataID;

		#ifdef _DEBUG
            // For checking later
			m_TooLargeData[appHandle.dataID].pData = (void*) __INVALID;
		#endif
	}

	FreeHandle((size_t) handle);
}

boolean CSDBOctantOrderLooseOctTree::RemoveParentCells(OctantData* pOctant, uint octantOrderHash, uint parentOffset, uint levelOffset)
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

		OctantOrderCell* pParentCell = pOctant->pCellTable->Get(parentCellKey);
		_DEBUG_ASSERT(pParentCell);

		uint childFlag = (1 << parentOffset);

		// Then find the offset relative to its parent
		parentOffset = (pParentCell->children >> 8) & 0x7; //(octantOrdering >> (curLevel * 3)) & 0x7;

		// Reset the parent cell child flag
		uint parentChildren = pParentCell->children & (~childFlag);

		if(((parentChildren & 0xFF) != 0) ||
			(pParentCell->firstDataID != __INVALID))
		{
			// We set the children flags and exit early
			// because all ancestor cells won't be removed
			pParentCell->children = parentChildren;
			return FALSE;
		}

		// If there are no more children, and no data 
		// then remove this cell too
		pOctant->pCellTable->Remove(parentCellKey);
				
		// Compute the level offset of the parent cell for removal
		uint curLevelOffset = levelOffset - i - 1;
		curLevel = m_TopLevel - curLevelOffset;
	}

	// There are now no more cells in the octant
	return TRUE;
}

//==============================================================================
//
//		Query Helpers
//
//==============================================================================
template<CSDBOctantOrderLooseOctTree::QueryOctantFn QueryFn> 
inline uint CSDBOctantOrderLooseOctTree::SearchOctants(QueryContext& context, const AABox& searchAA, uint curIndex)
{
	gmtl::VecA3f offset;
	offset[0] = offset[1] = offset[2] = m_MaxGridSizeForData * 0.5f;

	gmtl::VecA3f minBVOffset;
	VecVecSub(&minBVOffset, &(searchAA.min), &offset);
	gmtl::VecA3f maxBVOffset;
	VecVecAdd(&maxBVOffset, &(searchAA.max), &offset);

	gmtl::VecA3i minOctantPos;
	GetOctantPosFromWorld(minBVOffset, minOctantPos);

	gmtl::VecA3i maxOctantPos;
	GetOctantPosFromWorld(maxBVOffset, maxOctantPos);

	int dx = maxOctantPos[0] - minOctantPos[0];
	int dy = maxOctantPos[1] - minOctantPos[1];
	int dz = maxOctantPos[2] - minOctantPos[2];

	_DEBUG_ASSERT(dx >= 0);
	_DEBUG_ASSERT(dy >= 0);
	_DEBUG_ASSERT(dz >= 0);

	uint octantHash = GetOctantHash(minOctantPos);

	for(int i=0; i <= dy; i++)
	{
		uint dzOctantHash = octantHash;

		for(int j=0; j <= dz; j++)
		{
			uint dxOctantHash = dzOctantHash;

			for(int k=0; k <= dx; k++)
			{
				OctantKey octantKey;
				octantKey.octantPos = dxOctantHash;
				OctantData* pOctant = m_Octants.Get(octantKey);
				if(pOctant)
				{
					// Set the current octant
					context.octant = *pOctant;

					// Derive the current cell pos
					gmtl::VecA3i curOctantPos;
					VecVecAdd(&curOctantPos, &minOctantPos, &gmtl::VecA3i(k, i, j));

					gmtl::VecA3f curOctantPosFloat;
					VecIntToFloat(&curOctantPosFloat, &curOctantPos);
                    VecScalarMult(&curOctantPosFloat, &curOctantPosFloat, m_TopGridSize);

					OctantOrderCellKey cellKey;
					cellKey.gridUniqueHash = 0;
					OctantOrderCell* pCell = pOctant->pCellTable->Get(cellKey);

					// Call query for the octant
					curIndex = (this->*QueryFn)(context, pCell, curOctantPosFloat, m_TopGridSize, 0, m_TopLevel, curIndex);
				}

				dxOctantHash += 1;
			}

			dzOctantHash += (1 << NUM_BITS_XZ);
		}

		octantHash += (1 << (NUM_BITS_XZ + NUM_BITS_XZ));
	}

	return curIndex;
}

template<CSDBOctantOrderLooseOctTree::QueryOctantFn QueryFn> 
inline uint CSDBOctantOrderLooseOctTree::QueryChildren(const QueryContext& context, OctantOrderCell* pCell, const gmtl::VecA3f& cellPos, float gridSize, uint octantOrderHash, uint level, uint curIndex)
{
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
			curIndex = (this->*QueryFn)(context, pChild, childPos, childGridSize, childOctantOrderHash, childLevel, curIndex);
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
			curIndex = (this->*QueryFn)(context, pChild, childPos, childGridSize, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x04)
		{
			// (0, 0, 1)
			uint childOctantOrderHash = octantOrderHash + 2 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			gmtl::VecA3f childPos = cellPos;
			childPos[2] += childGridSize;
			curIndex = (this->*QueryFn)(context, pChild, childPos, childGridSize, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x08)
		{
			// (1, 0, 1)
			uint childOctantOrderHash = octantOrderHash + 3 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			gmtl::VecA3f childPos;
			VecVecAdd(&childPos, &cellPos, &gmtl::VecA3f(childGridSize, 0.0f, childGridSize));
			curIndex = (this->*QueryFn)(context, pChild, childPos, childGridSize, childOctantOrderHash, childLevel, curIndex);
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
			curIndex = (this->*QueryFn)(context, pChild, childPos, childGridSize, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x20)
		{
			// (1, 1, 0)
			uint childOctantOrderHash = octantOrderHash + 5 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			gmtl::VecA3f childPos;
			VecVecAdd(&childPos, &cellPos, &gmtl::VecA3f(childGridSize, childGridSize, 0.0f));
			curIndex = (this->*QueryFn)(context, pChild, childPos, childGridSize, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x40)
		{
			// (0, 1, 1)
			uint childOctantOrderHash = octantOrderHash + 6 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			gmtl::VecA3f childPos;
			VecVecAdd(&childPos, &cellPos, &gmtl::VecA3f(0.0f, childGridSize, childGridSize));
			curIndex = (this->*QueryFn)(context, pChild, childPos, childGridSize, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x80)
		{
			// (1, 1, 1)
			uint childOctantOrderHash = octantOrderHash + 7 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			gmtl::VecA3f childPos;
			VecVecAdd(&childPos, &cellPos, &gmtl::VecA3f(childGridSize, childGridSize, childGridSize));
			curIndex = (this->*QueryFn)(context, pChild, childPos, childGridSize, childOctantOrderHash, childLevel, curIndex);
		}
	}

	return curIndex;
}

template<CSDBOctantOrderLooseOctTree::QueryOctantContainedFn QueryFn> 
inline uint CSDBOctantOrderLooseOctTree::QueryChildrenContained(const QueryContext& context, OctantOrderCell* pCell, uint octantOrderHash, uint level, uint curIndex)
{
	uint childrenFlag = pCell->children & 0xFF;
	if(childrenFlag)
	{
		_DEBUG_ASSERT(level > 0);
		_DEBUG_ASSERT( level == (m_TopLevel - ((pCell->children >> (8+3)) & 0xF)) );

		//float childGridSize = gridSize * 0.5f;
		uint childLevel		= level-1;

		if(childrenFlag & 0x01)
		{
			// (0, 0, 0)
			uint childOctantOrderHash = octantOrderHash + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = (this->*QueryFn)(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x02)
		{
			// (1, 0, 0)
			uint childOctantOrderHash = octantOrderHash + OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = (this->*QueryFn)(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x04)
		{
			// (0, 0, 1)
			uint childOctantOrderHash = octantOrderHash + 2 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = (this->*QueryFn)(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x08)
		{
			// (1, 0, 1)
			uint childOctantOrderHash = octantOrderHash + 3 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = (this->*QueryFn)(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x10)
		{
			// (0, 1, 0)
			uint childOctantOrderHash = octantOrderHash + 4 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = (this->*QueryFn)(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x20)
		{
			// (1, 1, 0)
			uint childOctantOrderHash = octantOrderHash + 5 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = (this->*QueryFn)(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x40)
		{
			// (0, 1, 1)
			uint childOctantOrderHash = octantOrderHash + 6 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = (this->*QueryFn)(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}

		if(childrenFlag & 0x80)
		{
			// (1, 1, 1)
			uint childOctantOrderHash = octantOrderHash + 7 * OCTANT_ORDER_VAL[childLevel] + 1;

			OctantOrderCellKey childKey;
			childKey.gridUniqueHash = childOctantOrderHash;
			OctantOrderCell* pChild = context.octant.pCellTable->Get(childKey);

			curIndex = (this->*QueryFn)(context, pChild, childOctantOrderHash, childLevel, curIndex);
		}
	}

	return curIndex;
}


//==============================================================================
//
//		Queries
//
//==============================================================================
uint CSDBOctantOrderLooseOctTree::Query(void** ppToStore, uint bufferSize, const AABox& queryBV, uint filterType)
{
	// Search oversized
	uint curIndex = 0;
	size_t curHead = m_TooLargeDataHeadID;
	while(curHead != __INVALID)
	{	
		OctantOrderData& tooLargeData = m_TooLargeData[curHead];		
		if(filterType & tooLargeData.filterType)
		{
			if(AABoxInAABox(tooLargeData.pAABB, &queryBV))
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

	return SearchOctants<&CSDBOctantOrderLooseOctTree::QueryOctant>(context, queryBV, curIndex);
}

uint CSDBOctantOrderLooseOctTree::QuerySphere(void** ppToStore, uint bufferSize, const AABox& sphereAA, const Sphere& sphereBV, uint filterType)
{
	// Search oversized
	uint curIndex = 0;
	size_t curHead = m_TooLargeDataHeadID;
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

	return SearchOctants<&CSDBOctantOrderLooseOctTree::QuerySphereOctant>(context, sphereAA, curIndex);
}

uint CSDBOctantOrderLooseOctTree::QueryInFrustumOrtho(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, AABox& frustumAA, const gmtl::MatrixA44f& viewProj, uint filterType)
{
	// Search oversized
	uint curIndex = 0;
	size_t curHead = m_TooLargeDataHeadID;
	while(curHead != __INVALID)
	{	
		OctantOrderData& tooLargeData = m_TooLargeData[curHead];		
		if(filterType & tooLargeData.filterType)
		{
			_DEBUG_ASSERT(curIndex < bufferSize);
			
			AABox* pCurTransformed = pScreenSpaceRes+curIndex;
			AABox temp;

			if(!pScreenSpaceRes)
				pCurTransformed = &temp;

			TransformOOBoxToAABox(&viewProj, tooLargeData.pOOBB, pCurTransformed);
			if(AABoxInAABox(&m_FrustumQueryLimits, pCurTransformed))
			{
				ppToStore[curIndex] = tooLargeData.pData;
				curIndex++;
			}
		}

		curHead = tooLargeData.nextID;
	}

	QueryContext context;
	context.pQueryBV	= &viewProj;
	context.ppToStore	= ppToStore;
	context.bufferSize	= bufferSize;
	context.filterType	= filterType;
	context.pScreenSpaceRes	= pScreenSpaceRes;

	return SearchOctants<&CSDBOctantOrderLooseOctTree::QueryInFrustumOrthoOctant>(context, frustumAA, curIndex);
}

uint CSDBOctantOrderLooseOctTree::QueryInFrustumPersp(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, AABox& frustumAA, const gmtl::MatrixA44f& viewProj, float nearZ, float farZ, uint filterType)
{
	// Search oversized
	uint curIndex = 0;
	size_t curHead = m_TooLargeDataHeadID;
	while(curHead != __INVALID)
	{	
		OctantOrderData& tooLargeData = m_TooLargeData[curHead];		
		if(filterType & tooLargeData.filterType)
		{
			_DEBUG_ASSERT(curIndex < bufferSize);
			
			AABox* pCurTransformed = pScreenSpaceRes+curIndex;
			AABox temp;

			if(!pScreenSpaceRes)
				pCurTransformed = &temp;

			//if(AABoxInAABox(pData->pBV, &frustumAA))
			{
				TransformAndProjectOOBoxToAABox(&viewProj, nearZ, farZ, tooLargeData.pOOBB, pCurTransformed);
				if(AABoxInAABox(&m_FrustumQueryLimits, pCurTransformed))
				{
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
	context.pScreenSpaceRes	= pScreenSpaceRes;
	context.nearZ		= nearZ;
	context.farZ		= farZ;

	return SearchOctants<&CSDBOctantOrderLooseOctTree::QueryInFrustumPerspOctant>(context, frustumAA, curIndex);
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

	return SearchOctants<&CSDBOctantOrderLooseOctTree::QueryCellInfoOctant>(context, queryBV, curIndex);
}


//==============================================================================
//
//		Octant Queries
//
//==============================================================================

//------------------------------------------------------------------------------
//	AABOX
//
//------------------------------------------------------------------------------
uint CSDBOctantOrderLooseOctTree::QueryOctantContained(const QueryContext& context, OctantOrderCell* pCell, uint octantOrderHash, uint level, uint curIndex)
{
	_DEBUG_ASSERT(pCell);

	size_t dataID = pCell->firstDataID;
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

	return QueryChildrenContained<&CSDBOctantOrderLooseOctTree::QueryOctantContained>(context, pCell, octantOrderHash, level, curIndex);
}

uint CSDBOctantOrderLooseOctTree::QueryOctant(const QueryContext& context, OctantOrderCell* pCell, const gmtl::VecA3f& cellPos, float gridSize, uint octantOrderHash, uint level, uint curIndex)
{
	_DEBUG_ASSERT(pCell);

	const AABox* pQueryBV = (const AABox*) context.pQueryBV;

	// Set loose bounds
	float looseOffset = gmtl::Math::Min(m_MaxGridSizeForData, gridSize) * 0.5f;

	AABox cellBounds;
	VecScalarAdd((gmtl::VecA4f*) &cellBounds.min, (const gmtl::VecA4f*) &cellPos, -looseOffset);
	VecScalarAdd((gmtl::VecA4f*) &cellBounds.max, (const gmtl::VecA4f*) &cellPos, looseOffset + gridSize);

	if(AABoxInAABox(pQueryBV, &cellBounds))
	{
		if(AABoxContainsAABox(pQueryBV, &cellBounds))
		{
			return QueryOctantContained(context, pCell, octantOrderHash, level, curIndex);
		}

		size_t dataID = pCell->firstDataID;
		while(dataID != __INVALID)
		{
			OctantOrderDataKey dataKey;
			dataKey.cellKey.gridUniqueHash	= octantOrderHash;
			dataKey.dataID					= dataID;
			OctantOrderData* pData = context.octant.pDataTable->Get(dataKey);
			_DEBUG_ASSERT(pData);

			if(pData->filterType & context.filterType)
			{
				if(AABoxInAABox(pQueryBV, pData->pAABB))
				{
					_DEBUG_ASSERT(curIndex < context.bufferSize);
					context.ppToStore[curIndex] = pData->pData;
					curIndex++;
				}
			}

			dataID = pData->nextID;
		}

		curIndex = QueryChildren<&CSDBOctantOrderLooseOctTree::QueryOctant>(context, pCell, cellPos, gridSize, octantOrderHash, level, curIndex);
	}

	return curIndex;
}

//------------------------------------------------------------------------------
//	Sphere
//
//------------------------------------------------------------------------------
uint CSDBOctantOrderLooseOctTree::QuerySphereOctant(const QueryContext& context, OctantOrderCell* pCell, const gmtl::VecA3f& cellPos, float gridSize, uint octantOrderHash, uint level, uint curIndex)
{
	_DEBUG_ASSERT(pCell);

	const Sphere* pQueryBV = (const Sphere*) context.pQueryBV;

	// Set loose bounds
	float looseOffset = gmtl::Math::Min(m_MaxGridSizeForData, gridSize) * 0.5f;

	AABox cellBounds;
	VecScalarAdd((gmtl::VecA4f*) &cellBounds.min, (const gmtl::VecA4f*) &cellPos, -looseOffset);
	VecScalarAdd((gmtl::VecA4f*) &cellBounds.max, (const gmtl::VecA4f*) &cellPos, looseOffset + gridSize);

	if(SphereInAABox(pQueryBV, &cellBounds))
	{
		// TODO: We probably want a special case where we do not do these containment tests
		float sphereRadius	= pQueryBV->center[3];
		float looseGridSize = gridSize + 2.0f * looseOffset;
		if(sphereRadius >= (looseGridSize * 1.4142136f) && SphereContainsAABox(pQueryBV, &cellBounds))
		{
			return QueryOctantContained(context, pCell, octantOrderHash, level, curIndex);
		}

		size_t dataID = pCell->firstDataID;
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

		curIndex = QueryChildren<&CSDBOctantOrderLooseOctTree::QuerySphereOctant>(context, pCell, cellPos, gridSize, octantOrderHash, level, curIndex);
	}

	return curIndex;
}

//------------------------------------------------------------------------------
//	Frustum Ortho
//
//------------------------------------------------------------------------------
uint CSDBOctantOrderLooseOctTree::QueryInFrustumOrthoOctantContained(const QueryContext& context, OctantOrderCell* pCell, uint octantOrderHash, uint level, uint curIndex)
{
	_DEBUG_ASSERT(pCell);

	size_t dataID = pCell->firstDataID;
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
            const gmtl::MatrixA44f* pQueryProj = (const gmtl::MatrixA44f*) context.pQueryBV;
            TransformOOBoxToAABox(pQueryProj, pData->pOOBB, context.pScreenSpaceRes+curIndex);

            curIndex++;			
		}

		dataID = pData->nextID;
	}

	return QueryChildrenContained<&CSDBOctantOrderLooseOctTree::QueryInFrustumOrthoOctantContained>(context, pCell, octantOrderHash, level, curIndex);
}

uint CSDBOctantOrderLooseOctTree::QueryInFrustumOrthoOctant(const QueryContext& context, OctantOrderCell* pCell, const gmtl::VecA3f& cellPos, float gridSize, uint octantOrderHash, uint level, uint curIndex)
{
	_DEBUG_ASSERT(pCell);

	const gmtl::MatrixA44f* pQueryProj = (const gmtl::MatrixA44f*) context.pQueryBV;

	// Set loose bounds
	float looseOffset = gmtl::Math::Min(m_MaxGridSizeForData, gridSize) * 0.5f;

	AABox cellBounds;
	VecScalarAdd((gmtl::VecA4f*) &cellBounds.min, (const gmtl::VecA4f*) &cellPos, -looseOffset);
	VecScalarAdd((gmtl::VecA4f*) &cellBounds.max, (const gmtl::VecA4f*) &cellPos, looseOffset + gridSize);

	AABox transformedAA;
	TransformAABoxToAABox(pQueryProj, &cellBounds, &transformedAA);

	if(AABoxInAABox(&m_FrustumQueryLimits, &transformedAA))
	{
		if(AABoxContainsAABox(&m_FrustumQueryLimits, &transformedAA))
		{
            if(context.pScreenSpaceRes)
			    return QueryInFrustumOrthoOctantContained(context, pCell, octantOrderHash, level, curIndex);
               
            return QueryOctantContained(context, pCell, octantOrderHash, level, curIndex);
		}

		size_t dataID = pCell->firstDataID;
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

                AABox* pTransformedAA = context.pScreenSpaceRes+curIndex;
                if(!context.pScreenSpaceRes)
                    pTransformedAA = &transformedAA;

				TransformOOBoxToAABox(pQueryProj, pData->pOOBB, pTransformedAA);
				if(AABoxInAABox(&m_FrustumQueryLimits, pTransformedAA))
				{
					context.ppToStore[curIndex] = pData->pData;
					curIndex++;
				}
			}

			dataID = pData->nextID;
		}

		curIndex = QueryChildren<&CSDBOctantOrderLooseOctTree::QueryInFrustumOrthoOctant>(context, pCell, cellPos, gridSize, octantOrderHash, level, curIndex);
	}

	return curIndex;
}

//------------------------------------------------------------------------------
//	Frustum Perspective
//
//------------------------------------------------------------------------------
uint CSDBOctantOrderLooseOctTree::QueryInFrustumPerspOctantContained(const QueryContext& context, OctantOrderCell* pCell, uint octantOrderHash, uint level, uint curIndex)
{
	_DEBUG_ASSERT(pCell);

	size_t dataID = pCell->firstDataID;
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

            const gmtl::MatrixA44f* pQueryProj = (const gmtl::MatrixA44f*) context.pQueryBV;
            TransformAndProjectOOBoxToAABox(pQueryProj, context.nearZ, context.farZ, pData->pOOBB, context.pScreenSpaceRes+curIndex);
            
            curIndex++;			
		}

		dataID = pData->nextID;
	}

	return QueryChildrenContained<&CSDBOctantOrderLooseOctTree::QueryInFrustumPerspOctantContained>(context, pCell, octantOrderHash, level, curIndex);
}

uint CSDBOctantOrderLooseOctTree::QueryInFrustumPerspOctant(const QueryContext& context, OctantOrderCell* pCell, const gmtl::VecA3f& cellPos, float gridSize, uint octantOrderHash, uint level, uint curIndex)
{
	_DEBUG_ASSERT(pCell);

	const gmtl::MatrixA44f* pQueryProj = (const gmtl::MatrixA44f*) context.pQueryBV;

	// Set loose bounds
	float looseOffset = gmtl::Math::Min(m_MaxGridSizeForData, gridSize) * 0.5f;

	AABox cellBounds;
	VecScalarAdd((gmtl::VecA4f*) &cellBounds.min, (const gmtl::VecA4f*) &cellPos, -looseOffset);
	VecScalarAdd((gmtl::VecA4f*) &cellBounds.max, (const gmtl::VecA4f*) &cellPos, looseOffset + gridSize);

	AABox transformedAA;
	TransformAndProjectAABoxToAABox(pQueryProj, context.nearZ, context.farZ, &cellBounds, &transformedAA);

	if(AABoxInAABox(&m_FrustumQueryLimits, &transformedAA))
	{
		if(AABoxContainsAABox(&m_FrustumQueryLimits, &transformedAA))
		{
			if(context.pScreenSpaceRes)
                return QueryInFrustumPerspOctantContained(context, pCell, octantOrderHash, level, curIndex);

            return QueryOctantContained(context, pCell, octantOrderHash, level, curIndex);
		}

		size_t dataID = pCell->firstDataID;
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

                AABox* pTransformedAA = context.pScreenSpaceRes+curIndex;
                if(!context.pScreenSpaceRes)
                    pTransformedAA = &transformedAA;

                TransformAndProjectOOBoxToAABox(pQueryProj, context.nearZ, context.farZ, pData->pOOBB, pTransformedAA);
                if(AABoxInAABox(&m_FrustumQueryLimits, pTransformedAA))
                {
                    context.ppToStore[curIndex] = pData->pData;
                    curIndex++;
                }
			}

			dataID = pData->nextID;
		}

		curIndex = QueryChildren<&CSDBOctantOrderLooseOctTree::QueryInFrustumPerspOctant>(context, pCell, cellPos, gridSize, octantOrderHash, level, curIndex);
	}

	return curIndex;
}

//------------------------------------------------------------------------------
//	Cell Info
//
//------------------------------------------------------------------------------
uint CSDBOctantOrderLooseOctTree::QueryCellInfoOctantContained(const QueryContext& context, OctantOrderCell* pCell, const gmtl::VecA3f& cellPos, float gridSize, uint octantOrderHash, uint level, uint curIndex)
{
	_DEBUG_ASSERT(pCell);

	AABox cellBounds;
	cellBounds.min = cellPos;
	VecScalarAdd((gmtl::VecA4f*) &cellBounds.max, (const gmtl::VecA4f*) &cellPos, gridSize);

	_DEBUG_ASSERT(curIndex < context.bufferSize);
	context.pCellInfo[curIndex]	= cellBounds;		
	curIndex++;

	return QueryChildren<&CSDBOctantOrderLooseOctTree::QueryCellInfoOctantContained>(context, pCell, cellPos, gridSize, octantOrderHash, level, curIndex);
}

uint CSDBOctantOrderLooseOctTree::QueryCellInfoOctant(const QueryContext& context, OctantOrderCell* pCell, const gmtl::VecA3f& cellPos, float gridSize, uint octantOrderHash, uint level, uint curIndex)
{
	_DEBUG_ASSERT(pCell);

	const AABox* pQueryBV = (const AABox*) context.pQueryBV;

	AABox cellBounds;
	cellBounds.min = cellPos;
	VecScalarAdd((gmtl::VecA4f*) &cellBounds.max, (const gmtl::VecA4f*) &cellPos, gridSize);

	_DEBUG_ASSERT(curIndex < context.bufferSize);
	context.pCellInfo[curIndex]	= cellBounds;		
	curIndex++;

	if(AABoxInAABox(pQueryBV, &cellBounds))
	{
		if(AABoxContainsAABox(pQueryBV, &cellBounds))
		{
			return QueryCellInfoOctantContained(context, pCell, cellPos, gridSize, octantOrderHash, level, curIndex);
		}

		curIndex = QueryChildren<&CSDBOctantOrderLooseOctTree::QueryCellInfoOctant>(context, pCell, cellPos, gridSize, octantOrderHash, level, curIndex);
	}

	return curIndex;
}

_NAMESPACE_END