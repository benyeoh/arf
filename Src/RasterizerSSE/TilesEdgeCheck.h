//================================================================================
//
//		TilesEdgeCheck.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		17/1/2012
//
//================================================================================

#pragma once

#define _GET_NUM_TILES(bufferWidth, bufferHeight, tileWidth, tileHeight) \
	( ((bufferWidth * bufferHeight) / (tileWidth * tileHeight)) )

#define _GET_TILE_BUFFER_SIZE(numTiles) \
	( numTiles * sizeof(float) * 2 )

#define __TEC_DEC_X \
	e1Reject = _mm_sub_ps(e1Reject, pRaster->e1XLayerAdd[layer]); \
	e2Reject = _mm_sub_ps(e2Reject, pRaster->e2XLayerAdd[layer]); \
	e3Reject = _mm_sub_ps(e3Reject, pRaster->e3XLayerAdd[layer]); \
	\
	curX -= OUT_TILE_SIZE_2X_F; \

#define __TEC_INC_X \
	e1Reject = _mm_add_ps(e1Reject, pRaster->e1XLayerAdd[layer]); \
	e2Reject = _mm_add_ps(e2Reject, pRaster->e2XLayerAdd[layer]); \
	e3Reject = _mm_add_ps(e3Reject, pRaster->e3XLayerAdd[layer]); \
	\
	curX += OUT_TILE_SIZE_2X_F; \

#define __TEC_INC_Y \
	e1Reject = _mm_add_ps(e1Reject, pRaster->e1YLayerAdd[layer]); \
	e2Reject = _mm_add_ps(e2Reject, pRaster->e2YLayerAdd[layer]); \
	e3Reject = _mm_add_ps(e3Reject, pRaster->e3YLayerAdd[layer]); \
	\
	curY += OUT_TILE_SIZE_2X_F; \


//\
//	__m128 insideTest = _mm_and_ps( _mm_cmpge_ps(e1Reject, zero), _mm_cmpge_ps(e2Reject, zero) ); \
//	insideTest = _mm_and_ps( insideTest, _mm_cmpge_ps(e3Reject, zero) ); \



#define __TEC_DO \
{ \
	\
	__m128 insideTest = _mm_or_ps( _mm_or_ps( e1Reject, e2Reject ), e3Reject ); \
	insideTest = *((__m128*) &_mm_cmpgt_epi32( *((__m128i*)&insideTest), *((__m128i*) &zero) )); \
 \
	int notRejectInsideMask = _mm_movemask_ps(insideTest); \
 \
	__m128 e1Accept = _mm_add_ps(e1Reject, pRaster->e1AcceptLayerOffset[layer]); \
	__m128 e2Accept = _mm_add_ps(e2Reject, pRaster->e2AcceptLayerOffset[layer]); \
	__m128 e3Accept = _mm_add_ps(e3Reject, pRaster->e3AcceptLayerOffset[layer]); \
    insideTest = _mm_or_ps( _mm_or_ps( e1Accept, e2Accept ), e3Accept ); \
    insideTest = *((__m128*) &_mm_cmpgt_epi32( *((__m128i*)&insideTest), *((__m128i*) &zero) )); \
	int acceptInsideMask = _mm_movemask_ps(insideTest); \
	\
	notRejectInsideMask -= acceptInsideMask; \
	\
	ulong acceptBitIndex; \
	while(_BitScanForward(&acceptBitIndex, (ulong) acceptInsideMask)) \
	{ \
		float drawX = curX + QUAD_IN_TILE_OFFSETS_X[acceptBitIndex]; \
		float drawY = curY + QUAD_IN_TILE_OFFSETS_Y[acceptBitIndex]; \
		\
		fullFn(drawX, drawY, pUser); \
		\
		acceptInsideMask = (acceptInsideMask + 0xF) & acceptInsideMask; \
	} \
	\
	ulong notRejectBitIndex; \
	while(_BitScanForward(&notRejectBitIndex, (ulong) notRejectInsideMask)) \
	{ \
		float drawX = curX + QUAD_IN_TILE_OFFSETS_X[notRejectBitIndex]; \
		float drawY = curY + QUAD_IN_TILE_OFFSETS_Y[notRejectBitIndex]; \
		\
		partialFn(drawX, drawY, pUser); \
		\
		notRejectInsideMask = (notRejectInsideMask + 0xF) & notRejectInsideMask; \
	} \
	\
}



