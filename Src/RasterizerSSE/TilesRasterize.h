//================================================================================
//
//		TilesRasterize.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		17/1/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

#define __TRAS_INTERPOLATE1 \
	{ \
		__m128 w = _mm_rcp_ps(invW); \
		finalInter[0] = _mm_mul_ps(inter[0], w); \
	}

#define __TRAS_INTERPOLATE2 \
	{ \
		__m128 w = _mm_rcp_ps(invW); \
		finalInter[0] = _mm_mul_ps(inter[0], w); \
		finalInter[1] = _mm_mul_ps(inter[1], w); \
	} \

#define __TRAS_INTERPOLATE \
	 \
	if(numInterpolants) \
	{ \
		__m128 w = _mm_rcp_ps(invW); \
	 \
		_LOOPk(numInterpolants) \
		{ \
			finalInter[k] = _mm_mul_ps(inter[k], w); \
		} \
	}

#define __TRAS_INC_X \
	{ \
		pixelBlockX += 2; \
	}

#define __TRAS_INC_Y \
	{ \
		pixelBlockY += 2; \
	}

#define __TRAS_DEC_X \
	{ \
		pixelBlockX -= 2; \
	}

#define __TRAS_EC_INC_X \
	{ \
		e1Reject = _mm_add_ps(e1Reject, pRaster->e1XFinalAdd); \
		e2Reject = _mm_add_ps(e2Reject, pRaster->e2XFinalAdd); \
		e3Reject = _mm_add_ps(e3Reject, pRaster->e3XFinalAdd); \
		pixelBlockX += 2; \
	}

#define __TRAS_EC_INC_Y \
	{ \
		e1Reject = _mm_add_ps(e1Reject, pRaster->e1YFinalAdd); \
		e2Reject = _mm_add_ps(e2Reject, pRaster->e2YFinalAdd); \
		e3Reject = _mm_add_ps(e3Reject, pRaster->e3YFinalAdd); \
		pixelBlockY += 2; \
	}

#define __TRAS_EC_DEC_X \
	{ \
		e1Reject = _mm_sub_ps(e1Reject, pRaster->e1XFinalAdd); \
		e2Reject = _mm_sub_ps(e2Reject, pRaster->e2XFinalAdd); \
		e3Reject = _mm_sub_ps(e3Reject, pRaster->e3XFinalAdd); \
		pixelBlockX -= 2; \
	}

#define __TRAS_INTER_INC_X \
	{ \
		depth = _mm_add_ps(depth, pRaster->depthXFinalAdd); \
		 \
		if(numInterpolants) \
		{ \
			invW = _mm_add_ps(invW, pRaster->invWXFinalAdd); \
			__m128 w = _mm_rcp_ps(invW); \
		 \
			_LOOPk(numInterpolants) \
			{ \
				inter[k] = _mm_add_ps(inter[k], pRaster->edgeInterpolantsXFinalAdd[k]); \
				finalInter[k] = _mm_mul_ps(inter[k], w); \
			} \
		} \
	}

#define __TRAS_INTER1_INC_X \
	{ \
		\
		depth = _mm_add_ps(depth, pRaster->depthXFinalAdd); \
		\
		invW = _mm_add_ps(invW, pRaster->invWXFinalAdd); \
		__m128 w = _mm_rcp_ps(invW); \
		inter[0] = _mm_add_ps(inter[0], pRaster->edgeInterpolantsXFinalAdd[0]); \
		finalInter[0] = _mm_mul_ps(inter[0], w); \
	} \

#define __TRAS_INTER2_INC_X \
	{ \
		depth = _mm_add_ps(depth, pRaster->depthXFinalAdd); \
		\
		invW = _mm_add_ps(invW, pRaster->invWXFinalAdd); \
		__m128 w = _mm_rcp_ps(invW); \
		inter[0] = _mm_add_ps(inter[0], pRaster->edgeInterpolantsXFinalAdd[0]); \
		inter[1] = _mm_add_ps(inter[1], pRaster->edgeInterpolantsXFinalAdd[1]); \
		finalInter[0] = _mm_mul_ps(inter[0], w); \
		finalInter[1] = _mm_mul_ps(inter[1], w); \
	} \


