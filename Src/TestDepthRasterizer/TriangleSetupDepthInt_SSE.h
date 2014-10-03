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

//#pragma optimize("", off)
//#define _FORCE_INLINE inline

_FORCE_INLINE __m128 Interpolate(__m128 a, __m128 b, float t)
{
	return _mm_add_ps( a, _mm_mul_ps( _mm_sub_ps(b, a), _mm_set1_ps(t) ) );
}

_FORCE_INLINE gmtl::VecA4f* ClipTriangleZNear(__m128& v0, __m128& v1, __m128& v2, gmtl::VecA4f* pExtraTriangleOutVertices, uint& validTriMask, uint curTri)
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

_FORCE_INLINE int ClipPoly1Plane(__m128 src[8], int numSrc, __m128 dst[8], int insideOutsideMask, __m128 tArr[2])
{
	int numDst = 0;

	float* t = (float*) tArr;
	_LOOPi(numSrc)
	{
		int startIndex = i;
		int endIndex = (i+1) % numSrc;
		int startVertMask = 1 << startIndex;
		int endVertMask = 1 << endIndex;
		int insideOutsideVal = ((insideOutsideMask & startVertMask) >> startIndex) | (((insideOutsideMask & endVertMask) >> endIndex) << 1);

		__m128 startV = src[startIndex];
		__m128 endV = src[endIndex];

		switch(insideOutsideVal)
		{
		case 0x1:
			{
				// in - out
				// Output intersection
				dst[numDst] = Interpolate(startV, endV, t[startIndex] / (t[startIndex] - t[endIndex]));
				numDst++;
				break;
			}

		case 0x2:
			{
				// out - in
				// Output intersection and next
				// We make sure we always clip in-out order to preserve bit-exactness
				dst[numDst] = Interpolate(endV, startV, t[endIndex] / (t[endIndex] - t[startIndex]));
				dst[numDst+1] = endV;
				numDst += 2;
				break;
			}

		case 0x3:
			{
				// in - in
				dst[numDst] = endV;
				numDst++;
				break;
			}
		}
	}

	return numDst;
}

