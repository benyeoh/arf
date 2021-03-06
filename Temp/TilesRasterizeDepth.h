#ifndef EBONY_RENDERER_DEPTH_RASTERIZER_TILES_RASTERIZE
#define EBONY_RENDERER_DEPTH_RASTERIZER_TILES_RASTERIZE

namespace SQEX { namespace Ebony { namespace Renderer {

#ifndef EBONY_ORBIS
	#define GET_ELEMENT(xmm, i)		(xmm.m128_f32[i])
	#define GET_ELEMENTi(xmm, i)	(xmm.m128i_i32[i])
#else
	#define GET_ELEMENT(xmm, i)		(((float*) (&xmm))[i])
	#define GET_ELEMENTi(xmm, i)	(((int*) (&xmm))[i])
#endif

template<unsigned int binWidth, unsigned int binHeight, unsigned int bufferWidth, unsigned int bufferHeight, unsigned int numContexts>
void TilesRasterizeDepthUnrollXInt(const TriangleBin* pBins, const unsigned int * pNumTrisInBins, float* pDepthBuffer, int binStartX, int binStartY)
{
	//_mm_setcsr( _mm_getcsr() | 0x8040 );

	const unsigned int NUM_BINS_X = bufferWidth / binWidth;
	const unsigned int NUM_BINS_Y = bufferHeight / binHeight;

	__m128i colOffset = _mm_setr_epi32(0, 1, 0, 1);
	__m128i rowOffset = _mm_setr_epi32(0, 0, 1, 1);

	// Based on TaskId determine which tile to process
	//UINT tileX = taskId % screenWidthInTiles;
	//UINT tileY = taskId / screenWidthInTiles;

	int tileStartX = binStartX * binWidth;
	int tileEndX   = (tileStartX + binWidth - 1) > bufferWidth - 1 ? bufferWidth - 1 : (tileStartX + binWidth - 1);

	int tileStartY = binStartY * binHeight;
	int tileEndY   = (tileStartY + binHeight - 1) > bufferHeight - 1 ? bufferHeight - 1 : (tileStartY + binHeight - 1);

	EBONY_ASSERT( (tileStartX & 1) == 0 && (tileStartY & 1) == 0 );
	EBONY_ASSERT( (tileEndX & 1) == 1 && (tileEndY & 1) == 1 );

	//// Clear tile buffer to 1.0
	//// Note we need to account for tiling pattern here
	//__m128 clearVal = _mm_set1_ps(1.0f);
	//for(int i=tileStartY; i < tileEndY; i+=2)
	//{
	//	float* pToClear = (pDepthBuffer + (i * bufferWidth + 2 * tileStartX));       
	//	for(int j=0; j < binWidth; j+=2)
	//	{
	//		//_mm_stream_ps(pToClear, clearVal);
	//		_mm_store_ps(pToClear, clearVal);
	//		pToClear += 4;
	//	}

	//	//memset(&pDepthBuffer[index], 0, sizeof(float) * 2 * binWidth);
	//}

	// Process all triangles in all contexts for this bin
	unsigned int curContext = 0;
	const TriangleBin* pCurBin = pBins + (curContext * NUM_BINS_X * NUM_BINS_Y);
	const unsigned int* pCurNumTrisInBin = pNumTrisInBins + (curContext * NUM_BINS_X * NUM_BINS_Y);
	unsigned int numTrisInBin = *pCurNumTrisInBin;
	unsigned int numTrisFetchedFromBin = 0;

	__m128 triVerts[4][2];

	do 
	{
		// Grab 4 triangles from bins to process
		unsigned int numTrisFetched = 0;
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
		for(unsigned int curTri=0; curTri < numTrisFetched; curTri++)
		{
			// Extract this triangle's properties from the SIMD versions

			// zz[0] contains Z[0]
			// zz[1] contains Z[1] - Z[0]
			// zz[2] contains Z[2] - Z[0]
			__m128 depth0;
			__m128 depthDiff1;
			__m128 depthDiff2;

			depth0 = _mm_set1_ps( GET_ELEMENT(triVerts[curTri][0], 3) );
			depthDiff1 = _mm_set1_ps( GET_ELEMENT(triVerts[curTri][1], 0) );
			depthDiff2 = _mm_set1_ps( GET_ELEMENT(triVerts[curTri][1], 1) );

			int startXx = GET_ELEMENTi(startX, curTri);
			int startYy = GET_ELEMENTi(startY, curTri);
			int numY	= GET_ELEMENTi(diffY, curTri);
			int numX    = GET_ELEMENTi(diffX, curTri);

			// Incrementally compute Fab(x, y) for all the pixels inside the bounding box formed by (startX, endX) and (startY, endY) 
			__m128i aa0 = _mm_set1_epi32(GET_ELEMENTi(A0, curTri));
			__m128i aa1 = _mm_set1_epi32(GET_ELEMENTi(A1, curTri));
			__m128i aa2 = _mm_set1_epi32(GET_ELEMENTi(A2, curTri));

			__m128i bb0 = _mm_set1_epi32(GET_ELEMENTi(B0, curTri));
			__m128i bb1 = _mm_set1_epi32(GET_ELEMENTi(B1, curTri));
			__m128i bb2 = _mm_set1_epi32(GET_ELEMENTi(B2, curTri));

			// Find A * x for 2x2 quad
			__m128i col = _mm_add_epi32(colOffset, _mm_set1_epi32(startXx));
			__m128i aa0Col = _mm_mullo_epi32(aa0, col);
			__m128i aa1Col = _mm_mullo_epi32(aa1, col);
			__m128i aa2Col = _mm_mullo_epi32(aa2, col);

			// Find B * y + C for 2x2 quad
			__m128i row = _mm_add_epi32(rowOffset, _mm_set1_epi32(startYy));
			__m128i bb0Row = _mm_add_epi32(_mm_mullo_epi32(bb0, row), _mm_set1_epi32(GET_ELEMENTi(C0, curTri)));
			__m128i bb1Row = _mm_add_epi32(_mm_mullo_epi32(bb1, row), _mm_set1_epi32(GET_ELEMENTi(C1, curTri)));
			__m128i bb2Row = _mm_add_epi32(_mm_mullo_epi32(bb2, row), _mm_set1_epi32(GET_ELEMENTi(C2, curTri)));

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

	}while(true);
}

}}}

#endif 