#define __TRAS_INTER_INC_Y \
	{ \
		depth = _mm_add_ps(depth, pRaster->depthYFinalAdd); \
	 \
		if(numInterpolants) \
		{ \
			invW = _mm_add_ps(invW, pRaster->invWYFinalAdd); \
			__m128 w = _mm_rcp_ps(invW); \
	 \
			_LOOPk(numInterpolants) \
			{ \
				inter[k] = _mm_add_ps(inter[k], pRaster->edgeInterpolantsYFinalAdd[k]); \
				finalInter[k] = _mm_mul_ps(inter[k], w); \
			} \
		} \
	}

#define __TRAS_INTER1_INC_Y \
	{ \
		depth = _mm_add_ps(depth, pRaster->depthYFinalAdd); \
		\
		invW = _mm_add_ps(invW, pRaster->invWYFinalAdd); \
		__m128 w = _mm_rcp_ps(invW); \
		\
		inter[0] = _mm_add_ps(inter[0], pRaster->edgeInterpolantsYFinalAdd[0]); \
		finalInter[0] = _mm_mul_ps(inter[0], w); \
	}

#define __TRAS_INTER2_INC_Y \
	{ \
		depth = _mm_add_ps(depth, pRaster->depthYFinalAdd); \
		\
		invW = _mm_add_ps(invW, pRaster->invWYFinalAdd); \
		__m128 w = _mm_rcp_ps(invW); \
		\
		inter[0] = _mm_add_ps(inter[0], pRaster->edgeInterpolantsYFinalAdd[0]); \
		inter[1] = _mm_add_ps(inter[1], pRaster->edgeInterpolantsYFinalAdd[1]); \
		finalInter[0] = _mm_mul_ps(inter[0], w); \
		finalInter[1] = _mm_mul_ps(inter[1], w); \
	}

#define __TRAS_INTER_DEC_X \
	{ \
		depth = _mm_sub_ps(depth, pRaster->depthXFinalAdd); \
	 \
		if(numInterpolants) \
		{ \
			invW = _mm_sub_ps(invW, pRaster->invWXFinalAdd); \
			__m128 w = _mm_rcp_ps(invW); \
	 \
			_LOOPk(numInterpolants) \
			{ \
				inter[k] = _mm_sub_ps(inter[k], pRaster->edgeInterpolantsXFinalAdd[k]); \
				finalInter[k] = _mm_mul_ps(inter[k], w); \
			} \
		} \
	}

#define __TRAS_INTER1_DEC_X \
	{ \
		depth = _mm_sub_ps(depth, pRaster->depthXFinalAdd); \
		\
		invW = _mm_sub_ps(invW, pRaster->invWXFinalAdd); \
		__m128 w = _mm_rcp_ps(invW); \
		\
		inter[0] = _mm_sub_ps(inter[0], pRaster->edgeInterpolantsXFinalAdd[0]); \
		finalInter[0] = _mm_mul_ps(inter[0], w); \
	}

#define __TRAS_INTER2_DEC_X \
	{ \
		depth = _mm_sub_ps(depth, pRaster->depthXFinalAdd); \
		\
		invW = _mm_sub_ps(invW, pRaster->invWXFinalAdd); \
		__m128 w = _mm_rcp_ps(invW); \
		\
		inter[0] = _mm_sub_ps(inter[0], pRaster->edgeInterpolantsXFinalAdd[0]); \
		inter[1] = _mm_sub_ps(inter[1], pRaster->edgeInterpolantsXFinalAdd[1]); \
		finalInter[0] = _mm_mul_ps(inter[0], w); \
		finalInter[1] = _mm_mul_ps(inter[1], w); \
	}

//#define __TRAS_EC_PIXEL \
//	{ \
//		__m128 insideTest = _mm_and_ps( _mm_cmpgt_ps(e1Reject, zero), _mm_cmpgt_ps(e2Reject, zero) ); \
//		insideTest = _mm_and_ps( insideTest, _mm_cmpgt_ps(e3Reject, zero) ); \
//		\
//		pixelShaderFn(pixelBlockX, pixelBlockY, insideTest, depth, finalInter, pContext); \
//	} \

#define __TRAS_EC_PIXEL \
	{ \
		__m128 insideTest = _mm_or_ps( _mm_or_ps( e1Reject, e2Reject ), e3Reject ); \
        insideTest = *((__m128*) &_mm_cmpgt_epi32( *((__m128i*)&insideTest), *((__m128i*) &zero) )); \
        \
        pixelShaderFn(pixelBlockX, pixelBlockY, insideTest, depth, finalInter, pContext); \
	} \