//#define __TEC_DO \
//{ \
//	__m128 insideTest = _mm_and_ps( _mm_cmpgt_ps(e1Reject, zero), _mm_cmpgt_ps(e2Reject, zero) ); \
//	insideTest = _mm_and_ps( insideTest, _mm_cmpgt_ps(e3Reject, zero) ); \
//	int notRejectInsideMask = _mm_movemask_ps(insideTest); \
//	\
//	__m128 e1Accept = _mm_add_ps(e1Reject, pRaster->e1AcceptLayerOffset[layer]); \
//	__m128 e2Accept = _mm_add_ps(e2Reject, pRaster->e2AcceptLayerOffset[layer]); \
//	insideTest = _mm_and_ps( _mm_cmpgt_ps(e1Accept, zero), _mm_cmpgt_ps(e2Accept, zero) ); \
//	\
//	__m128 e3Accept = _mm_add_ps(e3Reject, pRaster->e3AcceptLayerOffset[layer]); \
//	insideTest = _mm_and_ps( insideTest, _mm_cmpgt_ps(e3Accept, zero) ); \
//	int acceptInsideMask = _mm_movemask_ps(insideTest); \
//	\
//	notRejectInsideMask -= acceptInsideMask; \
//	\
//	/*if(acceptInsideMask & 0x1) \
//	{ \
//		float drawX = curX + QUAD_IN_TILE_OFFSETS_X[0]; \
//		float drawY = curY + QUAD_IN_TILE_OFFSETS_Y[0]; \
//		\
//		fullFn(drawX, drawY, pUser); \
//	} \
//	\
//	if(acceptInsideMask & 0x2) \
//	{ \
//		float drawX = curX + QUAD_IN_TILE_OFFSETS_X[1]; \
//		float drawY = curY + QUAD_IN_TILE_OFFSETS_Y[1]; \
//		\
//		fullFn(drawX, drawY, pUser); \
//	} \
//	\
//	if(acceptInsideMask & 0x4) \
//	{ \
//		float drawX = curX + QUAD_IN_TILE_OFFSETS_X[2]; \
//		float drawY = curY + QUAD_IN_TILE_OFFSETS_Y[2]; \
//		\
//		fullFn(drawX, drawY, pUser); \
//	} \
//	\
//	if(acceptInsideMask & 0x8) \
//	{ \
//		float drawX = curX + QUAD_IN_TILE_OFFSETS_X[3]; \
//		float drawY = curY + QUAD_IN_TILE_OFFSETS_Y[3]; \
//		\
//		fullFn(drawX, drawY, pUser); \
//	} \
//	*/\
//	ulong acceptBitIndex; \
//	while(_BitScanForward(&acceptBitIndex, (ulong) acceptInsideMask)) \
//	{ \
//		float drawX = curX + QUAD_IN_TILE_OFFSETS_X[acceptBitIndex]; \
//		float drawY = curY + QUAD_IN_TILE_OFFSETS_Y[acceptBitIndex]; \
//		\
//		fullFn(drawX, drawY, pUser); \
//		\
//		acceptInsideMask = (acceptInsideMask + 0xF) & acceptInsideMask; \
//	} \
//	\
//	ulong notRejectBitIndex; \
//	while(_BitScanForward(&notRejectBitIndex, (ulong) notRejectInsideMask)) \
//	{ \
//		float drawX = curX + QUAD_IN_TILE_OFFSETS_X[notRejectBitIndex]; \
//		float drawY = curY + QUAD_IN_TILE_OFFSETS_Y[notRejectBitIndex]; \
//		\
//		partialFn(drawX, drawY, pUser); \
//		\
//		notRejectInsideMask = (notRejectInsideMask + 0xF) & notRejectInsideMask; \
//	} \
//	\
//	/*\
//	if(notRejectInsideMask & 0x1) \
//	{ \
//		float drawX = curX + QUAD_IN_TILE_OFFSETS_X[0]; \
//		float drawY = curY + QUAD_IN_TILE_OFFSETS_Y[0]; \
//		\
//		partialFn(drawX, drawY, pUser); \
//	} \
//	\
//	if(notRejectInsideMask & 0x2) \
//	{ \
//		float drawX = curX + QUAD_IN_TILE_OFFSETS_X[1]; \
//		float drawY = curY + QUAD_IN_TILE_OFFSETS_Y[1]; \
//		\
//		partialFn(drawX, drawY, pUser); \
//	} \
//	\
//	if(notRejectInsideMask & 0x4) \
//	{ \
//		float drawX = curX + QUAD_IN_TILE_OFFSETS_X[2]; \
//		float drawY = curY + QUAD_IN_TILE_OFFSETS_Y[2]; \
//		\
//		partialFn(drawX, drawY, pUser); \
//	} \
//	\
//	if(notRejectInsideMask & 0x8) \
//	{ \
//		float drawX = curX + QUAD_IN_TILE_OFFSETS_X[3]; \
//		float drawY = curY + QUAD_IN_TILE_OFFSETS_Y[3]; \
//		\
//		partialFn(drawX, drawY, pUser); \
//	} \
//	*/\
//}


