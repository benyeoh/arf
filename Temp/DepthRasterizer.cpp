
#include "stdafx.h"
#include "../../RenderInterface.h"
#include "../../Base/Performance/PerformanceManager.h"

#include "Ebony/Math/Math.h"
#include "Math/LmMath.h"

#include "DepthRasterizer.h"
#include "TilesRasterizeDepth.h"
#include "TriangleSetupDepth.h"

namespace SQEX { namespace Ebony { namespace Renderer {

using namespace RenderInterface;
using namespace Math;

void DepthRasterizer::Initialize(bool isUseHiZ)
{
	// Do we have a cache line constant somewhere in Ebony???
	const static int CACHE_LINE = 64;

	pDepthBuffer_[0] = (float*) (Ebony::Base::Memory::Malloc(BUFFER_WIDTH * BUFFER_HEIGHT * sizeof(float), CACHE_LINE, MEMORY_CATEGORY_RENDERER));

	if(isUseHiZ)
	{
		for(int i=1; i < MAX_NUM_MIPS; i++)
		{
			int width = BUFFER_WIDTH >> i;
			int height = BUFFER_HEIGHT >> i;
			pDepthBuffer_[i] = (float*) (Ebony::Base::Memory::Malloc(width * height * sizeof(float), CACHE_LINE, MEMORY_CATEGORY_RENDERER));
		}
	}
}

void DepthRasterizer::ClearDepth()
{
	Ebony::Base::PerformanceManager::BeginCpu("SW Rasterizer Clear Depth");

	__m128 clearVal = _mm_set1_ps(1.0f);
	for(int i=0; i < (BUFFER_WIDTH * BUFFER_HEIGHT) / 4; i++)
	{
		_mm_store_ps(&(pDepthBuffer_[0][i * 4]), clearVal);
	}

	Ebony::Base::PerformanceManager::EndCpu();

	//for(int i=0; i < (BUFFER_HEIGHT - 1); i+=2)
	//{
	//	float* pToClear = depthBuffer_ + (i * BUFFER_WIDTH);       
	//	for(int j=0; j < (BUFFER_WIDTH - 1); j+=2)
	//	{
	//		//_mm_stream_ps(pToClear, clearVal);
	//		_mm_store_ps(pToClear, clearVal);
	//		pToClear += 4;
	//	}
	//}
}

void DepthRasterizer::TransformVertices(VectorA* pXYZWOut, const Math::MatrixA& worldViewProj, const float* pXYZ, unsigned int posStride, unsigned int numVertices)
{
	__m128 xmm0, xmm1, xmm2, xmm3;

	float* EBONY_RESTRICT pIn = (float*) pXYZ;
	float* EBONY_RESTRICT pOut = (float*) pXYZWOut;

	unsigned int inPosStride = posStride;
	unsigned int outPosStride = sizeof(VectorA) / sizeof(float);

	__m128 xmm4 = (worldViewProj.GetCol0().GetVec());	// col 0
	__m128 xmm5 = (worldViewProj.GetCol1().GetVec());	// col 1
	__m128 xmm6 = (worldViewProj.GetCol2().GetVec());	// col 2
	__m128 xmm7 = (worldViewProj.GetCol3().GetVec());	// col 3

	if(numVertices & 0x1)
	{
		xmm0 = _mm_load_ps1(pIn);		// Load and broadcast x
		xmm1 = _mm_load_ps1(pIn + 1);	// Load and broadcast y
		xmm2 = _mm_load_ps1(pIn + 2);	// Load and broadcast z

		xmm0 = _mm_mul_ps(xmm0, xmm4);
		xmm1 = _mm_mul_ps(xmm1, xmm5);	
		xmm2 = _mm_mul_ps(xmm2, xmm6);		

		xmm0 = _mm_add_ps(xmm0, xmm1);		
		xmm2 = _mm_add_ps(xmm2, xmm7);
		xmm0 = _mm_add_ps(xmm0, xmm2);

		//__m128 notNearClip = _mm_cmpge_ps( _mm_shuffle_ps(xmm0, xmm0, _MM_SHUFFLE(2, 2, 2, 2)), _mm_setzero_ps() );
		//__m128 wRcp = ( _mm_shuffle_ps(xmm0, xmm0, _MM_SHUFFLE(3, 3, 3, 3)) );
		//xmm0 = _mm_div_ps( xmm0, wRcp );
		//xmm0 = _mm_and_ps( xmm0, notNearClip );

		_mm_store_ps(pOut, xmm0);

		pIn += inPosStride;
		pOut += outPosStride;
	}

	numVertices >>= 1;

	for(unsigned int i=0; i < numVertices; i++)
	{	
		// Process 2 vertices at a time to take advantage of memory locality
		// Process x
		xmm0 = _mm_load_ps1(pIn);
		xmm1 = _mm_load_ps1(pIn + inPosStride);
		xmm2 = _mm_mul_ps(xmm0, xmm4);
		xmm3 = _mm_mul_ps(xmm1, xmm4);

		// Process y
		xmm0 = _mm_load_ps1(pIn + 1);
		xmm1 = _mm_load_ps1(pIn + inPosStride + 1);
		xmm0 = _mm_mul_ps(xmm0, xmm5);
		xmm1 = _mm_mul_ps(xmm1, xmm5);	
		xmm2 = _mm_add_ps(xmm2, xmm0);		
		xmm3 = _mm_add_ps(xmm3, xmm1);

		// Process z
		xmm0 = _mm_load_ps1(pIn + 2);
		xmm1 = _mm_load_ps1(pIn + inPosStride + 2);
		xmm0 = _mm_mul_ps(xmm0, xmm6);
		xmm1 = _mm_mul_ps(xmm1, xmm6);	
		xmm2 = _mm_add_ps(xmm2, xmm0);		
		xmm3 = _mm_add_ps(xmm3, xmm1);

		// Process w	
		xmm2 = _mm_add_ps(xmm2, xmm7);		
		xmm3 = _mm_add_ps(xmm3, xmm7);

		//__m128 notNearClip = _mm_cmpge_ps( _mm_shuffle_ps(xmm2, xmm2, _MM_SHUFFLE(2, 2, 2, 2)), _mm_setzero_ps() );
		//__m128 wRcp = ( _mm_shuffle_ps(xmm2, xmm2, _MM_SHUFFLE(3, 3, 3, 3)) );
		//xmm2 = _mm_div_ps( xmm2, wRcp );
		//xmm2 = _mm_and_ps( xmm2, notNearClip );

		//notNearClip = _mm_cmpge_ps( _mm_shuffle_ps(xmm3, xmm3, _MM_SHUFFLE(2, 2, 2, 2)), _mm_setzero_ps() );
		//wRcp = ( _mm_shuffle_ps(xmm3, xmm3, _MM_SHUFFLE(3, 3, 3, 3)) );
		//xmm3 = _mm_div_ps( xmm3, wRcp );
		//xmm3 = _mm_and_ps( xmm3, notNearClip );

		_mm_store_ps(pOut, xmm2);
		_mm_store_ps(pOut + outPosStride, xmm3);

		pIn += (inPosStride * 2);
		pOut += (outPosStride * 2);
	}
}

void DepthRasterizer::UntileDepthBuffer(float* pDest, int mipLevel)
{
	int width = BUFFER_WIDTH >> mipLevel;
	int height = BUFFER_HEIGHT >> mipLevel;

	for(int i=0; i < (height >> 1); i++)
	{
		int row = i * 2;

		for(int j=0; j < (width >> 2); j++)
		{
			int col = j * 2;
			__m128 quad1 = _mm_load_ps(pDepthBuffer_[mipLevel] + row * width + (col << 2));
			__m128 quad2 = _mm_load_ps(pDepthBuffer_[mipLevel] + row * width + ((col+1) << 2));

			__m128 line1 = _mm_movelh_ps(quad1, quad2);
			__m128 line2 = _mm_movehl_ps(quad2, quad1);

			_mm_store_ps(pDest + row * width + (j << 2), line1);
			_mm_store_ps(pDest + (row+1) * width + (j << 2), line2);
		}
	}
}

void DepthRasterizer::UntileAndLinearizeDepthBuffer(float* pDest, float farZ, float nearZ, int mipLevel)
{
	int width = BUFFER_WIDTH >> mipLevel;
	int height = BUFFER_HEIGHT >> mipLevel;

	__m128 farPlane = _mm_set_ps1(farZ);
	__m128 nearPlane = _mm_set_ps1(nearZ);
	__m128 nearMinusFar = _mm_sub_ps(nearPlane, farPlane);

	for(int i=0; i < (height >> 1); i++)
	{
		int row = i * 2;

		for(int j=0; j < (width >> 2); j++)
		{
			int col = j * 2;
			__m128 quad1 = _mm_load_ps(pDepthBuffer_[0] + row * width + (col << 2));
			__m128 quad2 = _mm_load_ps(pDepthBuffer_[0] + row * width + ((col+1) << 2));

			__m128 line1 = _mm_movelh_ps(quad1, quad2);
			__m128 line2 = _mm_movehl_ps(quad2, quad1);

			// Linearize
			__m128 denom1 = _mm_rcp_ps( _mm_add_ps( _mm_mul_ps(line1, nearMinusFar), farPlane ) );
			__m128 denom2 = _mm_rcp_ps( _mm_add_ps( _mm_mul_ps(line2, nearMinusFar), farPlane ) );
			line1 = _mm_mul_ps( nearPlane, denom1 );
			line2 = _mm_mul_ps( nearPlane, denom2 );
			//__m128 denom1 = ( _mm_add_ps( _mm_mul_ps(line1, nearMinusFar), farPlane ) );
			//__m128 denom2 = ( _mm_add_ps( _mm_mul_ps(line2, nearMinusFar), farPlane ) );
			//line1 = _mm_div_ps( nearPlane, denom1 );
			//line2 = _mm_div_ps( nearPlane, denom2 );

			_mm_store_ps(pDest + row * width + (j << 2), line1);
			_mm_store_ps(pDest + (row+1) * width + (j << 2), line2);
		}
	}
}

EBONY_FORCEINLINE __m128 ComputeMaxZOneQuad(__m128 topLeft, __m128 topRight, __m128 bottomLeft, __m128 bottomRight )
{
	// Transpose
	// Before, a register contains 1 2x2 pixel quad:			r0 = quad0_00, quad0_10, quad0_01, quad1_11 ... etc
	// After, a register contains 1 element from 4 pixel quads:	r0 = quad0_00, quad1_00, quad2_00, quad3_00
	//															r1 = quad0_10, quad1_10, quad2_10, quad3_10 ... etc
	_MM_TRANSPOSE4_PS(topLeft, topRight, bottomLeft, bottomRight);

	// Find the maximum z
	__m128 maxZ = _mm_max_ps(topLeft, topRight);
	maxZ = _mm_max_ps(maxZ, bottomLeft);
	maxZ = _mm_max_ps(maxZ, bottomRight);

	return maxZ;
}

void DepthRasterizer::GenerateHiZ2Levels(int startMip)
{
	// Generate HiZ for 2 levels at once
	EBONY_ASSERT(startMip >= 1);
	EBONY_ASSERT(startMip < (MAX_NUM_MIPS-1));

	int topLevelMip = startMip - 1;
	int width = BUFFER_WIDTH >> topLevelMip;
	int height = BUFFER_HEIGHT >> topLevelMip;

	EBONY_ASSERT(width >= 8);
	EBONY_ASSERT(height >= 8);

	for(int i=0; i < height; i+=8)
	{
		for(int j=0; j < width; j+=8)
		{
			// TODO: Use streaming loads (and stores?) to not pollute cache if possible
			// 1st level
			__m128 topLeft00 = _mm_load_ps(pDepthBuffer_[topLevelMip] + i * width + j * 2);
			__m128 topRight00 = _mm_load_ps(pDepthBuffer_[topLevelMip] + i * width + j * 2 + 4);
			__m128 topLeft10 = _mm_load_ps(pDepthBuffer_[topLevelMip] + i * width + j * 2 + 8);
			__m128 topRight10 = _mm_load_ps(pDepthBuffer_[topLevelMip] + i * width + j * 2 + 12);

			__m128 bottomLeft00 = _mm_load_ps(pDepthBuffer_[topLevelMip] + (i+2) * width + j * 2);
			__m128 bottomRight00 = _mm_load_ps(pDepthBuffer_[topLevelMip] + (i+2) * width + j * 2 + 4);
			__m128 bottomLeft10 = _mm_load_ps(pDepthBuffer_[topLevelMip] + (i+2) * width + j * 2 + 8);
			__m128 bottomRight10 = _mm_load_ps(pDepthBuffer_[topLevelMip] + (i+2) * width + j * 2 + 12);

			// Block 00
			__m128 maxZ00 = ComputeMaxZOneQuad(topLeft00, topRight00, bottomLeft00, bottomRight00);
			_mm_store_ps(pDepthBuffer_[startMip] + (i >> 1) * (width >> 1) + (j >> 1) * 2, maxZ00);
			// Block 10
			__m128 maxZ10 = ComputeMaxZOneQuad(topLeft10, topRight10, bottomLeft10, bottomRight10);
			_mm_store_ps(pDepthBuffer_[startMip] + (i >> 1) * (width >> 1) + (j >> 1) * 2 + 4, maxZ10);

			__m128 topLeft01 = _mm_load_ps(pDepthBuffer_[topLevelMip] + (i+4) * width + j * 2);
			__m128 topRight01 = _mm_load_ps(pDepthBuffer_[topLevelMip] + (i+4) * width + j * 2 + 4);
			__m128 topLeft11 = _mm_load_ps(pDepthBuffer_[topLevelMip] + (i+4) * width + j * 2 + 8);
			__m128 topRight11 = _mm_load_ps(pDepthBuffer_[topLevelMip] + (i+4) * width + j * 2 + 12);
			
			__m128 bottomLeft01 = _mm_load_ps(pDepthBuffer_[topLevelMip] + (i+6) * width + j * 2);
			__m128 bottomRight01 = _mm_load_ps(pDepthBuffer_[topLevelMip] + (i+6) * width + j * 2 + 4);
			__m128 bottomLeft11 = _mm_load_ps(pDepthBuffer_[topLevelMip] + (i+6) * width + j * 2 + 8);
			__m128 bottomRight11 = _mm_load_ps(pDepthBuffer_[topLevelMip] + (i+6) * width + j * 2 + 12);

			// Block 01
			__m128 maxZ01 = ComputeMaxZOneQuad(topLeft01, topRight01, bottomLeft01, bottomRight01);
			_mm_store_ps(pDepthBuffer_[startMip] + ((i >> 1) + 2) * (width >> 1) + (j >> 1) * 2, maxZ01);
			// Block 11
			__m128 maxZ11 = ComputeMaxZOneQuad(topLeft11, topRight11, bottomLeft11, bottomRight11);
			_mm_store_ps(pDepthBuffer_[startMip] + ((i >> 1) + 2) * (width >> 1) + (j >> 1) * 2 + 4, maxZ11);

			// 2nd level
			__m128 maxZ = ComputeMaxZOneQuad(maxZ00, maxZ10, maxZ01, maxZ11);
			_mm_store_ps(pDepthBuffer_[startMip+1] + (i >> 2) * (width >> 2) + (j >> 2) * 2, maxZ);
		}
	}
}

void DepthRasterizer::Draw(const Math::MatrixA& worldViewProj, const float* pXYZ, unsigned int posStride, unsigned int numVertices, const unsigned short* pIndices, unsigned int numTriangles)
{
	EBONY_ASSERT(numTriangles < TriangleBin::MAX_NUM_TRIANGLES_PER_BIN);

	if((numTrisInBin_ + numTriangles) > TriangleBin::MAX_NUM_TRIANGLES_PER_BIN)
		Flush();

	const static unsigned int MAX_NUM_VERTICES = 2048;
	EBONY_ASSERT(numVertices < MAX_NUM_VERTICES);

	Ebony::Base::PerformanceManager::BeginCpu("SW Rasterizer Transform and Setup");

	VectorA outXYZW[MAX_NUM_VERTICES];
	TransformVertices(outXYZW, worldViewProj, pXYZ, posStride, numVertices);
	TriangleSetupDepth<BUFFER_WIDTH, BUFFER_HEIGHT, BUFFER_WIDTH, BUFFER_HEIGHT>(outXYZW, pIndices, numTriangles, &triBin_, &numTrisInBin_);

	Ebony::Base::PerformanceManager::EndCpu();
}

void DepthRasterizer::Flush()
{
	Ebony::Base::PerformanceManager::BeginCpu("SW Rasterizer Flush");

	// Flush all triangles and rasterize
	TilesRasterizeDepthUnrollXInt<BUFFER_WIDTH, BUFFER_HEIGHT, BUFFER_WIDTH, BUFFER_HEIGHT, 1>(&triBin_, &numTrisInBin_, pDepthBuffer_[0], 0, 0);

	// Generate HiZ if mip levels exist
	if(pDepthBuffer_[1])
	{
		GenerateHiZ2Levels(1);
		GenerateHiZ2Levels(3);
	}

	Ebony::Base::PerformanceManager::EndCpu();

	numTrisInBin_ = 0;
}

}}}
