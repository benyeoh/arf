//==============================================================================
//
//		GridHash.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		12/8/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

inline void MurmurHash2Pos(SDBGridPos& gridPos)
	//const void* key, int len, unsigned int seed )
{
	// 'm' and 'r' are mixing constants generated offline.
	// They're not really 'magic', they just happen to work well.

	const uint m = 0x5bd1e995;
	const int r = 24;

	// Initialize the hash to a 'random' value
	uint h = 1234567 ^ 12;

	// Mix 4 bytes at a time into the hash
	//const unsigned char * data = (const unsigned char *)key;

	//_LOOPi(3)
	{
		uint k = gridPos.pos[0]; // *(unsigned int *)data;
		k *= m; 
		k ^= k >> r; 
		k *= m; 

		h *= m; 
		h ^= k;

		k = gridPos.pos[1];
		k *= m; 
		k ^= k >> r; 
		k *= m; 

		h *= m; 
		h ^= k;

		k = gridPos.pos[2];		
		k *= m; 
		k ^= k >> r; 
		k *= m; 

		h *= m; 
		h ^= k;
		//data += 4;
		//len -= 4;
	}

	// Handle the last few bytes of the input array

	//switch(len)
	//{
	//case 3: h ^= data[2] << 16;
	//case 2: h ^= data[1] << 8;
	//case 1: h ^= data[0];
	//        h *= m;
	//};

	// Do a few final mixes of the hash to ensure the last few
	// bytes are well-incorporated.

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	gridPos.posHash = h;
	//return h;
} 

inline void MurmurHash2SaveVal(uint& res, uint val)
{
	// 'm' and 'r' are mixing constants generated offline.
	// They're not really 'magic', they just happen to work well.

	const uint m = 0x5bd1e995;
	const int r = 24;

	// Initialize the hash to a 'random' value
	uint h = 1234567 ^ 12;

	uint k = val;
	k *= m; 
	k ^= k >> r; 
	k *= m; 

	res = k;
} 

inline void MurmurHash2LoadGridPos(SDBGridPos& gridPos, uint x, uint y, uint z)
{
	// 'm' and 'r' are mixing constants generated offline.
	// They're not really 'magic', they just happen to work well.

	const uint m = 0x5bd1e995;
	const int r = 24;

	// Initialize the hash to a 'random' value
	uint h = 1234567 ^ 12;

	{
		h *= m; 
		h ^= x;

		h *= m; 
		h ^= y;

		h *= m; 
		h ^= z;
	}

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	gridPos.posHash = h;
} 

inline void MurmurHash3Pos(SDBGridPos& gridPos)
{
	uint h1 = 1234567 ^ 12;//0x5bd1e995;

	const uint c1 = 0xcc9e2d51;
	const uint c2 = 0x1b873593;

	{
		uint k1 = gridPos.pos[0];
		k1 *= c1; 
		k1 = __rot(k1, 15); 
		k1 *= c2; 

		h1 ^= k1;
		h1 = __rot(h1, 13);
		h1 = h1 * 5 + 0xe6546b64;

		k1 = gridPos.pos[1];
		k1 *= c1; 
		k1 = __rot(k1, 15); 
		k1 *= c2; 

		h1 ^= k1;
		h1 = __rot(h1, 13);
		h1 = h1 * 5 + 0xe6546b64;

		k1 = gridPos.pos[2];
		k1 *= c1; 
		k1 = __rot(k1, 15); 
		k1 *= c2; 

		h1 ^= k1;
		h1 = __rot(h1, 13);
		h1 = h1 * 5 + 0xe6546b64;
	}

	h1 ^= (3 * sizeof(uint));

	h1 ^= h1 >> 16;
	h1 *= 0x85ebca6b;
	h1 ^= h1 >> 13;
	h1 *= 0xc2b2ae35;
	h1 ^= h1 >> 16;

	gridPos.posHash = h1;
} 

inline void MurmurHash3SaveVal(uint& res, uint val)
{
	const uint c1 = 0xcc9e2d51;
	const uint c2 = 0x1b873593;

	uint k1 = val;
	
	k1 *= c1; 
	k1 = __rot(k1, 15); 
	k1 *= c2; 

	res = k1;
} 

inline void MurmurHash3LoadGridPos(SDBGridPos& gridPos, uint x, uint y, uint z)
{
	uint h1 = 1234567 ^ 12;//0x5bd1e995;

	const uint c1 = 0xcc9e2d51;
	const uint c2 = 0x1b873593;

	// Initialize the hash to a 'random' value
	uint h = 1234567 ^ 12;

	{
		h1 ^= x;
		h1 = __rot(h1, 13);
		h1 = h1 * 5 + 0xe6546b64;

		h1 ^= y;
		h1 = __rot(h1, 13);
		h1 = h1 * 5 + 0xe6546b64;

		h1 ^= z;
		h1 = __rot(h1, 13);
		h1 = h1 * 5 + 0xe6546b64;
	}

	h1 ^= (3 * sizeof(uint));

	h1 ^= h1 >> 16;
	h1 *= 0x85ebca6b;
	h1 ^= h1 >> 13;
	h1 *= 0xc2b2ae35;
	h1 ^= h1 >> 16;

	gridPos.posHash = h1;
} 

#define MurmurHashPos MurmurHash3Pos
#define MurmurHashSaveVal MurmurHash3SaveVal
#define MurmurHashLoadGridPos MurmurHash3LoadGridPos

_NAMESPACE_END