#define __TEC_PREP \
	const __m128 zero = _mm_setzero_ps(); \
 \
	__m128 xxxx = _mm_load_ps1(&curX); \
	xxxx = _mm_add_ps(xxxx, _mm_load_ps(QUAD_IN_TILE_OFFSETS_X)); \
	__m128 yyyy = _mm_load_ps1(&curY); \
	yyyy = _mm_add_ps(yyyy, _mm_load_ps(QUAD_IN_TILE_OFFSETS_Y)); \
 \
	__m128 e1 = _mm_mul_ps(pRaster->e1aaaa, xxxx); \
	e1 = _mm_add_ps( _mm_mul_ps(pRaster->e1bbbb, yyyy), e1 ); \
	__m128 e1Reject = _mm_add_ps( e1, pRaster->e1RejectLayerCCCC[layer] ); \
	__m128 e2 = _mm_mul_ps(pRaster->e2aaaa, xxxx); \
	e2 = _mm_add_ps( _mm_mul_ps(pRaster->e2bbbb, yyyy), e2 ); \
	__m128 e2Reject = _mm_add_ps( e2, pRaster->e2RejectLayerCCCC[layer] ); \
 \
	__m128 e3 = _mm_mul_ps(pRaster->e3aaaa, xxxx); \
	e3 = _mm_add_ps( _mm_mul_ps(pRaster->e3bbbb, yyyy), e3 ); \
	__m128 e3Reject = _mm_add_ps( e3, pRaster->e3RejectLayerCCCC[layer] ); \


typedef void (*CustomAcceptFn)(float pixelBlockX, float pixelBlockY, void* pContext);

