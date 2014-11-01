
#ifndef EBONY_RENDERER_MESHOCCLUSIONTEST_H
#define EBONY_RENDERER_MESHOCCLUSIONTEST_H

#include "Ebony/Math/Math.h"
#include "Math/LmMath.h"

namespace SQEX { namespace Ebony { namespace Renderer {

using namespace Math;

#ifndef EBONY_ORBIS
	#define GET_ELEMENT(xmm, i)		(xmm.m128_f32[i])
	#define GET_ELEMENTi(xmm, i)	(xmm.m128i_i32[i])
#else
	#define GET_ELEMENT(xmm, i)		(((float*) (&xmm))[i])
	#define GET_ELEMENTi(xmm, i)	(((int*) (&xmm))[i])
#endif

inline void ComputeOcclusionTests(bool* pIsOccludedResults, const float* pDepthBuffer, int width, int height, LM_NS_MATH::LmAABB* pSSBounds, int numToTest)
{
	float widthF	= (float) width;
	float heightF	= (float) height;

	const __m128 interleaveHalf = _mm_setr_ps(0.5f, -0.5f, 0.5f, -0.5f);
	const __m128 interleaveSize = _mm_setr_ps(widthF, heightF, widthF, heightF);
	const __m128i minConst = _mm_setr_epi32(0, 0, -1, -1);
	const __m128i maxConst = _mm_setr_epi32(width, height, width - 1, height - 1);
	//const __m128i interleaveSizei = _mm_setr_epi32(width - 1, height - 1, width - 1, height - 1);
	const __m128 half = _mm_set1_ps(0.5f);

	for(int i=0; i < numToTest; i++)
	{
		pIsOccludedResults[i] = true;

		__m128 minXY = pSSBounds[i].Min().Vec128();
		__m128 maxXY = pSSBounds[i].Max().Vec128();
		__m128 minMaxXY  = _mm_movelh_ps(minXY, maxXY);
	
		minMaxXY = _mm_add_ps( _mm_mul_ps(minMaxXY, interleaveHalf), half );
		minMaxXY = _mm_mul_ps( minMaxXY, interleaveSize );
		__m128i minMaxXYi = _mm_cvttps_epi32(minMaxXY);

		//float minXF = (pSSBounds[i].Min().X() * 0.5f + 0.5f) * widthF;
		//float maxXF = (pSSBounds[i].Max().X() * 0.5f + 0.5f) * widthF;
		//int minX = _mm_cvtt_ss2si( _mm_load_ss( &minXF ) );
		//int maxX = _mm_cvtt_ss2si( _mm_load_ss( &maxXF ) );

		//float minYF = (pSSBounds[i].Max().Y() * -0.5f + 0.5f) * heightF;
		//float maxYF = (pSSBounds[i].Min().Y() * -0.5f + 0.5f) * heightF;
		//int minY = _mm_cvtt_ss2si( _mm_load_ss( &minYF ) );
		//int maxY = _mm_cvtt_ss2si( _mm_load_ss( &maxYF ) );
		minMaxXYi = _mm_max_epi32(minMaxXYi, minConst);
		minMaxXYi = _mm_min_epi32(minMaxXYi, maxConst);

		int quadEdgeCheck = _mm_movemask_ps( _mm_castsi128_ps( _mm_slli_epi32(minMaxXYi, 31) ) );

		minMaxXYi = _mm_and_si128(minMaxXYi, _mm_set1_epi32(~0x1));

		int minXQuad = GET_ELEMENTi(minMaxXYi, 0);
		int minYQuad = GET_ELEMENTi(minMaxXYi, 3);
		int maxXQuad = GET_ELEMENTi(minMaxXYi, 2);
		int maxYQuad = GET_ELEMENTi(minMaxXYi, 1);

		//minX = minX < 0 ? 0 : minX;
		//int minXQuad = minX & ~0x1;
		////int minXQuad = (minX + 1) & ~0x1;

		//maxX = maxX > (width - 1) ? (width - 1) : maxX;
		//int maxXQuad = maxX & ~0x1;
		////int maxXQuad = (maxX - 1) & ~0x1;

		//minY = minY < 0 ? 0 : minY;
		//int minYQuad = minY & ~0x1;
		////int minYQuad = (minY + 1) & ~0x1;

		//maxY = maxY > (height - 1) ? (height - 1) : maxY;
		//int maxYQuad = maxY & ~0x1;
		////int maxYQuad = (maxY - 1) & ~0x1;

		//if(minX <= maxX && minY <= maxY)
		{
			__m128 minZ = _mm_set1_ps(pSSBounds[i].Min().Z());

			bool requiresTopEdge = (quadEdgeCheck & 0x8) != 0;
			bool requiresBottomEdge = (quadEdgeCheck & 0x2) == 0;
			bool requiresLeftEdge = (quadEdgeCheck & 0x1) != 0;
			bool requiresRightEdge = (quadEdgeCheck & 0x4) == 0;
			
			// These are masks to make sure only the relevant pixels in a 2x2 quad 
			// are tested, in the case of edges
			const static int CMP_MASK[3][3] = 
			{
				// 0, L, R
				{ 0xF, 0xA, 0x5 }, // 0
				{ 0xC, 0x8, 0x4 }, // T
				{ 0x3, 0x2, 0x1 }, // B
			};

			for(int j=minYQuad; j <= maxYQuad; j+=2)
			{
				bool atTopEdge = requiresTopEdge && (j == minYQuad);
				bool atBottomEdge = requiresBottomEdge && (j == maxYQuad);
				int cmpYIndex = atTopEdge ? 1 : atBottomEdge ? 2 : 0;

				for(int k=minXQuad; k <= maxXQuad; k+=2)
				{
					bool atLeftEdge = requiresLeftEdge && (k == minXQuad) ;
					bool atRightEdge = requiresRightEdge && (k == maxXQuad);
					int cmpXIndex = atLeftEdge ? 1 : atRightEdge ? 2 : 0;

					__m128 depthValues = _mm_load_ps(pDepthBuffer + j * width + k*2);
					__m128 cmpRes = _mm_cmplt_ps(minZ, depthValues);

					int cmpResMask = _mm_movemask_ps(cmpRes) & CMP_MASK[cmpYIndex][cmpXIndex];

					if(cmpResMask > 0)
					{
						pIsOccludedResults[i] = false;
						goto LABEL_CONTINUE;
					}
				}
			}

		}

LABEL_CONTINUE:	;

	}
}

inline void ComputeOcclusionTestsWithHiZ(bool* pIsOccludedResults, float** ppHiZDepthBuffer, int topWidth, int topHeight, LM_NS_MATH::LmAABB* pSSBounds, int numToTest)
{
	const static int BUFFER_SIZE = 128;
	const static int COARSE_PIXEL_SIZE = 8;
	EBONY_ASSERT(topHeight <= BUFFER_SIZE || topWidth <= BUFFER_SIZE);

	const static int HIZ_LEVEL_LOOKUP[(BUFFER_SIZE / COARSE_PIXEL_SIZE) + 1] =
	{
		0, //0,	// 0
		0, //0,	// 1	8
		1, //1,	// 2	16
		1, //1,	// 3	24
		2, //2,	// 4	32
		2, //2,	// 5	40
		2, //2,	// 6	48
		2, //2,	// 7	56
		3, //3,	// 8	64
		3, //3,	// 9	72
		3, //3,	// 10	80
		3, //3,	// 11	88
		3, //3,	// 12	96
		3, //3,	// 13	104
		4, //3,	// 14	112
		4, //3,	// 15	120
		4, //4,	// 16	128
	};


	float topWidthF		= (float) topWidth;
	float topHeightF	= (float) topHeight;

	const __m128 interleaveHalf = _mm_setr_ps(0.5f, -0.5f, 0.5f, -0.5f);
	const __m128i interleaveSizeTop = _mm_setr_epi32(topWidth, topHeight, topWidth, topHeight);
	//const __m128 interleaveSize = _mm_setr_ps(topWidthF, topHeightF, topWidthF, topHeightF);
	const __m128 half = _mm_set1_ps(0.5f);

	//const __m128i minConst = _mm_setr_epi32(0, 0, -1, -1);
	//const __m128i maxConst = _mm_setr_epi32(width, height, width - 1, height - 1);
	//const __m128i interleaveSizei = _mm_setr_epi32(width - 1, height - 1, width - 1, height - 1);
	
	for(int i=0; i < numToTest; i++)
	{
		pIsOccludedResults[i] = true;

		__m128 minXY = pSSBounds[i].Min().Vec128();
		__m128 maxXY = pSSBounds[i].Max().Vec128();
		__m128 minMaxXY  = _mm_movelh_ps(minXY, maxXY);

		minMaxXY = _mm_add_ps( _mm_mul_ps(minMaxXY, interleaveHalf), half );
		__m128 minMaxXYClamped = _mm_max_ps(minMaxXY, _mm_setzero_ps());
		minMaxXYClamped = _mm_min_ps(minMaxXYClamped, _mm_set1_ps(1.0f));

		__m128 maxXYClamped = _mm_movehl_ps(minMaxXYClamped, minMaxXYClamped);
		__m128 diffXY = _mm_sub_ps(maxXYClamped, minMaxXYClamped);
		float xDiff = GET_ELEMENT(diffXY, 0) * topWidthF;
		float yDiff = GET_ELEMENT(diffXY, 1) * -topHeightF;
		float minDiff = xDiff > yDiff ? yDiff : xDiff;
		int minDiffi = _mm_cvtt_ss2si(_mm_load_ss(&minDiff));
		
		// We want a maximum of 8x8 coarse level pixels to test against
		//static volatile int _forceHiZLevel = -1; 
		minDiffi >>= 3;
		//EBONY_ASSERT(minDiffi < ((BUFFER_SIZE / COARSE_PIXEL_SIZE) + 1));
		int hiZLevel = HIZ_LEVEL_LOOKUP[minDiffi];//_forceHiZLevel >= 0 ? _forceHiZLevel : minDiffi];

		__m128i interleaveSizeI = _mm_srli_epi32( interleaveSizeTop, hiZLevel );
		__m128 minMaxXYClampedTest = _mm_mul_ps( minMaxXYClamped, _mm_cvtepi32_ps(interleaveSizeI) );
		__m128i minMaxXYClampedTesti = _mm_cvttps_epi32(minMaxXYClampedTest);

		//minMaxXYi = _mm_max_epi32(minMaxXYi, minConst);
		minMaxXYClampedTesti = _mm_min_epi32(minMaxXYClampedTesti, _mm_sub_epi32(interleaveSizeI, _mm_set1_epi32(1)));

		int quadEdgeCheck = _mm_movemask_ps( _mm_castsi128_ps( _mm_slli_epi32(minMaxXYClampedTesti, 31) ) );

		minMaxXYClampedTesti = _mm_and_si128(minMaxXYClampedTesti, _mm_set1_epi32(~0x1));

		int minXQuad = GET_ELEMENTi(minMaxXYClampedTesti, 0);
		int minYQuad = GET_ELEMENTi(minMaxXYClampedTesti, 3);
		int maxXQuad = GET_ELEMENTi(minMaxXYClampedTesti, 2);
		int maxYQuad = GET_ELEMENTi(minMaxXYClampedTesti, 1);

		// Now do occlusion test with the appropriate depth buffer
		int width = topWidth >> hiZLevel;
		const float* pDepthBuffer = ppHiZDepthBuffer[hiZLevel];

		__m128 minZ = _mm_set1_ps(pSSBounds[i].Min().Z());

		bool requiresTopEdge = (quadEdgeCheck & 0x8) != 0;
		bool requiresBottomEdge = (quadEdgeCheck & 0x2) == 0;
		bool requiresLeftEdge = (quadEdgeCheck & 0x1) != 0;
		bool requiresRightEdge = (quadEdgeCheck & 0x4) == 0;

		// These are masks to make sure only the relevant pixels in a 2x2 quad 
		// are tested, in the case of edges
		const static int CMP_MASK[3][3] = 
		{
			// 0, L, R
			{ 0xF, 0xA, 0x5 }, // 0
			{ 0xC, 0x8, 0x4 }, // T
			{ 0x3, 0x2, 0x1 }, // B
		};

		for(int j=minYQuad; j <= maxYQuad; j+=2)
		{
			bool atTopEdge = requiresTopEdge && (j == minYQuad);
			bool atBottomEdge = requiresBottomEdge && (j == maxYQuad);
			int cmpYIndex = atTopEdge ? 1 : atBottomEdge ? 2 : 0;

			{
				bool atLeftEdge = requiresLeftEdge;
				int cmpXIndex = atLeftEdge ? 1 : 0;

				__m128 depthValues = _mm_load_ps(pDepthBuffer + j * width + minXQuad*2);
				__m128 cmpRes = _mm_cmplt_ps(minZ, depthValues);

				int cmpResMask = _mm_movemask_ps(cmpRes) & CMP_MASK[cmpYIndex][cmpXIndex];

				if(cmpResMask > 0)
				{
					pIsOccludedResults[i] = false;
					goto LABEL_CONTINUE;
				}
			}

			// TODO: Unroll loop
			int k=minXQuad+2;
			for(; k < maxXQuad; k+=2)
			{
				//bool atLeftEdge = requiresLeftEdge && (k == minXQuad) ;
				//bool atRightEdge = requiresRightEdge && (k == maxXQuad);
				//int cmpXIndex = atLeftEdge ? 1 : atRightEdge ? 2 : 0;

				__m128 depthValues = _mm_load_ps(pDepthBuffer + j * width + k*2);
				__m128 cmpRes = _mm_cmplt_ps(minZ, depthValues);

				int cmpResMask = _mm_movemask_ps(cmpRes) & CMP_MASK[cmpYIndex][0];

				if(cmpResMask > 0)
				{
					pIsOccludedResults[i] = false;
					goto LABEL_CONTINUE;
				}
			}

			if(k <= maxXQuad)
			{
				bool atRightEdge = requiresRightEdge;
				int cmpXIndex = atRightEdge ? 2 : 0;

				__m128 depthValues = _mm_load_ps(pDepthBuffer + j * width + k*2);
				__m128 cmpRes = _mm_cmplt_ps(minZ, depthValues);

				int cmpResMask = _mm_movemask_ps(cmpRes) & CMP_MASK[cmpYIndex][cmpXIndex];

				if(cmpResMask > 0)
				{
					pIsOccludedResults[i] = false;
					goto LABEL_CONTINUE;
				}
			}
		}

LABEL_CONTINUE:	;

	}
}

}}}

#endif
