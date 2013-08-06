//================================================================================
//
//		TriangleSetup.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		20/11/2011
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

//template <uint numInterpolants, uint numTileLevels>
//struct RasterizationInfo
//{
//	// Edge values
//	// These should be clamped
//	__m128 e1aaaa;	
//	__m128 e2aaaa;
//	__m128 e3aaaa;
//
//	__m128 e1bbbb;
//	__m128 e2bbbb;
//	__m128 e3bbbb;
//
//	__m128 e1cccc;	// These should include offset for top left fill
//	__m128 e2cccc;
//	__m128 e3cccc;
//
//	// Block trivial reject offset
//	__m128 e1RejectCCCC[numTileLevels];
//	__m128 e2RejectCCCC[numTileLevels];
//	__m128 e3RejectCCCC[numTileLevels];
//
//	// Block trivial accept offset
//	__m128 e1AcceptCCCC[numTileLevels];	
//	__m128 e2AcceptCCCC[numTileLevels];
//	__m128 e3AcceptCCCC[numTileLevels];
//
//	// depth values
//	__m128 zAAAA;	
//	__m128 zBBBB;
//	__m128 zCCCC;
//
//	// inv W values
//	__m128 invWaaaa;	
//	__m128 invWbbbb;
//	__m128 invWcccc;
//
//	// All interpolants
//	__m128 interAAAA[numInterpolants]; 
//	__m128 interBBBB[numInterpolants];
//	__m128 interCCCC[numInterpolants];
//};

template <uint numInterpolants, uint bufferWidth, uint bufferHeight, uint numTileLevels>
boolean TriangleSetup(const gmtl::VecA4f& v1, const gmtl::VecA4f& v2, const gmtl::VecA4f& v3,
				      const float* pInterpolantsV1, const float* pInterpolantsV2, const float* pInterpolantsV3,
					  RasterizationInfo<numInterpolants, numTileLevels>& raster);

_NAMESPACE_END