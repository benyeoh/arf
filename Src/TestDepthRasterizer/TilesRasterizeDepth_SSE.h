//================================================================================
//
//		TilesRasterizeDepth_SSE.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		17/1/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

const static __m128 zeroConstant = _mm_setzero_ps();

#define _ZERO (_mm_setzero_ps())

#if defined(_DEBUG)
    #define _CHECK_DEPTH(depthBuffer, min) \
    { \
        __m128 ___checkDepth = *((__m128*) (depthBuffer)); \
        ___checkDepth = _mm_cmplt_ps(___checkDepth, _mm_set_ps1(1.0f)); \
        min = _mm_andnot_ps(___checkDepth, min); \
        g_NumPixelsProcessed += 4; \
    }

    #define _CHECK_DEPTH_EDGE(depthBuffer, min, e1, e2, e3) \
    { \
        __m128 ___insideTest = _mm_or_ps( _mm_or_ps( e1, e2 ), e3 ); \
        ___insideTest = *((__m128*) &_mm_cmpgt_epi32( *((__m128i*)&___insideTest), *((__m128i*) &_ZERO) )); \
        int ___insideMask = _mm_movemask_ps(___insideTest); \
        g_NumPixelsProcessed += (___insideMask & 0x1); \
        g_NumPixelsProcessed += ((___insideMask & 0x2) >> 1); \
        g_NumPixelsProcessed += ((___insideMask & 0x4) >> 2); \
        g_NumPixelsProcessed += ((___insideMask & 0x8) >> 3); \
        \
        __m128 ___checkDepth = *((__m128*) (depthBuffer)); \
        ___checkDepth = _mm_cmplt_ps(___checkDepth, _mm_set_ps1(1.0f)); \
        ___checkDepth = _mm_and_ps(___checkDepth, ___insideTest); \
        min = _mm_andnot_ps(___checkDepth, min); \
    }

        //int ___checkMask = _mm_movemask_ps(___checkDepth); \
        //static bool isAssert = false; \
        //if(___checkMask > 0) \
        //{ \
        //    if(!isAssert) \
        //        _DEBUG_ASSERT(FALSE); \
        //    \
        //    isAssert = true; \
        //} \

#else
    #define _CHECK_DEPTH(depthBuffer, min)
    #define _CHECK_DEPTH_EDGE(depthBuffer, min, e1, e2, e3)
#endif

template<uint outTileSize>
__forceinline void TilesRasterizeEdgeCheckDepth(const void* pRasterInfo, float* pDepthBuffer, __m128& depth, __m128& e1, __m128& e2, __m128& e3)
{
    const RasterInfoDepth<1>* __restrict pRaster = ((RasterInfoDepth<1>*) pRasterInfo);

    const uint PIXEL_SIZE = 1;
    const float PIXEL_SIZE_F = (float) PIXEL_SIZE;
    const float PIXEL_SIZE_2X_F = 2.0f;

    const uint OUT_TILE_SIZE = outTileSize;
    const uint NUM_PIXEL_TILES_XY = OUT_TILE_SIZE / (PIXEL_SIZE << 1);
    
    const uint QUAD_MEM_OFFSET_Y = PIXEL_SIZE * 2;
    const uint QUAD_MEM_OFFSET_X = PIXEL_SIZE * 4;

    e1 = _mm_add_ps( e1, pRaster->e1XYFinalAdd );
    e2 = _mm_add_ps( e2, pRaster->e2XYFinalAdd );
    e3 = _mm_add_ps( e3, pRaster->e3XYFinalAdd );
    depth = _mm_add_ps( depth, pRaster->depthXYFinalAdd );

    _LOOPi(NUM_PIXEL_TILES_XY >> 1)
    {
        //__TRAS_INTERPOLATE

        //__TRAS_EC_PIXEL
        
        // Write out to depth with mask
        __m128 depthPrev = *((__m128*) (pDepthBuffer));    // Prev
        __m128 depthMin = _mm_min_ps( depth, depthPrev );                       // Check depth test
        _CHECK_DEPTH_EDGE(pDepthBuffer, depthMin, e1, e2, e3);

        __m128 insideTest = _mm_or_ps( _mm_or_ps( e1, e2 ), e3 );
        insideTest = *((__m128*) &_mm_cmpgt_epi32( *((__m128i*)&insideTest), *((__m128i*) &_ZERO) ));
        __m128 depthStore = _mm_xor_ps( depthPrev, _mm_and_ps( insideTest, _mm_xor_ps(depthMin, depthPrev) ) ); // Mask with edge function
        _mm_store_ps(pDepthBuffer, depthStore);

        _LOOPj(NUM_PIXEL_TILES_XY - 1)
        {
            //__TRAS_EC_INC_X

            pDepthBuffer += QUAD_MEM_OFFSET_X;
            e1 = _mm_add_ps(e1, pRaster->e1XFinalAdd);
            e2 = _mm_add_ps(e2, pRaster->e2XFinalAdd);
            e3 = _mm_add_ps(e3, pRaster->e3XFinalAdd);
            depth = _mm_add_ps( depth, pRaster->depthXFinalAdd );

            depthPrev = *((__m128*) (pDepthBuffer));    // Prev
            depthMin = _mm_min_ps( depth, depthPrev );                       // Check depth test
            _CHECK_DEPTH_EDGE(pDepthBuffer, depthMin, e1, e2, e3);

            insideTest = _mm_or_ps( _mm_or_ps( e1, e2 ), e3 );
            insideTest = *((__m128*) &_mm_cmpgt_epi32( *((__m128i*)&insideTest), *((__m128i*) &_ZERO) ));
            depthStore = _mm_xor_ps( depthPrev, _mm_and_ps( insideTest, _mm_xor_ps(depthMin, depthPrev) ) ); // Mask with edge function
            _mm_store_ps(pDepthBuffer, depthStore);
        }

        // __TRAS_EC_INC_Y
        pDepthBuffer += (QUAD_MEM_OFFSET_Y * RASTERIZE_BUFFER_W);
        e1 = _mm_add_ps(e1, pRaster->e1YFinalAdd);
        e2 = _mm_add_ps(e2, pRaster->e2YFinalAdd);
        e3 = _mm_add_ps(e3, pRaster->e3YFinalAdd);
        depth = _mm_add_ps( depth, pRaster->depthYFinalAdd );
   
        //__TRAS_EC_PIXEL
        depthPrev = *((__m128*) (pDepthBuffer));    // Prev
        depthMin = _mm_min_ps( depth, depthPrev );                       // Check depth test
        _CHECK_DEPTH_EDGE(pDepthBuffer, depthMin, e1, e2, e3);

        insideTest = _mm_or_ps( _mm_or_ps( e1, e2 ), e3 );
        insideTest = *((__m128*) &_mm_cmpgt_epi32( *((__m128i*)&insideTest), *((__m128i*) &_ZERO) ));
        depthStore = _mm_xor_ps( depthPrev, _mm_and_ps( insideTest, _mm_xor_ps(depthMin, depthPrev) ) ); // Mask with edge function
        _mm_store_ps(pDepthBuffer, depthStore);

        _LOOPj(NUM_PIXEL_TILES_XY - 1)
        {
            //__TRAS_EC_DEC_X
            pDepthBuffer -= QUAD_MEM_OFFSET_X;
            e1 = _mm_sub_ps(e1, pRaster->e1XFinalAdd);
            e2 = _mm_sub_ps(e2, pRaster->e2XFinalAdd);
            e3 = _mm_sub_ps(e3, pRaster->e3XFinalAdd);
            depth = _mm_sub_ps( depth, pRaster->depthXFinalAdd );

            //__TRAS_INTER_DEC_X
           
            //__TRAS_EC_PIXEL
            depthPrev = *((__m128*) (pDepthBuffer));    // Prev
            depthMin = _mm_min_ps( depth, depthPrev );                       // Check depth test
            _CHECK_DEPTH_EDGE(pDepthBuffer, depthMin, e1, e2, e3);

            insideTest = _mm_or_ps( _mm_or_ps( e1, e2 ), e3 );
            insideTest = *((__m128*) &_mm_cmpgt_epi32( *((__m128i*)&insideTest), *((__m128i*) &_ZERO) ));
            depthStore = _mm_xor_ps( depthPrev, _mm_and_ps( insideTest, _mm_xor_ps(depthMin, depthPrev) ) ); // Mask with edge function
            _mm_store_ps(pDepthBuffer, depthStore);
        }

        // __TRAS_EC_INC_Y
        pDepthBuffer += (QUAD_MEM_OFFSET_Y * RASTERIZE_BUFFER_W);
        e1 = _mm_add_ps(e1, pRaster->e1YFinalAdd);
        e2 = _mm_add_ps(e2, pRaster->e2YFinalAdd);
        e3 = _mm_add_ps(e3, pRaster->e3YFinalAdd);
        depth = _mm_add_ps( depth, pRaster->depthYFinalAdd );                      
    }
}

