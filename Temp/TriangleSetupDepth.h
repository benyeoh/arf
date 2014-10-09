#ifndef EBONY_RENDERER_DEPTH_RASTERIZER_TRIANGLE_SETUP_DEPTH
#define EBONY_RENDERER_DEPTH_RASTERIZER_TRIANGLE_SETUP_DEPTH

#include "Ebony/Math/Math.h"

namespace SQEX { namespace Ebony { namespace Renderer {

#ifndef EBONY_ORBIS
#define GET_ELEMENT(xmm, i)		(xmm.m128_f32[i])
#define GET_ELEMENTi(xmm, i)	(xmm.m128i_i32[i])
#else
#define GET_ELEMENT(xmm, i)		(((float*) (&xmm))[i])
#define GET_ELEMENTi(xmm, i)	(((int*) (&xmm))[i])
#endif

EBONY_FORCEINLINE __m128 Interpolate(__m128 a, __m128 b, float t)
{
	return _mm_add_ps( a, _mm_mul_ps( _mm_sub_ps(b, a), _mm_set1_ps(t) ) );
}

EBONY_FORCEINLINE int ClipPoly1Plane(__m128 src[8], int numSrc, __m128 dst[8], int insideOutsideMask, __m128 tArr[2])
{
	int numDst = 0;

	float* t = (float*) tArr;
	for(int i=0; i < numSrc; i++)
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

EBONY_FORCEINLINE Math::VectorA* ClipTriangle(const __m128& v0, const __m128& v1, const __m128& v2, float guardBand, Math::VectorA* pExtraTriangleOutVertices)
{	
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
	tVal[0] = _mm_add_ps( _mm_mul_ps(vX, _mm_set1_ps(guardBand)), vW );
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
		tVal[1] = _mm_add_ps( _mm_mul_ps(vX, _mm_set1_ps(guardBand)), vW );
		insideOutsideMask |= _mm_movemask_ps( _mm_cmpge_ps(tVal[1], _mm_setzero_ps()) ) << 4;
	}

	numDst = ClipPoly1Plane(src, numDst, dst, insideOutsideMask, tVal);
	//EBONY_ASSERT(numDst < 8);

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
	tVal[0] = _mm_add_ps( _mm_mul_ps(vX, _mm_set1_ps(-guardBand)), vW );
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
		tVal[1] = _mm_add_ps( _mm_mul_ps(vX, _mm_set1_ps(-guardBand)), vW );
		insideOutsideMask |= _mm_movemask_ps( _mm_cmpge_ps(tVal[1], _mm_setzero_ps()) ) << 4;
	}

	numDst = ClipPoly1Plane(src, numDst, dst, insideOutsideMask, tVal);
	//EBONY_ASSERT(numDst < 8);

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
	tVal[0] = _mm_add_ps( _mm_mul_ps(vY, _mm_set1_ps(guardBand)), vW );
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
		tVal[1] = _mm_add_ps( _mm_mul_ps(vY, _mm_set1_ps(guardBand)), vW );
		insideOutsideMask |= _mm_movemask_ps( _mm_cmpge_ps(tVal[1], _mm_setzero_ps()) ) << 4;
	}

	numDst = ClipPoly1Plane(src, numDst, dst, insideOutsideMask, tVal);
	//EBONY_ASSERT(numDst < 8);

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
	tVal[0] = _mm_add_ps( _mm_mul_ps(vY, _mm_set1_ps(-guardBand)), vW );
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
		tVal[1] = _mm_add_ps( _mm_mul_ps(vY, _mm_set1_ps(-guardBand)), vW );
		insideOutsideMask |= _mm_movemask_ps( _mm_cmpge_ps(tVal[1], _mm_setzero_ps()) ) << 4;
	}

	numDst = ClipPoly1Plane(src, numDst, dst, insideOutsideMask, tVal);
	//EBONY_ASSERT(numDst < 8);