template<uint numInterpolants, uint numLayers, uint outTileSize, uint layer, CustomAcceptFn partialFn, CustomAcceptFn fullFn>
 void TilesEdgeCheckWithBB(const void* pRasterInfo, float startX, float startY, uint bufferWidth, uint bufferHeight, void* pUser)
{
	const RasterInfo<numInterpolants, numLayers>* __restrict pRaster = ((RasterInfo<numInterpolants, numLayers>*) pRasterInfo);

	const uint OUT_TILE_SIZE = outTileSize;
	const float OUT_TILE_SIZE_F = (float) OUT_TILE_SIZE;
	const float OUT_TILE_SIZE_2X_F = OUT_TILE_SIZE_F * 2.0f;
	const float IN_TILE_X_F = (float) bufferWidth - 1.0f;
	const float IN_TILE_Y_F = (float) bufferHeight - 1.0f;

	_DEBUG_ASSERT( (bufferWidth % (OUT_TILE_SIZE << 1)) == 0 );
	_DEBUG_ASSERT( (bufferHeight % (OUT_TILE_SIZE << 1)) == 0 );

	//_DEBUG_ASSERT( ((bufferWidth / (OUT_TILE_SIZE << 1)) & 0x1) == 0 );
	//_DEBUG_ASSERT( ((bufferHeight / (OUT_TILE_SIZE << 1)) & 0x1) == 0 );
	//const uint NUM_OUT_TILE_XY = inTileSize / (OUT_TILE_SIZE << 1);
	//const uint NUM_OUT_TILE_WIDTH = bufferWidth / (OUT_TILE_SIZE << 1);
	//const uint NUM_OUT_TILE_HEIGHT = bufferHeight / (OUT_TILE_SIZE << 1);

	const static _ALIGN(16) float QUAD_IN_TILE_OFFSETS_X[] = {0.0f, OUT_TILE_SIZE_F, 0.0f, OUT_TILE_SIZE_F};
	const static _ALIGN(16) float QUAD_IN_TILE_OFFSETS_Y[] = {0.0f, 0.0f, OUT_TILE_SIZE_F, OUT_TILE_SIZE_F};
	//const static _ALIGN(16) float TILE3_OFFSET_2X[] = {OUT_TILE_SIZE_2X_F, OUT_TILE_SIZE_2X_F, OUT_TILE_SIZE_2X_F, OUT_TILE_SIZE_2X_F};

	// Draw commands
	//float* __restrict pOutTilesPartialStrict = pOutTilesPartial;
	//float* __restrict pOutTilesFullStrict = pOutTilesFull;

	//const __m128 zero = _mm_setzero_ps();

	//_LOOP(tileIndex, numInTiles)
	{
		//float startX = pInTiles[tileIndex << 1];
		//float startY = pInTiles[(tileIndex << 1) + 1];
		float startXClamped = startX > pRaster->triBB[4*layer] ? startX : pRaster->triBB[4*layer];
		float startYClamped = startY > pRaster->triBB[4*layer+1] ? startY : pRaster->triBB[4*layer+1];
		float endX = startX + IN_TILE_X_F;
		float endY = startY + IN_TILE_Y_F;
		float endXClamped = endX < pRaster->triBB[4*layer+2] ? endX : pRaster->triBB[4*layer+2];
		float endYClamped = endY < pRaster->triBB[4*layer+3] ? endY : pRaster->triBB[4*layer+3];

		if(startXClamped < endXClamped)
		{
			// Start at tile pixel corners
			float curX = startXClamped;
			float curY = startYClamped;
			
			__TEC_PREP

			//_DEBUG_ASSERT( (NUM_OUT_TILE_HEIGHT & 0x1) == 0 );

			while(curY < (endYClamped - OUT_TILE_SIZE_2X_F))
			{
				__TEC_DO

				while(curX < (endXClamped - OUT_TILE_SIZE_2X_F))
				{
					__TEC_INC_X

					__TEC_DO
				}

				__TEC_INC_Y

				__TEC_DO

				while(curX >= (startXClamped + OUT_TILE_SIZE_2X_F))
				{
					__TEC_DEC_X

					__TEC_DO
				}

				__TEC_INC_Y
			}

			// Do odd number row
			if(curY < endYClamped)
			{
				__TEC_DO

				while(curX < (endXClamped - OUT_TILE_SIZE_2X_F))
				{
					__TEC_INC_X

					__TEC_DO
				}
			}
		}
	}

	//*pNumPartial += ((size_t)( pOutTilesPartialStrict - pOutTilesPartial )) >> 1;
	//*pNumFull += ((size_t)( pOutTilesFullStrict - pOutTilesFull )) >> 1;
}

