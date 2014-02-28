//==============================================================================
//
//		TilesRasterizeDepthInt_SSE.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		17/02/2014
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN 

template<uint binWidth, uint binHeight, uint bufferWidth, uint bufferHeight, uint numContexts>
void TilesRasterizeDepthInt(const TriangleBin* pBins, const uint* pNumTrisInBins, float* pDepthBuffer, int binStartX, int binStartY)
{
    //_mm_setcsr( _mm_getcsr() | 0x8040 );

    const uint NUM_BINS_X = bufferWidth / binWidth;
    const uint NUM_BINS_Y = bufferHeight / binHeight;

    __m128i colOffset = _mm_setr_epi32(0, 1, 0, 1);
    __m128i rowOffset = _mm_setr_epi32(0, 0, 1, 1);

    // Based on TaskId determine which tile to process
    //UINT tileX = taskId % screenWidthInTiles;
    //UINT tileY = taskId / screenWidthInTiles;

    int tileStartX = binStartX * binWidth;
    int tileEndX   = min(tileStartX + binWidth - 1, bufferWidth - 1);

    int tileStartY = binStartY * binHeight;
    int tileEndY   = min(tileStartY + binHeight - 1, bufferHeight - 1);

    _DEBUG_ASSERT( (tileStartX & 1) == 0 && (tileStartY & 1) == 0 );
    _DEBUG_ASSERT( (tileEndX & 1) == 1 && (tileEndY & 1) == 1 );

    // Clear tile buffer to 1.0
    // Note we need to account for tiling pattern here
    __m128 clearVal = _mm_set1_ps(1.0f);
    for(int i=tileStartY; i < tileEndY; i+=2)
    {
        float* pToClear = (pDepthBuffer + (i * bufferWidth + 2 * tileStartX));       
        for(int j=0; j < binWidth; j+=2)
        {
            //_mm_stream_ps(pToClear, clearVal);
            _mm_store_ps(pToClear, clearVal);
            pToClear += 4;
        }

        //memset(&pDepthBuffer[index], 0, sizeof(float) * 2 * binWidth);
    }

    // Process all triangles in all contexts for this bin
    uint curContext = 0;
    const TriangleBin* pCurBin = pBins + (curContext * NUM_BINS_X * NUM_BINS_Y);
    const uint* pCurNumTrisInBin = pNumTrisInBins + (curContext * NUM_BINS_X * NUM_BINS_Y);
    uint numTrisInBin = *pCurNumTrisInBin;
    uint numTrisFetchedFromBin = 0;

    __m128 triVerts[4][2];

    do 
    {
        // Grab 4 triangles from bins to process
        uint numTrisFetched = 0;
        while(numTrisFetched < 4)
        { 
            // Search for the next bin with triangles to process
            while(numTrisFetchedFromBin >= numTrisInBin)
            {
                curContext++;
                if(curContext >= numContexts)
                {
                    // We have no more bins
                    // So either we're all done
                    if(numTrisFetched == 0)
                        return;

                    // Or we still have a final batch of triangles to process
                    goto LABEL_PROCESS_TRIANGLES;
                }

                pCurBin += (NUM_BINS_X * NUM_BINS_Y);
                pCurNumTrisInBin += (NUM_BINS_X * NUM_BINS_Y);
                numTrisInBin = *pCurNumTrisInBin;
                numTrisFetchedFromBin = 0;
            }

            // Grab 1 triangle from the bin
            triVerts[numTrisFetched][0] = _mm_castsi128_ps( _mm_loadu_si128((const __m128i*) &(pCurBin->tris[numTrisFetchedFromBin].xy[0])) );
            triVerts[numTrisFetched][1] = _mm_castsi128_ps( _mm_loadl_epi64((const __m128i*) &(pCurBin->tris[numTrisFetchedFromBin].z[1])) );

            numTrisFetchedFromBin++;
            numTrisFetched++;
        }

    LABEL_PROCESS_TRIANGLES:

        // Convert vertex for each triangle into SOA       
        __m128 v0 = triVerts[0][0];
        __m128 v1 = triVerts[1][0];
        __m128 v2 = triVerts[2][0];
        __m128 v3 = triVerts[3][0];
       
        _MM_TRANSPOSE4_PS(v0, v1, v2, v3);

        // Now v0, v1, v2 contain the corresponding verts
        // v3 also contains Z[0] but we don't care here

        // Extract X and Y from each vertex of each triangle
        __m128i fxPtX[3];
        __m128i fxPtY[3];
        fxPtX[0] = _mm_srai_epi32(_mm_slli_epi32(_mm_castps_si128(v0), 16), 16);    // Contains vertex 0 X for each triangle
        fxPtY[0] = _mm_srai_epi32(_mm_castps_si128(v0), 16);                        // Contains vertex 0 Y for each triangle
        fxPtX[1] = _mm_srai_epi32(_mm_slli_epi32(_mm_castps_si128(v1), 16), 16);    // Contains vertex 1 X for each triangle
        fxPtY[1] = _mm_srai_epi32(_mm_castps_si128(v1), 16);                        // Contains vertex 1 Y for each triangle
        fxPtX[2] = _mm_srai_epi32(_mm_slli_epi32(_mm_castps_si128(v2), 16), 16);    // Contains vertex 2 X for each triangle
        fxPtY[2] = _mm_srai_epi32(_mm_castps_si128(v2), 16);                        // Contains vertex 2 Y for each triangle

        // Note that since we convert Y from clip space to screen space, with 0 starting from the top
        // the line equations in screen space are in fact computed clockwise
        // Fab(x, y) =     Ax       +       By     +      C              = 0
        // Fab(x, y) = (ya - yb)x   +   (xb - xa)y + (xa * yb - xb * ya) = 0
        // Compute A = (ya - yb) for the 3 line segments that make up each triangle
        __m128i A0 = _mm_sub_epi32(fxPtY[1], fxPtY[2]);
        __m128i A1 = _mm_sub_epi32(fxPtY[2], fxPtY[0]);
        __m128i A2 = _mm_sub_epi32(fxPtY[0], fxPtY[1]);

        // Compute B = (xb - xa) for the 3 line segments that make up each triangle
        __m128i B0 = _mm_sub_epi32(fxPtX[2], fxPtX[1]);
        __m128i B1 = _mm_sub_epi32(fxPtX[0], fxPtX[2]);
        __m128i B2 = _mm_sub_epi32(fxPtX[1], fxPtX[0]);

        // Compute C = (xa * yb - xb * ya) for the 3 line segments that make up each triangle
        __m128i C0 = _mm_sub_epi32( _mm_mullo_epi32(fxPtX[1], fxPtY[2]), _mm_mullo_epi32(fxPtX[2], fxPtY[1]) );
        __m128i C1 = _mm_sub_epi32( _mm_mullo_epi32(fxPtX[2], fxPtY[0]), _mm_mullo_epi32(fxPtX[0], fxPtY[2]) );
        __m128i C2 = _mm_sub_epi32( _mm_mullo_epi32(fxPtX[0], fxPtY[1]), _mm_mullo_epi32(fxPtX[1], fxPtY[0]) );

        // Use bounding box traversal strategy to determine which pixels to rasterize
        // This guarantees the starting point is always rounded to the nearest even pixel, conservatively
        __m128i startX = _mm_and_si128( _mm_max_epi32( _mm_min_epi32( _mm_min_epi32(fxPtX[0], fxPtX[1]), fxPtX[2] ), _mm_set1_epi32(tileStartX) ), _mm_set1_epi32(0xFFFFFFFE) );    
 
        // This guarantees the end point always ends on the next pixel (but less than the tileEnd) because the inner loop increments by 2
        // and we want to make sure that the end pixel also gets rasterized (because < is used instead of <=)
        //__m128i endX   = _mm_min_epi32( _mm_add_epi32( _mm_max_epi32( _mm_max_epi32(fxPtX[0], fxPtX[1]), fxPtX[2] ), _mm_set1_epi32(1) ), _mm_set1_epi32(tileEndX) );   
        __m128i endX   = _mm_min_epi32( _mm_max_epi32( _mm_max_epi32(fxPtX[0], fxPtX[1]), fxPtX[2] ), _mm_set1_epi32(tileEndX) );   

        __m128i startY = _mm_and_si128( _mm_max_epi32( _mm_min_epi32( _mm_min_epi32(fxPtY[0], fxPtY[1]), fxPtY[2] ), _mm_set1_epi32(tileStartY) ), _mm_set1_epi32(0xFFFFFFFE) );
        //__m128i endY   = _mm_min_epi32( _mm_add_epi32( _mm_max_epi32( _mm_max_epi32(fxPtY[0], fxPtY[1]), fxPtY[2] ), _mm_set1_epi32(1) ), _mm_set1_epi32(tileEndY) );
        __m128i endY   = _mm_min_epi32( _mm_max_epi32( _mm_max_epi32(fxPtY[0], fxPtY[1]), fxPtY[2] ), _mm_set1_epi32(tileEndY) );

       // __m128i diffY = _mm_add_epi32( _mm_sub_epi32( endY, startY ), _mm_set1_epi32(1) );
       // __m128i diffX = _mm_add_epi32( _mm_sub_epi32( endX, startX ), _mm_set1_epi32(1) );
       // __m128i diffY = _mm_sub_epi32( endY, startY );
       // __m128i diffX = _mm_sub_epi32( endX, startX );

        // Rasterize all triangles individually
        for(uint curTri=0; curTri < numTrisFetched; curTri++)
        {
            // Extract this triangle's properties from the SIMD versions

            // zz[0] contains Z[0]
            // zz[1] contains Z[1] - Z[0]
            // zz[2] contains Z[2] - Z[0]
            __m128 depth0;
            __m128 depthDiff1;
            __m128 depthDiff2;
            depth0 = _mm_set1_ps(triVerts[curTri][0].m128_f32[3]);
            depthDiff1 = _mm_set1_ps(triVerts[curTri][1].m128_f32[0]);
            depthDiff2 = _mm_set1_ps(triVerts[curTri][1].m128_f32[1]);

            int startXx = startX.m128i_i32[curTri];
            int startYy = startY.m128i_i32[curTri];
            int endYy	= endY.m128i_i32[curTri];
            int endXx   = endX.m128i_i32[curTri];

            // Incrementally compute Fab(x, y) for all the pixels inside the bounding box formed by (startX, endX) and (startY, endY) 
            __m128i aa0 = _mm_set1_epi32(A0.m128i_i32[curTri]);
            __m128i aa1 = _mm_set1_epi32(A1.m128i_i32[curTri]);
            __m128i aa2 = _mm_set1_epi32(A2.m128i_i32[curTri]);

            __m128i bb0 = _mm_set1_epi32(B0.m128i_i32[curTri]);
            __m128i bb1 = _mm_set1_epi32(B1.m128i_i32[curTri]);
            __m128i bb2 = _mm_set1_epi32(B2.m128i_i32[curTri]);

            // Find A * x for 2x2 quad
            __m128i col = _mm_add_epi32(colOffset, _mm_set1_epi32(startXx));
            __m128i aa0Col = _mm_mullo_epi32(aa0, col);
            __m128i aa1Col = _mm_mullo_epi32(aa1, col);
            __m128i aa2Col = _mm_mullo_epi32(aa2, col);

            // Find B * y + C for 2x2 quad
            __m128i row = _mm_add_epi32(rowOffset, _mm_set1_epi32(startYy));
            __m128i bb0Row = _mm_add_epi32(_mm_mullo_epi32(bb0, row), _mm_set1_epi32(C0.m128i_i32[curTri]));
            __m128i bb1Row = _mm_add_epi32(_mm_mullo_epi32(bb1, row), _mm_set1_epi32(C1.m128i_i32[curTri]));
            __m128i bb2Row = _mm_add_epi32(_mm_mullo_epi32(bb2, row), _mm_set1_epi32(C2.m128i_i32[curTri]));

            // Find line equation = A * x + B * y + C
            // This also effectively gives 2x area of triangle from 2 vertices to the current point (x, y)
            __m128i sum0Row = _mm_add_epi32(aa0Col, bb0Row);
            __m128i sum1Row = _mm_add_epi32(aa1Col, bb1Row);
            __m128i sum2Row = _mm_add_epi32(aa2Col, bb2Row);

            // Find increment for A, B and depth
            // We increment by 2 pixels
            __m128i aa0Inc = _mm_slli_epi32(aa0, 1);
            __m128i aa1Inc = _mm_slli_epi32(aa1, 1);
            __m128i aa2Inc = _mm_slli_epi32(aa2, 1);

            __m128i bb0Inc = _mm_slli_epi32(bb0, 1);
            __m128i bb1Inc = _mm_slli_epi32(bb1, 1);
            __m128i bb2Inc = _mm_slli_epi32(bb2, 1);

            // This is the increment of (Z * area of the triangle / total area)
            // Computing (Z[1] - Z[0]) * dAreaOfTriangle1 / AreaOfTriangle 
            __m128 depthXInc = _mm_mul_ps(_mm_cvtepi32_ps(aa1Inc), depthDiff1);

            // Computing (Z[2] - Z[0]) * dAreaOfTriangle2 / AreaOfTriangle
            depthXInc = _mm_add_ps(depthXInc, _mm_mul_ps(_mm_cvtepi32_ps(aa2Inc), depthDiff2));

            //__m128 depthYInc = _mm_mul_ps(_mm_cvtepi32_ps(bb1Inc), depthDiff1);
            //depthYInc = _mm_add_ps(depthYInc, _mm_mul_ps(_mm_cvtepi32_ps(bb2Inc), depthDiff2));

            //__m128 curDepth = depth0;
            //curDepth = _mm_add_ps(curDepth, _mm_mul_ps( _mm_cvtepi32_ps(sum1Row), depthDiff1 ));
            //curDepth = _mm_add_ps(curDepth, _mm_mul_ps( _mm_cvtepi32_ps(sum2Row), depthDiff2 ));

            // Tranverse pixels in 2x2 blocks and store 2x2 pixel quad depths contiguously in memory ==> 2*X
            int pixelIndex = (startYy * bufferWidth + 2 * startXx);
           
            // Rasterization inner loop
            // Incrementally compute Fab(x, y) for all the pixels inside the bounding box formed by (startX, endX) and (startY, endY)
            for(int i=startYy; i <= endYy; i+=2)
            {
                // Compute barycentric coordinates 
                // Z = Z0 * A0/A + Z1 * A1/A + Z2 * A2/A
                //   = Z0 + (Z1 - Z0) * A1/A + (Z2 - Z0) * A2/A
                // Where F1 = line equation for line 1 = 2x area of triangle from vertices on line 1 to current pt 
                // and F2 = ..
                int index = pixelIndex;
                __m128i alpha = sum0Row;
                __m128i beta = sum1Row;
                __m128i gamma = sum2Row;

                // Using Z0 + (Z1 - Z0) * A1/A + (Z2 - Z0) * A2/A, compute barycentric-interpolated depth
                __m128 depth = depth0;
                depth = _mm_add_ps(depth, _mm_mul_ps( _mm_cvtepi32_ps(beta), depthDiff1 ));
                depth = _mm_add_ps(depth, _mm_mul_ps( _mm_cvtepi32_ps(gamma), depthDiff2 ));
                //__m128 depth = curDepth;

                for(int j=startXx; j < endXx; j+=2)
                {
                    // Test pixel inside triangle
                    // Basically, check if any edge fn is negative
                    __m128i mask = _mm_or_si128(_mm_or_si128(alpha, beta), gamma);

                    //if(_mm_testc_si128(_mm_set1_epi32(0x80000000), mask) == 0)
                    {
                        __m128 previousDepthValue = _mm_load_ps(&pDepthBuffer[index]);
                        __m128 mergedDepth = _mm_min_ps(depth, previousDepthValue);
                        __m128 finalDepth = _mm_blendv_ps(mergedDepth, previousDepthValue, _mm_castsi128_ps(mask));
                        _mm_store_ps(&pDepthBuffer[index], finalDepth);
                    }

                    // NOTE: This will be compiled out if context > 1
                    //if(numContexts == 1)
                    //    g_NumPixelsProcessed += 4;

                    index += 4;
                    alpha = _mm_add_epi32(alpha, aa0Inc);
                    beta  = _mm_add_epi32(beta, aa1Inc);

                    gamma = _mm_add_epi32(gamma, aa2Inc);
                    depth = _mm_add_ps(depth, depthXInc);
                }

                pixelIndex += 2 * bufferWidth;
                sum0Row = _mm_add_epi32(sum0Row, bb0Inc);
                sum1Row = _mm_add_epi32(sum1Row, bb1Inc);
                sum2Row = _mm_add_epi32(sum2Row, bb2Inc);
               // curDepth = _mm_add_ps(curDepth, depthYInc);
            }
        }

    }while(true);
}