#define __TRAS_FULL_MASK_PIXEL \
	{ \
		pixelShaderFn(pixelBlockX, pixelBlockY, fullMask, depth, finalInter, pContext); \
	}

#define __TRAS_INTER_PREP \
	\
	__m128 depthX = _mm_mul_ps(xxxx, pRaster->depthAAAA); \
	__m128 depthY = _mm_mul_ps(yyyy, pRaster->depthBBBB); \
	__m128 depth = _mm_add_ps(depthX, depthY); \
	depth = _mm_add_ps( depth, pRaster->depthCCCC ); \
	\
	__m128 invW; \
	__m128 inter[numInterpolants > 0 ? numInterpolants : 1]; \
	__m128 finalInter[numInterpolants > 0 ? numInterpolants : 1]; \
	if(numInterpolants) \
	{ \
		__m128 invWx = _mm_mul_ps(xxxx, pRaster->invWAAAA); \
		__m128 invWy = _mm_mul_ps(yyyy, pRaster->invWBBBB); \
		invW = _mm_add_ps(invWx, invWy); \
		invW = _mm_add_ps( invW, pRaster->invWCCCC ); \
		\
		_LOOPk(numInterpolants) \
		{ \
		__m128 interX = _mm_mul_ps(xxxx, pRaster->edgeInterpolantsAAAA[k]); \
		__m128 interY = _mm_mul_ps(yyyy, pRaster->edgeInterpolantsBBBB[k]); \
		inter[k] = _mm_add_ps(interX, interY); \
		inter[k] = _mm_add_ps( inter[k], pRaster->edgeInterpolantsCCCC[k]); \
		} \
	}

#define __TRAS_PIXEL_PREP \
	__m128 xxxx = _mm_load_ps1(&curX); \
	int pixelBlockX = _mm_cvtt_ss2si(xxxx); \
	xxxx = _mm_add_ps(xxxx, _mm_load_ps(QUAD_IN_TILE_OFFSETS_X)); \
	\
	__m128 yyyy = _mm_load_ps1(&curY); \
	int pixelBlockY = _mm_cvtt_ss2si(yyyy); \
	yyyy = _mm_add_ps(yyyy, _mm_load_ps(QUAD_IN_TILE_OFFSETS_Y)); \

#define __TRAS_FULL_PREP \
	__m128 fullMask = _mm_setzero_ps(); \
	fullMask = _mm_cmpeq_ps(fullMask, fullMask);

#define __TRAS_EC_PREP \
	 \
	 __m128 zero = _mm_setzero_ps(); \
	 \
	__m128 e1 = _mm_mul_ps(pRaster->e1aaaa, xxxx); \
	e1 = _mm_add_ps( _mm_mul_ps(pRaster->e1bbbb, yyyy), e1 ); \
	__m128 e1Reject = _mm_add_ps( e1, pRaster->e1cccc ); \
	 \
	__m128 e2 = _mm_mul_ps(pRaster->e2aaaa, xxxx); \
	e2 = _mm_add_ps( _mm_mul_ps(pRaster->e2bbbb, yyyy), e2 ); \
	__m128 e2Reject = _mm_add_ps( e2, pRaster->e2cccc ); \
	 \
	__m128 e3 = _mm_mul_ps(pRaster->e3aaaa, xxxx); \
	e3 = _mm_add_ps( _mm_mul_ps(pRaster->e3bbbb, yyyy), e3 ); \
	__m128 e3Reject = _mm_add_ps( e3, pRaster->e3cccc ); \

typedef void (*CustomPixelShaderFn)(int pixelBlockX, int pixelBlockY, __m128 coverage, __m128 depthVals, __m128* interVals, void* pContext);