template<uint numInterpolants, uint numLayers, uint inTileWidth, uint inTileHeight, uint outTileSize, uint layer, CustomAcceptFn partialFn, CustomAcceptFn fullFn>
_RASTERIZER_SSE_INLINE void TilesEdgeCheck(const void* pRasterInfo, float curX, float curY, void* pUser)
{
	const RasterInfo<numInterpolants, numLayers>* __restrict pRaster = ((RasterInfo<numInterpolants, numLayers>*) pRasterInfo);

	const uint OUT_TILE_SIZE = outTileSize;
	const float OUT_TILE_SIZE_F = (float) OUT_TILE_SIZE;
	const float OUT_TILE_SIZE_2X_F = OUT_TILE_SIZE_F * 2.0f;
	//const float IN_TILE_X_F = (float) inTileWidth - 1.0f;
	//const float IN_TILE_Y_F = (float) inTileHeight - 1.0f;

	//const uint NUM_OUT_TILE_XY = inTileSize / (OUT_TILE_SIZE << 1);
	const uint NUM_OUT_TILE_WIDTH = inTileWidth / (OUT_TILE_SIZE << 1);
	const uint NUM_OUT_TILE_HEIGHT = inTileHeight / (OUT_TILE_SIZE << 1);

	const static _ALIGN(16) float QUAD_IN_TILE_OFFSETS_X[] = {0.0f, OUT_TILE_SIZE_F, 0.0f, OUT_TILE_SIZE_F};
	const static _ALIGN(16) float QUAD_IN_TILE_OFFSETS_Y[] = {0.0f, 0.0f, OUT_TILE_SIZE_F, OUT_TILE_SIZE_F};
	//const static _ALIGN(16) float TILE3_OFFSET_2X[] = {OUT_TILE_SIZE_2X_F, OUT_TILE_SIZE_2X_F, OUT_TILE_SIZE_2X_F, OUT_TILE_SIZE_2X_F};

	// Draw commands
	//float* __restrict pOutTilesPartialStrict = pOutTilesPartial;
	//float* __restrict pOutTilesFullStrict = pOutTilesFull;

	//const __m128 zero = _mm_setzero_ps();

	//_LOOP(tileIndex, numInTiles)
	{
		// Start at tile pixel corners
		//float curX = pInTiles[tileIndex << 1];
		//float curY = pInTiles[(tileIndex << 1) + 1];
		__TEC_PREP

		_DEBUG_COMPILE_ASSERT( (NUM_OUT_TILE_HEIGHT & 0x1) == 0 );

		_LOOPi(NUM_OUT_TILE_HEIGHT >> 1)
		{
			__TEC_DO

			_LOOPj(NUM_OUT_TILE_WIDTH - 1)
			{
				__TEC_INC_X

				__TEC_DO
			}

			__TEC_INC_Y

			__TEC_DO

			_LOOPj(NUM_OUT_TILE_WIDTH - 1)
			{
				__TEC_DEC_X

				__TEC_DO
			}

			__TEC_INC_Y
		}
	}

	//*pNumPartial += ((size_t)( pOutTilesPartialStrict - pOutTilesPartial )) >> 1;
	//*pNumFull += ((size_t)( pOutTilesFullStrict - pOutTilesFull )) >> 1;
}