template<uint binWidth, uint binHeight, uint bufferWidth, uint bufferHeight, uint numContexts>
void TilesRasterizeDepthUnrollYInt(const TriangleBin* pBins, const uint* pNumTrisInBins, float* pDepthBuffer, int binStartX, int binStartY)
{
    //_mm_setcsr( _mm_getcsr() | 0x8040 );

    const uint NUM_BINS_X = bufferWidth / binWidth;
    const uint NUM_BINS_Y = bufferHeight / binHeight;

    __m128i colOffset = _mm_setr_epi32(0, 1, 0, 1);
    __m128i rowOffset = _mm_setr_epi32(0, 0, 1, 1);

    // Based on TaskId determine which tile to process
    //UINT tileX = taskId % screenWidthInTiles;
    //UINT tileY = taskId / screenWidthInTiles;

    int tileStartX = binStartX * binWidth;
    int tileEndX   = min(tileStartX + binWidth - 1, bufferWidth - 1);

    int tileStartY = binStartY * binHeight;
    int tileEndY   = min(tileStartY + binHeight - 1, bufferHeight - 1);

    _DEBUG_ASSERT( (tileStartX & 1) == 0 && (tileStartY & 1) == 0 );
    _DEBUG_ASSERT( (tileEndX & 1) == 1 && (tileEndY & 1) == 1 );

    // Clear tile buffer to 1.0
    // Note we need to account for tiling pattern here
    __m128 clearVal = _mm_set1_ps(1.0f);
    for(int i=tileStartY; i < tileEndY; i+=2)
    {
        float* pToClear = (pDepthBuffer + (i * bufferWidth + 2 * tileStartX));       
        for(int j=0; j < binWidth; j+=2)
        {
            //_mm_stream_ps(pToClear, clearVal);
            _mm_store_ps(pToClear, clearVal);
            pToClear += 4;
        }

        //memset(&pDepthBuffer[index], 0, sizeof(float) * 2 * binWidth);
    }

    // Process all triangles in all contexts for this bin
    uint curContext = 0;
    const TriangleBin* pCurBin = pBins + (curContext * NUM_BINS_X * NUM_BINS_Y);
    const uint* pCurNumTrisInBin = pNumTrisInBins + (curContext * NUM_BINS_X * NUM_BINS_Y);
    uint numTrisInBin = *pCurNumTrisInBin;
    uint numTrisFetchedFromBin = 0;

    __m128 triVerts[4][2];

    do 
    {
        // Grab 4 triangles from bins to process
        uint numTrisFetched = 0;
        while(numTrisFetched < 4)
        { 
            // Search for the next bin with triangles to process
            while(numTrisFetchedFromBin >= numTrisInBin)
            {
                curContext++;
                if(curContext >= numContexts)
                {
                    // We have no more bins
                    // So either we're all done
                    if(numTrisFetched == 0)
                        return;

                    // Or we still have a final batch of triangles to process
                    goto LABEL_PROCESS_TRIANGLES;
                }

                pCurBin += (NUM_BINS_X * NUM_BINS_Y);
                pCurNumTrisInBin += (NUM_BINS_X * NUM_BINS_Y);
                numTrisInBin = *pCurNumTrisInBin;
                numTrisFetchedFromBin = 0;
            }

            // Grab 1 triangle from the bin
            triVerts[numTrisFetched][0] = _mm_castsi128_ps( _mm_loadu_si128((const __m128i*) &(pCurBin->tris[numTrisFetchedFromBin].xy[0])) );
            triVerts[numTrisFetched][1] = _mm_castsi128_ps( _mm_loadl_epi64((const __m128i*) &(pCurBin->tris[numTrisFetchedFromBin].z[1])) );

            numTrisFetchedFromBin++;
            numTrisFetched++;
        }

LABEL_PROCESS_TRIANGLES:

        // Convert vertex for each triangle into SOA       
        __m128 v0 = triVerts[0][0];
        __m128 v1 = triVerts[1][0];
        __m128 v2 = triVerts[2][0];
        __m128 v3 = triVerts[3][0];

        _MM_TRANSPOSE4_PS(v0, v1, v2, v3);

        // Now v0, v1, v2 contain the corresponding verts
        // v3 also contains Z[0] but we don't care here

        // Extract X and Y from each vertex of each triangle
        __m128i fxPtX[3];
        __m128i fxPtY[3];
        fxPtX[0] = _mm_srai_epi32(_mm_slli_epi32(_mm_castps_si128(v0), 16), 16);    // Contains vertex 0 X for each triangle
        fxPtY[0] = _mm_srai_epi32(_mm_castps_si128(v0), 16);                        // Contains vertex 0 Y for each triangle
        fxPtX[1] = _mm_srai_epi32(_mm_slli_epi32(_mm_castps_si128(v1), 16), 16);    // Contains vertex 1 X for each triangle
        fxPtY[1] = _mm_srai_epi32(_mm_castps_si128(v1), 16);                        // Contains vertex 1 Y for each triangle
        fxPtX[2] = _mm_srai_epi32(_mm_slli_epi32(_mm_castps_si128(v2), 16), 16);    // Contains vertex 2 X for each triangle
        fxPtY[2] = _mm_srai_epi32(_mm_castps_si128(v2), 16);                        // Contains vertex 2 Y for each triangle

        // Note that since we convert Y from clip space to screen space, with 0 starting from the top
        // the line equations in screen space are in fact computed clockwise
        // Fab(x, y) =     Ax       +       By     +      C              = 0
        // Fab(x, y) = (ya - yb)x   +   (xb - xa)y + (xa * yb - xb * ya) = 0
        // Compute A = (ya - yb) for the 3 line segments that make up each triangle
        __m128i A0 = _mm_sub_epi32(fxPtY[1], fxPtY[2]);
        __m128i A1 = _mm_sub_epi32(fxPtY[2], fxPtY[0]);
        __m128i A2 = _mm_sub_epi32(fxPtY[0], fxPtY[1]);

        // Compute B = (xb - xa) for the 3 line segments that make up each triangle
        __m128i B0 = _mm_sub_epi32(fxPtX[2], fxPtX[1]);
        __m128i B1 = _mm_sub_epi32(fxPtX[0], fxPtX[2]);
        __m128i B2 = _mm_sub_epi32(fxPtX[1], fxPtX[0]);

        // Compute C = (xa * yb - xb * ya) for the 3 line segments that make up each triangle
        __m128i C0 = _mm_sub_epi32( _mm_mullo_epi32(fxPtX[1], fxPtY[2]), _mm_mullo_epi32(fxPtX[2], fxPtY[1]) );
        __m128i C1 = _mm_sub_epi32( _mm_mullo_epi32(fxPtX[2], fxPtY[0]), _mm_mullo_epi32(fxPtX[0], fxPtY[2]) );
        __m128i C2 = _mm_sub_epi32( _mm_mullo_epi32(fxPtX[0], fxPtY[1]), _mm_mullo_epi32(fxPtX[1], fxPtY[0]) );

        // Use bounding box traversal strategy to determine which pixels to rasterize
        // This guarantees the starting point is always rounded to the nearest even pixel, conservatively
        __m128i startX = _mm_and_si128( _mm_max_epi32( _mm_min_epi32( _mm_min_epi32(fxPtX[0], fxPtX[1]), fxPtX[2] ), _mm_set1_epi32(tileStartX) ), _mm_set1_epi32(0xFFFFFFFE) );    

        // This guarantees the end point always ends on the next pixel (but less than the tileEnd) because the inner loop increments by 2
        // and we want to make sure that the end pixel also gets rasterized (because < is used instead of <=)
        //__m128i endX   = _mm_min_epi32( _mm_add_epi32( _mm_max_epi32( _mm_max_epi32(fxPtX[0], fxPtX[1]), fxPtX[2] ), _mm_set1_epi32(1) ), _mm_set1_epi32(tileEndX) );   
        __m128i endX   = _mm_min_epi32( _mm_max_epi32( _mm_max_epi32(fxPtX[0], fxPtX[1]), fxPtX[2] ), _mm_set1_epi32(tileEndX) );   

        __m128i startY = _mm_and_si128( _mm_max_epi32( _mm_min_epi32( _mm_min_epi32(fxPtY[0], fxPtY[1]), fxPtY[2] ), _mm_set1_epi32(tileStartY) ), _mm_set1_epi32(0xFFFFFFFE) );
        //__m128i endY   = _mm_min_epi32( _mm_add_epi32( _mm_max_epi32( _mm_max_epi32(fxPtY[0], fxPtY[1]), fxPtY[2] ), _mm_set1_epi32(1) ), _mm_set1_epi32(tileEndY) );
        __m128i endY   = _mm_min_epi32( _mm_max_epi32( _mm_max_epi32(fxPtY[0], fxPtY[1]), fxPtY[2] ), _mm_set1_epi32(tileEndY) );

        // __m128i diffY = _mm_add_epi32( _mm_sub_epi32( endY, startY ), _mm_set1_epi32(1) );
        // __m128i diffX = _mm_add_epi32( _mm_sub_epi32( endX, startX ), _mm_set1_epi32(1) );
        __m128i diffY = _mm_add_epi32( _mm_srli_epi32(_mm_sub_epi32( endY, startY ), 1), _mm_set1_epi32(1) );
        __m128i diffX = _mm_add_epi32( _mm_srli_epi32(_mm_sub_epi32( endX, startX ), 1), _mm_set1_epi32(1) );

        // Rasterize all triangles individually
        for(uint curTri=0; curTri < numTrisFetched; curTri++)
        {
            // Extract this triangle's properties from the SIMD versions

            // zz[0] contains Z[0]
            // zz[1] contains Z[1] - Z[0]
            // zz[2] contains Z[2] - Z[0]
            __m128 depth0;
            __m128 depthDiff1;
            __m128 depthDiff2;
            depth0 = _mm_set1_ps(triVerts[curTri][0].m128_f32[3]);
            depthDiff1 = _mm_set1_ps(triVerts[curTri][1].m128_f32[0]);
            depthDiff2 = _mm_set1_ps(triVerts[curTri][1].m128_f32[1]);

            int startXx = startX.m128i_i32[curTri];
            int startYy = startY.m128i_i32[curTri];
            int numY	= (diffY.m128i_i32[curTri]);
            int numX    = (diffX.m128i_i32[curTri]);

            // Incrementally compute Fab(x, y) for all the pixels inside the bounding box formed by (startX, endX) and (startY, endY) 
            __m128i aa0 = _mm_set1_epi32(A0.m128i_i32[curTri]);
            __m128i aa1 = _mm_set1_epi32(A1.m128i_i32[curTri]);
            __m128i aa2 = _mm_set1_epi32(A2.m128i_i32[curTri]);

            __m128i bb0 = _mm_set1_epi32(B0.m128i_i32[curTri]);
            __m128i bb1 = _mm_set1_epi32(B1.m128i_i32[curTri]);
            __m128i bb2 = _mm_set1_epi32(B2.m128i_i32[curTri]);

            // Find A * x for 2x2 quad
            __m128i col = _mm_add_epi32(colOffset, _mm_set1_epi32(startXx));
            __m128i aa0Col = _mm_mullo_epi32(aa0, col);
            __m128i aa1Col = _mm_mullo_epi32(aa1, col);
            __m128i aa2Col = _mm_mullo_epi32(aa2, col);

            // Find B * y + C for 2x2 quad
            __m128i row = _mm_add_epi32(rowOffset, _mm_set1_epi32(startYy));
            __m128i bb0Row = _mm_add_epi32(_mm_mullo_epi32(bb0, row), _mm_set1_epi32(C0.m128i_i32[curTri]));
            __m128i bb1Row = _mm_add_epi32(_mm_mullo_epi32(bb1, row), _mm_set1_epi32(C1.m128i_i32[curTri]));
            __m128i bb2Row = _mm_add_epi32(_mm_mullo_epi32(bb2, row), _mm_set1_epi32(C2.m128i_i32[curTri]));

            // Find line equation = A * x + B * y + C
            // This also effectively gives 2x area of triangle from 2 vertices to the current point (x, y)
            __m128i sum0Row = _mm_add_epi32(aa0Col, bb0Row);
            __m128i sum1Row = _mm_add_epi32(aa1Col, bb1Row);
            __m128i sum2Row = _mm_add_epi32(aa2Col, bb2Row);

            // Find increment for A, B and depth
            // We increment by 2 pixels
            __m128i aa0Inc = _mm_slli_epi32(aa0, 1);
            __m128i aa1Inc = _mm_slli_epi32(aa1, 1);
            __m128i aa2Inc = _mm_slli_epi32(aa2, 1);

            __m128i bb0Inc = _mm_slli_epi32(bb0, 1);
            __m128i bb1Inc = _mm_slli_epi32(bb1, 1);
            __m128i bb2Inc = _mm_slli_epi32(bb2, 1);

            // This is the increment of (Z * area of the triangle / total area)
            // Computing (Z[1] - Z[0]) * dAreaOfTriangle1 / AreaOfTriangle 
            __m128 depthXInc = _mm_mul_ps(_mm_cvtepi32_ps(aa1Inc), depthDiff1);

            // Computing (Z[2] - Z[0]) * dAreaOfTriangle2 / AreaOfTriangle
            depthXInc = _mm_add_ps(depthXInc, _mm_mul_ps(_mm_cvtepi32_ps(aa2Inc), depthDiff2));

            //__m128 depthYInc = _mm_mul_ps(_mm_cvtepi32_ps(bb1Inc), depthDiff1);
            //depthYInc = _mm_add_ps(depthYInc, _mm_mul_ps(_mm_cvtepi32_ps(bb2Inc), depthDiff2));

            //__m128 curDepth = depth0;
            //curDepth = _mm_add_ps(curDepth, _mm_mul_ps( _mm_cvtepi32_ps(sum1Row), depthDiff1 ));
            //curDepth = _mm_add_ps(curDepth, _mm_mul_ps( _mm_cvtepi32_ps(sum2Row), depthDiff2 ));

            // Tranverse pixels in 2x2 blocks and store 2x2 pixel quad depths contiguously in memory ==> 2*X
            int pixelIndex = (startYy * bufferWidth + 2 * startXx);

            // Rasterization inner loop
            // Incrementally compute Fab(x, y) for all the pixels inside the bounding box formed by (startX, endX) and (startY, endY)         
            if(numY & 1)
            {
                // Compute barycentric coordinates 
                // Z = Z0 * A0/A + Z1 * A1/A + Z2 * A2/A
                //   = Z0 + (Z1 - Z0) * A1/A + (Z2 - Z0) * A2/A
                // Where F1 = line equation for line 1 = 2x area of triangle from vertices on line 1 to current pt 
                // and F2 = ..
                int index = pixelIndex;
                __m128i alpha = sum0Row;
                __m128i beta = sum1Row;
                __m128i gamma = sum2Row;

                // Using Z0 + (Z1 - Z0) * A1/A + (Z2 - Z0) * A2/A, compute barycentric-interpolated depth
                __m128 depth = depth0;
                depth = _mm_add_ps(depth, _mm_mul_ps( _mm_cvtepi32_ps(beta), depthDiff1 ));
                depth = _mm_add_ps(depth, _mm_mul_ps( _mm_cvtepi32_ps(gamma), depthDiff2 ));
                //__m128 depth = curDepth;

                for(int j=0; j < numX; j++)
                {
                    // Test pixel inside triangle
                    // Basically, check if any edge fn is negative
                    __m128i mask = _mm_or_si128(_mm_or_si128(alpha, beta), gamma);

                    //if(_mm_testc_si128(_mm_set1_epi32(0x80000000), mask) == 0)
                    {
                        __m128 previousDepthValue = _mm_load_ps(&pDepthBuffer[index]);
                        __m128 mergedDepth = _mm_min_ps(depth, previousDepthValue);
                        __m128 finalDepth = _mm_blendv_ps(mergedDepth, previousDepthValue, _mm_castsi128_ps(mask));
                        _mm_store_ps(&pDepthBuffer[index], finalDepth);
                    }

                    // NOTE: This will be compiled out if context > 1
                    //if(numContexts == 1)
                    //    g_NumPixelsProcessed += 4;

                    index += 4;
                    alpha = _mm_add_epi32(alpha, aa0Inc);
                    beta  = _mm_add_epi32(beta, aa1Inc);
                    gamma = _mm_add_epi32(gamma, aa2Inc);
                    depth = _mm_add_ps(depth, depthXInc);
                }

                pixelIndex += 2 * bufferWidth;
                sum0Row = _mm_add_epi32(sum0Row, bb0Inc);
                sum1Row = _mm_add_epi32(sum1Row, bb1Inc);
                sum2Row = _mm_add_epi32(sum2Row, bb2Inc);

                numY--;
            }

            //int numYDiv2 = numY >> 1;

            for(int i=0; i < numY; i+=2)
            {
                // Compute barycentric coordinates 
                // Z = Z0 * A0/A + Z1 * A1/A + Z2 * A2/A
                //   = Z0 + (Z1 - Z0) * A1/A + (Z2 - Z0) * A2/A
                // Where F1 = line equation for line 1 = 2x area of triangle from vertices on line 1 to current pt 
                // and F2 = ..
                int index = pixelIndex;
                __m128i alpha = sum0Row;
                __m128i beta = sum1Row;
                __m128i gamma = sum2Row;

                // Using Z0 + (Z1 - Z0) * A1/A + (Z2 - Z0) * A2/A, compute barycentric-interpolated depth
                __m128 depth = depth0;
                depth = _mm_add_ps(depth, _mm_mul_ps( _mm_cvtepi32_ps(beta), depthDiff1 ));
                depth = _mm_add_ps(depth, _mm_mul_ps( _mm_cvtepi32_ps(gamma), depthDiff2 ));
                //__m128 depth = curDepth;

                for(int j=0; j < numX; j++)
                {
                    // Test pixel inside triangle
                    // Basically, check if any edge fn is negative
                    __m128i mask = _mm_or_si128(_mm_or_si128(alpha, beta), gamma);

                    //if(_mm_testc_si128(_mm_set1_epi32(0x80000000), mask) == 0)
                    {
                        __m128 previousDepthValue = _mm_load_ps(&pDepthBuffer[index]);
                        __m128 mergedDepth = _mm_min_ps(depth, previousDepthValue);
                        __m128 finalDepth = _mm_blendv_ps(mergedDepth, previousDepthValue, _mm_castsi128_ps(mask));
                        _mm_store_ps(&pDepthBuffer[index], finalDepth);
                    }

                    // NOTE: This will be compiled out if context > 1
                    //if(numContexts == 1)
                    //    g_NumPixelsProcessed += 4;

                    index += 4;
                    alpha = _mm_add_epi32(alpha, aa0Inc);
                    beta  = _mm_add_epi32(beta, aa1Inc);
                    gamma = _mm_add_epi32(gamma, aa2Inc);
                    depth = _mm_add_ps(depth, depthXInc);
                }

                pixelIndex += 2 * bufferWidth;
                sum0Row = _mm_add_epi32(sum0Row, bb0Inc);
                sum1Row = _mm_add_epi32(sum1Row, bb1Inc);
                sum2Row = _mm_add_epi32(sum2Row, bb2Inc);
                // curDepth = _mm_add_ps(curDepth, depthYInc);

                index = pixelIndex;
                alpha = sum0Row;
                beta = sum1Row;
                gamma = sum2Row;

                depth = depth0;
                depth = _mm_add_ps(depth, _mm_mul_ps( _mm_cvtepi32_ps(beta), depthDiff1 ));
                depth = _mm_add_ps(depth, _mm_mul_ps( _mm_cvtepi32_ps(gamma), depthDiff2 ));
                //__m128 depth = curDepth;

                for(int j=0; j < numX; j++)
                {
                    __m128i mask = _mm_or_si128(_mm_or_si128(alpha, beta), gamma);

                    __m128 previousDepthValue = _mm_load_ps(&pDepthBuffer[index]);
                    __m128 mergedDepth = _mm_min_ps(depth, previousDepthValue);
                    __m128 finalDepth = _mm_blendv_ps(mergedDepth, previousDepthValue, _mm_castsi128_ps(mask));
                    _mm_store_ps(&pDepthBuffer[index], finalDepth);

                    // NOTE: This will be compiled out if context > 1
                    //if(numContexts == 1)
                    //    g_NumPixelsProcessed += 4;

                    index += 4;
                    alpha = _mm_add_epi32(alpha, aa0Inc);
                    beta  = _mm_add_epi32(beta, aa1Inc);
                    gamma = _mm_add_epi32(gamma, aa2Inc);
                    depth = _mm_add_ps(depth, depthXInc);
                }

                pixelIndex += 2 * bufferWidth;
                sum0Row = _mm_add_epi32(sum0Row, bb0Inc);
                sum1Row = _mm_add_epi32(sum1Row, bb1Inc);
                sum2Row = _mm_add_epi32(sum2Row, bb2Inc);
            }
        }

    }while(true);
}

