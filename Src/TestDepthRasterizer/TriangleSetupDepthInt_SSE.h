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

#pragma optimize("", off)

inline __m128 Interpolate(__m128 a, __m128 b, float t)
{
	return _mm_add_ps( a, _mm_mul_ps( _mm_sub_ps(b, a), _mm_set1_ps(t) ) );
}

inline gmtl::VecA4f* ClipTriangle(__m128& v0, __m128& v1, __m128& v2, gmtl::VecA4f* pExtraTriangleOutVertices, uint& validTriMask, uint curTri)
{
	static const __m128 SIGNMASK = _mm_castsi128_ps(_mm_set1_epi32(0x80000000));

	// This allows a guard band of -10000 to 10000
	const static float CLIP_EPSILON = 0.0001f;
	__m128 clipMult = _mm_set1_ps(CLIP_EPSILON);
	__m128 v0W = _mm_shuffle_ps(v0, _MM_SHUFFLE(3, 3, 3, 3));
	__m128 v0WNeg = _mm_xor_ps(v0W, SIGNMASK);

	

	// Copy the source data ( add an extra vertex to avoid paying for a mod at each element )
	vec3 src [4]; 
	src [0]= v2 ; 
	src [1]= v0 ; 
	src [2]= v1 ; 
	src [3]= v2 ;

	vec3 dstVertex [4];
	int numDst = 0;

	// For each edge
	for ( int i = 0; i < 3; ++ i ) 
	{
		vec3 A = src [ i ] , B = src [ i + 1];
		float Adist = dot (A , n ) , Bdist = dot (B , n );
		if ( Adist >= clipEpsilon2 ) 
		{
			if ( Bdist >= clipEpsilon2 ) 
			{
				// Both are inside , so emit B only
				dst [ numDst ++] = B ;
			} 
			else 
			{
				// Exiting : emit the intersection only
				dst [ numDst ++] = intersect (A , Adist , B , Bdist );
			}
		}
		else if ( Bdist >= clipEpsilon2 ) 
		{
			// Entering : emit both the intersection and B
			dst [ numDst ++] = intersect (A , Adist , B , Bdist );
			dst [ numDst ++] = B ;
		}
	}

	// Put the data back into the variables used as input
	v0 = dst [0]; 
	v1 = dst [1]; 
	v2 = dst [2]; 
	v3 = dst [3];

	return numDst ;
}

inline  gmtl::VecA4f* ClipTriangleZNear(__m128& v0, __m128& v1, __m128& v2, gmtl::VecA4f* pExtraTriangleOutVertices, uint& validTriMask, uint curTri)
{
	__m128 clipEpsilon = _mm_setzero_ps();

	// Distances to the plane
	__m128 dist = _mm_movelh_ps( _mm_unpackhi_ps(v0, v1), _mm_movehl_ps(v2, v2) );

	__m128 isInFront = _mm_cmpge_ps(dist, clipEpsilon);
	int isInFrontMask = _mm_movemask_ps(isInFront) & 0x7;

	if (isInFrontMask == 0x7) 
	{
		// None clipped (original triangle vertices are unmodified)
		return pExtraTriangleOutVertices;
	}

	if (isInFrontMask == 0 || !(validTriMask & (1 << curTri))) 
	{
		// All clipped
		//v0 = _mm_set1_ps(-1.0f);
		//v1 = _mm_set1_ps(-1.0f);
		//v2 = _mm_set1_ps(-1.0f);
		validTriMask &= ~(1 << curTri);

		return pExtraTriangleOutVertices;
	}  

	//bvec3 above = greaterThanEqual(dist, vec3(0.0));
	int isAboveMask = isInFrontMask;

	// There are either 1 or 2 vertices above the clipping plane.
	int nextIsAbove;

	__m128 v3;

	// Find the CCW-most vertex above the plane by cycling
	// the vertices in place.  There are three cases.
	if ((isAboveMask & 0x2) && !(isAboveMask & 0x1)) 
	{
		nextIsAbove = isAboveMask & 0x4;
		
		// Cycle once CCW.  Use v3 as a temp
		v3 = v0; 
		v0 = v1; 
		v1 = v2; 
		v2 = v3;
		dist = _mm_shuffle_ps(dist, dist, _MM_SHUFFLE(0, 0, 2, 1));// dist.yzx;
	} 
	else if ((isAboveMask & 0x4) && !(isAboveMask & 0x2))
	{
		// Cycle once CW.  Use v3 as a temp.
		nextIsAbove = isAboveMask & 0x1;
		v3 = v2; 
		v2 = v1; 
		v1 = v0; 
		v0 = v3;
		dist = _mm_shuffle_ps(dist, dist, _MM_SHUFFLE(1, 1, 0, 2));// dist.zxy;
	} 
	else 
	{
		nextIsAbove = isAboveMask & 0x2;
	}
	// Note: The above[] values are no longer in sync with v values and dist[].

	// We always need to clip v2-v0.
	float t = dist.m128_f32[0] / (dist.m128_f32[0] - dist.m128_f32[2]);
	v3 = Interpolate(v0, v2, t);
	//mix(v0, v2, dist[0] / (dist[0] - dist[2]));

	if (nextIsAbove) 
	{
		// There is a quadrilateral above the plane
		//
		//    v0---------v1
		//      \        |
		//   ....v3......v2'...
		//          \    |
		//            \  |
		//              v2

		float t = dist.m128_f32[1] / (dist.m128_f32[1] - dist.m128_f32[2]);
		v2 = Interpolate(v1, v2, t);

		_mm_store_ps((float*) &(pExtraTriangleOutVertices[0]), v0);
		_mm_store_ps((float*) &(pExtraTriangleOutVertices[1]), v2);
		_mm_store_ps((float*) &(pExtraTriangleOutVertices[2]), v3);
		
		//v2 = mix(v1, v2, dist[1] / (dist[1] - dist[2]));
		return pExtraTriangleOutVertices + 3;
	} 
	else 
	{
		// There is a triangle above the plane
		//
		//            v0
		//           / |
		//         /   |
		//   ....v2'..v1'...
		//      /      |
		//    v2-------v1

		float t = dist.m128_f32[0] / (dist.m128_f32[0] - dist.m128_f32[1]);
		v1 = Interpolate(v0, v1, t);

		//v1 = mix(v0, v1, dist[0] / (dist[0] - dist[1]));
		v2 = v3;
		return pExtraTriangleOutVertices;
	}
}