template<uint numInterpolants, uint numLayers, uint inTileSize, CustomPixelShaderFn pixelShaderFn>
_RASTERIZER_SSE_INLINE void TileRasterizeEdgeCheck(const void* pRasterInfo, float curX, float curY, void* pContext)
{
	const RasterInfo<numInterpolants, numLayers>* __restrict pRaster = ((RasterInfo<numInterpolants, numLayers>*) pRasterInfo);

	const uint OUT_TILE_SIZE = 1;
	const float OUT_TILE_SIZE_F = (float) OUT_TILE_SIZE;
	const float OUT_TILE_SIZE_2X_F = OUT_TILE_SIZE_F * 2.0f;
	//const float IN_TILE_X_F = (float) inTileSize - 1.0f;
	//const float IN_TILE_Y_F = (float) inTileSize - 1.0f;

	const uint NUM_OUT_TILE_XY = inTileSize / (OUT_TILE_SIZE << 1);

	const _ALIGN(16) float QUAD_IN_TILE_OFFSETS_X[] = {0.0f, OUT_TILE_SIZE_F, 0.0f, OUT_TILE_SIZE_F};
	const _ALIGN(16) float QUAD_IN_TILE_OFFSETS_Y[] = {0.0f, 0.0f, OUT_TILE_SIZE_F, OUT_TILE_SIZE_F};
	//const static _ALIGN(16) float TILE3_OFFSET_2X[] = {OUT_TILE_SIZE_2X_F, OUT_TILE_SIZE_2X_F, OUT_TILE_SIZE_2X_F, OUT_TILE_SIZE_2X_F};

	//_LOOP(tileIndex, numInTiles)
	{
		//float startX = pInTiles[tileIndex << 1];
		//float startY = pInTiles[(tileIndex << 1) + 1];
		//float startXClamped = startX > pRaster->triBBFinal[0] ? startX : pRaster->triBBFinal[0];
		//float startYClamped = startY > pRaster->triBBFinal[1] ? startY : pRaster->triBBFinal[1];
		//float endX = startX + IN_TILE_X_F;
		//float endY = startY + IN_TILE_Y_F;
		//float endXClamped = endX < pRaster->triBBFinal[2] ? endX : pRaster->triBBFinal[2];
		//float endYClamped = endY < pRaster->triBBFinal[3] ? endY : pRaster->triBBFinal[3];

		//if(startXClamped >= endXClamped)
		//	continue;

		//float curX = *(pInTiles + (tileIndex << 1));
		//float curY = *(pInTiles + (tileIndex << 1) + 1);

		__TRAS_PIXEL_PREP

		__TRAS_EC_PREP

		__TRAS_INTER_PREP

		_DEBUG_COMPILE_ASSERT( (NUM_OUT_TILE_XY & 0x1) == 0 );

		_LOOPi(NUM_OUT_TILE_XY >> 1)
		{
			__TRAS_INTERPOLATE

			__TRAS_EC_PIXEL

			_LOOPj(NUM_OUT_TILE_XY - 1)
			{
				__TRAS_EC_INC_X
				__TRAS_INTER_INC_X
				
				__TRAS_EC_PIXEL
			}

			__TRAS_EC_INC_Y
			__TRAS_INTER_INC_Y

			__TRAS_EC_PIXEL

			_LOOPj(NUM_OUT_TILE_XY - 1)
			{
				__TRAS_EC_DEC_X
				__TRAS_INTER_DEC_X

				__TRAS_EC_PIXEL
			}

			__TRAS_EC_INC_Y
			__TRAS_INTER_INC_Y
		}

		//if(curY < endYClamped)
		//{
		//	//-----------------------------------------------------------------------------------------
		//	// Iterate +X direction

		//	// Trivial reject test
		//	__m128 insideTest = _mm_and_ps( _mm_cmpgt_ps(e1Reject, zero), _mm_cmpgt_ps(e2Reject, zero) );
		//	insideTest = _mm_and_ps( insideTest, _mm_cmpgt_ps(e3Reject, zero) );
		//	//int notRejectInsideMask = _mm_movemask_ps(insideTest);

		//	// Interpolate all user params
		//	__m128 finalInter[numInterpolants > 0 ? numInterpolants : 1];
		//	if(numInterpolants)
		//	{
		//		__m128 w = _mm_rcp_ps(invW);

		//		_LOOPk(numInterpolants)
		//		{
		//			finalInter[k] = _mm_mul_ps(inter[k], w);
		//		}
		//	}

		//	pixelShaderFn(pixelBlockX, pixelBlockY, insideTest, depth, finalInter);

		//	//_LOOPj(NUM_OUT_TILE_XY - 1)
		//	while(curX < (endXClamped - OUT_TILE_SIZE_2X_F))
		//	{
		//		// Increment to the next block X
		//		e1Reject = _mm_add_ps(e1Reject, pRaster->e1XFinalAdd);
		//		e2Reject = _mm_add_ps(e2Reject, pRaster->e2XFinalAdd);
		//		e3Reject = _mm_add_ps(e3Reject, pRaster->e3XFinalAdd);

		//		curX += 2;
		//		pixelBlockX += 2;

		//		depth = _mm_add_ps(depth, pRaster->depthXFinalAdd);

		//		if(numInterpolants)
		//		{
		//			invW = _mm_add_ps(invW, pRaster->invWXFinalAdd);
		//			__m128 w = _mm_rcp_ps(invW);

		//			_LOOPk(numInterpolants)
		//			{
		//				// Increment interpolated values
		//				inter[k] = _mm_add_ps(inter[k], pRaster->edgeInterpolantsXFinalAdd[k]);

		//				// Compute interpolatants
		//				finalInter[k] = _mm_mul_ps(inter[k], w);
		//			}
		//		}

		//		insideTest = _mm_and_ps( _mm_cmpgt_ps(e1Reject, zero), _mm_cmpgt_ps(e2Reject, zero) );
		//		insideTest = _mm_and_ps( insideTest, _mm_cmpgt_ps(e3Reject, zero) );

		//		pixelShaderFn(pixelBlockX, pixelBlockY, insideTest, depth, finalInter);
		//	}
		//}
	}
}