template<uint binWidth, uint binHeight, uint bufferWidth, uint bufferHeight, uint numContexts>
void TilesRasterizeDepthUnrollXInt(const TriangleBin* pBins, const uint* pNumTrisInBins, float* pDepthBuffer, int binStartX, int binStartY)
{
    //_mm_setcsr( _mm_getcsr() | 0x8040 );

    const uint NUM_BINS_X = bufferWidth / binWidth;
    const uint NUM_BINS_Y = bufferHeight / binHeight;

    __m128i colOffset = _mm_setr_epi32(0, 1, 0, 1);
    __m128i rowOffset = _mm_setr_epi32(0, 0, 1, 1);

    // Based on TaskId determine which tile to process
    //UINT tileX = taskId % screenWidthInTiles;
    //UINT tileY = taskId / screenWidthInTiles;

    int tileStartX = binStartX * binWidth;
    int tileEndX   = min(tileStartX + binWidth - 1, bufferWidth - 1);

    int tileStartY = binStartY * binHeight;
    int tileEndY   = min(tileStartY + binHeight - 1, bufferHeight - 1);

    _DEBUG_ASSERT( (tileStartX & 1) == 0 && (tileStartY & 1) == 0 );
    _DEBUG_ASSERT( (tileEndX & 1) == 1 && (tileEndY & 1) == 1 );

    // Clear tile buffer to 1.0
    // Note we need to account for tiling pattern here
    __m128 clearVal = _mm_set1_ps(1.0f);
    for(int i=tileStartY; i < tileEndY; i+=2)
    {
        float* pToClear = (pDepthBuffer + (i * bufferWidth + 2 * tileStartX));       
        for(int j=0; j < binWidth; j+=2)
        {
            //_mm_stream_ps(pToClear, clearVal);
            _mm_store_ps(pToClear, clearVal);
            pToClear += 4;
        }

        //memset(&pDepthBuffer[index], 0, sizeof(float) * 2 * binWidth);
    }

    // Process all triangles in all contexts for this bin
    uint curContext = 0;
    const TriangleBin* pCurBin = pBins + (curContext * NUM_BINS_X * NUM_BINS_Y);
    const uint* pCurNumTrisInBin = pNumTrisInBins + (curContext * NUM_BINS_X * NUM_BINS_Y);
    uint numTrisInBin = *pCurNumTrisInBin;
    uint numTrisFetchedFromBin = 0;

    __m128 triVerts[4][2];

    do 
    {
        // Grab 4 triangles from bins to process
        uint numTrisFetched = 0;
        while(numTrisFetched < 4)
        { 
            // Search for the next bin with triangles to process
            while(numTrisFetchedFromBin >= numTrisInBin)
            {
                curContext++;
                if(curContext >= numContexts)
                {
                    // We have no more bins
                    // So either we're all done
                    if(numTrisFetched == 0)
                        return;

                    // Or we still have a final batch of triangles to process
                    goto LABEL_PROCESS_TRIANGLES;
                }

                pCurBin += (NUM_BINS_X * NUM_BINS_Y);
                pCurNumTrisInBin += (NUM_BINS_X * NUM_BINS_Y);
                numTrisInBin = *pCurNumTrisInBin;
                numTrisFetchedFromBin = 0;
            }

            // Grab 1 triangle from the bin
            triVerts[numTrisFetched][0] = _mm_castsi128_ps( _mm_loadu_si128((const __m128i*) &(pCurBin->tris[numTrisFetchedFromBin].xy[0])) );
            triVerts[numTrisFetched][1] = _mm_castsi128_ps( _mm_loadl_epi64((const __m128i*) &(pCurBin->tris[numTrisFetchedFromBin].z[1])) );

            numTrisFetchedFromBin++;
            numTrisFetched++;
        }

LABEL_PROCESS_TRIANGLES:

        // Convert vertex for each triangle into SOA       
        __m128 v0 = triVerts[0][0];
        __m128 v1 = triVerts[1][0];
        __m128 v2 = triVerts[2][0];
        __m128 v3 = triVerts[3][0];

        _MM_TRANSPOSE4_PS(v0, v1, v2, v3);

        // Now v0, v1, v2 contain the corresponding verts
        // v3 also contains Z[0] but we don't care here

        // Extract X and Y from each vertex of each triangle
        __m128i fxPtX[3];
        __m128i fxPtY[3];
        fxPtX[0] = _mm_srai_epi32(_mm_slli_epi32(_mm_castps_si128(v0), 16), 16);    // Contains vertex 0 X for each triangle
        fxPtY[0] = _mm_srai_epi32(_mm_castps_si128(v0), 16);                        // Contains vertex 0 Y for each triangle
        fxPtX[1] = _mm_srai_epi32(_mm_slli_epi32(_mm_castps_si128(v1), 16), 16);    // Contains vertex 1 X for each triangle
        fxPtY[1] = _mm_srai_epi32(_mm_castps_si128(v1), 16);                        // Contains vertex 1 Y for each triangle
        fxPtX[2] = _mm_srai_epi32(_mm_slli_epi32(_mm_castps_si128(v2), 16), 16);    // Contains vertex 2 X for each triangle
        fxPtY[2] = _mm_srai_epi32(_mm_castps_si128(v2), 16);                        // Contains vertex 2 Y for each triangle

        // Note that since we convert Y from clip space to screen space, with 0 starting from the top
        // the line equations in screen space are in fact computed clockwise
        // Fab(x, y) =     Ax       +       By     +      C              = 0
        // Fab(x, y) = (ya - yb)x   +   (xb - xa)y + (xa * yb - xb * ya) = 0
        // Compute A = (ya - yb) for the 3 line segments that make up each triangle
        __m128i A0 = _mm_sub_epi32(fxPtY[1], fxPtY[2]);
        __m128i A1 = _mm_sub_epi32(fxPtY[2], fxPtY[0]);
        __m128i A2 = _mm_sub_epi32(fxPtY[0], fxPtY[1]);

        // Compute B = (xb - xa) for the 3 line segments that make up each triangle
        __m128i B0 = _mm_sub_epi32(fxPtX[2], fxPtX[1]);
        __m128i B1 = _mm_sub_epi32(fxPtX[0], fxPtX[2]);
        __m128i B2 = _mm_sub_epi32(fxPtX[1], fxPtX[0]);

        // Compute C = (xa * yb - xb * ya) for the 3 line segments that make up each triangle
        __m128i C0 = _mm_sub_epi32( _mm_mullo_epi32(fxPtX[1], fxPtY[2]), _mm_mullo_epi32(fxPtX[2], fxPtY[1]) );
        __m128i C1 = _mm_sub_epi32( _mm_mullo_epi32(fxPtX[2], fxPtY[0]), _mm_mullo_epi32(fxPtX[0], fxPtY[2]) );
        __m128i C2 = _mm_sub_epi32( _mm_mullo_epi32(fxPtX[0], fxPtY[1]), _mm_mullo_epi32(fxPtX[1], fxPtY[0]) );

        // Use bounding box traversal strategy to determine which pixels to rasterize
        // This guarantees the starting point is always rounded to the nearest even pixel, conservatively
        __m128i startX = _mm_and_si128( _mm_max_epi32( _mm_min_epi32( _mm_min_epi32(fxPtX[0], fxPtX[1]), fxPtX[2] ), _mm_set1_epi32(tileStartX) ), _mm_set1_epi32(0xFFFFFFFE) );    

        // This guarantees the end point always ends on the next pixel (but less than the tileEnd) because the inner loop increments by 2
        // and we want to make sure that the end pixel also gets rasterized (because < is used instead of <=)
        //__m128i endX   = _mm_min_epi32( _mm_add_epi32( _mm_max_epi32( _mm_max_epi32(fxPtX[0], fxPtX[1]), fxPtX[2] ), _mm_set1_epi32(1) ), _mm_set1_epi32(tileEndX) );   
        __m128i endX   = _mm_min_epi32( _mm_max_epi32( _mm_max_epi32(fxPtX[0], fxPtX[1]), fxPtX[2] ), _mm_set1_epi32(tileEndX) );   

        __m128i startY = _mm_and_si128( _mm_max_epi32( _mm_min_epi32( _mm_min_epi32(fxPtY[0], fxPtY[1]), fxPtY[2] ), _mm_set1_epi32(tileStartY) ), _mm_set1_epi32(0xFFFFFFFE) );
        //__m128i endY   = _mm_min_epi32( _mm_add_epi32( _mm_max_epi32( _mm_max_epi32(fxPtY[0], fxPtY[1]), fxPtY[2] ), _mm_set1_epi32(1) ), _mm_set1_epi32(tileEndY) );
        __m128i endY   = _mm_min_epi32( _mm_max_epi32( _mm_max_epi32(fxPtY[0], fxPtY[1]), fxPtY[2] ), _mm_set1_epi32(tileEndY) );

        // __m128i diffY = _mm_add_epi32( _mm_sub_epi32( endY, startY ), _mm_set1_epi32(1) );
        // __m128i diffX = _mm_add_epi32( _mm_sub_epi32( endX, startX ), _mm_set1_epi32(1) );
        __m128i diffY = _mm_add_epi32( _mm_srli_epi32(_mm_sub_epi32( endY, startY ), 1), _mm_set1_epi32(1) );
        __m128i diffX = _mm_add_epi32( _mm_srli_epi32(_mm_sub_epi32( endX, startX ), 1), _mm_set1_epi32(1) );

        // Since we always stride in the x direction by 4 pixels, we need to make sure that the pixels never overrun the buffer by
        // truncating the startX position to every 4th pixel...
        // But only if we detect that the end pixel is within 2 pixels of the edge, otherwise it's okay
        __m128i isAtEdge = _mm_cmpgt_epi32(endX, _mm_set1_epi32(tileEndX - 2));
        __m128i origStartX = startX;
        // startX = _mm_or_si128( _mm_and_si128(isAtEdge, _mm_and_si128(startX, _mm_set1_epi32(0xFFFFFFFC))), _mm_andnot_si128(isAtEdge, startX) );
        startX = _mm_xor_si128( startX, _mm_and_si128( isAtEdge, _mm_xor_si128( _mm_and_si128(startX, _mm_set1_epi32(0xFFFFFFFC)), startX ) ) );

        // Rasterize all triangles individually
        for(uint curTri=0; curTri < numTrisFetched; curTri++)
        {
            // Extract this triangle's properties from the SIMD versions

            // zz[0] contains Z[0]
            // zz[1] contains Z[1] - Z[0]
            // zz[2] contains Z[2] - Z[0]
            __m128 depth0;
            __m128 depthDiff1;
            __m128 depthDiff2;
            depth0 = _mm_set1_ps(triVerts[curTri][0].m128_f32[3]);
            depthDiff1 = _mm_set1_ps(triVerts[curTri][1].m128_f32[0]);
            depthDiff2 = _mm_set1_ps(triVerts[curTri][1].m128_f32[1]);

            int startXx = startX.m128i_i32[curTri];
            int startYy = startY.m128i_i32[curTri];
            int numY	= (diffY.m128i_i32[curTri]);
            int numX    = (diffX.m128i_i32[curTri]);

            // Incrementally compute Fab(x, y) for all the pixels inside the bounding box formed by (startX, endX) and (startY, endY) 
            __m128i aa0 = _mm_set1_epi32(A0.m128i_i32[curTri]);
            __m128i aa1 = _mm_set1_epi32(A1.m128i_i32[curTri]);
            __m128i aa2 = _mm_set1_epi32(A2.m128i_i32[curTri]);

            __m128i bb0 = _mm_set1_epi32(B0.m128i_i32[curTri]);
            __m128i bb1 = _mm_set1_epi32(B1.m128i_i32[curTri]);
            __m128i bb2 = _mm_set1_epi32(B2.m128i_i32[curTri]);

            // Find A * x for 2x2 quad
            __m128i col = _mm_add_epi32(colOffset, _mm_set1_epi32(startXx));
            __m128i aa0Col = _mm_mullo_epi32(aa0, col);
            __m128i aa1Col = _mm_mullo_epi32(aa1, col);
            __m128i aa2Col = _mm_mullo_epi32(aa2, col);

            // Find B * y + C for 2x2 quad
            __m128i row = _mm_add_epi32(rowOffset, _mm_set1_epi32(startYy));
            __m128i bb0Row = _mm_add_epi32(_mm_mullo_epi32(bb0, row), _mm_set1_epi32(C0.m128i_i32[curTri]));
            __m128i bb1Row = _mm_add_epi32(_mm_mullo_epi32(bb1, row), _mm_set1_epi32(C1.m128i_i32[curTri]));
            __m128i bb2Row = _mm_add_epi32(_mm_mullo_epi32(bb2, row), _mm_set1_epi32(C2.m128i_i32[curTri]));

            // Find line equation = A * x + B * y + C
            // This also effectively gives 2x area of triangle from 2 vertices to the current point (x, y)
            __m128i sum0Row = _mm_add_epi32(aa0Col, bb0Row);
            __m128i sum1Row = _mm_add_epi32(aa1Col, bb1Row);
            __m128i sum2Row = _mm_add_epi32(aa2Col, bb2Row);

            // Find increment for A, B and depth
            // We increment by 2 pixels
            __m128i aa0Inc = _mm_slli_epi32(aa0, 1);
            __m128i aa1Inc = _mm_slli_epi32(aa1, 1);
            __m128i aa2Inc = _mm_slli_epi32(aa2, 1);

            __m128i bb0Inc = _mm_slli_epi32(bb0, 1);
            __m128i bb1Inc = _mm_slli_epi32(bb1, 1);
            __m128i bb2Inc = _mm_slli_epi32(bb2, 1);

            // This is the increment of (Z * area of the triangle / total area)
            // Computing (Z[1] - Z[0]) * dAreaOfTriangle1 / AreaOfTriangle 
            __m128 depthXInc = _mm_mul_ps(_mm_cvtepi32_ps(aa1Inc), depthDiff1);

            // Computing (Z[2] - Z[0]) * dAreaOfTriangle2 / AreaOfTriangle
            depthXInc = _mm_add_ps(depthXInc, _mm_mul_ps(_mm_cvtepi32_ps(aa2Inc), depthDiff2));

            // Tranverse pixels in 2x2 blocks and store 2x2 pixel quad depths contiguously in memory ==> 2*X
            int pixelIndex = (startYy * bufferWidth + 2 * startXx);

            // Rasterization inner loop
            // Incrementally compute Fab(x, y) for all the pixels inside the bounding box formed by (startX, endX) and (startY, endY)
            //if(0)//numX & 1)
            //{
            //    numX--;
            //    for(int i=0; i < numY; i++)
            //    {
            //        int index = pixelIndex;
            //        __m128i alpha = sum0Row;
            //        __m128i beta = sum1Row;
            //        __m128i gamma = sum2Row;

            //        __m128 depth = depth0;
            //        depth = _mm_add_ps(depth, _mm_mul_ps( _mm_cvtepi32_ps(beta), depthDiff1 ));
            //        depth = _mm_add_ps(depth, _mm_mul_ps( _mm_cvtepi32_ps(gamma), depthDiff2 ));

            //        for(int j=0; j < numX; j+=2)
            //        {
            //            __m128i mask = _mm_or_si128(_mm_or_si128(alpha, beta), gamma);

            //            __m128 previousDepthValue = _mm_load_ps(&pDepthBuffer[index]);
            //            __m128 mergedDepth = _mm_min_ps(depth, previousDepthValue);
            //            __m128 finalDepth = _mm_blendv_ps(mergedDepth, previousDepthValue, _mm_castsi128_ps(mask));
            //            _mm_store_ps(&pDepthBuffer[index], finalDepth);

            //            // NOTE: This will be compiled out if context > 1
            //            //if(numContexts == 1)
            //            //    g_NumPixelsProcessed += 4;

            //            alpha = _mm_add_epi32(alpha, aa0Inc);
            //            beta  = _mm_add_epi32(beta, aa1Inc);
            //            gamma = _mm_add_epi32(gamma, aa2Inc);
            //            depth = _mm_add_ps(depth, depthXInc);

            //            mask = _mm_or_si128(_mm_or_si128(alpha, beta), gamma);

            //            previousDepthValue = _mm_load_ps(&pDepthBuffer[index+4]);
            //            mergedDepth = _mm_min_ps(depth, previousDepthValue);
            //            finalDepth = _mm_blendv_ps(mergedDepth, previousDepthValue, _mm_castsi128_ps(mask));
            //            _mm_store_ps(&pDepthBuffer[index+4], finalDepth);

            //            // NOTE: This will be compiled out if context > 1
            //            //if(numContexts == 1)
            //            //    g_NumPixelsProcessed += 4;

            //            index += 8;
            //            alpha = _mm_add_epi32(alpha, aa0Inc);
            //            beta  = _mm_add_epi32(beta, aa1Inc);
            //            gamma = _mm_add_epi32(gamma, aa2Inc);
            //            depth = _mm_add_ps(depth, depthXInc);
            //        }

            //        __m128i mask = _mm_or_si128(_mm_or_si128(alpha, beta), gamma);

            //        __m128 previousDepthValue = _mm_load_ps(&pDepthBuffer[index]);
            //        __m128 mergedDepth = _mm_min_ps(depth, previousDepthValue);
            //        __m128 finalDepth = _mm_blendv_ps(mergedDepth, previousDepthValue, _mm_castsi128_ps(mask));
            //        _mm_store_ps(&pDepthBuffer[index], finalDepth);

            //        pixelIndex += 2 * bufferWidth;
            //        sum0Row = _mm_add_epi32(sum0Row, bb0Inc);
            //        sum1Row = _mm_add_epi32(sum1Row, bb1Inc);
            //        sum2Row = _mm_add_epi32(sum2Row, bb2Inc);
            //    }
            //}
            //else
            {
                for(int i=0; i < numY; i++)
                {
                    int index = pixelIndex;
                    __m128i alpha = sum0Row;
                    __m128i beta = sum1Row;
                    __m128i gamma = sum2Row;

                    __m128 depth = depth0;
                    depth = _mm_add_ps(depth, _mm_mul_ps( _mm_cvtepi32_ps(beta), depthDiff1 ));
                    depth = _mm_add_ps(depth, _mm_mul_ps( _mm_cvtepi32_ps(gamma), depthDiff2 ));

                    for(int j=0; j < numX; j+=2)
                    {
                        __m128i mask = _mm_or_si128(_mm_or_si128(alpha, beta), gamma);

                        __m128 previousDepthValue = _mm_load_ps(&pDepthBuffer[index]);
                        __m128 mergedDepth = _mm_min_ps(depth, previousDepthValue);
                        __m128 finalDepth = _mm_blendv_ps(mergedDepth, previousDepthValue, _mm_castsi128_ps(mask));
                        _mm_store_ps(&pDepthBuffer[index], finalDepth);

                        // NOTE: This will be compiled out if context > 1
                        //if(numContexts == 1)
                        //    g_NumPixelsProcessed += 4;

                        alpha = _mm_add_epi32(alpha, aa0Inc);
                        beta  = _mm_add_epi32(beta, aa1Inc);
                        gamma = _mm_add_epi32(gamma, aa2Inc);
                        depth = _mm_add_ps(depth, depthXInc);

                        mask = _mm_or_si128(_mm_or_si128(alpha, beta), gamma);

                        previousDepthValue = _mm_load_ps(&pDepthBuffer[index+4]);
                        mergedDepth = _mm_min_ps(depth, previousDepthValue);
                        finalDepth = _mm_blendv_ps(mergedDepth, previousDepthValue, _mm_castsi128_ps(mask));
                        _mm_store_ps(&pDepthBuffer[index+4], finalDepth);

                        // NOTE: This will be compiled out if context > 1
                        //if(numContexts == 1)
                        //    g_NumPixelsProcessed += 4;

                        index += 8;
                        alpha = _mm_add_epi32(alpha, aa0Inc);
                        beta  = _mm_add_epi32(beta, aa1Inc);
                        gamma = _mm_add_epi32(gamma, aa2Inc);
                        depth = _mm_add_ps(depth, depthXInc);
                    }

                    pixelIndex += 2 * bufferWidth;
                    sum0Row = _mm_add_epi32(sum0Row, bb0Inc);
                    sum1Row = _mm_add_epi32(sum1Row, bb1Inc);
                    sum2Row = _mm_add_epi32(sum2Row, bb2Inc);        
                }
            }
        }

    }while(true);
}