_FORCE_INLINE gmtl::VecA4f* ClipTriangle(const __m128& v0, const __m128& v1, const __m128& v2, gmtl::VecA4f* pExtraTriangleOutVertices)
{
	// TODO: This NEEDS to be clipped against the guard band, not some arbitrary viewport range!
	// Might produce seams at triangle edges!!!!!!!!!!
	const static float VIEWPORT_CLIP_RCP = 1.0f / 160.0f;

	__m128 dst[8];
	__m128 src[8];
	__m128 tVal[2];
	src[0] = v0;
	src[1] = v1;
	src[2] = v2;
	src[3] = v0;

	// Near plane
	__m128 vX = src[0];
	__m128 vY = src[1];
	__m128 vZ = src[2];
	__m128 vW = src[3];
	_MM_TRANSPOSE4_PS(vX, vY, vZ, vW);
	tVal[0] = vZ;
	int insideOutsideMask = _mm_movemask_ps( _mm_cmpge_ps(tVal[0], _mm_setzero_ps()) );

	int numDst = ClipPoly1Plane(src, 3, dst, insideOutsideMask, tVal);
	//if(numDst == 0)
	//	return pExtraTriangleOutVertices;

	// Left plane
	src[0] = dst[0];
	src[1] = dst[1];
	src[2] = dst[2];
	src[3] = dst[3];

	vX = src[0];
	vY = src[1];
	vZ = src[2];
	vW = src[3];
	_MM_TRANSPOSE4_PS(vX, vY, vZ, vW);
	tVal[0] = _mm_add_ps( _mm_mul_ps(vX, _mm_set1_ps(VIEWPORT_CLIP_RCP)), vW );
	insideOutsideMask = _mm_movemask_ps( _mm_cmpge_ps(tVal[0], _mm_setzero_ps()) );
	if(numDst > 4)
	{
		src[4] = dst[4];
		src[5] = dst[5];
		src[6] = dst[6];
		src[7] = dst[7];

		vX = src[4];
		vY = src[5];
		vZ = src[6];
		vW = src[7];
		_MM_TRANSPOSE4_PS(vX, vY, vZ, vW);
		tVal[1] = _mm_add_ps( _mm_mul_ps(vX, _mm_set1_ps(VIEWPORT_CLIP_RCP)), vW );
		insideOutsideMask |= _mm_movemask_ps( _mm_cmpge_ps(tVal[1], _mm_setzero_ps()) ) << 4;
	}

	numDst = ClipPoly1Plane(src, numDst, dst, insideOutsideMask, tVal);
	_DEBUG_ASSERT(numDst < 8);

	// Right plane
	src[0] = dst[0];
	src[1] = dst[1];
	src[2] = dst[2];
	src[3] = dst[3];

	vX = src[0];
	vY = src[1];
	vZ = src[2];
	vW = src[3];
	_MM_TRANSPOSE4_PS(vX, vY, vZ, vW);
	tVal[0] = _mm_add_ps( _mm_mul_ps(vX, _mm_set1_ps(-VIEWPORT_CLIP_RCP)), vW );
	insideOutsideMask = _mm_movemask_ps( _mm_cmpge_ps(tVal[0], _mm_setzero_ps()) );
	if(numDst > 4)
	{
		src[4] = dst[4];
		src[5] = dst[5];
		src[6] = dst[6];
		src[7] = dst[7];

		vX = src[4];
		vY = src[5];
		vZ = src[6];
		vW = src[7];
		_MM_TRANSPOSE4_PS(vX, vY, vZ, vW);
		tVal[1] = _mm_add_ps( _mm_mul_ps(vX, _mm_set1_ps(-VIEWPORT_CLIP_RCP)), vW );
		insideOutsideMask |= _mm_movemask_ps( _mm_cmpge_ps(tVal[1], _mm_setzero_ps()) ) << 4;
	}

	numDst = ClipPoly1Plane(src, numDst, dst, insideOutsideMask, tVal);
	_DEBUG_ASSERT(numDst < 8);

	// Bottom plane
	src[0] = dst[0];
	src[1] = dst[1];
	src[2] = dst[2];
	src[3] = dst[3];

	vX = src[0];
	vY = src[1];
	vZ = src[2];
	vW = src[3];
	_MM_TRANSPOSE4_PS(vX, vY, vZ, vW);
	tVal[0] = _mm_add_ps( _mm_mul_ps(vY, _mm_set1_ps(VIEWPORT_CLIP_RCP)), vW );
	insideOutsideMask = _mm_movemask_ps( _mm_cmpge_ps(tVal[0], _mm_setzero_ps()) );
	if(numDst > 4)
	{
		src[4] = dst[4];
		src[5] = dst[5];
		src[6] = dst[6];
		src[7] = dst[7];

		vX = src[4];
		vY = src[5];
		vZ = src[6];
		vW = src[7];
		_MM_TRANSPOSE4_PS(vX, vY, vZ, vW);
		tVal[1] = _mm_add_ps( _mm_mul_ps(vY, _mm_set1_ps(VIEWPORT_CLIP_RCP)), vW );
		insideOutsideMask |= _mm_movemask_ps( _mm_cmpge_ps(tVal[1], _mm_setzero_ps()) ) << 4;
	}

	numDst = ClipPoly1Plane(src, numDst, dst, insideOutsideMask, tVal);
	_DEBUG_ASSERT(numDst < 8);

	// Top plane
	src[0] = dst[0];
	src[1] = dst[1];
	src[2] = dst[2];
	src[3] = dst[3];

	vX = src[0];
	vY = src[1];
	vZ = src[2];
	vW = src[3];
	_MM_TRANSPOSE4_PS(vX, vY, vZ, vW);
	tVal[0] = _mm_add_ps( _mm_mul_ps(vY, _mm_set1_ps(-VIEWPORT_CLIP_RCP)), vW );
	insideOutsideMask = _mm_movemask_ps( _mm_cmpge_ps(tVal[0], _mm_setzero_ps()) );
	if(numDst > 4)
	{
		src[4] = dst[4];
		src[5] = dst[5];
		src[6] = dst[6];
		src[7] = dst[7];

		vX = src[4];
		vY = src[5];
		vZ = src[6];
		vW = src[7];
		_MM_TRANSPOSE4_PS(vX, vY, vZ, vW);
		tVal[1] = _mm_add_ps( _mm_mul_ps(vY, _mm_set1_ps(-VIEWPORT_CLIP_RCP)), vW );
		insideOutsideMask |= _mm_movemask_ps( _mm_cmpge_ps(tVal[1], _mm_setzero_ps()) ) << 4;
	}

	numDst = ClipPoly1Plane(src, numDst, dst, insideOutsideMask, tVal);
	_DEBUG_ASSERT(numDst < 8);

	if(numDst >= 3)
	{
		_mm_store_ps((float*) &(pExtraTriangleOutVertices[0]), dst[0]);
		_mm_store_ps((float*) &(pExtraTriangleOutVertices[1]), dst[1]);
		_mm_store_ps((float*) &(pExtraTriangleOutVertices[2]), dst[2]);

		_LOOPi(numDst - 3)
		{
			_mm_store_ps((float*) &(pExtraTriangleOutVertices[(i+1)*3]), dst[0]);
			_mm_store_ps((float*) &(pExtraTriangleOutVertices[(i+1)*3+1]), dst[(i+1)+1]);
			_mm_store_ps((float*) &(pExtraTriangleOutVertices[(i+1)*3+2]), dst[(i+1)+2]);
		}

		return pExtraTriangleOutVertices + 3 + (numDst - 3) * 3;
	}	

	return pExtraTriangleOutVertices;
}