template<uint numInterpolants, uint numLayers, uint inTileSize, CustomPixelShaderFn pixelShaderFn>
_RASTERIZER_SSE_INLINE void TileRasterizeEdgeCheckOneQuad(const void* pRasterInfo, float curX, float curY, void* pContext)
{
	const RasterInfo<numInterpolants, numLayers>* __restrict pRaster = ((RasterInfo<numInterpolants, numLayers>*) pRasterInfo);

	const uint OUT_TILE_SIZE = 1;
	const float OUT_TILE_SIZE_F = (float) OUT_TILE_SIZE;
	const float OUT_TILE_SIZE_2X_F = OUT_TILE_SIZE_F * 2.0f;
	
	const uint NUM_OUT_TILE_XY = inTileSize / (OUT_TILE_SIZE << 1);

	const static _ALIGN(16) float QUAD_IN_TILE_OFFSETS_X[] = {0.0f, OUT_TILE_SIZE_F, 0.0f, OUT_TILE_SIZE_F};
	const static _ALIGN(16) float QUAD_IN_TILE_OFFSETS_Y[] = {0.0f, 0.0f, OUT_TILE_SIZE_F, OUT_TILE_SIZE_F};
	//const static _ALIGN(16) float TILE3_OFFSET_2X[] = {OUT_TILE_SIZE_2X_F, OUT_TILE_SIZE_2X_F, OUT_TILE_SIZE_2X_F, OUT_TILE_SIZE_2X_F};

//	_LOOP(tileIndex, numInTiles)
	{
		//float curX = *(pInTiles + (tileIndex << 1));
		//float curY = *(pInTiles + (tileIndex << 1) + 1);

		__TRAS_PIXEL_PREP

		__TRAS_EC_PREP

		__TRAS_INTER_PREP

		__TRAS_INTERPOLATE

		//// Interpolate depth
		//__m128 depthX = _mm_mul_ps(xxxx, pRaster->depthAAAA);
		//__m128 depthY = _mm_mul_ps(yyyy, pRaster->depthBBBB);
		//__m128 depth = _mm_add_ps(depthX, depthY);	
		//depth = _mm_add_ps( depth, pRaster->depthCCCC );

		//// Compute all interpolants (if they exist)
		//__m128 invW;
		//__m128 inter[numInterpolants > 0 ? numInterpolants : 1];
		//__m128 finalInter[numInterpolants > 0 ? numInterpolants : 1];
		//if(numInterpolants)
		//{
		//	// Interpolate w for the parameters
		//	__m128 invWx = _mm_mul_ps(xxxx, pRaster->invWAAAA);
		//	__m128 invWy = _mm_mul_ps(yyyy, pRaster->invWBBBB);
		//	invW = _mm_add_ps(invWx, invWy);
		//	invW = _mm_add_ps( invW, pRaster->invWCCCC );
		//	__m128 w = _mm_rcp_ps(invW);
		//	
		//	_LOOPk(numInterpolants)
		//	{
		//		__m128 interX = _mm_mul_ps(xxxx, pRaster->edgeInterpolantsAAAA[k]); 
		//		__m128 interY = _mm_mul_ps(yyyy, pRaster->edgeInterpolantsBBBB[k]);
		//		inter[k] = _mm_add_ps(interX, interY);
		//		inter[k] = _mm_add_ps(inter[k], pRaster->edgeInterpolantsCCCC[k]);
		//		finalInter[k] = _mm_mul_ps(inter[k], w);
		//	}
		//}

		__TRAS_EC_PIXEL
	}
}

