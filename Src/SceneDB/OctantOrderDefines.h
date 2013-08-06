//==============================================================================
//
//		OctantOrderDefines.h
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Monday, 24 June, 2013
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

const static size_t __INVALID = (size_t) (-1);
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

// Octant pos
const int NUM_BITS_XZ			= 12;
const int NUM_BITS_Y			= 32 - (NUM_BITS_XZ + NUM_BITS_XZ);
const int EFFECTIVE_RANGE_XZ	= 1 << (NUM_BITS_XZ - 1);
const int EFFECTIVE_RANGE_Y		= 1 << (NUM_BITS_Y - 1);

// Maximum number of data that can share a cell (without hash collision)
// Must be prime number
const static uint MAX_DATA_PER_CELL = 5; 

_NAMESPACE_END