template<uint outTileSize>
__forceinline void TilesRasterizeDepth(const void* pRasterInfo, float* pDepthBuffer, __m128& depth)
{
    const RasterInfoDepth<1>* __restrict pRaster = ((RasterInfoDepth<1>*) pRasterInfo);

    const uint PIXEL_SIZE = 1;
    const float PIXEL_SIZE_F = (float) PIXEL_SIZE;
    const float PIXEL_SIZE_2X_F = 2.0f;

    const uint OUT_TILE_SIZE = outTileSize;
    const uint NUM_PIXEL_TILES_XY = OUT_TILE_SIZE / (PIXEL_SIZE << 1);

    const uint QUAD_MEM_OFFSET_Y = PIXEL_SIZE * 2;
    const uint QUAD_MEM_OFFSET_X = PIXEL_SIZE * 4;

    depth = _mm_add_ps( depth, pRaster->depthXYFinalAdd );

    _LOOPi(NUM_PIXEL_TILES_XY >> 1)
    {  
        // Write out to depth with mask
        __m128 depthPrev = *((__m128*) (pDepthBuffer));    // Prev
        __m128 depthMin = _mm_min_ps( depth, depthPrev );                       // Check depth test

        _CHECK_DEPTH(pDepthBuffer, depthMin);

        _mm_store_ps(pDepthBuffer, depthMin);

        _LOOPj(NUM_PIXEL_TILES_XY - 1)
        {
            //__TRAS_EC_INC_X

            pDepthBuffer += QUAD_MEM_OFFSET_X;
            depth = _mm_add_ps( depth, pRaster->depthXFinalAdd );
          
            depthPrev = *((__m128*) (pDepthBuffer));    // Prev
            depthMin = _mm_min_ps( depth, depthPrev );                       // Check depth test
            
            _CHECK_DEPTH(pDepthBuffer, depthMin);

            _mm_store_ps(pDepthBuffer, depthMin);
        }

        // __TRAS_EC_INC_Y
        pDepthBuffer += (QUAD_MEM_OFFSET_Y * RASTERIZE_BUFFER_W);
        depth = _mm_add_ps( depth, pRaster->depthYFinalAdd );

        //__TRAS_EC_PIXEL
        
        depthPrev = *((__m128*) (pDepthBuffer));    // Prev
        depthMin = _mm_min_ps( depth, depthPrev );                       // Check depth test

        _CHECK_DEPTH(pDepthBuffer, depthMin);
        _mm_store_ps(pDepthBuffer, depthMin);

        _LOOPj(NUM_PIXEL_TILES_XY - 1)
        {
            //__TRAS_EC_DEC_X
            pDepthBuffer -= QUAD_MEM_OFFSET_X;
            depth = _mm_sub_ps( depth, pRaster->depthXFinalAdd );

            //__TRAS_INTER_DEC_X       
            //__TRAS_EC_PIXEL
            depthPrev = *((__m128*) (pDepthBuffer));    // Prev
            depthMin = _mm_min_ps( depth, depthPrev );                       // Check depth test

            _CHECK_DEPTH(pDepthBuffer, depthMin);
            _mm_store_ps(pDepthBuffer, depthMin);
        }

        // __TRAS_EC_INC_Y
        pDepthBuffer += (QUAD_MEM_OFFSET_Y * RASTERIZE_BUFFER_W);
        depth = _mm_add_ps( depth, pRaster->depthYFinalAdd );                        
    }
}