template<uint binWidth, uint binHeight, uint bufferWidth, uint bufferHeight>
inline  void ProcessTrianglesDepthInt(const gmtl::VecA4f* pVertices, const ushort* pIndices, TriangleBin* pBins, uint* pNumTrisInBins, int numTrisToProcess, gmtl::VecA4f* pClippedTriVertices, uint* pNumClippedTris)
{
	_DEBUG_ASSERT((numTrisToProcess > 0) && (numTrisToProcess <= 4));

	_DEBUG_COMPILE_ASSERT((bufferWidth % binWidth) == 0);
	_DEBUG_COMPILE_ASSERT((bufferHeight % binHeight) == 0);

	const uint NUM_BINS_X = bufferWidth / binWidth;
	const uint NUM_BINS_Y = bufferHeight / binHeight;

	__m128 vX[3];
	__m128 vY[3];
	__m128 vZ[3];
	__m128 vW[3];
	__m128i fxPtX[3], fxPtY[3];
	__m128i fxPackedXY[3];
	// __m128 vZ[3];

	uint numClippedTris = 0;
	uint validTriMask = (1 << numTrisToProcess) - 1;

	const ushort* pTriIndices1 = pIndices;
	const ushort* pTriIndices2 = pIndices + (numTrisToProcess > 1 ? 3 : 0);
	const ushort* pTriIndices3 = pIndices + (numTrisToProcess > 2 ? 6 : 0);
	const ushort* pTriIndices4 = pIndices + (numTrisToProcess > 3 ? 9 : 0);

	// Transform data from AOS to SOA
	__m128 v00 = _mm_load_ps( (float*) (pVertices + pTriIndices1[0]) );	// V0 of tri1
	__m128 v01 = _mm_load_ps( (float*) (pVertices + pTriIndices1[1]) );	// V1 of tri1
	__m128 v02 = _mm_load_ps( (float*) (pVertices + pTriIndices1[2]) );	// V2 of tri1

	__m128 v10 = _mm_load_ps( (float*) (pVertices + pTriIndices2[0]) );	// V0 of tri2
	__m128 v11 = _mm_load_ps( (float*) (pVertices + pTriIndices2[1]) );	// V1 of tri2
	__m128 v12 = _mm_load_ps( (float*) (pVertices + pTriIndices2[2]) );	// V2 of tri2

	__m128 v20 = _mm_load_ps( (float*) (pVertices + pTriIndices3[0]) );	// V0 of tri3
	__m128 v21 = _mm_load_ps( (float*) (pVertices + pTriIndices3[1]) );	// V1 of tri3
	__m128 v22 = _mm_load_ps( (float*) (pVertices + pTriIndices3[2]) );	// V2 of tri3

	__m128 v30 = _mm_load_ps( (float*) (pVertices + pTriIndices4[0]) );	// V0 of tri4     
	__m128 v31 = _mm_load_ps( (float*) (pVertices + pTriIndices4[1]) );	// V1 of tri4
	__m128 v32 = _mm_load_ps( (float*) (pVertices + pTriIndices4[2]) );	// V2 of tri4

	if(pClippedTriVertices)
	{
		gmtl::VecA4f* pCurClippedTriVertices = ClipTriangleZNear(v00, v01, v02, pClippedTriVertices, validTriMask, 0);
		pCurClippedTriVertices = ClipTriangleZNear(v10, v11, v12, pCurClippedTriVertices, validTriMask, 1);
		pCurClippedTriVertices = ClipTriangleZNear(v20, v21, v22, pCurClippedTriVertices, validTriMask, 2);
		pCurClippedTriVertices = ClipTriangleZNear(v30, v31, v32, pCurClippedTriVertices, validTriMask, 3);

		*pNumClippedTris = ((uint) ((size_t) (pCurClippedTriVertices - pClippedTriVertices))) / 3;
	}

	// V1
	_MM_TRANSPOSE4_PS(v00, v10, v20, v30);
	vX[0] = v00;
	vY[0] = v10;
	vZ[0] = v20;
	vW[0] = v30;

	// Warning: All fixed point integers need to be clipped to [-16384, 16383] in order to avoid overflows
	// Do viewport transform
	vX[0] = _mm_div_ps( vX[0], vW[0] );
	vX[0] = _mm_add_ps( _mm_mul_ps(vX[0], _mm_set1_ps(0.5f * bufferWidth)), _mm_set1_ps(0.5f * bufferWidth - 0.5f / bufferWidth) );
	
	// Viewport transform for y (also flip y so why starts from top in viewport space)
	vY[0] = _mm_div_ps( vY[0], vW[0] );
	vY[0] = _mm_add_ps( _mm_mul_ps(vY[0], _mm_set1_ps(-0.5f * bufferHeight)), _mm_set1_ps(0.5f * bufferHeight - 0.5f / bufferHeight) );
	
	vZ[0] = _mm_div_ps( vZ[0], vW[0] );

	// V2
	_MM_TRANSPOSE4_PS(v01, v11, v21, v31);
	vX[1] = v01;
	vY[1] = v11;
	vZ[1] = v21;
	vW[1] = v31;

	vX[1] = _mm_div_ps( vX[1], vW[1] );
	vX[1] = _mm_add_ps( _mm_mul_ps(vX[1], _mm_set1_ps(0.5f * bufferWidth)), _mm_set1_ps(0.5f * bufferWidth - 0.5f / bufferWidth) );
	
	vY[1] = _mm_div_ps( vY[1], vW[1] );
	vY[1] = _mm_add_ps( _mm_mul_ps(vY[1], _mm_set1_ps(-0.5f * bufferHeight)), _mm_set1_ps(0.5f * bufferHeight - 0.5f / bufferHeight) );
	
	vZ[1] = _mm_div_ps( vZ[1], vW[1] );

	// V3
	_MM_TRANSPOSE4_PS(v02, v12, v22, v32);
	vX[2] = v02;
	vY[2] = v12;
	vZ[2] = v22;
	vW[2] = v32;

	vX[2] = _mm_div_ps( vX[2], vW[2] );
	vX[2] = _mm_add_ps( _mm_mul_ps(vX[2], _mm_set1_ps(0.5f * bufferWidth)), _mm_set1_ps(0.5f * bufferWidth - 0.5f / bufferWidth) );
	
	vY[2] = _mm_div_ps( vY[2], vW[2] );
	vY[2] = _mm_add_ps( _mm_mul_ps(vY[2], _mm_set1_ps(-0.5f * bufferHeight)), _mm_set1_ps(0.5f * bufferHeight - 0.5f / bufferHeight) );
	
	vZ[2] = _mm_div_ps( vZ[2], vW[2] );

	// Convert X coordinates of triangles for vertex i
	fxPtX[0] = _mm_cvtps_epi32(vX[0]); 
	fxPtX[1] = _mm_cvtps_epi32(vX[1]); 
	fxPtX[2] = _mm_cvtps_epi32(vX[2]); 

	// Convert Y coordinates of triangles for vertex i
	fxPtY[0] = _mm_cvtps_epi32(vY[0]); 
	fxPtY[1] = _mm_cvtps_epi32(vY[1]); 
	fxPtY[2] = _mm_cvtps_epi32(vY[2]); 

	// Pack X and Y of vertex i for triangles into signed 16-bit integers
	__m128i inter0 = _mm_unpacklo_epi32(fxPtX[0], fxPtY[0]);
	__m128i inter1 = _mm_unpackhi_epi32(fxPtX[0], fxPtY[0]);
	fxPackedXY[0] = _mm_packs_epi32(inter0, inter1);

	inter0 = _mm_unpacklo_epi32(fxPtX[1], fxPtY[1]);
	inter1 = _mm_unpackhi_epi32(fxPtX[1], fxPtY[1]);
	fxPackedXY[1] = _mm_packs_epi32(inter0, inter1);

	inter0 = _mm_unpacklo_epi32(fxPtX[2], fxPtY[2]);
	inter1 = _mm_unpackhi_epi32(fxPtX[2], fxPtY[2]);
	fxPackedXY[2] = _mm_packs_epi32(inter0, inter1);

	// Compute 2x tri area using cross product
	//__m128i ad = _mm_sub_epi32(fxPtX[1], fxPtX[0]);
	//ad = _mm_mullo_epi32(ad, _mm_sub_epi32(fxPtY[2], fxPtY[0]));

	//__m128i bc = _mm_sub_epi32(fxPtX[2], fxPtX[0]);
	//bc = _mm_mullo_epi32(bc, _mm_sub_epi32(fxPtY[1], fxPtY[0]));

	//__m128i triArea = _mm_sub_ps(ad, bc);
	//__m128 oneOverTriArea = _mm_rcp_ps(_mm_cvtepi32_ps(triArea));

	__m128 ad = _mm_sub_ps(vX[1], vX[0]);
	ad = _mm_mul_ps(ad, _mm_sub_ps(vY[2], vY[0]));

	__m128 bc = _mm_sub_ps(vX[2], vX[0]);
	bc = _mm_mul_ps(bc, _mm_sub_ps(vY[1], vY[0]));

	__m128 triArea = _mm_sub_ps(ad, bc);
	__m128 oneOverTriArea = _mm_rcp_ps(triArea);

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
	__m128i isFront = _mm_castps_si128( _mm_cmpgt_ps(triArea, _mm_setzero_ps()) );

	// Cull triangles whose bounding box fall outside the buffer bounds
	__m128i isInsideBoundsX = _mm_cmpgt_epi32(vEndX, vStartX);
	__m128i isInsideBoundsY = _mm_cmpgt_epi32(vEndY, vStartY);

	//// Cull triangles if any vertex is clipped by near plane
	//__m128 isNotNearClippedV1 = _mm_cmpgt_ps(vW[0], _mm_setzero_ps());
	//__m128 isNotNearClippedV2 = _mm_cmpgt_ps(vW[1], _mm_setzero_ps());
	//__m128 isNotNearClippedV3 = _mm_cmpgt_ps(vW[2], _mm_setzero_ps());

	__m128 tmpAccept = _mm_castsi128_ps( _mm_and_si128( _mm_and_si128(isFront, isInsideBoundsX), isInsideBoundsY) );
	__m128 accept = tmpAccept;// _mm_and_ps(_mm_and_ps(isNotNearClippedV1, isNotNearClippedV2), _mm_and_ps(isNotNearClippedV3, tmpAccept));

	uint triMask =  validTriMask & _mm_movemask_ps(accept);

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

				_DEBUG_ASSERT(pNumTrisInBins[triBinOffset] < TriangleBin::MAX_NUM_TRIANGLES_PER_BIN);
			}
		}

		triMask &= (triMask - 1);           
	}
}