template<uint binWidth, uint binHeight, uint bufferWidth, uint bufferHeight, uint numContexts>
void TilesRasterizeDepthUnrollXYInt(const TriangleBin* pBins, const uint* pNumTrisInBins, float* pDepthBuffer, int binStartX, int binStartY)
{
    //_mm_setcsr( _mm_getcsr() | 0x8040 );

    const uint NUM_BINS_X = bufferWidth / binWidth;
    const uint NUM_BINS_Y = bufferHeight / binHeight;

    __m128i colOffset = _mm_setr_epi32(0, 1, 0, 1);
    __m128i rowOffset = _mm_setr_epi32(0, 0, 1, 1);

    // Based on TaskId determine which tile to process
    //UINT tileX = taskId % screenWidthInTiles;
    //UINT tileY = taskId / screenWidthInTiles;

    int tileStartX = binStartX * binWidth;
    int tileEndX   = min(tileStartX + binWidth - 1, bufferWidth - 1);

    int tileStartY = binStartY * binHeight;
    int tileEndY   = min(tileStartY + binHeight - 1, bufferHeight - 1);

    _DEBUG_ASSERT( (tileStartX & 1) == 0 && (tileStartY & 1) == 0 );
    _DEBUG_ASSERT( (tileEndX & 1) == 1 && (tileEndY & 1) == 1 );

    // Clear tile buffer to 1.0
    // Note we need to account for tiling pattern here
    __m128 clearVal = _mm_set1_ps(1.0f);
    for(int i=tileStartY; i < tileEndY; i+=2)
    {
        float* pToClear = (pDepthBuffer + (i * bufferWidth + 2 * tileStartX));       
        for(int j=0; j < binWidth; j+=2)
        {
            //_mm_stream_ps(pToClear, clearVal);
            _mm_store_ps(pToClear, clearVal);
            pToClear += 4;
        }

        //memset(&pDepthBuffer[index], 0, sizeof(float) * 2 * binWidth);
    }

    // Process all triangles in all contexts for this bin
    uint curContext = 0;
    const TriangleBin* pCurBin = pBins + (curContext * NUM_BINS_X * NUM_BINS_Y);
    const uint* pCurNumTrisInBin = pNumTrisInBins + (curContext * NUM_BINS_X * NUM_BINS_Y);
    uint numTrisInBin = *pCurNumTrisInBin;
    uint numTrisFetchedFromBin = 0;

    __m128 triVerts[4][2];

    do 
    {
        // Grab 4 triangles from bins to process
        uint numTrisFetched = 0;
        while(numTrisFetched < 4)
        { 
            // Search for the next bin with triangles to process
            while(numTrisFetchedFromBin >= numTrisInBin)
            {
                curContext++;
                if(curContext >= numContexts)
                {
                    // We have no more bins
                    // So either we're all done
                    if(numTrisFetched == 0)
                        return;

                    // Or we still have a final batch of triangles to process
                    goto LABEL_PROCESS_TRIANGLES;
                }

                pCurBin += (NUM_BINS_X * NUM_BINS_Y);
                pCurNumTrisInBin += (NUM_BINS_X * NUM_BINS_Y);
                numTrisInBin = *pCurNumTrisInBin;
                numTrisFetchedFromBin = 0;
            }

            // Grab 1 triangle from the bin
            triVerts[numTrisFetched][0] = _mm_castsi128_ps( _mm_loadu_si128((const __m128i*) &(pCurBin->tris[numTrisFetchedFromBin].xy[0])) );
            triVerts[numTrisFetched][1] = _mm_castsi128_ps( _mm_loadl_epi64((const __m128i*) &(pCurBin->tris[numTrisFetchedFromBin].z[1])) );

            numTrisFetchedFromBin++;
            numTrisFetched++;
        }

LABEL_PROCESS_TRIANGLES:

        // Convert vertex for each triangle into SOA       
        __m128 v0 = triVerts[0][0];
        __m128 v1 = triVerts[1][0];
        __m128 v2 = triVerts[2][0];
        __m128 v3 = triVerts[3][0];

        _MM_TRANSPOSE4_PS(v0, v1, v2, v3);

        // Now v0, v1, v2 contain the corresponding verts
        // v3 also contains Z[0] but we don't care here

        // Extract X and Y from each vertex of each triangle
        __m128i fxPtX[3];
        __m128i fxPtY[3];
        fxPtX[0] = _mm_srai_epi32(_mm_slli_epi32(_mm_castps_si128(v0), 16), 16);    // Contains vertex 0 X for each triangle
        fxPtY[0] = _mm_srai_epi32(_mm_castps_si128(v0), 16);                        // Contains vertex 0 Y for each triangle
        fxPtX[1] = _mm_srai_epi32(_mm_slli_epi32(_mm_castps_si128(v1), 16), 16);    // Contains vertex 1 X for each triangle
        fxPtY[1] = _mm_srai_epi32(_mm_castps_si128(v1), 16);                        // Contains vertex 1 Y for each triangle
        fxPtX[2] = _mm_srai_epi32(_mm_slli_epi32(_mm_castps_si128(v2), 16), 16);    // Contains vertex 2 X for each triangle
        fxPtY[2] = _mm_srai_epi32(_mm_castps_si128(v2), 16);                        // Contains vertex 2 Y for each triangle

        // Note that since we convert Y from clip space to screen space, with 0 starting from the top
        // the line equations in screen space are in fact computed clockwise
        // Fab(x, y) =     Ax       +       By     +      C              = 0
        // Fab(x, y) = (ya - yb)x   +   (xb - xa)y + (xa * yb - xb * ya) = 0
        // Compute A = (ya - yb) for the 3 line segments that make up each triangle
        __m128i A0 = _mm_sub_epi32(fxPtY[1], fxPtY[2]);
        __m128i A1 = _mm_sub_epi32(fxPtY[2], fxPtY[0]);
        __m128i A2 = _mm_sub_epi32(fxPtY[0], fxPtY[1]);

        // Compute B = (xb - xa) for the 3 line segments that make up each triangle
        __m128i B0 = _mm_sub_epi32(fxPtX[2], fxPtX[1]);
        __m128i B1 = _mm_sub_epi32(fxPtX[0], fxPtX[2]);
        __m128i B2 = _mm_sub_epi32(fxPtX[1], fxPtX[0]);

        // Compute C = (xa * yb - xb * ya) for the 3 line segments that make up each triangle
        __m128i C0 = _mm_sub_epi32( _mm_mullo_epi32(fxPtX[1], fxPtY[2]), _mm_mullo_epi32(fxPtX[2], fxPtY[1]) );
        __m128i C1 = _mm_sub_epi32( _mm_mullo_epi32(fxPtX[2], fxPtY[0]), _mm_mullo_epi32(fxPtX[0], fxPtY[2]) );
        __m128i C2 = _mm_sub_epi32( _mm_mullo_epi32(fxPtX[0], fxPtY[1]), _mm_mullo_epi32(fxPtX[1], fxPtY[0]) );

        // Use bounding box traversal strategy to determine which pixels to rasterize
        // This guarantees the starting point is always rounded to the nearest even pixel, conservatively
        __m128i startX = _mm_and_si128( _mm_max_epi32( _mm_min_epi32( _mm_min_epi32(fxPtX[0], fxPtX[1]), fxPtX[2] ), _mm_set1_epi32(tileStartX) ), _mm_set1_epi32(0xFFFFFFFE) );    

        // This guarantees the end point always ends on the next pixel (but less than the tileEnd) because the inner loop increments by 2
        // and we want to make sure that the end pixel also gets rasterized (because < is used instead of <=)
        //__m128i endX   = _mm_min_epi32( _mm_add_epi32( _mm_max_epi32( _mm_max_epi32(fxPtX[0], fxPtX[1]), fxPtX[2] ), _mm_set1_epi32(1) ), _mm_set1_epi32(tileEndX) );   
        __m128i endX   = _mm_min_epi32( _mm_max_epi32( _mm_max_epi32(fxPtX[0], fxPtX[1]), fxPtX[2] ), _mm_set1_epi32(tileEndX) );   

        __m128i startY = _mm_and_si128( _mm_max_epi32( _mm_min_epi32( _mm_min_epi32(fxPtY[0], fxPtY[1]), fxPtY[2] ), _mm_set1_epi32(tileStartY) ), _mm_set1_epi32(0xFFFFFFFE) );
        //__m128i endY   = _mm_min_epi32( _mm_add_epi32( _mm_max_epi32( _mm_max_epi32(fxPtY[0], fxPtY[1]), fxPtY[2] ), _mm_set1_epi32(1) ), _mm_set1_epi32(tileEndY) );
        __m128i endY   = _mm_min_epi32( _mm_max_epi32( _mm_max_epi32(fxPtY[0], fxPtY[1]), fxPtY[2] ), _mm_set1_epi32(tileEndY) );

        // __m128i diffY = _mm_add_epi32( _mm_sub_epi32( endY, startY ), _mm_set1_epi32(1) );
        // __m128i diffX = _mm_add_epi32( _mm_sub_epi32( endX, startX ), _mm_set1_epi32(1) );
        __m128i diffY = _mm_add_epi32( _mm_srli_epi32(_mm_sub_epi32( endY, startY ), 1), _mm_set1_epi32(1) );
        __m128i diffX = _mm_add_epi32( _mm_srli_epi32(_mm_sub_epi32( endX, startX ), 1), _mm_set1_epi32(1) );

        // Since we always stride in the x direction by 4 pixels, we need to make sure that the pixels never overrun the buffer by
        // truncating the startX position to every 4th pixel...
        // But only if we detect that the end pixel is within 2 pixels of the edge, otherwise it's okay since it will not overrun in that case
        __m128i isAtEdge = _mm_cmpgt_epi32(endX, _mm_set1_epi32(tileEndX - 2));
        __m128i origStartX = startX;
        // startX = _mm_or_si128( _mm_and_si128(isAtEdge, _mm_and_si128(startX, _mm_set1_epi32(0xFFFFFFFC))), _mm_andnot_si128(isAtEdge, startX) );
        startX = _mm_xor_si128( startX, _mm_and_si128( isAtEdge, _mm_xor_si128( _mm_and_si128(startX, _mm_set1_epi32(0xFFFFFFFC)), startX ) ) );

        // Rasterize all triangles individually
        for(uint curTri=0; curTri < numTrisFetched; curTri++)
        {
            // Extract this triangle's properties from the SIMD versions

            // zz[0] contains Z[0]
            // zz[1] contains Z[1] - Z[0]
            // zz[2] contains Z[2] - Z[0]
            __m128 depth0;
            __m128 depthDiff1;
            __m128 depthDiff2;
            depth0 = _mm_set1_ps(triVerts[curTri][0].m128_f32[3]);
            depthDiff1 = _mm_set1_ps(triVerts[curTri][1].m128_f32[0]);
            depthDiff2 = _mm_set1_ps(triVerts[curTri][1].m128_f32[1]);

            int startXx = startX.m128i_i32[curTri];
            int startYy = startY.m128i_i32[curTri];
            int numY	= (diffY.m128i_i32[curTri]);
            int numX    = (diffX.m128i_i32[curTri]);

            // Incrementally compute Fab(x, y) for all the pixels inside the bounding box formed by (startX, endX) and (startY, endY) 
            __m128i aa0 = _mm_set1_epi32(A0.m128i_i32[curTri]);
            __m128i aa1 = _mm_set1_epi32(A1.m128i_i32[curTri]);
            __m128i aa2 = _mm_set1_epi32(A2.m128i_i32[curTri]);

            __m128i bb0 = _mm_set1_epi32(B0.m128i_i32[curTri]);
            __m128i bb1 = _mm_set1_epi32(B1.m128i_i32[curTri]);
            __m128i bb2 = _mm_set1_epi32(B2.m128i_i32[curTri]);

            // Find A * x for 2x2 quad
            __m128i col = _mm_add_epi32(colOffset, _mm_set1_epi32(startXx));
            __m128i aa0Col = _mm_mullo_epi32(aa0, col);
            __m128i aa1Col = _mm_mullo_epi32(aa1, col);
            __m128i aa2Col = _mm_mullo_epi32(aa2, col);

            // Find B * y + C for 2x2 quad
            __m128i row = _mm_add_epi32(rowOffset, _mm_set1_epi32(startYy));
            __m128i bb0Row = _mm_add_epi32(_mm_mullo_epi32(bb0, row), _mm_set1_epi32(C0.m128i_i32[curTri]));
            __m128i bb1Row = _mm_add_epi32(_mm_mullo_epi32(bb1, row), _mm_set1_epi32(C1.m128i_i32[curTri]));
            __m128i bb2Row = _mm_add_epi32(_mm_mullo_epi32(bb2, row), _mm_set1_epi32(C2.m128i_i32[curTri]));

            // Find line equation = A * x + B * y + C
            // This also effectively gives 2x area of triangle from 2 vertices to the current point (x, y)
            __m128i sum0Row = _mm_add_epi32(aa0Col, bb0Row);
            __m128i sum1Row = _mm_add_epi32(aa1Col, bb1Row);
            __m128i sum2Row = _mm_add_epi32(aa2Col, bb2Row);

            // Find increment for A, B and depth
            // We increment by 2 pixels
            __m128i aa0Inc = _mm_slli_epi32(aa0, 1);
            __m128i aa1Inc = _mm_slli_epi32(aa1, 1);
            __m128i aa2Inc = _mm_slli_epi32(aa2, 1);

            __m128i bb0Inc = _mm_slli_epi32(bb0, 1);
            __m128i bb1Inc = _mm_slli_epi32(bb1, 1);
            __m128i bb2Inc = _mm_slli_epi32(bb2, 1);

            // This is the increment of (Z * area of the triangle / total area)
            // Computing (Z[1] - Z[0]) * dAreaOfTriangle1 / AreaOfTriangle 
            __m128 depthXInc = _mm_mul_ps(_mm_cvtepi32_ps(aa1Inc), depthDiff1);

            // Computing (Z[2] - Z[0]) * dAreaOfTriangle2 / AreaOfTriangle
            depthXInc = _mm_add_ps(depthXInc, _mm_mul_ps(_mm_cvtepi32_ps(aa2Inc), depthDiff2));

            // Tranverse pixels in 2x2 blocks and store 2x2 pixel quad depths contiguously in memory ==> 2*X
            int pixelIndex = (startYy * bufferWidth + 2 * startXx);

            // Rasterization inner loop
            // Incrementally compute Fab(x, y) for all the pixels inside the bounding box formed by (startX, endX) and (startY, endY)      
            if(numY & 0x1)
            {
                int index = pixelIndex;
                __m128i alpha = sum0Row;
                __m128i beta = sum1Row;
                __m128i gamma = sum2Row;

                __m128 depth = depth0;
                depth = _mm_add_ps(depth, _mm_mul_ps( _mm_cvtepi32_ps(beta), depthDiff1 ));
                depth = _mm_add_ps(depth, _mm_mul_ps( _mm_cvtepi32_ps(gamma), depthDiff2 ));

                for(int j=0; j < numX; j+=2)
                {
                    __m128i mask = _mm_or_si128(_mm_or_si128(alpha, beta), gamma);

                    __m128 previousDepthValue = _mm_load_ps(&pDepthBuffer[index]);
                    __m128 mergedDepth = _mm_min_ps(depth, previousDepthValue);
                    __m128 finalDepth = _mm_blendv_ps(mergedDepth, previousDepthValue, _mm_castsi128_ps(mask));
                    _mm_store_ps(&pDepthBuffer[index], finalDepth);

                    // NOTE: This will be compiled out if context > 1
                    //if(numContexts == 1)
                    //    g_NumPixelsProcessed += 4;

                    alpha = _mm_add_epi32(alpha, aa0Inc);
                    beta  = _mm_add_epi32(beta, aa1Inc);
                    gamma = _mm_add_epi32(gamma, aa2Inc);
                    depth = _mm_add_ps(depth, depthXInc);

                    mask = _mm_or_si128(_mm_or_si128(alpha, beta), gamma);

                    previousDepthValue = _mm_load_ps(&pDepthBuffer[index+4]);
                    mergedDepth = _mm_min_ps(depth, previousDepthValue);
                    finalDepth = _mm_blendv_ps(mergedDepth, previousDepthValue, _mm_castsi128_ps(mask));
                    _mm_store_ps(&pDepthBuffer[index+4], finalDepth);

                    // NOTE: This will be compiled out if context > 1
                    //if(numContexts == 1)
                    //    g_NumPixelsProcessed += 4;

                    index += 8;
                    alpha = _mm_add_epi32(alpha, aa0Inc);
                    beta  = _mm_add_epi32(beta, aa1Inc);
                    gamma = _mm_add_epi32(gamma, aa2Inc);
                    depth = _mm_add_ps(depth, depthXInc);
                }

                pixelIndex += 2 * bufferWidth;
                sum0Row = _mm_add_epi32(sum0Row, bb0Inc);
                sum1Row = _mm_add_epi32(sum1Row, bb1Inc);
                sum2Row = _mm_add_epi32(sum2Row, bb2Inc);

                numY--;
            }

            for(int i=0; i < numY; i+=2)
            {
                int index = pixelIndex;
                __m128i alpha = sum0Row;
                __m128i beta = sum1Row;
                __m128i gamma = sum2Row;

                __m128 depth = depth0;
                depth = _mm_add_ps(depth, _mm_mul_ps( _mm_cvtepi32_ps(beta), depthDiff1 ));
                depth = _mm_add_ps(depth, _mm_mul_ps( _mm_cvtepi32_ps(gamma), depthDiff2 ));

                for(int j=0; j < numX; j+=2)
                {
                    __m128i mask = _mm_or_si128(_mm_or_si128(alpha, beta), gamma);

                    __m128 previousDepthValue = _mm_load_ps(&pDepthBuffer[index]);
                    __m128 mergedDepth = _mm_min_ps(depth, previousDepthValue);
                    __m128 finalDepth = _mm_blendv_ps(mergedDepth, previousDepthValue, _mm_castsi128_ps(mask));
                    _mm_store_ps(&pDepthBuffer[index], finalDepth);

                    // NOTE: This will be compiled out if context > 1
                    //if(numContexts == 1)
                    //    g_NumPixelsProcessed += 4;

                    alpha = _mm_add_epi32(alpha, aa0Inc);
                    beta  = _mm_add_epi32(beta, aa1Inc);
                    gamma = _mm_add_epi32(gamma, aa2Inc);
                    depth = _mm_add_ps(depth, depthXInc);

                    mask = _mm_or_si128(_mm_or_si128(alpha, beta), gamma);

                    previousDepthValue = _mm_load_ps(&pDepthBuffer[index+4]);
                    mergedDepth = _mm_min_ps(depth, previousDepthValue);
                    finalDepth = _mm_blendv_ps(mergedDepth, previousDepthValue, _mm_castsi128_ps(mask));
                    _mm_store_ps(&pDepthBuffer[index+4], finalDepth);

                    // NOTE: This will be compiled out if context > 1
                    //if(numContexts == 1)
                    //    g_NumPixelsProcessed += 4;

                    index += 8;
                    alpha = _mm_add_epi32(alpha, aa0Inc);
                    beta  = _mm_add_epi32(beta, aa1Inc);
                    gamma = _mm_add_epi32(gamma, aa2Inc);
                    depth = _mm_add_ps(depth, depthXInc);
                }

                pixelIndex += 2 * bufferWidth;
                sum0Row = _mm_add_epi32(sum0Row, bb0Inc);
                sum1Row = _mm_add_epi32(sum1Row, bb1Inc);
                sum2Row = _mm_add_epi32(sum2Row, bb2Inc);

                index = pixelIndex;
                alpha = sum0Row;
                beta = sum1Row;
                gamma = sum2Row;

                depth = depth0;
                depth = _mm_add_ps(depth, _mm_mul_ps( _mm_cvtepi32_ps(beta), depthDiff1 ));
                depth = _mm_add_ps(depth, _mm_mul_ps( _mm_cvtepi32_ps(gamma), depthDiff2 ));

                for(int j=0; j < numX; j+=2)
                {
                    __m128i mask = _mm_or_si128(_mm_or_si128(alpha, beta), gamma);

                    __m128 previousDepthValue = _mm_load_ps(&pDepthBuffer[index]);
                    __m128 mergedDepth = _mm_min_ps(depth, previousDepthValue);
                    __m128 finalDepth = _mm_blendv_ps(mergedDepth, previousDepthValue, _mm_castsi128_ps(mask));
                    _mm_store_ps(&pDepthBuffer[index], finalDepth);

                    // NOTE: This will be compiled out if context > 1
                    //if(numContexts == 1)
                    //    g_NumPixelsProcessed += 4;

                    alpha = _mm_add_epi32(alpha, aa0Inc);
                    beta  = _mm_add_epi32(beta, aa1Inc);
                    gamma = _mm_add_epi32(gamma, aa2Inc);
                    depth = _mm_add_ps(depth, depthXInc);

                    mask = _mm_or_si128(_mm_or_si128(alpha, beta), gamma);

                    previousDepthValue = _mm_load_ps(&pDepthBuffer[index+4]);
                    mergedDepth = _mm_min_ps(depth, previousDepthValue);
                    finalDepth = _mm_blendv_ps(mergedDepth, previousDepthValue, _mm_castsi128_ps(mask));
                    _mm_store_ps(&pDepthBuffer[index+4], finalDepth);

                    // NOTE: This will be compiled out if context > 1
                    //if(numContexts == 1)
                    //    g_NumPixelsProcessed += 4;

                    index += 8;
                    alpha = _mm_add_epi32(alpha, aa0Inc);
                    beta  = _mm_add_epi32(beta, aa1Inc);
                    gamma = _mm_add_epi32(gamma, aa2Inc);
                    depth = _mm_add_ps(depth, depthXInc);
                }        

                pixelIndex += 2 * bufferWidth;
                sum0Row = _mm_add_epi32(sum0Row, bb0Inc);
                sum1Row = _mm_add_epi32(sum1Row, bb1Inc);
                sum2Row = _mm_add_epi32(sum2Row, bb2Inc);
            } 
        }

    }while(true);
}