template<uint outTileSize>
__forceinline void QuadEdgeCheckDepth(const void* pRasterInfo, float* pDepthBuffer, __m128& depthTopBlock, __m128& e1TopBlock, __m128& e2TopBlock, __m128& e3TopBlock)
{
    const RasterInfoDepth<1>* __restrict pRaster = ((RasterInfoDepth<1>*) pRasterInfo);

     const uint OUT_TILE_SIZE = outTileSize;
     const uint QUAD_X_OFFSET = 2;

    __m128 e1Reject = _mm_add_ps( e1TopBlock, pRaster->e1RejectLayerCCCC[0] );
    __m128 e2Reject = _mm_add_ps( e2TopBlock, pRaster->e2RejectLayerCCCC[0] );
    __m128 e3Reject = _mm_add_ps( e3TopBlock, pRaster->e3RejectLayerCCCC[0] );

    __m128 insideTest = _mm_or_ps( _mm_or_ps( e1Reject, e2Reject ), e3Reject );
    insideTest = *((__m128*) &_mm_cmpgt_epi32( *((__m128i*)&insideTest), *((__m128i*) &_ZERO) ));
    int notRejectInsideMask = _mm_movemask_ps(insideTest);

    __m128 e1Accept = _mm_add_ps(e1Reject, pRaster->e1AcceptLayerOffset[0]);
    __m128 e2Accept = _mm_add_ps(e2Reject, pRaster->e2AcceptLayerOffset[0]);
    __m128 e3Accept = _mm_add_ps(e3Reject, pRaster->e3AcceptLayerOffset[0]);
    insideTest = _mm_or_ps( _mm_or_ps( e1Accept, e2Accept ), e3Accept );
    insideTest = *((__m128*) &_mm_cmpgt_epi32( *((__m128i*)&insideTest), *((__m128i*) &_ZERO) ));
    int acceptInsideMask = _mm_movemask_ps(insideTest);

    if(acceptInsideMask & 0x1)
    {
        __m128 depth = _mm_shuffle_ps( depthTopBlock, depthTopBlock, _MM_SHUFFLE(0, 0, 0, 0) );
        TilesRasterizeDepth<outTileSize>(pRasterInfo, pDepthBuffer, depth);
    }
    else 
    if(notRejectInsideMask & 0x1)
    {
        __m128 e1 = _mm_shuffle_ps( e1TopBlock, e1TopBlock, _MM_SHUFFLE(0, 0, 0, 0) );
        __m128 e2 = _mm_shuffle_ps( e2TopBlock, e2TopBlock, _MM_SHUFFLE(0, 0, 0, 0) );
        __m128 e3 = _mm_shuffle_ps( e3TopBlock, e3TopBlock, _MM_SHUFFLE(0, 0, 0, 0) );                   
        __m128 depth = _mm_shuffle_ps( depthTopBlock, depthTopBlock, _MM_SHUFFLE(0, 0, 0, 0) );

        TilesRasterizeEdgeCheckDepth<outTileSize>(pRasterInfo, pDepthBuffer, depth, e1, e2, e3);
        //TilesRasterizeDepth<outTileSize>(pRasterInfo, pDepthBuffer, depth);
    }

    if(acceptInsideMask & 0x2)
    {
        __m128 depth = _mm_shuffle_ps( depthTopBlock, depthTopBlock, _MM_SHUFFLE(1, 1, 1, 1) );
        TilesRasterizeDepth<outTileSize>(pRasterInfo, pDepthBuffer + OUT_TILE_SIZE * QUAD_X_OFFSET, depth);
    }
    else 
    if(notRejectInsideMask & 0x2)
    {
        __m128 e1 = _mm_shuffle_ps( e1TopBlock, e1TopBlock, _MM_SHUFFLE(1, 1, 1, 1) );
        __m128 e2 = _mm_shuffle_ps( e2TopBlock, e2TopBlock, _MM_SHUFFLE(1, 1, 1, 1) );
        __m128 e3 = _mm_shuffle_ps( e3TopBlock, e3TopBlock, _MM_SHUFFLE(1, 1, 1, 1) );                   
        __m128 depth = _mm_shuffle_ps( depthTopBlock, depthTopBlock, _MM_SHUFFLE(1, 1, 1, 1) );

        TilesRasterizeEdgeCheckDepth<outTileSize>(pRasterInfo, pDepthBuffer + OUT_TILE_SIZE * QUAD_X_OFFSET, depth, e1, e2, e3);
        //TilesRasterizeDepth<outTileSize>(pRasterInfo, pDepthBuffer + OUT_TILE_SIZE * QUAD_X_OFFSET, depth);
    }

    if(acceptInsideMask & 0x4)
    {
        __m128 depth = _mm_shuffle_ps( depthTopBlock, depthTopBlock, _MM_SHUFFLE(2, 2, 2, 2) );
        TilesRasterizeDepth<outTileSize>(pRasterInfo, pDepthBuffer + OUT_TILE_SIZE * RASTERIZE_BUFFER_W, depth);
    }
    else 
    if(notRejectInsideMask & 0x4)
    {
        __m128 e1 = _mm_shuffle_ps( e1TopBlock, e1TopBlock, _MM_SHUFFLE(2, 2, 2, 2) );
        __m128 e2 = _mm_shuffle_ps( e2TopBlock, e2TopBlock, _MM_SHUFFLE(2, 2, 2, 2) );
        __m128 e3 = _mm_shuffle_ps( e3TopBlock, e3TopBlock, _MM_SHUFFLE(2, 2, 2, 2) );                    
        __m128 depth = _mm_shuffle_ps( depthTopBlock, depthTopBlock, _MM_SHUFFLE(2, 2, 2, 2) );

        TilesRasterizeEdgeCheckDepth<outTileSize>(pRasterInfo,  pDepthBuffer + OUT_TILE_SIZE * RASTERIZE_BUFFER_W, depth, e1, e2, e3);
        //TilesRasterizeDepth<outTileSize>(pRasterInfo, pDepthBuffer + OUT_TILE_SIZE * RASTERIZE_BUFFER_W, depth);
    }

    if(acceptInsideMask & 0x8)
    {
        __m128 depth = _mm_shuffle_ps( depthTopBlock, depthTopBlock, _MM_SHUFFLE(3, 3, 3, 3) );
        TilesRasterizeDepth<outTileSize>(pRasterInfo, pDepthBuffer + OUT_TILE_SIZE * QUAD_X_OFFSET + OUT_TILE_SIZE * RASTERIZE_BUFFER_W, depth);
    }
    else 
    if(notRejectInsideMask & 0x8)
    {
        __m128 e1 = _mm_shuffle_ps( e1TopBlock, e1TopBlock, _MM_SHUFFLE(3, 3, 3, 3) );
        __m128 e2 = _mm_shuffle_ps( e2TopBlock, e2TopBlock, _MM_SHUFFLE(3, 3, 3, 3) );
        __m128 e3 = _mm_shuffle_ps( e3TopBlock, e3TopBlock, _MM_SHUFFLE(3, 3, 3, 3) );                 
        __m128 depth = _mm_shuffle_ps( depthTopBlock, depthTopBlock, _MM_SHUFFLE(3, 3, 3, 3) );

        TilesRasterizeEdgeCheckDepth<outTileSize>(pRasterInfo, pDepthBuffer + OUT_TILE_SIZE * QUAD_X_OFFSET + OUT_TILE_SIZE * RASTERIZE_BUFFER_W, depth, e1, e2, e3);
        //TilesRasterizeDepth<outTileSize>(pRasterInfo, pDepthBuffer + OUT_TILE_SIZE * QUAD_X_OFFSET + OUT_TILE_SIZE * RASTERIZE_BUFFER_W, depth);
    }
}