//_FORCE_INLINE int ClipTriangle1Plane(__m128 src[], __m128 dst[], const __m128& t)
//{
//	int insideOutsideMask = _mm_movemask_ps( _mm_cmpge_ps(t, _mm_setzero_ps()) );
//	int numDst = 0;
//
//	__m128 tempDst[4];
//	_LOOPi(3)
//	{
//		int startIndex = i;
//		int endIndex = (i+1) % 3;
//		int startVertMask = 1 << startIndex;
//		int endVertMask = 1 << endIndex;
//		int insideOutsideVal = ((insideOutsideMask & startVertMask) >> startIndex) | (((insideOutsideMask & endVertMask) >> endIndex) << 1);
//
//		__m128 startV = src[startIndex];
//		__m128 endV = src[endIndex];
//
//		switch(insideOutsideVal)
//		{
//		case 0x1:
//			{
//				// in - out
//				// Output intersection
//				tempDst[numDst] = Interpolate(startV, endV, t.m128_f32[startIndex] / (t.m128_f32[startIndex] - t.m128_f32[endIndex]));
//				numDst++;
//				break;
//			}
//
//		case 0x2:
//			{
//				// out - in
//				// Output intersection and next
//				// We make sure we always clip in-out order to preserve bit-exactness
//				tempDst[numDst] = Interpolate(endV, startV, t.m128_f32[endIndex] / (t.m128_f32[endIndex] - t.m128_f32[startIndex]));
//				tempDst[numDst+1] = endV;
//				numDst += 2;
//				break;
//			}
//
//		case 0x3:
//			{
//				// in - in
//				tempDst[numDst] = endV;
//				numDst++;
//				break;
//			}
//		}
//	}
//
//	if(numDst == 3)
//	{
//		dst[0] = tempDst[0];
//		dst[1] = tempDst[1];
//		dst[2] = tempDst[2];
//
//		return 1;
//	}
//	else if(numDst == 4)
//	{
//		dst[0] = tempDst[0];
//		dst[1] = tempDst[1];
//		dst[2] = tempDst[2];
//		dst[3] = tempDst[0];
//		dst[4] = tempDst[2];
//		dst[5] = tempDst[3];
//
//		return 2;
//	}
//
//	return 0;
//}
//
//_FORCE_INLINE gmtl::VecA4f* ClipTriangle(const __m128& v0, const __m128& v1, const __m128& v2, gmtl::VecA4f* pExtraTriangleOutVertices)
//{
//	// TODO: This NEEDS to be clipped against the guard band, not some arbitrary viewport range!
//	const static float VIEWPORT_CLIP_RCP = 1.0f / 130.0f;
//
//	__m128 dst[32*3];
//	__m128 src[32*3];
//	src[0] = v0;
//	src[1] = v1;
//	src[2] = v2;
//	
//	// Near plane
//	__m128 vX = src[0];
//	__m128 vY = src[1];
//	__m128 vZ = src[2];
//	__m128 vW = vX;
//	_MM_TRANSPOSE4_PS(vX, vY, vZ, vW);
//	__m128 t = vZ;//_mm_add_ps( _mm_mul_ps(vX, _mm_set1_ps(VIEWPORT_CLIP_RCP)), _mm_set1_ps(1.0f) );
//	//t = _mm_mul_ps(t, vW);
//
//	int numTri = ClipTriangle1Plane(src, dst, t);
//	
//	// Left plane
//	int prevNumTri = numTri;
//	_LOOPi(prevNumTri)
//	{
//		src[i*3] = dst[i*3];
//		src[i*3+1] = dst[i*3+1];
//		src[i*3+2] = dst[i*3+2];
//	}
//
//	numTri = 0;
//	_LOOPi(prevNumTri)
//	{
//		vX = src[i*3];
//		vY = src[i*3+1];
//		vZ = src[i*3+2];
//		vW = vX;
//		_MM_TRANSPOSE4_PS(vX, vY, vZ, vW);
//		t = _mm_add_ps( _mm_mul_ps(vX, _mm_set1_ps(VIEWPORT_CLIP_RCP)), vW );
//		//t = _mm_mul_ps(t, vW);
//
//		numTri += ClipTriangle1Plane(&(src[i*3]), &(dst[numTri*3]), t);
//	}
//
//	// Right plane
//	prevNumTri = numTri;
//	_LOOPi(prevNumTri)
//	{
//		src[i*3] = dst[i*3];
//		src[i*3+1] = dst[i*3+1];
//		src[i*3+2] = dst[i*3+2];
//	}
//
//	numTri = 0;
//	_LOOPi(prevNumTri)
//	{
//		vX = src[i*3];
//		vY = src[i*3+1];
//		vZ = src[i*3+2];
//		vW = vX;
//		_MM_TRANSPOSE4_PS(vX, vY, vZ, vW);
//		t = _mm_add_ps( _mm_mul_ps(vX, _mm_set1_ps(-VIEWPORT_CLIP_RCP)), vW );
//		//t = _mm_mul_ps(t, vW);
//
//		numTri += ClipTriangle1Plane(&(src[i*3]), &(dst[numTri*3]), t);
//	}
//
//	// Bottom plane
//	prevNumTri = numTri;
//	_LOOPi(prevNumTri)
//	{
//		src[i*3] = dst[i*3];
//		src[i*3+1] = dst[i*3+1];
//		src[i*3+2] = dst[i*3+2];
//	}
//
//	numTri = 0;
//	_LOOPi(prevNumTri)
//	{
//		vX = src[i*3];
//		vY = src[i*3+1];
//		vZ = src[i*3+2];
//		vW = vX;
//		_MM_TRANSPOSE4_PS(vX, vY, vZ, vW);
//		t = _mm_add_ps( _mm_mul_ps(vY, _mm_set1_ps(VIEWPORT_CLIP_RCP)), vW );
//		//t = _mm_mul_ps(t, vW);
//
//		numTri += ClipTriangle1Plane(&(src[i*3]), &(dst[numTri*3]), t);
//	}
//
//	// Top plane
//	prevNumTri = numTri;
//	_LOOPi(prevNumTri)
//	{
//		src[i*3] = dst[i*3];
//		src[i*3+1] = dst[i*3+1];
//		src[i*3+2] = dst[i*3+2];
//	}
//
//	numTri = 0;
//	_LOOPi(prevNumTri)
//	{
//		vX = src[i*3];
//		vY = src[i*3+1];
//		vZ = src[i*3+2];
//		vW = vX;
//		_MM_TRANSPOSE4_PS(vX, vY, vZ, vW);
//		t = _mm_add_ps( _mm_mul_ps(vY, _mm_set1_ps(-VIEWPORT_CLIP_RCP)), vW );
//		//t = _mm_mul_ps(t, vW);
//
//		numTri += ClipTriangle1Plane(&(src[i*3]), &(dst[numTri*3]), t);
//	}
//
//	_DEBUG_ASSERT(numTri < 32);
//	_LOOPi(numTri)
//	{
//		_mm_store_ps((float*) &(pExtraTriangleOutVertices[i*3]), dst[i*3]);
//		_mm_store_ps((float*) &(pExtraTriangleOutVertices[i*3+1]), dst[i*3+1]);
//		_mm_store_ps((float*) &(pExtraTriangleOutVertices[i*3+2]), dst[i*3+2]);
//	}
//
//	return pExtraTriangleOutVertices + (numTri * 3);
//}