template<uint binWidth, uint binHeight, uint bufferWidth, uint bufferHeight, uint numContexts>
void TilesRasterizeDepthBlockInt(const TriangleBin* pBins, const uint* pNumTrisInBins, float* pDepthBuffer, int binStartX, int binStartY)
{
    //_mm_setcsr( _mm_getcsr() | 0x8040 );

    const uint NUM_BINS_X = bufferWidth / binWidth;
    const uint NUM_BINS_Y = bufferHeight / binHeight;

    __m128i colOffset = _mm_setr_epi32(0, 1, 0, 1);
    __m128i rowOffset = _mm_setr_epi32(0, 0, 1, 1);
    __m128i col4Offset = _mm_setr_epi32(0, 8, 0, 8);
    __m128i row4Offset = _mm_setr_epi32(0, 0, 8, 8);

    // Based on TaskId determine which tile to process
    //UINT tileX = taskId % screenWidthInTiles;
    //UINT tileY = taskId / screenWidthInTiles;

    int tileStartX = binStartX * binWidth;
    int tileEndX   = min(tileStartX + binWidth - 1, bufferWidth - 1);

    int tileStartY = binStartY * binHeight;
    int tileEndY   = min(tileStartY + binHeight - 1, bufferHeight - 1);

    _DEBUG_ASSERT( (tileStartX & 1) == 0 && (tileStartY & 1) == 0 );
    _DEBUG_ASSERT( (tileEndX & 1) == 1 && (tileEndY & 1) == 1 );

    // Clear tile buffer to 1.0
    // Note we need to account for tiling pattern here
    __m128 clearVal = _mm_set1_ps(1.0f);
    for(int i=tileStartY; i < tileEndY; i+=2)
    {
        float* pToClear = (pDepthBuffer + (i * bufferWidth + 2 * tileStartX));       
        for(int j=0; j < binWidth; j+=2)
        {
            //_mm_stream_ps(pToClear, clearVal);
            _mm_store_ps(pToClear, clearVal);
            pToClear += 4;
        }

        //memset(&pDepthBuffer[index], 0, sizeof(float) * 2 * binWidth);
    }

    // Process all triangles in all contexts for this bin
    uint curContext = 0;
    const TriangleBin* pCurBin = pBins + (curContext * NUM_BINS_X * NUM_BINS_Y);
    const uint* pCurNumTrisInBin = pNumTrisInBins + (curContext * NUM_BINS_X * NUM_BINS_Y);
    uint numTrisInBin = *pCurNumTrisInBin;
    uint numTrisFetchedFromBin = 0;

    __m128 triVerts[4][2];

    do 
    {
        // Grab 4 triangles from bins to process
        uint numTrisFetched = 0;
        while(numTrisFetched < 4)
        { 
            // Search for the next bin with triangles to process
            while(numTrisFetchedFromBin >= numTrisInBin)
            {
                curContext++;
                if(curContext >= numContexts)
                {
                    // We have no more bins
                    // So either we're all done
                    if(numTrisFetched == 0)
                        return;

                    // Or we still have a final batch of triangles to process
                    goto LABEL_PROCESS_TRIANGLES;
                }

                pCurBin += (NUM_BINS_X * NUM_BINS_Y);
                pCurNumTrisInBin += (NUM_BINS_X * NUM_BINS_Y);
                numTrisInBin = *pCurNumTrisInBin;
                numTrisFetchedFromBin = 0;
            }

            // Grab 1 triangle from the bin
            triVerts[numTrisFetched][0] = _mm_castsi128_ps( _mm_loadu_si128((const __m128i*) &(pCurBin->tris[numTrisFetchedFromBin].xy[0])) );
            triVerts[numTrisFetched][1] = _mm_castsi128_ps( _mm_loadl_epi64((const __m128i*) &(pCurBin->tris[numTrisFetchedFromBin].z[1])) );

            numTrisFetchedFromBin++;
            numTrisFetched++;
        }

LABEL_PROCESS_TRIANGLES:

        // Convert vertex for each triangle into SOA       
        __m128 v0 = triVerts[0][0];
        __m128 v1 = triVerts[1][0];
        __m128 v2 = triVerts[2][0];
        __m128 v3 = triVerts[3][0];

        _MM_TRANSPOSE4_PS(v0, v1, v2, v3);

        // Now v0, v1, v2 contain the corresponding verts
        // v3 also contains Z[0] but we don't care here

        // Extract X and Y from each vertex of each triangle
        __m128i fxPtX[3];
        __m128i fxPtY[3];
        fxPtX[0] = _mm_srai_epi32(_mm_slli_epi32(_mm_castps_si128(v0), 16), 16);    // Contains vertex 0 X for each triangle
        fxPtY[0] = _mm_srai_epi32(_mm_castps_si128(v0), 16);                        // Contains vertex 0 Y for each triangle
        fxPtX[1] = _mm_srai_epi32(_mm_slli_epi32(_mm_castps_si128(v1), 16), 16);    // Contains vertex 1 X for each triangle
        fxPtY[1] = _mm_srai_epi32(_mm_castps_si128(v1), 16);                        // Contains vertex 1 Y for each triangle
        fxPtX[2] = _mm_srai_epi32(_mm_slli_epi32(_mm_castps_si128(v2), 16), 16);    // Contains vertex 2 X for each triangle
        fxPtY[2] = _mm_srai_epi32(_mm_castps_si128(v2), 16);                        // Contains vertex 2 Y for each triangle

        // Note that since we convert Y from clip space to screen space, with 0 starting from the top
        // the line equations in screen space are in fact computed clockwise
        // Fab(x, y) =     Ax       +       By     +      C              = 0
        // Fab(x, y) = (ya - yb)x   +   (xb - xa)y + (xa * yb - xb * ya) = 0
        // Compute A = (ya - yb) for the 3 line segments that make up each triangle
        __m128i A0 = _mm_sub_epi32(fxPtY[1], fxPtY[2]);
        __m128i A1 = _mm_sub_epi32(fxPtY[2], fxPtY[0]);
        __m128i A2 = _mm_sub_epi32(fxPtY[0], fxPtY[1]);

        // Compute B = (xb - xa) for the 3 line segments that make up each triangle
        __m128i B0 = _mm_sub_epi32(fxPtX[2], fxPtX[1]);
        __m128i B1 = _mm_sub_epi32(fxPtX[0], fxPtX[2]);
        __m128i B2 = _mm_sub_epi32(fxPtX[1], fxPtX[0]);

        // Compute C = (xa * yb - xb * ya) for the 3 line segments that make up each triangle
        __m128i C0 = _mm_sub_epi32( _mm_mullo_epi32(fxPtX[1], fxPtY[2]), _mm_mullo_epi32(fxPtX[2], fxPtY[1]) );
        __m128i C1 = _mm_sub_epi32( _mm_mullo_epi32(fxPtX[2], fxPtY[0]), _mm_mullo_epi32(fxPtX[0], fxPtY[2]) );
        __m128i C2 = _mm_sub_epi32( _mm_mullo_epi32(fxPtX[0], fxPtY[1]), _mm_mullo_epi32(fxPtX[1], fxPtY[0]) );

        // Use bounding box traversal strategy to determine which pixels to rasterize
        // This guarantees the starting point is always rounded to the nearest even pixel, conservatively
        __m128i startX = _mm_and_si128( _mm_max_epi32( _mm_min_epi32( _mm_min_epi32(fxPtX[0], fxPtX[1]), fxPtX[2] ), _mm_set1_epi32(tileStartX) ), _mm_set1_epi32(0xFFFFFFFE) );    

        // This guarantees the end point always ends on the next pixel (but less than the tileEnd) because the inner loop increments by 2
        // and we want to make sure that the end pixel also gets rasterized (because < is used instead of <=)
        __m128i endX   = _mm_min_epi32( _mm_max_epi32( _mm_max_epi32(fxPtX[0], fxPtX[1]), fxPtX[2] ), _mm_set1_epi32(tileEndX) );   

        __m128i startY = _mm_and_si128( _mm_max_epi32( _mm_min_epi32( _mm_min_epi32(fxPtY[0], fxPtY[1]), fxPtY[2] ), _mm_set1_epi32(tileStartY) ), _mm_set1_epi32(0xFFFFFFFE) );
        __m128i endY   = _mm_min_epi32( _mm_max_epi32( _mm_max_epi32(fxPtY[0], fxPtY[1]), fxPtY[2] ), _mm_set1_epi32(tileEndY) );

        __m128i aOffset = _mm_mullo_epi32(A0, _mm_set1_epi32(8));
        __m128i bOffset = _mm_mullo_epi32(B0, _mm_set1_epi32(8));
        __m128i aGt = _mm_cmpgt_epi32(A0, _mm_setzero_si128());
        __m128i bGt = _mm_cmpgt_epi32(B0, _mm_setzero_si128());
        __m128i c0RejectBlock = _mm_add_epi32( _mm_and_si128( aGt, aOffset ), C0 );
        c0RejectBlock = _mm_add_epi32( c0RejectBlock, _mm_and_si128( bGt, bOffset ) );
        __m128i c0AcceptBlockOffset = _mm_add_epi32( _mm_abs_epi32(aOffset), _mm_abs_epi32(bOffset) );
                 
        aOffset = _mm_mullo_epi32(A1, _mm_set1_epi32(8));
        bOffset = _mm_mullo_epi32(B1, _mm_set1_epi32(8));
        aGt = _mm_cmpgt_epi32(A1, _mm_setzero_si128());
        bGt = _mm_cmpgt_epi32(B1, _mm_setzero_si128());
        __m128i c1RejectBlock = _mm_add_epi32( _mm_and_si128( aGt, aOffset ), C1 );
        c1RejectBlock = _mm_add_epi32( c1RejectBlock, _mm_and_si128( bGt, bOffset ) );
        __m128i c1AcceptBlockOffset = _mm_add_epi32( _mm_abs_epi32(aOffset), _mm_abs_epi32(bOffset) );

        aOffset = _mm_mullo_epi32(A2, _mm_set1_epi32(8));
        bOffset = _mm_mullo_epi32(B2, _mm_set1_epi32(8));
        aGt = _mm_cmpgt_epi32(A2, _mm_setzero_si128());
        bGt = _mm_cmpgt_epi32(B2, _mm_setzero_si128());
        __m128i c2RejectBlock = _mm_add_epi32( _mm_and_si128( aGt, aOffset ), C2 );
        c2RejectBlock = _mm_add_epi32( c2RejectBlock, _mm_and_si128( bGt, bOffset ) );
        __m128i c2AcceptBlockOffset = _mm_add_epi32( _mm_abs_epi32(aOffset), _mm_abs_epi32(bOffset) );

        // Rasterize all triangles individually
        for(uint curTri=0; curTri < numTrisFetched; curTri++)
        {
            // Extract this triangle's properties from the SIMD versions

            int startXx = startX.m128i_i32[curTri];
            int endXx	= endX.m128i_i32[curTri];
            int startYy = startY.m128i_i32[curTri];
            int endYy	= endY.m128i_i32[curTri];
          
            // Find increment for A, B blocks
            // Incrementally compute Fab(x, y) for all the pixels inside the bounding box formed by (startX, endX) and (startY, endY) 
            __m128i aa0 = _mm_set1_epi32(A0.m128i_i32[curTri]);
            __m128i aa1 = _mm_set1_epi32(A1.m128i_i32[curTri]);
            __m128i aa2 = _mm_set1_epi32(A2.m128i_i32[curTri]);

            __m128i bb0 = _mm_set1_epi32(B0.m128i_i32[curTri]);
            __m128i bb1 = _mm_set1_epi32(B1.m128i_i32[curTri]);
            __m128i bb2 = _mm_set1_epi32(B2.m128i_i32[curTri]);
            
            __m128i cc0 = _mm_set1_epi32(C0.m128i_i32[curTri]);
            __m128i cc1 = _mm_set1_epi32(C1.m128i_i32[curTri]);
            __m128i cc2 = _mm_set1_epi32(C2.m128i_i32[curTri]);

            // Find increment for A, B and depth
            // We increment by 2 pixels
            __m128i aa0Inc = _mm_slli_epi32(aa0, 1);
            __m128i aa1Inc = _mm_slli_epi32(aa1, 1);
            __m128i aa2Inc = _mm_slli_epi32(aa2, 1);

            __m128i bb0Inc = _mm_slli_epi32(bb0, 1);
            __m128i bb1Inc = _mm_slli_epi32(bb1, 1);
            __m128i bb2Inc = _mm_slli_epi32(bb2, 1);

            int startX8x8 = startXx & (~0xF);
            int endX8x8 = endXx & (~0xF);
            int startY8x8 = startYy & (~0xF);
            int endY8x8 = endYy & (~0xF);

                        // Increment by 16
            __m128i aa0BlockInc = _mm_slli_epi32(aa0, 4);
            __m128i aa1BlockInc = _mm_slli_epi32(aa1, 4);
            __m128i aa2BlockInc = _mm_slli_epi32(aa2, 4);

            __m128i bb0BlockInc = _mm_slli_epi32(bb0, 4);
            __m128i bb1BlockInc = _mm_slli_epi32(bb1, 4);
            __m128i bb2BlockInc = _mm_slli_epi32(bb2, 4);

            // Find A * x for 8x8 block
            __m128i blockCol = _mm_add_epi32(col4Offset, _mm_set1_epi32(startX8x8));
            __m128i aa0BlockCol = _mm_mullo_epi32(aa0, blockCol);
            __m128i aa1BlockCol = _mm_mullo_epi32(aa1, blockCol);
            __m128i aa2BlockCol = _mm_mullo_epi32(aa2, blockCol);

            // Find B * y + C for 2x2 quad
            __m128i blockRow = _mm_add_epi32(row4Offset, _mm_set1_epi32(startY8x8));
            __m128i bb0BlockRow = _mm_add_epi32(_mm_mullo_epi32(bb0, blockRow), _mm_set1_epi32(c0RejectBlock.m128i_i32[curTri]));
            __m128i bb1BlockRow = _mm_add_epi32(_mm_mullo_epi32(bb1, blockRow), _mm_set1_epi32(c1RejectBlock.m128i_i32[curTri]));
            __m128i bb2BlockRow = _mm_add_epi32(_mm_mullo_epi32(bb2, blockRow), _mm_set1_epi32(c2RejectBlock.m128i_i32[curTri]));

            __m128i c0AcceptBlockOffsetTri = _mm_set1_epi32(c0AcceptBlockOffset.m128i_i32[curTri]);
            __m128i c1AcceptBlockOffsetTri = _mm_set1_epi32(c1AcceptBlockOffset.m128i_i32[curTri]);
            __m128i c2AcceptBlockOffsetTri = _mm_set1_epi32(c2AcceptBlockOffset.m128i_i32[curTri]);

            // Find line equation = A * x + B * y + C
            // This also effectively gives 2x area of triangle from 2 vertices to the current point (x, y)
            __m128i sum0BlockRow = _mm_add_epi32(aa0BlockCol, bb0BlockRow);
            __m128i sum1BlockRow = _mm_add_epi32(aa1BlockCol, bb1BlockRow);
            __m128i sum2BlockRow = _mm_add_epi32(aa2BlockCol, bb2BlockRow);

            // Depth
            // zz[0] contains Z[0]
            // zz[1] contains Z[1] - Z[0]
            // zz[2] contains Z[2] - Z[0]
            __m128 depth0;
            __m128 depthDiff1;
            __m128 depthDiff2;
            depth0 = _mm_set1_ps(triVerts[curTri][0].m128_f32[3]);
            depthDiff1 = _mm_set1_ps(triVerts[curTri][1].m128_f32[0]);
            depthDiff2 = _mm_set1_ps(triVerts[curTri][1].m128_f32[1]);

            // This is the increment of (Z * area of the triangle / total area)
            // Computing (Z[1] - Z[0]) * dAreaOfTriangle1 / AreaOfTriangle 
            __m128 depthXInc = _mm_mul_ps(_mm_cvtepi32_ps(aa1Inc), depthDiff1);

            // Computing (Z[2] - Z[0]) * dAreaOfTriangle2 / AreaOfTriangle
            depthXInc = _mm_add_ps(depthXInc, _mm_mul_ps(_mm_cvtepi32_ps(aa2Inc), depthDiff2));

            __m128 depthYInc = _mm_mul_ps(_mm_cvtepi32_ps(bb1Inc), depthDiff1);
            depthYInc = _mm_add_ps(depthYInc, _mm_mul_ps(_mm_cvtepi32_ps(bb2Inc), depthDiff2));

            for(int yBlock=startY8x8; yBlock <= endY8x8; yBlock+=16)
            {
                __m128i e1Block = sum0BlockRow;
                __m128i e2Block = sum1BlockRow;
                __m128i e3Block = sum2BlockRow;

                for(int xBlock=startX8x8; xBlock <= endX8x8; xBlock+=16)
                {
                    // If any edge is outside, reject the block
                    // Edge is outside if negative (bit 31 is set)
                    __m128i rejectNotInsideTest = _mm_or_si128( _mm_or_si128( e1Block, e2Block ), e3Block );
                    int rejectNotInsideTestMask = ~_mm_movemask_ps( _mm_castsi128_ps(rejectNotInsideTest) ) & 0xF;
                    
                    __m128i accept0 = _mm_sub_epi32(e1Block, c0AcceptBlockOffsetTri);
                    __m128i accept1 = _mm_sub_epi32(e2Block, c1AcceptBlockOffsetTri);
                    __m128i accept2 = _mm_sub_epi32(e3Block, c2AcceptBlockOffsetTri);
                    __m128i acceptNotInsideTest = _mm_or_si128( _mm_or_si128( accept0, accept1 ), accept2 );
                    int acceptNotInsideTestMask = ~_mm_movemask_ps( _mm_castsi128_ps(acceptNotInsideTest) ) & 0xF;

                    rejectNotInsideTestMask = rejectNotInsideTestMask & ~acceptNotInsideTestMask;

                    uint blockBit;
                    while(_BitScanForward((DWORD*) &blockBit, acceptNotInsideTestMask))
                    {
                        int startXAccept = xBlock + ((blockBit & 1) << 3);
                        int startYAccept = yBlock + ((blockBit >> 1) << 3);
                       
                        // Find A * x for 2x2 quad
                        __m128i col = _mm_add_epi32(colOffset, _mm_set1_epi32(startXAccept));
                        __m128i aa1Col = _mm_mullo_epi32(aa1, col);
                        __m128i aa2Col = _mm_mullo_epi32(aa2, col);

                        // Find B * y + C for 2x2 quad
                        __m128i row = _mm_add_epi32(rowOffset, _mm_set1_epi32(startYAccept));
                        __m128i bb1Row = _mm_add_epi32(_mm_mullo_epi32(bb1, row), cc1);
                        __m128i bb2Row = _mm_add_epi32(_mm_mullo_epi32(bb2, row), cc2);

                        // Find line equation = A * x + B * y + C
                        // This also effectively gives 2x area of triangle from 2 vertices to the current point (x, y)
                        __m128i sum1Row = _mm_add_epi32(aa1Col, bb1Row);
                        __m128i sum2Row = _mm_add_epi32(aa2Col, bb2Row);

                        // Tranverse pixels in 2x2 blocks and store 2x2 pixel quad depths contiguously in memory ==> 2*X
                        int pixelIndex = (startYAccept * bufferWidth + 2 * startXAccept);

                        __m128 curDepth = depth0;
                        curDepth = _mm_add_ps(curDepth, _mm_mul_ps( _mm_cvtepi32_ps(sum1Row), depthDiff1 ));
                        curDepth = _mm_add_ps(curDepth, _mm_mul_ps( _mm_cvtepi32_ps(sum2Row), depthDiff2 ));

                        // Rasterization inner loop
                        // Incrementally compute Fab(x, y) for all the pixels inside the bounding box formed by (startX, endX) and (startY, endY)
                        for(int i=0; i < 4; i++)
                        {
                            // Compute barycentric coordinates 
                            // Z = Z0 * A0/A + Z1 * A1/A + Z2 * A2/A
                            //   = Z0 + (Z1 - Z0) * A1/A + (Z2 - Z0) * A2/A
                            // Where F1 = line equation for line 1 = 2x area of triangle from vertices on line 1 to current pt 
                            // and F2 = ..
                            int pixelIndexX = pixelIndex;
                            //__m128i beta = sum1Row;
                            //__m128i gamma = sum2Row;

                            // Using Z0 + (Z1 - Z0) * A1/A + (Z2 - Z0) * A2/A, compute barycentric-interpolated depth
                            //__m128 depth = depth0;
                            //depth = _mm_add_ps(depth, _mm_mul_ps( _mm_cvtepi32_ps(beta), depthDiff1 ));
                            //depth = _mm_add_ps(depth, _mm_mul_ps( _mm_cvtepi32_ps(gamma), depthDiff2 ));
                            __m128 depth = curDepth;

                            for(int j=0; j < 4; j++)
                            {
                                __m128 previousDepthValue = _mm_load_ps(&pDepthBuffer[pixelIndexX]);
                                __m128 mergedDepth = _mm_min_ps(depth, previousDepthValue);
                                __m128 finalDepth = mergedDepth;//_mm_blendv_ps(mergedDepth, previousDepthValue, _mm_castsi128_ps(mask));
                                _mm_store_ps(&pDepthBuffer[pixelIndexX], finalDepth);

                                // NOTE: This will be compiled out if context > 1
                                //if(numContexts == 1)
                                //    g_NumPixelsProcessed += 4;

                                pixelIndexX += 4;
                                depth = _mm_add_ps(depth, depthXInc);
                            }

                            pixelIndex += 2 * bufferWidth;
                            curDepth = _mm_add_ps(curDepth, depthYInc);

                            //sum1Row = _mm_add_epi32(sum1Row, bb1Inc);
                            //sum2Row = _mm_add_epi32(sum2Row, bb2Inc);
                        }

                        acceptNotInsideTestMask = (acceptNotInsideTestMask + 0xF) & acceptNotInsideTestMask;
                    }

                    while(_BitScanForward((DWORD*) &blockBit, rejectNotInsideTestMask))
                    {
                        int startXScan = xBlock + ((blockBit & 1) << 3);
                        int startYScan = yBlock + ((blockBit >> 1) << 3);
                        int endXScan = startXScan + 7;
                        int endYScan = startYScan + 7;

                        //startXScan = max(startXScan, startXx);
                        //startYScan = max(startYScan, startYy);
                        //endXScan = min(endXScan, endXx);
                        //endYScan = min(endYScan, endYy);

                        // Find A * x for 2x2 quad
                        __m128i col = _mm_add_epi32(colOffset, _mm_set1_epi32(startXScan));
                        __m128i aa0Col = _mm_mullo_epi32(aa0, col);
                        __m128i aa1Col = _mm_mullo_epi32(aa1, col);
                        __m128i aa2Col = _mm_mullo_epi32(aa2, col);

                        // Find B * y + C for 2x2 quad
                        __m128i row = _mm_add_epi32(rowOffset, _mm_set1_epi32(startYScan));
                        __m128i bb0Row = _mm_add_epi32(_mm_mullo_epi32(bb0, row), cc0);
                        __m128i bb1Row = _mm_add_epi32(_mm_mullo_epi32(bb1, row), cc1);
                        __m128i bb2Row = _mm_add_epi32(_mm_mullo_epi32(bb2, row), cc2);

                        // Find line equation = A * x + B * y + C
                        // This also effectively gives 2x area of triangle from 2 vertices to the current point (x, y)
                        __m128i sum0Row = _mm_add_epi32(aa0Col, bb0Row);
                        __m128i sum1Row = _mm_add_epi32(aa1Col, bb1Row);
                        __m128i sum2Row = _mm_add_epi32(aa2Col, bb2Row);

                        // Tranverse pixels in 2x2 blocks and store 2x2 pixel quad depths contiguously in memory ==> 2*X
                        int pixelIndex = (startYScan * bufferWidth + 2 * startXScan);

                        // Rasterization inner loop
                        // Incrementally compute Fab(x, y) for all the pixels inside the bounding box formed by (startX, endX) and (startY, endY)
                        for(int i=startYScan; i <= endYScan; i+=2)
                        {
                            // Compute barycentric coordinates 
                            // Z = Z0 * A0/A + Z1 * A1/A + Z2 * A2/A
                            //   = Z0 + (Z1 - Z0) * A1/A + (Z2 - Z0) * A2/A
                            // Where F1 = line equation for line 1 = 2x area of triangle from vertices on line 1 to current pt 
                            // and F2 = ..
                            int pixelIndexX = pixelIndex;
                            __m128i alpha = sum0Row;
                            __m128i beta = sum1Row;
                            __m128i gamma = sum2Row;

                            // Using Z0 + (Z1 - Z0) * A1/A + (Z2 - Z0) * A2/A, compute barycentric-interpolated depth
                            __m128 depth = depth0;
                            depth = _mm_add_ps(depth, _mm_mul_ps( _mm_cvtepi32_ps(beta), depthDiff1 ));
                            depth = _mm_add_ps(depth, _mm_mul_ps( _mm_cvtepi32_ps(gamma), depthDiff2 ));
                            //__m128 depth = curDepth;

                            for(int j=startXScan; j <= endXScan; j+=2)
                            {
                                // Test pixel inside triangle
                                // Basically, check if any edge fn is negative
                                __m128i mask = _mm_or_si128(_mm_or_si128(alpha, beta), gamma);

                                __m128 previousDepthValue = _mm_load_ps(&pDepthBuffer[pixelIndexX]);
                                __m128 mergedDepth = _mm_min_ps(depth, previousDepthValue);
                                __m128 finalDepth = _mm_blendv_ps(mergedDepth, previousDepthValue, _mm_castsi128_ps(mask));
                                _mm_store_ps(&pDepthBuffer[pixelIndexX], finalDepth);

                                // NOTE: This will be compiled out if context > 1
                                //if(numContexts == 1)
                                //    g_NumPixelsProcessed += 4;

                                pixelIndexX += 4;
                                alpha = _mm_add_epi32(alpha, aa0Inc);
                                beta  = _mm_add_epi32(beta, aa1Inc);
                                gamma = _mm_add_epi32(gamma, aa2Inc);
                                depth = _mm_add_ps(depth, depthXInc);
                            }

                            pixelIndex += 2 * bufferWidth;
                            sum0Row = _mm_add_epi32(sum0Row, bb0Inc);
                            sum1Row = _mm_add_epi32(sum1Row, bb1Inc);
                            sum2Row = _mm_add_epi32(sum2Row, bb2Inc);
                        }

                        rejectNotInsideTestMask = (rejectNotInsideTestMask + 0xF) & rejectNotInsideTestMask;
                    }

                    e1Block = _mm_add_epi32(e1Block, aa0BlockInc);
                    e2Block = _mm_add_epi32(e2Block, aa1BlockInc);
                    e3Block = _mm_add_epi32(e3Block, aa2BlockInc);
                }

                sum0BlockRow = _mm_add_epi32(sum0BlockRow, bb0BlockInc);
                sum1BlockRow = _mm_add_epi32(sum1BlockRow, bb1BlockInc);
                sum2BlockRow = _mm_add_epi32(sum2BlockRow, bb2BlockInc);
            }            


            
        }

    }while(true);
}


_NAMESPACE_END