template<uint outTileSize>
__forceinline void TilesRasterizeEdgeCheckWithBBDepth(const void* pRasterInfo, float* pDepthBuffer, float startX, float startY, uint bufferWidth, uint bufferHeight)
{
	const RasterInfoDepth<1>* __restrict pRaster = ((RasterInfoDepth<1>*) pRasterInfo);

	const uint OUT_TILE_SIZE        = 1;
    const int OUT_TILE_SIZE_2X    = (OUT_TILE_SIZE << 1);
    const float OUT_TILE_SIZE_F     = (float) OUT_TILE_SIZE;
	const float OUT_TILE_SIZE_2X_F  = OUT_TILE_SIZE_F * 2.0f;
	const float IN_TILE_X_F         = (float) bufferWidth - 1.0f;
	const float IN_TILE_Y_F         = (float) bufferHeight - 1.0f;

	_DEBUG_ASSERT( ((bufferWidth / OUT_TILE_SIZE_2X) & 0x1) == 0 );
	_DEBUG_ASSERT( ((bufferHeight / OUT_TILE_SIZE_2X) & 0x1) == 0 );
	//const uint NUM_OUT_TILE_XY = inTileSize / (OUT_TILE_SIZE << 1);
	//const uint NUM_OUT_TILE_WIDTH = bufferWidth / (OUT_TILE_SIZE << 1);
	//const uint NUM_OUT_TILE_HEIGHT = bufferHeight / (OUT_TILE_SIZE << 1);

	const static _ALIGN(16) float QUAD_IN_TILE_OFFSETS_X[] = {0.0f, OUT_TILE_SIZE_F, 0.0f, OUT_TILE_SIZE_F};
	const static _ALIGN(16) float QUAD_IN_TILE_OFFSETS_Y[] = {0.0f, 0.0f, OUT_TILE_SIZE_F, OUT_TILE_SIZE_F};

    const uint QUAD_X_OFFSET = 2;

	{
		float startXClamped = startX > pRaster->triBBFinal[0] ? startX : pRaster->triBBFinal[0];
		float startYClamped = startY > pRaster->triBBFinal[1] ? startY : pRaster->triBBFinal[1];
		float endX = startX + IN_TILE_X_F;
		float endY = startY + IN_TILE_Y_F;
		float endXClamped = endX < pRaster->triBBFinal[2] ? endX : pRaster->triBBFinal[2];
		float endYClamped = endY < pRaster->triBBFinal[3] ? endY : pRaster->triBBFinal[3];

		if(startXClamped < endXClamped)
		{
			// Start at tile pixel corners
			float curX = startXClamped;
			float curY = startYClamped;
         
            __m128 xxxx = _mm_load_ps1(&curX);
            xxxx = _mm_add_ps(xxxx, _mm_load_ps(QUAD_IN_TILE_OFFSETS_X));
            __m128 yyyy = _mm_load_ps1(&curY);
            yyyy = _mm_add_ps(yyyy, _mm_load_ps(QUAD_IN_TILE_OFFSETS_Y));

            __m128 e1 = _mm_mul_ps(pRaster->e1aaaa, xxxx);
            e1 = _mm_add_ps( e1, _mm_mul_ps(pRaster->e1bbbb, yyyy) );
            e1 = _mm_add_ps( e1, pRaster->e1cccc );
            
            __m128 e2 = _mm_mul_ps(pRaster->e2aaaa, xxxx);
            e2 = _mm_add_ps( e2, _mm_mul_ps(pRaster->e2bbbb, yyyy) );
            e2 = _mm_add_ps( e2, pRaster->e2cccc );

            __m128 e3 = _mm_mul_ps(pRaster->e3aaaa, xxxx);
            e3 = _mm_add_ps( e3, _mm_mul_ps(pRaster->e3bbbb, yyyy) );
            e3 = _mm_add_ps( e3, pRaster->e3cccc );
            //__m128 e3Reject = _mm_add_ps( e3, pRaster->e3RejectLayerCCCC[0] );

            int pixelBlockX = _mm_cvtt_ss2si(xxxx);
            int pixelBlockY = _mm_cvtt_ss2si(yyyy);
            int endPixelBlockOffsetX = _mm_cvtt_ss2si( _mm_set_ss(endXClamped) ) - pixelBlockX;
            int endPixelBlockOffsetY = _mm_cvtt_ss2si( _mm_set_ss(endYClamped) ) - pixelBlockY;

            pDepthBuffer += pixelBlockY * RASTERIZE_BUFFER_W + (pixelBlockX * QUAD_X_OFFSET);
            float* pEndDepthAddressOffsetTopBlock = pDepthBuffer + endPixelBlockOffsetY * RASTERIZE_BUFFER_W;
      
			__m128 depthX = _mm_mul_ps(xxxx, pRaster->depthAAAA);
            __m128 depthY = _mm_mul_ps(yyyy, pRaster->depthBBBB);
            __m128 depth = _mm_add_ps(depthX, depthY);
            depth = _mm_add_ps( depth, pRaster->depthCCCC );

            //while(curY < (endYClamped - OUT_TILE_SIZE_2X_F))

            // TODO: This might have a problem if the i falls exactly on the tile boundary of endPixelBlockOffsetY - OUT_TILE_SIZE_2X
            // Since the BB is always rounded to multiples of OUT_TILE_SIZE, if i == (endPixelBlockOffsetY), then the pixel tiles starting at endPixelBlockOffsetY
            // will be ignored according to the loop below.
            // This works if the BB is always 1 OUT_TILE_SIZE more than what is needed, and if BUFFER_W and BUFFER_H is always a pow of 2
           
            for(int i=0; i < (endPixelBlockOffsetY - OUT_TILE_SIZE_2X); i += (OUT_TILE_SIZE_2X * 2))
			{
                // Write out to depth with mask
                __m128 depthPrev = *((__m128*) (pDepthBuffer));    // Prev
                __m128 depthMin = _mm_min_ps( depth, depthPrev );                       // Check depth test
                _CHECK_DEPTH_EDGE(pDepthBuffer, depthMin, e1, e2, e3);

                __m128 insideTest = _mm_or_ps( _mm_or_ps( e1, e2 ), e3 );
                insideTest = *((__m128*) &_mm_cmpgt_epi32( *((__m128i*)&insideTest), *((__m128i*) &_ZERO) ));
                __m128 depthStore = _mm_xor_ps( depthPrev, _mm_and_ps( insideTest, _mm_xor_ps(depthMin, depthPrev) ) ); // Mask with edge function
                _mm_store_ps(pDepthBuffer, depthStore);

				//while(curX < (endXClamped - OUT_TILE_SIZE_2X_F))
                for(int j=0; j < (endPixelBlockOffsetX - OUT_TILE_SIZE_2X); j += OUT_TILE_SIZE_2X)
				{
					//__TEC_INC_X
                    pDepthBuffer += (OUT_TILE_SIZE_2X * QUAD_X_OFFSET);
                    e1 = _mm_add_ps(e1, pRaster->e1XFinalAdd);
                    e2 = _mm_add_ps(e2, pRaster->e2XFinalAdd);
                    e3 = _mm_add_ps(e3, pRaster->e3XFinalAdd);
                    depth = _mm_add_ps( depth, pRaster->depthXFinalAdd );
                                
                    //__TEC_DO
                    __m128 depthPrev = *((__m128*) (pDepthBuffer));    // Prev
                    __m128 depthMin = _mm_min_ps( depth, depthPrev );                       // Check depth test
                    _CHECK_DEPTH_EDGE(pDepthBuffer, depthMin, e1, e2, e3);

                    __m128 insideTest = _mm_or_ps( _mm_or_ps( e1, e2 ), e3 );
                    insideTest = *((__m128*) &_mm_cmpgt_epi32( *((__m128i*)&insideTest), *((__m128i*) &_ZERO) ));
                    __m128 depthStore = _mm_xor_ps( depthPrev, _mm_and_ps( insideTest, _mm_xor_ps(depthMin, depthPrev) ) ); // Mask with edge function
                    _mm_store_ps(pDepthBuffer, depthStore);
				}

                // __TRAS_EC_INC_Y
                pDepthBuffer += (OUT_TILE_SIZE_2X * RASTERIZE_BUFFER_W);
                e1 = _mm_add_ps(e1, pRaster->e1YFinalAdd);
                e2 = _mm_add_ps(e2, pRaster->e2YFinalAdd);
                e3 = _mm_add_ps(e3, pRaster->e3YFinalAdd);
                depth = _mm_add_ps( depth, pRaster->depthYFinalAdd );

                //__TRAS_EC_PIXEL
                depthPrev = *((__m128*) (pDepthBuffer));    // Prev
                depthMin = _mm_min_ps( depth, depthPrev );                       // Check depth test
                _CHECK_DEPTH_EDGE(pDepthBuffer, depthMin, e1, e2, e3);

                insideTest = _mm_or_ps( _mm_or_ps( e1, e2 ), e3 );
                insideTest = *((__m128*) &_mm_cmpgt_epi32( *((__m128i*)&insideTest), *((__m128i*) &_ZERO) ));
                depthStore = _mm_xor_ps( depthPrev, _mm_and_ps( insideTest, _mm_xor_ps(depthMin, depthPrev) ) ); // Mask with edge function
                _mm_store_ps(pDepthBuffer, depthStore);

				//while(curX >= (startXClamped + OUT_TILE_SIZE_2X_F))
                for(int j=0; j < (endPixelBlockOffsetX - OUT_TILE_SIZE_2X); j+= OUT_TILE_SIZE_2X)
                {
                    //__TEC_INC_X
                    pDepthBuffer -= (OUT_TILE_SIZE_2X * QUAD_X_OFFSET);
                    e1 = _mm_sub_ps(e1, pRaster->e1XFinalAdd);
                    e2 = _mm_sub_ps(e2, pRaster->e2XFinalAdd);
                    e3 = _mm_sub_ps(e3, pRaster->e3XFinalAdd);
                    depth = _mm_sub_ps( depth, pRaster->depthXFinalAdd );

                    //__TEC_DO
                    __m128 depthPrev = *((__m128*) (pDepthBuffer));    // Prev
                    __m128 depthMin = _mm_min_ps( depth, depthPrev );                       // Check depth test
                    _CHECK_DEPTH_EDGE(pDepthBuffer, depthMin, e1, e2, e3);

                    __m128 insideTest = _mm_or_ps( _mm_or_ps( e1, e2 ), e3 );
                    insideTest = *((__m128*) &_mm_cmpgt_epi32( *((__m128i*)&insideTest), *((__m128i*) &_ZERO) ));
                    __m128 depthStore = _mm_xor_ps( depthPrev, _mm_and_ps( insideTest, _mm_xor_ps(depthMin, depthPrev) ) ); // Mask with edge function
                    _mm_store_ps(pDepthBuffer, depthStore);
				}

				//__TEC_INC_Y
                pDepthBuffer += (OUT_TILE_SIZE_2X * RASTERIZE_BUFFER_W);
                e1 = _mm_add_ps(e1, pRaster->e1YFinalAdd);
                e2 = _mm_add_ps(e2, pRaster->e2YFinalAdd);
                e3 = _mm_add_ps(e3, pRaster->e3YFinalAdd);
                depth = _mm_add_ps( depth, pRaster->depthYFinalAdd );
			}

			// Do odd number row
			//if(curY < endYClamped)
            if( pDepthBuffer < pEndDepthAddressOffsetTopBlock )
			{
                // Write out to depth with mask
                __m128 depthPrev = *((__m128*) (pDepthBuffer));    // Prev
                __m128 depthMin = _mm_min_ps( depth, depthPrev );                       // Check depth test
                _CHECK_DEPTH_EDGE(pDepthBuffer, depthMin, e1, e2, e3);

                __m128 insideTest = _mm_or_ps( _mm_or_ps( e1, e2 ), e3 );
                insideTest = *((__m128*) &_mm_cmpgt_epi32( *((__m128i*)&insideTest), *((__m128i*) &_ZERO) ));
                __m128 depthStore = _mm_xor_ps( depthPrev, _mm_and_ps( insideTest, _mm_xor_ps(depthMin, depthPrev) ) ); // Mask with edge function
                _mm_store_ps(pDepthBuffer, depthStore);

				//while(curX < (endXClamped - OUT_TILE_SIZE_2X_F))
                for(int j=0; j < (endPixelBlockOffsetX - OUT_TILE_SIZE_2X); j += OUT_TILE_SIZE_2X)
                {
                    //__TEC_INC_X
                    pDepthBuffer += (OUT_TILE_SIZE_2X * QUAD_X_OFFSET);
                    e1 = _mm_add_ps(e1, pRaster->e1XFinalAdd);
                    e2 = _mm_add_ps(e2, pRaster->e2XFinalAdd);
                    e3 = _mm_add_ps(e3, pRaster->e3XFinalAdd);
                    depth = _mm_add_ps( depth, pRaster->depthXFinalAdd );

                    //__TEC_DO
                    __m128 depthPrev = *((__m128*) (pDepthBuffer));    // Prev
                    __m128 depthMin = _mm_min_ps( depth, depthPrev );                       // Check depth test
                    _CHECK_DEPTH_EDGE(pDepthBuffer, depthMin, e1, e2, e3);

                    __m128 insideTest = _mm_or_ps( _mm_or_ps( e1, e2 ), e3 );
                    insideTest = *((__m128*) &_mm_cmpgt_epi32( *((__m128i*)&insideTest), *((__m128i*) &_ZERO) ));
                    __m128 depthStore = _mm_xor_ps( depthPrev, _mm_and_ps( insideTest, _mm_xor_ps(depthMin, depthPrev) ) ); // Mask with edge function
                    _mm_store_ps(pDepthBuffer, depthStore);
				}
			}
		}
	}

	//*pNumPartial += ((size_t)( pOutTilesPartialStrict - pOutTilesPartial )) >> 1;
	//*pNumFull += ((size_t)( pOutTilesFullStrict - pOutTilesFull )) >> 1;
}