template<uint numInterpolants, uint numLayers, uint outTileSize, uint layer, CustomAcceptFn partialFn, CustomAcceptFn fullFn>
_RASTERIZER_SSE_INLINE void TilesEdgeCheck4x4(const void* pRasterInfo, float curX, float curY, void* pUser)
{
	const RasterInfo<numInterpolants, numLayers>* __restrict pRaster = ((RasterInfo<numInterpolants, numLayers>*) pRasterInfo);

	const uint OUT_TILE_SIZE = outTileSize;
	const float OUT_TILE_SIZE_F = (float) OUT_TILE_SIZE;
	const float OUT_TILE_SIZE_2X_F = OUT_TILE_SIZE_F * 2.0f;
	//const float IN_TILE_X_F = (float) inTileWidth - 1.0f;
	//const float IN_TILE_Y_F = (float) inTileHeight - 1.0f;

	//const uint NUM_OUT_TILE_XY = inTileSize / (OUT_TILE_SIZE << 1);
	//const uint NUM_OUT_TILE_WIDTH = inTileWidth / (OUT_TILE_SIZE << 1);
	//const uint NUM_OUT_TILE_HEIGHT = inTileHeight / (OUT_TILE_SIZE << 1);

	const static _ALIGN(16) float QUAD_IN_TILE_OFFSETS_X[] = {0.0f, OUT_TILE_SIZE_F, 0.0f, OUT_TILE_SIZE_F};
	const static _ALIGN(16) float QUAD_IN_TILE_OFFSETS_Y[] = {0.0f, 0.0f, OUT_TILE_SIZE_F, OUT_TILE_SIZE_F};
	//const static _ALIGN(16) float TILE3_OFFSET_2X[] = {OUT_TILE_SIZE_2X_F, OUT_TILE_SIZE_2X_F, OUT_TILE_SIZE_2X_F, OUT_TILE_SIZE_2X_F};

	// Draw commands
	//float* __restrict pOutTilesPartialStrict = pOutTilesPartial;
	//float* __restrict pOutTilesFullStrict = pOutTilesFull;

	//const __m128 zero = _mm_setzero_ps();

	//_LOOP(tileIndex, numInTiles)
	{
		// Start at tile pixel corners
		//float curX = pInTiles[tileIndex << 1];
		//float curY = pInTiles[(tileIndex << 1) + 1];
		__TEC_PREP

		//__m128 e1XAdd = _mm_mul_ps(e1aaaa, TILE3_OFFSET_2X);
		//__m128 e2XAdd = _mm_mul_ps(e2aaaa, TILE3_OFFSET_2X);
		//__m128 e3XAdd = _mm_mul_ps(e3aaaa, TILE3_OFFSET_2X);

		//__m128 e1YAdd = _mm_mul_ps(e1bbbb, TILE3_OFFSET_2X);
		//__m128 e2YAdd = _mm_mul_ps(e2bbbb, TILE3_OFFSET_2X);
		//__m128 e3YAdd = _mm_mul_ps(e3bbbb, TILE3_OFFSET_2X);

		__TEC_DO  __TEC_INC_X  __TEC_DO  __TEC_INC_X  __TEC_DO  __TEC_INC_X  __TEC_DO

		__TEC_INC_Y  

		__TEC_DO  __TEC_INC_X  __TEC_DO  __TEC_INC_X  __TEC_DO  __TEC_INC_X  __TEC_DO

		__TEC_INC_Y

		__TEC_DO  __TEC_INC_X  __TEC_DO  __TEC_INC_X  __TEC_DO  __TEC_INC_X  __TEC_DO

		__TEC_INC_Y

		__TEC_DO  __TEC_INC_X  __TEC_DO  __TEC_INC_X  __TEC_DO  __TEC_INC_X  __TEC_DO
	}

	//*pNumPartial += ((size_t)( pOutTilesPartialStrict - pOutTilesPartial )) >> 1;
	//*pNumFull += ((size_t)( pOutTilesFullStrict - pOutTilesFull )) >> 1;
}