template<uint numInterpolants, uint numLayers, uint inTileSize, CustomPixelShaderFn pixelShaderFn>
_RASTERIZER_SSE_INLINE void TileRasterizeNoEdgeCheckOneQuad(const void* pRasterInfo, float curX, float curY, void* pContext)
{
	const RasterInfo<numInterpolants, numLayers>* __restrict pRaster = ((RasterInfo<numInterpolants, numLayers>*) pRasterInfo);

	const uint OUT_TILE_SIZE = 1;
	const float OUT_TILE_SIZE_F = (float) OUT_TILE_SIZE;
	const float OUT_TILE_SIZE_2X_F = OUT_TILE_SIZE_F * 2.0f;

	const uint NUM_OUT_TILE_XY = inTileSize / (OUT_TILE_SIZE << 1);

	const static _ALIGN(16) float QUAD_IN_TILE_OFFSETS_X[] = {0.0f, OUT_TILE_SIZE_F, 0.0f, OUT_TILE_SIZE_F};
	const static _ALIGN(16) float QUAD_IN_TILE_OFFSETS_Y[] = {0.0f, 0.0f, OUT_TILE_SIZE_F, OUT_TILE_SIZE_F};

//	_LOOP(tileIndex, numInTiles)
	{
		//float curX = *(pInTiles + (tileIndex << 1));
		//float curY = *(pInTiles + (tileIndex << 1) + 1);

		__TRAS_PIXEL_PREP

		__TRAS_FULL_PREP

		__TRAS_INTER_PREP

		__TRAS_INTERPOLATE

		__TRAS_FULL_MASK_PIXEL			
	}
}

template<uint numInterpolants, uint numLayers, uint inTileSize, CustomPixelShaderFn pixelShaderFn>
_RASTERIZER_SSE_INLINE void TileRasterizeNoEdgeCheck(const void* pRasterInfo, float curX, float curY, void* pContext)
{
	const RasterInfo<numInterpolants, numLayers>* __restrict pRaster = ((RasterInfo<numInterpolants, numLayers>*) pRasterInfo);

	const uint OUT_TILE_SIZE = 1;
	const float OUT_TILE_SIZE_F = (float) OUT_TILE_SIZE;
	const float OUT_TILE_SIZE_2X_F = OUT_TILE_SIZE_F * 2.0f;

	const uint NUM_OUT_TILE_XY = inTileSize / (OUT_TILE_SIZE << 1);

	const static _ALIGN(16) float QUAD_IN_TILE_OFFSETS_X[] = {0.0f, OUT_TILE_SIZE_F, 0.0f, OUT_TILE_SIZE_F};
	const static _ALIGN(16) float QUAD_IN_TILE_OFFSETS_Y[] = {0.0f, 0.0f, OUT_TILE_SIZE_F, OUT_TILE_SIZE_F};

	//_LOOP(tileIndex, numInTiles)
	{
		//float curX = *(pInTiles + (tileIndex << 1));
		//float curY = *(pInTiles + (tileIndex << 1) + 1);

		__TRAS_PIXEL_PREP
		__TRAS_FULL_PREP
		__TRAS_INTER_PREP

		_DEBUG_COMPILE_ASSERT( (NUM_OUT_TILE_XY & 0x1) == 0 );

		_LOOPi(NUM_OUT_TILE_XY >> 1)
		{
			//-----------------------------------------------------------------------------------------
			// Iterate +X direction

			__TRAS_INTERPOLATE

			__TRAS_FULL_MASK_PIXEL

			_LOOPj(NUM_OUT_TILE_XY - 1)
			{
				__TRAS_INC_X

				__TRAS_INTER_INC_X

				__TRAS_FULL_MASK_PIXEL
			}

			__TRAS_INC_Y

			__TRAS_INTER_INC_Y

			__TRAS_FULL_MASK_PIXEL

			_LOOPj(NUM_OUT_TILE_XY - 1)
			{
				__TRAS_DEC_X

				__TRAS_INTER_DEC_X

				__TRAS_FULL_MASK_PIXEL
			}

			__TRAS_INC_Y
			__TRAS_INTER_INC_Y
		}
	}
}

_NAMESPACE_END