template<uint binWidth, uint binHeight, uint bufferWidth, uint bufferHeight>
_FORCE_INLINE void ProcessTrianglesDepthInt(const gmtl::VecA4f* pVertices, const ushort* pIndices, TriangleBin* pBins, uint* pNumTrisInBins, int numTrisToProcess, gmtl::VecA4f* pClippedTriVertices, uint* pNumClippedTris)
{
	_DEBUG_ASSERT((numTrisToProcess > 0) && (numTrisToProcess <= 4));

	_DEBUG_COMPILE_ASSERT((bufferWidth % binWidth) == 0);
	_DEBUG_COMPILE_ASSERT((bufferHeight % binHeight) == 0);

	const uint NUM_BINS_X = bufferWidth / binWidth;
	const uint NUM_BINS_Y = bufferHeight / binHeight;

	__m128 vX;
	__m128 vY;
	__m128 vZ[3];
	__m128 vW;
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

	// V0
	_MM_TRANSPOSE4_PS(v00, v10, v20, v30);
	vX = v00;
	vY = v10;
	vZ[0] = v20;
	vW = v30;
	
	__m128 isNotNearClippedV0 = _mm_cmpgt_ps(v20, _mm_setzero_ps());
	int isNotNearClippedMask = _mm_movemask_ps(isNotNearClippedV0);
	int isAllNearClippedMask = ~isNotNearClippedMask;

	// Warning: All fixed point integers need to be clipped to [-16384, 16383] in order to avoid overflows
	// Do viewport transform
	vX = _mm_div_ps( vX, vW );	
	vX = _mm_add_ps( _mm_mul_ps(vX, _mm_set1_ps(0.5f * bufferWidth)), _mm_set1_ps(0.5f * bufferWidth - 0.5f / bufferWidth) );

	// TODO: Do guardband and viewport check in homogeneous space
	const float GUARD_BAND_LIMIT = 32000.0f;
	int isNotGuardBandClipped = _mm_movemask_ps( _mm_and_ps( _mm_cmplt_ps(vX, _mm_set1_ps(GUARD_BAND_LIMIT)), _mm_cmpgt_ps(vX, _mm_set1_ps(-GUARD_BAND_LIMIT)) ) );

	fxPtX[0] = _mm_cvtps_epi32(vX); 

	// Viewport transform for Y (also flip Y so Y starts from top in viewport space)
	vY = _mm_div_ps( vY, vW );
	vY = _mm_add_ps( _mm_mul_ps(vY, _mm_set1_ps(-0.5f * bufferHeight)), _mm_set1_ps(0.5f * bufferHeight - 0.5f / bufferHeight) );
	isNotGuardBandClipped &= _mm_movemask_ps( _mm_and_ps( _mm_cmplt_ps(vY, _mm_set1_ps(GUARD_BAND_LIMIT)), _mm_cmpgt_ps(vY, _mm_set1_ps(-GUARD_BAND_LIMIT)) ) );

	fxPtY[0] = _mm_cvtps_epi32(vY); 

	__m128i inter0 = _mm_unpacklo_epi32(fxPtX[0], fxPtY[0]);
	__m128i inter1 = _mm_unpackhi_epi32(fxPtX[0], fxPtY[0]);
	fxPackedXY[0] = _mm_packs_epi32(inter0, inter1);

	vZ[0] = _mm_div_ps( vZ[0], vW );

	// V1
	_MM_TRANSPOSE4_PS(v01, v11, v21, v31);
	vX = v01;
	vY = v11;
	vZ[1] = v21;
	vW = v31;

	__m128 isNotNearClippedV1 = _mm_cmpgt_ps(v21, _mm_setzero_ps());
	int isNotNearClippedMaskV1 = _mm_movemask_ps(isNotNearClippedV1);
	isNotNearClippedMask &= isNotNearClippedMaskV1;
	isAllNearClippedMask &= ~isNotNearClippedMaskV1;

	vX = _mm_div_ps( vX, vW );
	vX = _mm_add_ps( _mm_mul_ps(vX, _mm_set1_ps(0.5f * bufferWidth)), _mm_set1_ps(0.5f * bufferWidth - 0.5f / bufferWidth) );
	isNotGuardBandClipped &= _mm_movemask_ps( _mm_and_ps( _mm_cmplt_ps(vX, _mm_set1_ps(GUARD_BAND_LIMIT)), _mm_cmpgt_ps(vX, _mm_set1_ps(-GUARD_BAND_LIMIT)) ) );
	fxPtX[1] = _mm_cvtps_epi32(vX); 

	vY = _mm_div_ps( vY, vW );
	vY = _mm_add_ps( _mm_mul_ps(vY, _mm_set1_ps(-0.5f * bufferHeight)), _mm_set1_ps(0.5f * bufferHeight - 0.5f / bufferHeight) );
	isNotGuardBandClipped &= _mm_movemask_ps( _mm_and_ps( _mm_cmplt_ps(vY, _mm_set1_ps(GUARD_BAND_LIMIT)), _mm_cmpgt_ps(vY, _mm_set1_ps(-GUARD_BAND_LIMIT)) ) );
	fxPtY[1] = _mm_cvtps_epi32(vY); 

	inter0 = _mm_unpacklo_epi32(fxPtX[1], fxPtY[1]);
	inter1 = _mm_unpackhi_epi32(fxPtX[1], fxPtY[1]);
	fxPackedXY[1] = _mm_packs_epi32(inter0, inter1);

	vZ[1] = _mm_div_ps( vZ[1], vW );

	// V2
	_MM_TRANSPOSE4_PS(v02, v12, v22, v32);		
	vX = v02;
	vY = v12;
	vZ[2] = v22;
	vW = v32;

	__m128 isNotNearClippedV2 = _mm_cmpgt_ps(v22, _mm_setzero_ps());
	int isNotNearClippedMaskV2 = _mm_movemask_ps(isNotNearClippedV2);
	isNotNearClippedMask &= isNotNearClippedMaskV2;
	isAllNearClippedMask &= ~isNotNearClippedMaskV2;

	vX = _mm_div_ps( vX, vW );
	vX = _mm_add_ps( _mm_mul_ps(vX, _mm_set1_ps(0.5f * bufferWidth)), _mm_set1_ps(0.5f * bufferWidth - 0.5f / bufferWidth) );
	isNotGuardBandClipped &= _mm_movemask_ps( _mm_and_ps( _mm_cmplt_ps(vX, _mm_set1_ps(GUARD_BAND_LIMIT)), _mm_cmpgt_ps(vX, _mm_set1_ps(-GUARD_BAND_LIMIT)) ) );
	fxPtX[2] = _mm_cvtps_epi32(vX); 

	vY = _mm_div_ps( vY, vW );
	vY = _mm_add_ps( _mm_mul_ps(vY, _mm_set1_ps(-0.5f * bufferHeight)), _mm_set1_ps(0.5f * bufferHeight - 0.5f / bufferHeight) );
	isNotGuardBandClipped &= _mm_movemask_ps( _mm_and_ps( _mm_cmplt_ps(vY, _mm_set1_ps(GUARD_BAND_LIMIT)), _mm_cmpgt_ps(vY, _mm_set1_ps(-GUARD_BAND_LIMIT)) ) );
	fxPtY[2] = _mm_cvtps_epi32(vY); 

	inter0 = _mm_unpacklo_epi32(fxPtX[2], fxPtY[2]);
	inter1 = _mm_unpackhi_epi32(fxPtX[2], fxPtY[2]);
	fxPackedXY[2] = _mm_packs_epi32(inter0, inter1);

	vZ[2] = _mm_div_ps( vZ[2], vW );

	// Compute 2x tri area using cross product
	__m128i ad = _mm_sub_epi32(fxPtX[1], fxPtX[0]);
	ad = _mm_mullo_epi32(ad, _mm_sub_epi32(fxPtY[2], fxPtY[0]));

	__m128i bc = _mm_sub_epi32(fxPtX[2], fxPtX[0]);
	bc = _mm_mullo_epi32(bc, _mm_sub_epi32(fxPtY[1], fxPtY[0]));

	__m128i triArea = _mm_sub_epi32(ad, bc);
	__m128 oneOverTriArea = _mm_div_ps(_mm_set1_ps(1.0f), _mm_cvtepi32_ps(triArea));//_mm_rcp_ps(_mm_cvtepi32_ps(triArea));

	// Z setup
	// This will be used in interpolation later when we derive the area of the triangle
	// using the line equation (actually 2x area of triangle)
	vZ[1] = _mm_mul_ps(_mm_sub_ps(vZ[1], vZ[0]), oneOverTriArea);
	vZ[2] = _mm_mul_ps(_mm_sub_ps(vZ[2], vZ[0]), oneOverTriArea);

	//__m128 ad = _mm_sub_ps(vX[1], vX[0]);
	//ad = _mm_mul_ps(ad, _mm_sub_ps(vY[2], vY[0]));

	//__m128 bc = _mm_sub_ps(vX[2], vX[0]);
	//bc = _mm_mul_ps(bc, _mm_sub_ps(vY[1], vY[0]));

	//__m128 triArea = _mm_sub_ps(ad, bc);
	//__m128 oneOverTriArea = _mm_rcp_ps(triArea);

	// Cull triangles if any vertex is clipped by viewport planes
	int isNotClippedMask = isNotNearClippedMask & isNotGuardBandClipped;
	
	// Find bounding box for screen space triangle in terms of pixels
	__m128i vStartX = _mm_min_epi32( _mm_min_epi32(fxPtX[0], fxPtX[1]), fxPtX[2] );
	__m128i vEndX   = _mm_max_epi32( _mm_max_epi32(fxPtX[0], fxPtX[1]), fxPtX[2] );
	__m128i vStartY = _mm_min_epi32( _mm_min_epi32(fxPtY[0], fxPtY[1]), fxPtY[2] );
	__m128i vEndY   = _mm_max_epi32( _mm_max_epi32(fxPtY[0], fxPtY[1]), fxPtY[2] );

	// Cull triangles that need to be clipped at X and Y
	// Since we are packing into 16-bit integers, we need to make sure they do not overflow
	//const int GUARD_BAND_LIMIT = 30000;

	//__m128i isInsideGuardX = _mm_and_si128( _mm_cmpgt_epi32(vStartX, _mm_set1_epi32(-GUARD_BAND_LIMIT)), _mm_cmplt_epi32(vEndX, _mm_set1_epi32(GUARD_BAND_LIMIT)) );
	//__m128i isInsideGuardY = _mm_and_si128( _mm_cmpgt_epi32(vStartY, _mm_set1_epi32(-GUARD_BAND_LIMIT)), _mm_cmplt_epi32(vEndY, _mm_set1_epi32(GUARD_BAND_LIMIT)) );
	//__m128i isInsideGuardXY = _mm_and_si128( isInsideGuardX, isInsideGuardY );
	//isNotClippedMask &= _mm_movemask_ps( _mm_castsi128_ps(isInsideGuardXY) );
	
	// Cull triangles that fall entirely outside
	__m128i isNotAllOutsideX = _mm_and_si128( _mm_cmplt_epi32(vStartX, _mm_set1_epi32(bufferWidth - 1)), _mm_cmpgt_epi32(vEndX, _mm_set1_epi32(0)) );
	__m128i isNotAllOutsideY = _mm_and_si128( _mm_cmplt_epi32(vStartY, _mm_set1_epi32(bufferHeight - 1)), _mm_cmpgt_epi32(vEndY, _mm_set1_epi32(0)) );

	__m128i isInsideBoundsX = isNotAllOutsideX;
	__m128i isInsideBoundsY = isNotAllOutsideY;

	// Cull triangles that are back-facing
	__m128i isFront = _mm_cmpgt_epi32(triArea, _mm_setzero_si128());

	__m128 tmpAccept = _mm_castsi128_ps( _mm_and_si128( _mm_and_si128(isFront, isInsideBoundsX), isInsideBoundsY) );
	__m128 accept = tmpAccept;//_mm_and_ps(_mm_and_ps(isNotNearClippedV1, isNotNearClippedV2), _mm_and_ps(isNotNearClippedV3, tmpAccept));

	// If this is inlined, all these should be compiled out
	if(!pClippedTriVertices)
		isNotClippedMask = 0xF;

	uint triMask = isNotClippedMask & validTriMask & _mm_movemask_ps(accept);

	// Add each valid triangle to bin
	while(triMask)
	{
		uint triangleIndex;
		_BitScanForward((unsigned long*) &triangleIndex, triMask);

		// Convert bounding box in terms of pixels to bounding box in terms of tiles
		int startX = max(vStartX.m128i_i32[triangleIndex] / (int) binWidth, 0);
		int endX   = min(vEndX.m128i_i32[triangleIndex] / (int) binWidth, (bufferWidth - 1) / binWidth);
		int startY = max(vStartY.m128i_i32[triangleIndex] / (int) binHeight, 0);
		int endY   = min(vEndY.m128i_i32[triangleIndex] / (int) binHeight, (bufferHeight - 1) / binHeight);

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

	// Do clipping if needed for any triangle
	// We only bother to clip if:
	//	1. At least 1 vertex is outside the guardband
	//	2. At least 1 vertex is behind the near plane
	//	3. At least 1 vertex is in front of the near plane (we ignore tris that are completely behind)
	//
	// TODO: We should also not bother if a triangle is entirely outside the bounds of the viewport
	// We should do this in homogeneous space and avoid (back-projected) perspective divide
	int isClippedMask = ~isNotClippedMask & validTriMask & ~isAllNearClippedMask;
	if(pClippedTriVertices && isClippedMask)
	{
		gmtl::VecA4f* pCurClippedTriVertices = pClippedTriVertices;

		while(isClippedMask)
		{
			uint triangleIndex;
			_BitScanForward((unsigned long*) &triangleIndex, isClippedMask);

			const ushort* pTriIndices = pIndices + triangleIndex * 3;

			__m128 v0 = _mm_load_ps( (float*) (pVertices + pTriIndices[0]) );	// V0 of tri1
			__m128 v1 = _mm_load_ps( (float*) (pVertices + pTriIndices[1]) );	// V1 of tri1
			__m128 v2 = _mm_load_ps( (float*) (pVertices + pTriIndices[2]) );	// V2 of tri1

			pCurClippedTriVertices = ClipTriangle(v0, v1, v2, pCurClippedTriVertices);
			isClippedMask &= (isClippedMask - 1);    
		}

		_DEBUG_ASSERT( (((uint) ((size_t) (pCurClippedTriVertices - pClippedTriVertices))) / 3) < (uint)(numTrisToProcess * 8) ); 
		*pNumClippedTris = ((uint) ((size_t) (pCurClippedTriVertices - pClippedTriVertices))) / 3;
	}
}

template<uint binWidth, uint binHeight, uint bufferWidth, uint bufferHeight>
void TriangleSetupDepthIntBatch(const gmtl::VecA4f* pVertices, const ushort* pIndices, uint numTriangles, TriangleBin* pBins, uint* pNumTrisInBins)
{
    _DEBUG_COMPILE_ASSERT((bufferWidth % binWidth) == 0);
    _DEBUG_COMPILE_ASSERT((bufferHeight % binHeight) == 0);

	const static uint NUM_TRIANGLE_LANES = 4;

	const static ushort clippedTriIndices[NUM_TRIANGLE_LANES * 3] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 }; 
	
	gmtl::VecA4f clippedTriVerts[NUM_TRIANGLE_LANES * 8 * 3];
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

		uint numClippedTrisProcessed = 0;
		while(numClippedTrisProcessed < numClippedTris)
		{
			uint numClippedTrisGather = NUM_TRIANGLE_LANES;
			if((numClippedTrisGather + numClippedTrisProcessed) > numClippedTris)
				numClippedTrisGather = numClippedTris - numClippedTrisProcessed;

			ProcessTrianglesDepthInt<binWidth, binHeight, bufferWidth, bufferHeight>(clippedTriVerts + numClippedTrisProcessed*3, clippedTriIndices, pBins, pNumTrisInBins, numClippedTrisGather, NULL, NULL);

			numClippedTrisProcessed += 4;
		}
    }
}

_NAMESPACE_END