template<uint numInterpolants, uint numLayers, uint outTileSize, uint layer, CustomAcceptFn partialFn, CustomAcceptFn fullFn>
_RASTERIZER_SSE_INLINE void TilesEdgeCheck2x2(const void* pRasterInfo, float curX, float curY, void* pUser)
{
	const RasterInfo<numInterpolants, numLayers>* __restrict pRaster = ((RasterInfo<numInterpolants, numLayers>*) pRasterInfo);

	const uint OUT_TILE_SIZE = outTileSize;
	const float OUT_TILE_SIZE_F = (float) OUT_TILE_SIZE;
	const float OUT_TILE_SIZE_2X_F = OUT_TILE_SIZE_F * 2.0f;
	//const float IN_TILE_X_F = (float) inTileWidth - 1.0f;
	//const float IN_TILE_Y_F = (float) inTileHeight - 1.0f;

	//const uint NUM_OUT_TILE_XY = inTileSize / (OUT_TILE_SIZE << 1);
	//const uint NUM_OUT_TILE_WIDTH = inTileWidth / (OUT_TILE_SIZE << 1);
	//const uint NUM_OUT_TILE_HEIGHT = inTileHeight / (OUT_TILE_SIZE << 1);

	const static _ALIGN(16) float QUAD_IN_TILE_OFFSETS_X[] = {0.0f, OUT_TILE_SIZE_F, 0.0f, OUT_TILE_SIZE_F};
	const static _ALIGN(16) float QUAD_IN_TILE_OFFSETS_Y[] = {0.0f, 0.0f, OUT_TILE_SIZE_F, OUT_TILE_SIZE_F};
	//const static _ALIGN(16) float TILE3_OFFSET_2X[] = {OUT_TILE_SIZE_2X_F, OUT_TILE_SIZE_2X_F, OUT_TILE_SIZE_2X_F, OUT_TILE_SIZE_2X_F};

	// Draw commands
	//float* __restrict pOutTilesPartialStrict = pOutTilesPartial;
	//float* __restrict pOutTilesFullStrict = pOutTilesFull;

	//const __m128 zero = _mm_setzero_ps();

	//_LOOP(tileIndex, numInTiles)
	{
		// Start at tile pixel corners
		//float curX = startX;//pInTiles[tileIndex << 1];
		//float curY = startY; //pInTiles[(tileIndex << 1) + 1];
		__TEC_PREP

		__TEC_DO __TEC_INC_X __TEC_DO  

		__TEC_INC_Y

		__TEC_DO __TEC_DEC_X __TEC_DO

			//tileIndex++;
	}

	//*pNumPartial += ((size_t)( pOutTilesPartialStrict - pOutTilesPartial )) >> 1;
	//*pNumFull += ((size_t)( pOutTilesFullStrict - pOutTilesFull )) >> 1;
}

template<uint numInterpolants, uint numLayers, uint outTileSize, uint layer, CustomAcceptFn partialFn, CustomAcceptFn fullFn>
_RASTERIZER_SSE_INLINE void TilesEdgeCheck1x1(const void* pRasterInfo, float curX, float curY, void* pUser)
{
	const RasterInfo<numInterpolants, numLayers>* __restrict pRaster = ((RasterInfo<numInterpolants, numLayers>*) pRasterInfo);

	const uint OUT_TILE_SIZE = outTileSize;
	const float OUT_TILE_SIZE_F = (float) OUT_TILE_SIZE;
	const float OUT_TILE_SIZE_2X_F = OUT_TILE_SIZE_F * 2.0f;
	//const float IN_TILE_X_F = (float) inTileWidth - 1.0f;
	//const float IN_TILE_Y_F = (float) inTileHeight - 1.0f;

	//const uint NUM_OUT_TILE_XY = inTileSize / (OUT_TILE_SIZE << 1);
	//const uint NUM_OUT_TILE_WIDTH = inTileWidth / (OUT_TILE_SIZE << 1);
	//const uint NUM_OUT_TILE_HEIGHT = inTileHeight / (OUT_TILE_SIZE << 1);

	const static _ALIGN(16) float QUAD_IN_TILE_OFFSETS_X[] = {0.0f, OUT_TILE_SIZE_F, 0.0f, OUT_TILE_SIZE_F};
	const static _ALIGN(16) float QUAD_IN_TILE_OFFSETS_Y[] = {0.0f, 0.0f, OUT_TILE_SIZE_F, OUT_TILE_SIZE_F};
	//const static _ALIGN(16) float TILE3_OFFSET_2X[] = {OUT_TILE_SIZE_2X_F, OUT_TILE_SIZE_2X_F, OUT_TILE_SIZE_2X_F, OUT_TILE_SIZE_2X_F};

	// Draw commands
	//float* __restrict pOutTilesPartialStrict = pOutTilesPartial;
	//float* __restrict pOutTilesFullStrict = pOutTilesFull;

	//const __m128 zero = _mm_setzero_ps();

	//_LOOP(tileIndex, numInTiles)
	{
		// Start at tile pixel corners
		//float curX = pInTiles[tileIndex << 1];
		//float curY = pInTiles[(tileIndex << 1) + 1];
		__TEC_PREP

		__TEC_DO
	}

	//*pNumPartial += ((size_t)( pOutTilesPartialStrict - pOutTilesPartial )) >> 1;
	//*pNumFull += ((size_t)( pOutTilesFullStrict - pOutTilesFull )) >> 1;
}