	if(numDst >= 3)
	{
		_mm_store_ps((float*) &(pExtraTriangleOutVertices[0]), dst[0]);
		_mm_store_ps((float*) &(pExtraTriangleOutVertices[1]), dst[1]);
		_mm_store_ps((float*) &(pExtraTriangleOutVertices[2]), dst[2]);

		for(int i=0; i < (numDst - 3); i++)
		{
			_mm_store_ps((float*) &(pExtraTriangleOutVertices[(i+1)*3]), dst[0]);
			_mm_store_ps((float*) &(pExtraTriangleOutVertices[(i+1)*3+1]), dst[(i+1)+1]);
			_mm_store_ps((float*) &(pExtraTriangleOutVertices[(i+1)*3+2]), dst[(i+1)+2]);
		}

		return pExtraTriangleOutVertices + 3 + (numDst - 3) * 3;
	}	

	return pExtraTriangleOutVertices;
}

template<unsigned int binWidth, unsigned int binHeight, unsigned int bufferWidth, unsigned int bufferHeight>
EBONY_FORCEINLINE void ProcessTrianglesDepthInt(const Math::VectorA* pVertices, const unsigned short* pIndices, TriangleBin* pBins, unsigned int* pNumTrisInBins, int numTrisToProcess, Math::VectorA* pClippedTriVertices, unsigned int* pNumClippedTris, bool isUseClipping)
{
	const static unsigned int NUM_TRIANGLE_LANES = 4;

	EBONY_ASSERT((numTrisToProcess > 0) && (numTrisToProcess <= NUM_TRIANGLE_LANES));

	const unsigned int NUM_BINS_X = bufferWidth / binWidth;
	const unsigned int NUM_BINS_Y = bufferHeight / binHeight;

	__m128 vX;
	__m128 vY;
	__m128 vZ[3];
	__m128 vW;
	__m128i fxPtX[3], fxPtY[3];
	__m128i fxPackedXY[3];
	// __m128 vZ[3];

	unsigned int numClippedTris = 0;
	unsigned int validTriMask = (1 << numTrisToProcess) - 1;

	const unsigned short* EBONY_RESTRICT pTriIndices1 = pIndices;
	const unsigned short* EBONY_RESTRICT pTriIndices2 = pIndices + (numTrisToProcess > 1 ? 3 : 0);
	const unsigned short* EBONY_RESTRICT pTriIndices3 = pIndices + (numTrisToProcess > 2 ? 6 : 0);
	const unsigned short* EBONY_RESTRICT pTriIndices4 = pIndices + (numTrisToProcess > 3 ? 9 : 0);

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

	// This gives us a viewport guard band of up to 30000 pixels, assuming a viewport of size bufferWidth
	// We need to set the GUARD_BAND_PIXELS to be something less than 32767 since we later pack the x/y
	// into one 32-bit integer for performance reasons
	const float GUARD_BAND_PIXELS = 30000.0f;
	const float GUARD_BAND_LIMIT = 1.0f / ((GUARD_BAND_PIXELS / bufferWidth - 0.5f) * 2.0f);

	// We do the clipping test in homogeneous space
	int isNotGuardBandClipped = _mm_movemask_ps( _mm_and_ps( _mm_cmplt_ps( _mm_mul_ps(vX, _mm_set1_ps(GUARD_BAND_LIMIT)), vW ), _mm_cmpgt_ps(_mm_mul_ps(vX, _mm_set1_ps(GUARD_BAND_LIMIT)), _mm_xor_ps(vW, _mm_set1_ps(-0.0))) ) );
	isNotGuardBandClipped &= _mm_movemask_ps( _mm_and_ps( _mm_cmplt_ps( _mm_mul_ps(vY, _mm_set1_ps(GUARD_BAND_LIMIT)), vW ), _mm_cmpgt_ps(_mm_mul_ps(vY, _mm_set1_ps(GUARD_BAND_LIMIT)), _mm_xor_ps(vW, _mm_set1_ps(-0.0))) ) );

	// Do viewport transform
	vX = _mm_div_ps( vX, vW );
	vX = _mm_add_ps( _mm_mul_ps(vX, _mm_set1_ps(0.5f * bufferWidth)), _mm_set1_ps(0.5f * bufferWidth - 0.5f) );

	fxPtX[0] = _mm_cvtps_epi32(vX); 

	// Viewport transform for Y (also flip Y so Y starts from top in viewport space)
	vY = _mm_div_ps( vY, vW );
	vY = _mm_add_ps( _mm_mul_ps(vY, _mm_set1_ps(-0.5f * bufferHeight)), _mm_set1_ps(0.5f * bufferHeight - 0.5f) );
	//isNotGuardBandClipped &= _mm_movemask_ps( _mm_and_ps( _mm_cmplt_ps(vY, _mm_set1_ps(GUARD_BAND_LIMIT)), _mm_cmpgt_ps(vY, _mm_set1_ps(-GUARD_BAND_LIMIT)) ) );

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

	isNotGuardBandClipped &= _mm_movemask_ps( _mm_and_ps( _mm_cmplt_ps( _mm_mul_ps(vX, _mm_set1_ps(GUARD_BAND_LIMIT)), vW ), _mm_cmpgt_ps(_mm_mul_ps(vX, _mm_set1_ps(GUARD_BAND_LIMIT)), _mm_xor_ps(vW, _mm_set1_ps(-0.0))) ) );
	isNotGuardBandClipped &= _mm_movemask_ps( _mm_and_ps( _mm_cmplt_ps( _mm_mul_ps(vY, _mm_set1_ps(GUARD_BAND_LIMIT)), vW ), _mm_cmpgt_ps(_mm_mul_ps(vY, _mm_set1_ps(GUARD_BAND_LIMIT)), _mm_xor_ps(vW, _mm_set1_ps(-0.0))) ) );

	vX = _mm_div_ps( vX, vW );
	vX = _mm_add_ps( _mm_mul_ps(vX, _mm_set1_ps(0.5f * bufferWidth)), _mm_set1_ps(0.5f * bufferWidth - 0.5f) );
	//isNotGuardBandClipped &= _mm_movemask_ps( _mm_and_ps( _mm_cmplt_ps(vX, _mm_set1_ps(GUARD_BAND_LIMIT)), _mm_cmpgt_ps(vX, _mm_set1_ps(-GUARD_BAND_LIMIT)) ) );
	fxPtX[1] = _mm_cvtps_epi32(vX); 

	vY = _mm_div_ps( vY, vW );
	vY = _mm_add_ps( _mm_mul_ps(vY, _mm_set1_ps(-0.5f * bufferHeight)), _mm_set1_ps(0.5f * bufferHeight - 0.5f) );
	//	isNotGuardBandClipped &= _mm_movemask_ps( _mm_and_ps( _mm_cmplt_ps(vY, _mm_set1_ps(GUARD_BAND_LIMIT)), _mm_cmpgt_ps(vY, _mm_set1_ps(-GUARD_BAND_LIMIT)) ) );
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

	isNotGuardBandClipped &= _mm_movemask_ps( _mm_and_ps( _mm_cmplt_ps( _mm_mul_ps(vX, _mm_set1_ps(GUARD_BAND_LIMIT)), vW ), _mm_cmpgt_ps(_mm_mul_ps(vX, _mm_set1_ps(GUARD_BAND_LIMIT)), _mm_xor_ps(vW, _mm_set1_ps(-0.0))) ) );
	isNotGuardBandClipped &= _mm_movemask_ps( _mm_and_ps( _mm_cmplt_ps( _mm_mul_ps(vY, _mm_set1_ps(GUARD_BAND_LIMIT)), vW ), _mm_cmpgt_ps(_mm_mul_ps(vY, _mm_set1_ps(GUARD_BAND_LIMIT)), _mm_xor_ps(vW, _mm_set1_ps(-0.0))) ) );

	vX = _mm_div_ps( vX, vW );
	vX = _mm_add_ps( _mm_mul_ps(vX, _mm_set1_ps(0.5f * bufferWidth)), _mm_set1_ps(0.5f * bufferWidth - 0.5f) );
	fxPtX[2] = _mm_cvtps_epi32(vX); 

	vY = _mm_div_ps( vY, vW );
	vY = _mm_add_ps( _mm_mul_ps(vY, _mm_set1_ps(-0.5f * bufferHeight)), _mm_set1_ps(0.5f * bufferHeight - 0.5f) );
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

	// Cull triangles that fall entirely outside of the viewport
	__m128i isNotAllOutsideX = _mm_and_si128( _mm_cmplt_epi32(vStartX, _mm_set1_epi32(bufferWidth - 1)), _mm_cmpgt_epi32(vEndX, _mm_set1_epi32(0)) );
	__m128i isNotAllOutsideY = _mm_and_si128( _mm_cmplt_epi32(vStartY, _mm_set1_epi32(bufferHeight - 1)), _mm_cmpgt_epi32(vEndY, _mm_set1_epi32(0)) );

	__m128i isInsideBoundsX = isNotAllOutsideX;
	__m128i isInsideBoundsY = isNotAllOutsideY;

	// Cull triangles that are back-facing
	__m128i isFront = _mm_cmpgt_epi32(triArea, _mm_setzero_si128());

	__m128 tmpAccept = _mm_castsi128_ps( _mm_and_si128( _mm_and_si128(isFront, isInsideBoundsX), isInsideBoundsY) );
	__m128 accept = tmpAccept;//_mm_and_ps(_mm_and_ps(isNotNearClippedV1, isNotNearClippedV2), _mm_and_ps(isNotNearClippedV3, tmpAccept));

	unsigned int triMask = isUseClipping ? isNotClippedMask & validTriMask & _mm_movemask_ps(accept) : validTriMask & _mm_movemask_ps(accept);

	// Add each valid triangle to bin
	for(unsigned int triangleIndex = 0; triangleIndex < NUM_TRIANGLE_LANES; triangleIndex++)
	{
		if(triMask & (1 << triangleIndex))
		{
			// Convert bounding box in terms of pixels to bounding box in terms of tiles
			int startX = (GET_ELEMENTi(vStartX, triangleIndex) / (int) binWidth) < 0 ? 0 : GET_ELEMENTi(vStartX, triangleIndex) / (int) binWidth;
			int endX   = (GET_ELEMENTi(vEndX, triangleIndex) / (int) binWidth) > ((bufferWidth - 1) / binWidth) ? ((bufferWidth - 1) / binWidth) : (GET_ELEMENTi(vEndX, triangleIndex) / (int) binWidth);
			int startY = (GET_ELEMENTi(vStartY, triangleIndex) / (int) binHeight) < 0 ? 0 : (GET_ELEMENTi(vStartY, triangleIndex) / (int) binHeight);
			int endY   = (GET_ELEMENTi(vEndY, triangleIndex) / (int) binHeight) > ((bufferHeight - 1) / binHeight) ? ((bufferHeight - 1) / binHeight) : (GET_ELEMENTi(vEndY, triangleIndex) / (int) binHeight);

			// Add triangle to the tiles or bins that the bounding box covers
			for(int i = startY; i <= endY; i++)
			{
				for(int j = startX; j <= endX; j++)
				{
					int triBinOffset = i * NUM_BINS_X + j;
					TriangleData* pTriData = &( pBins[triBinOffset].tris[ pNumTrisInBins[triBinOffset] ] );
					pTriData->xy[0] = GET_ELEMENTi(fxPackedXY[0], triangleIndex);
					pTriData->xy[1] = GET_ELEMENTi(fxPackedXY[1], triangleIndex);
					pTriData->xy[2] = GET_ELEMENTi(fxPackedXY[2], triangleIndex);
					pTriData->z[0] = GET_ELEMENT(vZ[0], triangleIndex);
					pTriData->z[1] = GET_ELEMENT(vZ[1], triangleIndex);
					pTriData->z[2] = GET_ELEMENT(vZ[2], triangleIndex);
					pNumTrisInBins[triBinOffset] += 1;
				}
			}
		}
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
	if(isUseClipping && isClippedMask)
	{
		Math::VectorA* pCurClippedTriVertices = pClippedTriVertices;

		// Add each valid triangle to bin
		for(unsigned int triangleIndex = 0; triangleIndex < NUM_TRIANGLE_LANES; triangleIndex++)
		{
			if(isClippedMask & (1 << triangleIndex))
			{
				const unsigned short* EBONY_RESTRICT pTriIndices = pIndices + triangleIndex * 3;

				__m128 v0 = _mm_load_ps( (float*) (pVertices + pTriIndices[0]) );	// V0 of tri1
				__m128 v1 = _mm_load_ps( (float*) (pVertices + pTriIndices[1]) );	// V1 of tri1
				__m128 v2 = _mm_load_ps( (float*) (pVertices + pTriIndices[2]) );	// V2 of tri1

				pCurClippedTriVertices = ClipTriangle(v0, v1, v2, GUARD_BAND_LIMIT, pCurClippedTriVertices); 
			}
		}

		EBONY_ASSERT( (((unsigned int) ((size_t) (pCurClippedTriVertices - pClippedTriVertices))) / 3) < (unsigned int)(numTrisToProcess * 8) ); 
		*pNumClippedTris = ((unsigned int) ((size_t) (pCurClippedTriVertices - pClippedTriVertices))) / 3;
	}
}

template<unsigned int binWidth, unsigned int binHeight, unsigned int bufferWidth, unsigned int bufferHeight>
void TriangleSetupDepth(const Math::VectorA* pVertices, const unsigned short* pIndices, unsigned int numTriangles, TriangleBin* pBins, unsigned int* pNumTrisInBins)
{
	EBONY_ASSERT((bufferWidth % binWidth) == 0);
	EBONY_ASSERT((bufferHeight % binHeight) == 0);

	const static unsigned int NUM_TRIANGLE_LANES = 4;
	const static unsigned short clippedTriIndices[NUM_TRIANGLE_LANES * 3] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 }; 

	// The destination buffer for the newly generated clipped triangles is 8 * num of triangle lanes wide
	// (Each clipped triangle can potentially generate up to 7 new triangles, ie, num of clip planes + 2)
	Math::VectorA clippedTriVerts[NUM_TRIANGLE_LANES * 8 * 3];
	unsigned int numClippedTris = 0;

	unsigned int numTrisProcessed = 0;
	while(numTrisProcessed < numTriangles)
	{
		// Process NUM_TRIANGLE_LANES triangles at a time
		unsigned int numTrisGather = NUM_TRIANGLE_LANES;
		if((numTrisGather + numTrisProcessed) > numTriangles)
			numTrisGather = numTriangles - numTrisProcessed;

		ProcessTrianglesDepthInt<binWidth, binHeight, bufferWidth, bufferHeight>(pVertices, pIndices + numTrisProcessed * 3, pBins, pNumTrisInBins, numTrisGather, clippedTriVerts, &numClippedTris, true);

		numTrisProcessed += 4;

		unsigned int numClippedTrisProcessed = 0;
		while(numClippedTrisProcessed < numClippedTris)
		{
			unsigned int numClippedTrisGather = NUM_TRIANGLE_LANES;
			if((numClippedTrisGather + numClippedTrisProcessed) > numClippedTris)
				numClippedTrisGather = numClippedTris - numClippedTrisProcessed;

			ProcessTrianglesDepthInt<binWidth, binHeight, bufferWidth, bufferHeight>(clippedTriVerts + numClippedTrisProcessed*3, clippedTriIndices, pBins, pNumTrisInBins, numClippedTrisGather, NULL, NULL, false);

			numClippedTrisProcessed += 4;
		}
	}
}

}}}

#endif