template<uint outTileSize>
void TilesEdgeCheckWithBB1LayerDepth(const void* pRasterInfo, float* pDepthBuffer, float startX, float startY, uint bufferWidth, uint bufferHeight)
{
	const RasterInfoDepth<1>* __restrict pRaster = ((RasterInfoDepth<1>*) pRasterInfo);

	const uint OUT_TILE_SIZE        = outTileSize;
    const int OUT_TILE_SIZE_2X    = (OUT_TILE_SIZE << 1);
    const float OUT_TILE_SIZE_F     = (float) OUT_TILE_SIZE;
	const float OUT_TILE_SIZE_2X_F  = OUT_TILE_SIZE_F * 2.0f;
	const float IN_TILE_X_F         = (float) bufferWidth - 1.0f;
	const float IN_TILE_Y_F         = (float) bufferHeight - 1.0f;

	_DEBUG_ASSERT( ((bufferWidth / OUT_TILE_SIZE_2X) & 0x1) == 0 );
	_DEBUG_ASSERT( ((bufferHeight / OUT_TILE_SIZE_2X) & 0x1) == 0 );
	//const uint NUM_OUT_TILE_XY = inTileSize / (OUT_TILE_SIZE << 1);
	//const uint NUM_OUT_TILE_WIDTH = bufferWidth / (OUT_TILE_SIZE << 1);
	//const uint NUM_OUT_TILE_HEIGHT = bufferHeight / (OUT_TILE_SIZE << 1);

	const static _ALIGN(16) float QUAD_IN_TILE_OFFSETS_X[] = {0.0f, OUT_TILE_SIZE_F, 0.0f, OUT_TILE_SIZE_F};
	const static _ALIGN(16) float QUAD_IN_TILE_OFFSETS_Y[] = {0.0f, 0.0f, OUT_TILE_SIZE_F, OUT_TILE_SIZE_F};

    const uint QUAD_X_OFFSET = 2;

	{
		float startXClamped = startX > pRaster->triBB[0] ? startX : pRaster->triBB[0];
		float startYClamped = startY > pRaster->triBB[1] ? startY : pRaster->triBB[1];
		float endX = startX + IN_TILE_X_F;
		float endY = startY + IN_TILE_Y_F;
		float endXClamped = endX < pRaster->triBB[2] ? endX : pRaster->triBB[2];
		float endYClamped = endY < pRaster->triBB[3] ? endY : pRaster->triBB[3];

		if(startXClamped < endXClamped)
		{
			// Start at tile pixel corners
			float curX = startXClamped;
			float curY = startYClamped;
         
            __m128 xxxx = _mm_load_ps1(&curX);
            xxxx = _mm_add_ps(xxxx, _mm_load_ps(QUAD_IN_TILE_OFFSETS_X));
            __m128 yyyy = _mm_load_ps1(&curY);
            yyyy = _mm_add_ps(yyyy, _mm_load_ps(QUAD_IN_TILE_OFFSETS_Y));

            __m128 e1TopBlock = _mm_mul_ps(pRaster->e1aaaa, xxxx);
            e1TopBlock = _mm_add_ps( e1TopBlock, _mm_mul_ps(pRaster->e1bbbb, yyyy) );
            e1TopBlock = _mm_add_ps( e1TopBlock, pRaster->e1cccc );
            
            __m128 e2TopBlock = _mm_mul_ps(pRaster->e2aaaa, xxxx);
            e2TopBlock = _mm_add_ps( e2TopBlock, _mm_mul_ps(pRaster->e2bbbb, yyyy) );
            e2TopBlock = _mm_add_ps( e2TopBlock, pRaster->e2cccc );

            __m128 e3TopBlock = _mm_mul_ps(pRaster->e3aaaa, xxxx);
            e3TopBlock = _mm_add_ps( e3TopBlock, _mm_mul_ps(pRaster->e3bbbb, yyyy) );
            e3TopBlock = _mm_add_ps( e3TopBlock, pRaster->e3cccc );
            //__m128 e3Reject = _mm_add_ps( e3, pRaster->e3RejectLayerCCCC[0] );

            int pixelBlockX = _mm_cvtt_ss2si(xxxx);
            int pixelBlockY = _mm_cvtt_ss2si(yyyy);
            int endPixelBlockOffsetX = _mm_cvtt_ss2si( _mm_set_ss(endXClamped) ) - pixelBlockX;
            int endPixelBlockOffsetY = _mm_cvtt_ss2si( _mm_set_ss(endYClamped) ) - pixelBlockY;

            pDepthBuffer += pixelBlockY * RASTERIZE_BUFFER_W + (pixelBlockX * QUAD_X_OFFSET);
            float* pEndDepthAddressOffsetTopBlock = pDepthBuffer + endPixelBlockOffsetY * RASTERIZE_BUFFER_W;
      
			__m128 depthX = _mm_mul_ps(xxxx, pRaster->depthAAAA);
            __m128 depthY = _mm_mul_ps(yyyy, pRaster->depthBBBB);
            __m128 depthTopBlock = _mm_add_ps(depthX, depthY);
            depthTopBlock = _mm_add_ps( depthTopBlock, pRaster->depthCCCC );

            //while(curY < (endYClamped - OUT_TILE_SIZE_2X_F))

            // TODO: This might have a problem if the i falls exactly on the tile boundary of endPixelBlockOffsetY - OUT_TILE_SIZE_2X
            // Since the BB is always rounded to multiples of OUT_TILE_SIZE, if i == (endPixelBlockOffsetY), then the pixel tiles starting at endPixelBlockOffsetY
            // will be ignored according to the loop below.
            // This works if the BB is always 1 OUT_TILE_SIZE more than what is needed, and if BUFFER_W and BUFFER_H is always a pow of 2
           
            for(int i=0; i < (endPixelBlockOffsetY - OUT_TILE_SIZE_2X); i += (OUT_TILE_SIZE_2X * 2))
			{
                QuadEdgeCheckDepth<outTileSize>(pRasterInfo, pDepthBuffer, depthTopBlock, e1TopBlock, e2TopBlock, e3TopBlock);

				//while(curX < (endXClamped - OUT_TILE_SIZE_2X_F))
                for(int j=0; j < (endPixelBlockOffsetX - OUT_TILE_SIZE_2X); j += OUT_TILE_SIZE_2X)
				{
					//__TEC_INC_X
                    e1TopBlock = _mm_add_ps(e1TopBlock, pRaster->e1XLayerAdd[0]);
                    e2TopBlock = _mm_add_ps(e2TopBlock, pRaster->e2XLayerAdd[0]);
                    e3TopBlock = _mm_add_ps(e3TopBlock, pRaster->e3XLayerAdd[0]);
                    depthTopBlock = _mm_add_ps(depthTopBlock, pRaster->depthXLayerAdd[0]);
                    pDepthBuffer += (OUT_TILE_SIZE_2X * QUAD_X_OFFSET);
                    
                    //__TEC_DO
                    QuadEdgeCheckDepth<outTileSize>(pRasterInfo, pDepthBuffer, depthTopBlock, e1TopBlock, e2TopBlock, e3TopBlock);
				}

				//__TEC_INC_Y
                e1TopBlock = _mm_add_ps(e1TopBlock, pRaster->e1YLayerAdd[0]);
                e2TopBlock = _mm_add_ps(e2TopBlock, pRaster->e2YLayerAdd[0]);
                e3TopBlock = _mm_add_ps(e3TopBlock, pRaster->e3YLayerAdd[0]);
                depthTopBlock = _mm_add_ps(depthTopBlock, pRaster->depthYLayerAdd[0]);
                pDepthBuffer += (OUT_TILE_SIZE_2X * RASTERIZE_BUFFER_W);

				//__TEC_DO
                QuadEdgeCheckDepth<outTileSize>(pRasterInfo, pDepthBuffer, depthTopBlock, e1TopBlock, e2TopBlock, e3TopBlock);

				//while(curX >= (startXClamped + OUT_TILE_SIZE_2X_F))
                for(int j=0; j < (endPixelBlockOffsetX - OUT_TILE_SIZE_2X); j+= OUT_TILE_SIZE_2X)
                {
                    //__TEC_DEC_X
                    e1TopBlock = _mm_sub_ps(e1TopBlock, pRaster->e1XLayerAdd[0]);
                    e2TopBlock = _mm_sub_ps(e2TopBlock, pRaster->e2XLayerAdd[0]);
                    e3TopBlock = _mm_sub_ps(e3TopBlock, pRaster->e3XLayerAdd[0]);
                    depthTopBlock = _mm_sub_ps(depthTopBlock, pRaster->depthXLayerAdd[0]);
                    pDepthBuffer -= (OUT_TILE_SIZE_2X * QUAD_X_OFFSET);

                    //__TEC_DO
                    QuadEdgeCheckDepth<outTileSize>(pRasterInfo, pDepthBuffer, depthTopBlock, e1TopBlock, e2TopBlock, e3TopBlock);
				}

				//__TEC_INC_Y
                e1TopBlock = _mm_add_ps(e1TopBlock, pRaster->e1YLayerAdd[0]);
                e2TopBlock = _mm_add_ps(e2TopBlock, pRaster->e2YLayerAdd[0]);
                e3TopBlock = _mm_add_ps(e3TopBlock, pRaster->e3YLayerAdd[0]);
                depthTopBlock = _mm_add_ps(depthTopBlock, pRaster->depthYLayerAdd[0]);
                pDepthBuffer += (OUT_TILE_SIZE_2X * RASTERIZE_BUFFER_W);
			}

			// Do odd number row
			//if(curY < endYClamped)
            if( pDepthBuffer < pEndDepthAddressOffsetTopBlock )
			{
				//__TEC_DO
                QuadEdgeCheckDepth<outTileSize>(pRasterInfo, pDepthBuffer, depthTopBlock, e1TopBlock, e2TopBlock, e3TopBlock);

				//while(curX < (endXClamped - OUT_TILE_SIZE_2X_F))
                for(int j=0; j < (endPixelBlockOffsetX - OUT_TILE_SIZE_2X); j += OUT_TILE_SIZE_2X)
                {
                    //__TEC_INC_X
                    e1TopBlock = _mm_add_ps(e1TopBlock, pRaster->e1XLayerAdd[0]);
                    e2TopBlock = _mm_add_ps(e2TopBlock, pRaster->e2XLayerAdd[0]);
                    e3TopBlock = _mm_add_ps(e3TopBlock, pRaster->e3XLayerAdd[0]);
                    depthTopBlock = _mm_add_ps(depthTopBlock, pRaster->depthXLayerAdd[0]);
                    pDepthBuffer += (OUT_TILE_SIZE_2X * QUAD_X_OFFSET);

                    //__TEC_DO
                    QuadEdgeCheckDepth<outTileSize>(pRasterInfo, pDepthBuffer, depthTopBlock, e1TopBlock, e2TopBlock, e3TopBlock);
				}
			}
		}
	}

	//*pNumPartial += ((size_t)( pOutTilesPartialStrict - pOutTilesPartial )) >> 1;
	//*pNumFull += ((size_t)( pOutTilesFullStrict - pOutTilesFull )) >> 1;
}

_NAMESPACE_END