template<uint binWidth, uint binHeight, uint bufferWidth, uint bufferHeight>
void TriangleSetupDepthIntBatch(const gmtl::VecA4f* pVertices, const ushort* pIndices, uint numTriangles, TriangleBin* pBins, uint* pNumTrisInBins)
{
    _DEBUG_COMPILE_ASSERT((bufferWidth % binWidth) == 0);
    _DEBUG_COMPILE_ASSERT((bufferHeight % binHeight) == 0);

	const static uint NUM_TRIANGLE_LANES = 4;

	const static ushort clippedTriIndices[NUM_TRIANGLE_LANES * 3] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 }; 
	
	gmtl::VecA4f clippedTriVerts[NUM_TRIANGLE_LANES * 3];
	uint numClippedTris = 0;
	
    // Assumes that all vertices have been transformed to viewport space - top left corner is (0, 0)
    // NOTE: We should center the viewport such that (0, 0) is in the center rather than the corner
    uint numTrisProcessed = 0;
    while(numTrisProcessed < numTriangles)
    {
        // Process NUM_TRIANGLE_LANES triangles at a time
        uint numTrisGather = NUM_TRIANGLE_LANES;
        if((numTrisGather + numTrisProcessed) > numTriangles)
            numTrisGather = numTriangles - numTrisProcessed;

		ProcessTrianglesDepthInt<binWidth, binHeight, bufferWidth, bufferHeight>(pVertices, pIndices + numTrisProcessed*3, pBins, pNumTrisInBins, numTrisGather, clippedTriVerts, &numClippedTris);

        numTrisProcessed += 4;

		if(numClippedTris > 0)
		{
			ProcessTrianglesDepthInt<binWidth, binHeight, bufferWidth, bufferHeight>(clippedTriVerts, clippedTriIndices, pBins, pNumTrisInBins, numClippedTris, NULL, NULL);
		}
    }
}

_NAMESPACE_END