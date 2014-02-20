//==============================================================================
//
//		TriangleSetupDepthInt_SSE.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		17/02/2014
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN 

template<uint binWidth, uint binHeight, uint bufferWidth, uint bufferHeight>
void TriangleSetupDepthIntBatch(const gmtl::VecA4f* pVertices, const ushort* pIndices, uint numTriangles, TriangleBin* pBins, uint* pNumTrisInBins)
{
    _DEBUG_COMPILE_ASSERT((bufferWidth % binWidth) == 0);
    _DEBUG_COMPILE_ASSERT((bufferHeight % binHeight) == 0);

    const uint NUM_BINS_X = bufferWidth / binWidth;
    const uint NUM_BINS_Y = bufferHeight / binHeight;
    //_LOOPi(numTriangles)
    //{
    //    __m128 vert1 = _mm_load_ps((float*) (pVertices + pIndices[i]));
    //    __m128 vert2 = _mm_load_ps((float*) (pVertices + pIndices[i+1]));
    //    __m128 vert3 = _mm_load_ps((float*) (pVertices + pIndices[i+2]));

    //    __m128 wRcp = _mm_rcp_ps( _mm_shuffle_ps(vert1, vert1, _MM_SHUFFLE(3, 3, 3, 3)) );
    //    __m128 vProj = _mm_mul_ps( vert1, wRcp );
    //    __m128 notNearClip = _mm_cmpge_ps( _mm_shuffle_ps(vert1, vert1, _MM_SHUFFLE(2, 2, 2, 2)), _mm_setzero_ps() );
    //    __m128 v1Proj = _mm_and_ps( vProj, notNearClip );

    //    wRcp = _mm_rcp_ps( _mm_shuffle_ps(vert2, vert2, _MM_SHUFFLE(3, 3, 3, 3)) );
    //    vProj = _mm_mul_ps( vert2, wRcp );
    //    notNearClip = _mm_cmpge_ps( _mm_shuffle_ps(vert2, vert2, _MM_SHUFFLE(2, 2, 2, 2)), _mm_setzero_ps() );
    //    __m128 v2Proj = _mm_and_ps( vProj, notNearClip );

    //    wRcp = _mm_rcp_ps( _mm_shuffle_ps(vert3, vert3, _MM_SHUFFLE(3, 3, 3, 3)) );
    //    vProj = _mm_mul_ps( vert3, wRcp );
    //    notNearClip = _mm_cmpge_ps( _mm_shuffle_ps(vert3, vert3, _MM_SHUFFLE(2, 2, 2, 2)), _mm_setzero_ps() );
    //    __m128 v3Proj = _mm_and_ps( vProj, notNearClip );
    //}

    // Assumes that all vertices have been transformed to viewport space - top left corner is (0, 0)
    // NOTE: We should center the viewport such that (0, 0) is in the center rather than the corner
    uint numTrisProcessed = 0;
    while(numTrisProcessed < numTriangles)
    {
        // Process 4 triangles at a time
        uint numTrisGather = 4;
        if((numTrisGather + numTrisProcessed) > numTriangles)
            numTrisGather = numTriangles - numTrisProcessed;

        const ushort* pTriIndices1 = pIndices + numTrisProcessed*3;
        const ushort* pTriIndices2 = pIndices + numTrisProcessed*3 + (numTrisGather > 1 ? 3 : 0);
        const ushort* pTriIndices3 = pIndices + numTrisProcessed*3 + (numTrisGather > 2 ? 6 : 0);
        const ushort* pTriIndices4 = pIndices + numTrisProcessed*3 + (numTrisGather > 3 ? 9 : 0);
   
        __m128 vZ[3];
        __m128 vW[3];
        __m128i fxPtX[3], fxPtY[3];
        __m128i fxPackedXY[3];
       // __m128 vZ[3];

        // For each vertex in triangles
        _LOOPi(3)
        {
            // Transform data from AOS to SOA
            __m128 v0 = _mm_load_ps( (float*) (pVertices + pTriIndices1[i]) );
            __m128 v1 = _mm_load_ps( (float*) (pVertices + pTriIndices2[i]) );
            __m128 v2 = _mm_load_ps( (float*) (pVertices + pTriIndices3[i]) );
            __m128 v3 = _mm_load_ps( (float*) (pVertices + pTriIndices4[i]) );
             _MM_TRANSPOSE4_PS(v0, v1, v2, v3);
            __m128 vX = v0;
            __m128 vY = v1;
            vZ[i] = v2;
            vW[i] = v3;

            // Warning: All fixed point integers need to be clipped to [-16384, 16383] in order to avoid overflows
            // Do viewport transform
            vX = _mm_add_ps( _mm_mul_ps(vX, _mm_set1_ps(0.5f * RASTERIZE_BUFFER_W)), _mm_set1_ps(0.5f * RASTERIZE_BUFFER_W - 0.5f / RASTERIZE_BUFFER_W) );

            // Convert X coordinates of triangles for vertex i
            fxPtX[i] = _mm_cvtps_epi32(vX); 

            // Viewport transform for y (also flip y so why starts from top in viewport space)
            vY = _mm_add_ps( _mm_mul_ps(vY, _mm_set1_ps(-0.5f * RASTERIZE_BUFFER_H)), _mm_set1_ps(0.5f * RASTERIZE_BUFFER_H - 0.5f / RASTERIZE_BUFFER_H) );

            // Convert Y coordinates of triangles for vertex i
            fxPtY[i] = _mm_cvtps_epi32(vY); 

            // Pack X and Y of vertex i for triangles into signed 16-bit integers
            __m128i inter0 = _mm_unpacklo_epi32(fxPtX[i], fxPtY[i]);
            __m128i inter1 = _mm_unpackhi_epi32(fxPtX[i], fxPtY[i]);
            fxPackedXY[i] = _mm_packs_epi32(inter0, inter1);
            //vZ[i] = xformedPos[i].Z;
        }

        // Compute 2x tri area using cross product
        __m128i ad = _mm_sub_epi32(fxPtX[1], fxPtX[0]);
        ad = _mm_mullo_epi32(ad, _mm_sub_epi32(fxPtY[2], fxPtY[0]));

        __m128i bc = _mm_sub_epi32(fxPtX[2], fxPtX[0]);
        bc = _mm_mullo_epi32(bc, _mm_sub_epi32(fxPtY[1], fxPtY[0]));

        __m128i triArea = _mm_sub_epi32(ad, bc);
        __m128 oneOverTriArea = _mm_rcp_ps(_mm_cvtepi32_ps(triArea));

        // Z setup
        // This will be used in interpolation later when we derive the area of the triangle
        // using the line equation (actually 2x area of triangle)
        vZ[1] = _mm_mul_ps(_mm_sub_ps(vZ[1], vZ[0]), oneOverTriArea);
        vZ[2] = _mm_mul_ps(_mm_sub_ps(vZ[2], vZ[0]), oneOverTriArea);

        // Find bounding box for screen space triangle in terms of pixels
        __m128i vStartX = _mm_max_epi32( _mm_min_epi32( _mm_min_epi32(fxPtX[0], fxPtX[1]), fxPtX[2] ), _mm_set1_epi32(0) );
        __m128i vEndX   = _mm_min_epi32( _mm_max_epi32( _mm_max_epi32(fxPtX[0], fxPtX[1]), fxPtX[2] ), _mm_set1_epi32(bufferWidth - 1) );

        __m128i vStartY = _mm_max_epi32( _mm_min_epi32( _mm_min_epi32(fxPtY[0], fxPtY[1]), fxPtY[2] ), _mm_set1_epi32(0) );
        __m128i vEndY   = _mm_min_epi32( _mm_max_epi32( _mm_max_epi32(fxPtY[0], fxPtY[1]), fxPtY[2] ), _mm_set1_epi32(bufferHeight - 1) );

        // Cull triangles that are back-facing
        __m128i isFront = _mm_cmpgt_epi32(triArea, _mm_setzero_si128());

        // Cull triangles whose bounding box fall outside the buffer bounds
        __m128i isInsideBoundsX = _mm_cmpgt_epi32(vEndX, vStartX);
        __m128i isInsideBoundsY = _mm_cmpgt_epi32(vEndY, vStartY);

        // Cull triangles if any vertex is clipped by near plane
        __m128 isNotNearClippedV1 = _mm_cmpgt_ps(vW[0], _mm_setzero_ps());
        __m128 isNotNearClippedV2 = _mm_cmpgt_ps(vW[1], _mm_setzero_ps());
        __m128 isNotNearClippedV3 = _mm_cmpgt_ps(vW[2], _mm_setzero_ps());

        __m128 tmpAccept = _mm_castsi128_ps( _mm_and_si128( _mm_and_si128(isFront, isInsideBoundsX), isInsideBoundsY) );
        __m128 accept = _mm_and_ps(_mm_and_ps(isNotNearClippedV1, isNotNearClippedV2), _mm_and_ps(isNotNearClippedV3, tmpAccept));

        uint triMask = _mm_movemask_ps(accept) & ((1 << 4) - 1);

        // Add each valid triangle to bin
        while(triMask)
        {
            uint triangleIndex;
            _BitScanForward((unsigned long*) &triangleIndex, triMask);
            
            // Convert bounding box in terms of pixels to bounding box in terms of tiles
            int startX = max(vStartX.m128i_i32[triangleIndex] / binWidth, 0);
            int endX   = min(vEndX.m128i_i32[triangleIndex] / binWidth, bufferWidth - 1);
            int startY = max(vStartY.m128i_i32[triangleIndex] / binHeight, 0);
            int endY   = min(vEndY.m128i_i32[triangleIndex] / binHeight, bufferHeight - 1);

            // Add triangle to the tiles or bins that the bounding box covers
            for(int i = startY; i <= endY; i++)
            {
                for(int j = startX; j <= endX; j++)
                {
                    int triBinOffset = i * NUM_BINS_X + j;
                    TriangleData* pTriData = &( pBins[triBinOffset].tris[ pNumTrisInBins[triBinOffset] ] );
                    pTriData->xy[0] = fxPackedXY[0].m128i_i32[triangleIndex];
                    pTriData->xy[1] = fxPackedXY[1].m128i_i32[triangleIndex];
                    pTriData->xy[2] = fxPackedXY[2].m128i_i32[triangleIndex];
                    pTriData->z[0] = vZ[0].m128_f32[triangleIndex];
                    pTriData->z[1] = vZ[1].m128_f32[triangleIndex];
                    pTriData->z[2] = vZ[2].m128_f32[triangleIndex];
                    pNumTrisInBins[triBinOffset] += 1;
                }
            }

            triMask &= (triMask - 1);           
        }

        numTrisProcessed += 4;
    }
}

_NAMESPACE_END