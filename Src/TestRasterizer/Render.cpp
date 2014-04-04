//==============================================================================
//
//		Render.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/11/2009
//
//==============================================================================

#include "stdafx.h"

//#define USE_FAST_DEPTH_CLEAR
void WhitePixelShader(int pixelBlockX, int pixelBlockY, __m128 coverage, __m128 depthVals, __m128* interVals, void* pContext)
{
	g_NumPixelsProcessed += 4;

	int coverageMask = _mm_movemask_ps(coverage);

	if(coverageMask & 0x1)
	{
		_DEBUG_ASSERT(g_pRasterizeBuffer[pixelBlockY * RASTERIZE_BUFFER_W + pixelBlockX] == 0);
		g_pRasterizeBuffer[pixelBlockY * RASTERIZE_BUFFER_W + pixelBlockX] = 0xFF;
		//g_NumPixelsProcessed++;
		//if(g_pRasterizeBuffer[pixelBlockY * RASTERIZE_BUFFER_W + pixelBlockX])
		//	g_pRasterizeBuffer[pixelBlockY * RASTERIZE_BUFFER_W + pixelBlockX] = 0xFF;
		//else
		//	g_pRasterizeBuffer[pixelBlockY * RASTERIZE_BUFFER_W + pixelBlockX]++;
	}

	if(coverageMask & 0x2)
	{
		_DEBUG_ASSERT(g_pRasterizeBuffer[pixelBlockY * RASTERIZE_BUFFER_W + pixelBlockX + 1] == 0);
		g_pRasterizeBuffer[pixelBlockY * RASTERIZE_BUFFER_W + pixelBlockX + 1] = 0xFF;
		//g_NumPixelsProcessed++;
		//if(g_pRasterizeBuffer[pixelBlockY * RASTERIZE_BUFFER_W + pixelBlockX + 1])
		//	g_pRasterizeBuffer[pixelBlockY * RASTERIZE_BUFFER_W + pixelBlockX + 1] = 0xFF;
		//else
		//	g_pRasterizeBuffer[pixelBlockY * RASTERIZE_BUFFER_W + pixelBlockX + 1]++;
	}

	if(coverageMask & 0x4)
	{
		_DEBUG_ASSERT(g_pRasterizeBuffer[(pixelBlockY + 1) * RASTERIZE_BUFFER_W + pixelBlockX] == 0);
		g_pRasterizeBuffer[(pixelBlockY + 1) * RASTERIZE_BUFFER_W + pixelBlockX] = 0xFF;
		//g_NumPixelsProcessed++;
		//if(g_pRasterizeBuffer[(pixelBlockY + 1) * RASTERIZE_BUFFER_W + pixelBlockX])
		//	g_pRasterizeBuffer[(pixelBlockY + 1) * RASTERIZE_BUFFER_W + pixelBlockX] = 0xFF;
		//else
		//	g_pRasterizeBuffer[(pixelBlockY + 1) * RASTERIZE_BUFFER_W + pixelBlockX]++;
	}

	if(coverageMask & 0x8)
	{
		_DEBUG_ASSERT(g_pRasterizeBuffer[(pixelBlockY + 1) * RASTERIZE_BUFFER_W + pixelBlockX + 1] == 0);
		g_pRasterizeBuffer[(pixelBlockY + 1) * RASTERIZE_BUFFER_W + pixelBlockX + 1] = 0xFF;
		//g_NumPixelsProcessed++;
		//if(g_pRasterizeBuffer[(pixelBlockY + 1) * RASTERIZE_BUFFER_W + pixelBlockX + 1])
		//	g_pRasterizeBuffer[(pixelBlockY + 1) * RASTERIZE_BUFFER_W + pixelBlockX + 1] = 0xFF;
		//else
		//	g_pRasterizeBuffer[(pixelBlockY + 1) * RASTERIZE_BUFFER_W + pixelBlockX + 1]++;
	}
}

void CheckerBoardPixelShader(int pixelBlockX, int pixelBlockY, __m128 coverage, __m128 depthVals, __m128* interVals, void* pContext)
{
	g_NumPixelsProcessed += 4;

	const __m128 UV_SHIFT = _mm_set_ps1(4.0f);
	__m128 uMult = _mm_mul_ps(UV_SHIFT, interVals[0]);
	__m128 vMult = _mm_mul_ps(UV_SHIFT, interVals[1]);
	__m128i uInt = _mm_cvttps_epi32(uMult);
	__m128i vInt = _mm_cvttps_epi32(vMult);
	__m128 uFloat = _mm_cvtepi32_ps(uInt);
	__m128 vFloat = _mm_cvtepi32_ps(vInt);
	__m128 uFrac = _mm_sub_ps(uMult, uFloat);
	__m128 vFrac = _mm_sub_ps(vMult, vFloat);

	const __m128 CMP = _mm_set_ps1(0.5f);
	const __m128 TWO = _mm_set_ps1(2.0f);
	const __m128 ONE = _mm_set_ps1(1.0f);
	__m128 uCmp = _mm_and_ps( _mm_cmpgt_ps(uFrac, CMP), ONE );
	__m128 vCmp = _mm_and_ps( _mm_cmpgt_ps(vFrac, CMP), ONE );
	__m128 uvCmpAdd = _mm_add_ps(uCmp, vCmp);
	__m128 uvCmpMult2 = _mm_mul_ps(uCmp, vCmp);
	uvCmpMult2 = _mm_mul_ps(uvCmpMult2, TWO);

	__m128 finalColor = _mm_sub_ps(uvCmpAdd, uvCmpMult2);
	//__m128 finalColor = interVals[1];// _mm_mul_ps(interVals[0], finalColor);

	const static __m128 COLOR_MAX = _mm_set_ps1(255.0f);
	__m128 colorScaled = _mm_mul_ps(finalColor, COLOR_MAX);
	_ALIGN(16) int colorScaledInt[4];
	_mm_store_si128( (__m128i*) colorScaledInt, _mm_cvtps_epi32( colorScaled ) );

	int coverageMask = _mm_movemask_ps(coverage);

	if(coverageMask & 0x1)
	{
		_DEBUG_ASSERT(g_pRasterizeBuffer[pixelBlockY * RASTERIZE_BUFFER_W + pixelBlockX] == 0);
		g_pRasterizeBuffer[pixelBlockY * RASTERIZE_BUFFER_W + pixelBlockX] = (byte)colorScaledInt[0];
		//g_NumPixelsProcessed++;
		//if(g_pRasterizeBuffer[pixelBlockY * RASTERIZE_BUFFER_W + pixelBlockX])
		//	g_pRasterizeBuffer[pixelBlockY * RASTERIZE_BUFFER_W + pixelBlockX] = 0xFF;
		//else
		//	g_pRasterizeBuffer[pixelBlockY * RASTERIZE_BUFFER_W + pixelBlockX]++;
	}

	if(coverageMask & 0x2)
	{
		_DEBUG_ASSERT(g_pRasterizeBuffer[pixelBlockY * RASTERIZE_BUFFER_W + pixelBlockX + 1] == 0);
		g_pRasterizeBuffer[pixelBlockY * RASTERIZE_BUFFER_W + pixelBlockX + 1] = (byte)colorScaledInt[1];
		//g_NumPixelsProcessed++;
		//if(g_pRasterizeBuffer[pixelBlockY * RASTERIZE_BUFFER_W + pixelBlockX + 1])
		//	g_pRasterizeBuffer[pixelBlockY * RASTERIZE_BUFFER_W + pixelBlockX + 1] = 0xFF;
		//else
		//	g_pRasterizeBuffer[pixelBlockY * RASTERIZE_BUFFER_W + pixelBlockX + 1]++;
	}

	if(coverageMask & 0x4)
	{
		_DEBUG_ASSERT(g_pRasterizeBuffer[(pixelBlockY + 1) * RASTERIZE_BUFFER_W + pixelBlockX] == 0);
		g_pRasterizeBuffer[(pixelBlockY + 1) * RASTERIZE_BUFFER_W + pixelBlockX] = (byte)colorScaledInt[2];
		//g_NumPixelsProcessed++;
		//if(g_pRasterizeBuffer[(pixelBlockY + 1) * RASTERIZE_BUFFER_W + pixelBlockX])
		//	g_pRasterizeBuffer[(pixelBlockY + 1) * RASTERIZE_BUFFER_W + pixelBlockX] = 0xFF;
		//else
		//	g_pRasterizeBuffer[(pixelBlockY + 1) * RASTERIZE_BUFFER_W + pixelBlockX]++;
	}

	if(coverageMask & 0x8)
	{
		_DEBUG_ASSERT(g_pRasterizeBuffer[(pixelBlockY + 1) * RASTERIZE_BUFFER_W + pixelBlockX + 1] == 0);
		g_pRasterizeBuffer[(pixelBlockY + 1) * RASTERIZE_BUFFER_W + pixelBlockX + 1] = (byte)colorScaledInt[3];
		//g_NumPixelsProcessed++;
		//if(g_pRasterizeBuffer[(pixelBlockY + 1) * RASTERIZE_BUFFER_W + pixelBlockX + 1])
		//	g_pRasterizeBuffer[(pixelBlockY + 1) * RASTERIZE_BUFFER_W + pixelBlockX + 1] = 0xFF;
		//else
		//	g_pRasterizeBuffer[(pixelBlockY + 1) * RASTERIZE_BUFFER_W + pixelBlockX + 1]++;
	}
}

void LinearDepthOutPixelShader(int pixelBlockX, int pixelBlockY, __m128 coverage, __m128 depthVals, __m128* interVals, void* pContext)
{	
	//g_NumPixelsProcessed += 4;
	//AtomicAdd((int*)&g_NumPixelsProcessed, 4);

	//__m128 linearDepthScaled = _mm_mul_ps(interVals[0], COLOR_MAX);
	//_ALIGN(16) int linearDepthInt[4];
	//_mm_store_si128( (__m128i*) linearDepthInt, _mm_cvtps_epi32( linearDepthScaled ) );

	// Ensure 8 byte aligned
	_DEBUG_ASSERT( ((size_t)(g_pRasterizeDepthBuffer + pixelBlockY * RASTERIZE_BUFFER_W + pixelBlockX) & 0x7) == 0 );
	_DEBUG_ASSERT( ((size_t)(g_pRasterizeDepthBuffer + (pixelBlockY + 1) * RASTERIZE_BUFFER_W + pixelBlockX) & 0x7) == 0 );


#ifdef USE_FAST_DEPTH_CLEAR
	// Fetch fast depth clear
	uint fastDepthClearFlagOffset = pixelBlockX >> 1;
	uint& fastDepthFlag = *(g_pFastDepthClearBuffer + (pixelBlockY >> 1) * FAST_DEPTH_CLEAR_W + (fastDepthClearFlagOffset >> 5));
	uint flagIndex = (1 << (fastDepthClearFlagOffset & 31));
	if( fastDepthFlag & flagIndex )
	{
		//depthInBuffer = _mm_set_ps1(1.0f);	
		g_pRasterizeDepthBuffer[pixelBlockY * RASTERIZE_BUFFER_W + pixelBlockX] = 1.0f;
		g_pRasterizeDepthBuffer[pixelBlockY * RASTERIZE_BUFFER_W + pixelBlockX + 1] = 1.0f;
		g_pRasterizeDepthBuffer[(pixelBlockY + 1) * RASTERIZE_BUFFER_W + pixelBlockX] = 1.0f;
		g_pRasterizeDepthBuffer[(pixelBlockY + 1) * RASTERIZE_BUFFER_W + pixelBlockX + 1] = 1.0f;

		fastDepthFlag &= (~flagIndex);
	}
#endif

	// Depth buffer compare
	//const uint DEPTH_TILE_W = 16;
	//const uint DEPTH_TILE_H = 16;
	//const uint NUM_DEPTH_TILES_W = RASTERIZE_BUFFER_W / DEPTH_TILE_W;
	//const uint NUM_DEPTH_TILES_H = RASTERIZE_BUFFER_H / DEPTH_TILE_H;
	//const uint NUM_PIXELS_IN_TILE = DEPTH_TILE_W * DEPTH_TILE_H;
	//const uint DEPTH_TILE_STRIDE = NUM_DEPTH_TILES_W * NUM_PIXELS_IN_TILE;

	//uint depthTileIndexX = pixelBlockX >> 4;
	//uint depthTileIndexY = pixelBlockY >> 4;
	//uint depthQuadIndexX = (pixelBlockX & 0xF);
	//uint depthQuadIndexY = (pixelBlockY & 0xF);

	//uint depthAddressOffset = depthTileIndexY * DEPTH_TILE_STRIDE + depthTileIndexX * NUM_PIXELS_IN_TILE +
	//						  depthQuadIndexY * DEPTH_TILE_W + (depthQuadIndexX << 1);

	uint depthAddressOffset = pixelBlockY * RASTERIZE_BUFFER_W + (pixelBlockX << 1);

	__m128 depthInBuffer;
	depthInBuffer = _mm_load_ps( g_pRasterizeDepthBuffer + depthAddressOffset );

    //__m128 depthMin = _mm_min_ps( depthVals, depthInBuffer );                       // Check depth test
    //__m128 depthStore = _mm_xor_ps( depthInBuffer, _mm_and_ps( coverage, _mm_xor_ps(depthMin, depthInBuffer) ) ); // Mask with edge function
    //_mm_store_ps(g_pRasterizeDepthBuffer + depthAddressOffset, depthStore);

	//depthInBuffer = _mm_loadl_pi(depthInBuffer, (__m64*)(g_pRasterizeDepthBuffer + pixelBlockY * RASTERIZE_BUFFER_W + pixelBlockX));
	//depthInBuffer = _mm_loadh_pi(depthInBuffer, (__m64*)(g_pRasterizeDepthBuffer + (pixelBlockY + 1) * RASTERIZE_BUFFER_W + pixelBlockX));

	//const static _ALIGN(16) float __ZERO[] = {0.0f, 0.0f, 0.0f, 0.0f};

	__m128 ZERO = _mm_setzero_ps();
	__m128 depthValsCmpBuffer	= _mm_cmple_ps(depthVals, depthInBuffer);
	__m128 depthValsCmpNear		= _mm_cmpge_ps(depthVals, ZERO);
	__m128 depthValsCmp			= _mm_and_ps(depthValsCmpBuffer, depthValsCmpNear);
	//int depthMask = _mm_movemask_ps(depthValsCmp);

	__m128 depthAndCoverage = _mm_and_ps(depthValsCmp, coverage);
	int depthAndCoverageMask = _mm_movemask_ps(depthAndCoverage);

	if(depthAndCoverageMask > 0)
	{
		// Output depth
		__m128 depthBufferOut	= _mm_andnot_ps(depthAndCoverage, depthInBuffer);
		__m128 depthOut			= _mm_and_ps(depthAndCoverage, depthVals);
		depthOut = _mm_add_ps(depthBufferOut, depthOut);
		_mm_store_ps(g_pRasterizeDepthBuffer + depthAddressOffset, depthOut);

		//const static __m128 UV_SHIFT = _mm_set_ps1(4.0f);
		//__m128 uMult = _mm_mul_ps(UV_SHIFT, interVals[1]);
		//__m128 vMult = _mm_mul_ps(UV_SHIFT, interVals[2]);
		//__m128i uInt = _mm_cvttps_epi32(uMult);
		//__m128i vInt = _mm_cvttps_epi32(vMult);
		//__m128 uFloat = _mm_cvtepi32_ps(uInt);
		//__m128 vFloat = _mm_cvtepi32_ps(vInt);
		//__m128 uFrac = _mm_sub_ps(uMult, uFloat);
		//__m128 vFrac = _mm_sub_ps(vMult, vFloat);

		//const static __m128 CMP = _mm_set_ps1(0.5f);
		//const static __m128 TWO = _mm_set_ps1(2.0f);
		//const static __m128 ONE = _mm_set_ps1(1.0f);
		//__m128 uCmp = _mm_and_ps( _mm_cmpgt_ps(uFrac, CMP), ONE );
		//__m128 vCmp = _mm_and_ps( _mm_cmpgt_ps(vFrac, CMP), ONE );
		//__m128 uvCmpAdd = _mm_add_ps(uCmp, vCmp);
		//__m128 uvCmpMult2 = _mm_mul_ps(uCmp, vCmp);
		//uvCmpMult2 = _mm_mul_ps(uvCmpMult2, TWO);

		//finalColor = _mm_sub_ps(uvCmpAdd, uvCmpMult2);
#if 1
		// Output color
		__m128 finalColor = interVals[0];// _mm_mul_ps(interVals[0], finalColor);

		const static __m128 COLOR_MAX = _mm_set_ps1(255.0f);
		__m128 colorScaled = _mm_mul_ps(finalColor, COLOR_MAX);
		_ALIGN(16) int colorScaledInt[4];
		_mm_store_si128( (__m128i*) colorScaledInt, _mm_cvtps_epi32( colorScaled ) );

		// Depth
		//_ALIGN(16) float outDepth[4];
		//_mm_store_ps((float*) outDepth, depthVals);

		if(depthAndCoverageMask & 0x1)
		{
			//_DEBUG_ASSERT(g_pRasterizeBuffer[pixelBlockY * RASTERIZE_BUFFER_W + pixelBlockX] == 0);
			g_pRasterizeBuffer[pixelBlockY * RASTERIZE_BUFFER_W + pixelBlockX] = (byte)colorScaledInt[0];
			//g_pRasterizeDepthBuffer[pixelBlockY * RASTERIZE_BUFFER_W + pixelBlockX] = outDepth[0];
			//g_pRasterizeDepthBuffer[depthAddressOffset] = outDepth[0];
		}

		if(depthAndCoverageMask & 0x2)
		{
			//_DEBUG_ASSERT(g_pRasterizeBuffer[pixelBlockY * RASTERIZE_BUFFER_W + pixelBlockX + 1] == 0);
			g_pRasterizeBuffer[pixelBlockY * RASTERIZE_BUFFER_W + pixelBlockX + 1] = (byte)colorScaledInt[1];
			//g_pRasterizeDepthBuffer[pixelBlockY * RASTERIZE_BUFFER_W + pixelBlockX + 1] = outDepth[1];
			//g_pRasterizeDepthBuffer[depthAddressOffset + 1] = outDepth[1];
		}

		if(depthAndCoverageMask & 0x4)
		{
			//_DEBUG_ASSERT(g_pRasterizeBuffer[(pixelBlockY + 1) * RASTERIZE_BUFFER_W + pixelBlockX] == 0);
			g_pRasterizeBuffer[(pixelBlockY + 1) * RASTERIZE_BUFFER_W + pixelBlockX] = (byte)colorScaledInt[2];
			//g_pRasterizeDepthBuffer[(pixelBlockY + 1) * RASTERIZE_BUFFER_W + pixelBlockX] = outDepth[2];
			//g_pRasterizeDepthBuffer[depthAddressOffset + 2] = outDepth[2];

		}

		if(depthAndCoverageMask & 0x8)
		{
			//_DEBUG_ASSERT(g_pRasterizeBuffer[(pixelBlockY + 1) * RASTERIZE_BUFFER_W + pixelBlockX + 1] == 0);
			g_pRasterizeBuffer[(pixelBlockY + 1) * RASTERIZE_BUFFER_W + pixelBlockX + 1] = (byte)colorScaledInt[3];
			//g_pRasterizeDepthBuffer[(pixelBlockY + 1) * RASTERIZE_BUFFER_W + pixelBlockX + 1] = outDepth[3];
			//g_pRasterizeDepthBuffer[depthAddressOffset + 3] = outDepth[3];
		}
#endif
	}
}

struct AcceptData
{
	float* pFullData;
	int numFull;

	float* pPartialData;
	int numPartial;
};

inline void AcceptPartial(float pixelBlockX, float pixelBlockY, void* pContext)
{
	AcceptData* __restrict pAccept = (AcceptData*) pContext;

	pAccept->numPartial++;
	int numPartial = (pAccept->numPartial - 1) << 1;
	//int numPartial = pAccept->numPartial;
	//numPartial++;
	//pAccept->numPartial = numPartial;

	pAccept->pPartialData[numPartial] = pixelBlockX;
	pAccept->pPartialData[numPartial + 1] = pixelBlockY;
}

inline void AcceptFull(float pixelBlockX, float pixelBlockY, void* pContext)
{
	AcceptData* __restrict pAccept = (AcceptData*) pContext;

	pAccept->numFull++;
	int numFull = (pAccept->numFull - 1) << 1;
	//int numPartial = pAccept->numPartial;
	//numPartial++;
	//pAccept->numPartial = numPartial;

	pAccept->pFullData[numFull] = pixelBlockX;
	pAccept->pFullData[numFull + 1] = pixelBlockY;
}

void CubeVertexTransform(const gmtl::MatrixA44f& worldViewProj, const float* pXYZ, const float* pUV, gmtl::VecA4f* pXYZWOut, float* pInterpolantOut)
{
	TransformVecW1((gmtl::MatrixA44f*) &worldViewProj, pXYZ, pXYZWOut);
	pInterpolantOut[0] = (*pXYZWOut)[3] * FAR_PLANE_RCP; // Redundant if no linear depth out for color
	pInterpolantOut[1] = pUV[0];
	pInterpolantOut[2] = pUV[1];
}

//__declspec(noinline) void DoSomething(void* pTable, void* pDest)
//{
//	const RasterInfo<1, 2>* __restrict pRaster = ((RasterInfo<1, 2>*) pTable);
//
//	__m128 zero = _mm_setzero_ps();
//
//	float curX = 1.0f;
//	float curY = 1.0f;
//	__m128 xxxx = _mm_load_ps1(&curX);
//	xxxx = _mm_add_ps(xxxx, xxxx);
//	__m128 yyyy = _mm_load_ps1(&curY);
//	yyyy = _mm_add_ps(yyyy, yyyy);
//
//	__m128 e1 = _mm_mul_ps(pRaster->e1aaaa, xxxx);
//	e1 = _mm_add_ps( _mm_mul_ps(pRaster->e1bbbb, yyyy), e1 );
//	__m128 e1Reject = _mm_add_ps( e1, pRaster->e1RejectLayerCCCC[1] );
//	__m128 e2 = _mm_mul_ps(pRaster->e2aaaa, xxxx);
//	e2 = _mm_add_ps( _mm_mul_ps(pRaster->e2bbbb, yyyy), e2 );
//	__m128 e2Reject = _mm_add_ps( e2, pRaster->e2RejectLayerCCCC[1] );
//
//	__m128 e3 = _mm_mul_ps(pRaster->e3aaaa, xxxx);
//	e3 = _mm_add_ps( _mm_mul_ps(pRaster->e3bbbb, yyyy), e3 );
//	__m128 e3Reject = _mm_add_ps( e3, pRaster->e3RejectLayerCCCC[1] );
//
//	__m128 insideTest = _mm_and_ps( _mm_cmpgt_ps(e1Reject, zero), _mm_cmpgt_ps(e2Reject, zero) );
//	insideTest = _mm_and_ps( insideTest, _mm_cmpgt_ps(e3Reject, zero) );
//	int notRejectInsideMask = _mm_movemask_ps(insideTest);
//
//	// Trivial accept test
//	__m128 e1Accept = _mm_add_ps(e1Reject, pRaster->e1AcceptLayerOffset[1]);
//	__m128 e2Accept = _mm_add_ps(e2Reject, pRaster->e2AcceptLayerOffset[1]);	
//	insideTest = _mm_and_ps( _mm_cmpgt_ps(e1Accept, zero), _mm_cmpgt_ps(e2Accept, zero) );
//
//	__m128 e3Accept = _mm_add_ps(e3Reject, pRaster->e3AcceptLayerOffset[1]);
//	insideTest = _mm_and_ps( insideTest, _mm_cmpgt_ps(e3Accept, zero) );
//	int acceptInsideMask = _mm_movemask_ps(insideTest);
//
//	notRejectInsideMask -= acceptInsideMask;
//	*((int*)pDest) = notRejectInsideMask;
//}



#define _THE_PIXEL_SHADER &LinearDepthOutPixelShader
#define NUM_LAYERS 2

const static uint TOP_TILE_SIZE = 16;
const static uint MID_TILE_SIZE = 4;
const static uint NUM_TOP_TILES = _GET_NUM_TILES(RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, TOP_TILE_SIZE, TOP_TILE_SIZE);
const static uint NUM_MID_TILES_IN_TOP_TILE = _GET_NUM_TILES(TOP_TILE_SIZE, TOP_TILE_SIZE, MID_TILE_SIZE, MID_TILE_SIZE);
const static uint NUM_MID_TILES = NUM_TOP_TILES * NUM_MID_TILES_IN_TOP_TILE;
const static uint NUM_INTER = 1;
const static uint RASTER_INFO_SIZE = _GET_RASTER_INFO_SIZE(NUM_INTER, NUM_LAYERS);
const static int HALF_RASTERIZE_BUFFER_W = (RASTERIZE_BUFFER_W >> 1);
const static int HALF_RASTERIZE_BUFFER_H = (RASTERIZE_BUFFER_H >> 1);
const static int CUBE_BATCH = NUM_CUBES;
const static int TRIANGLE_BATCH = CUBE_BATCH * 12;

_ALIGN(16) byte g_Raster[RASTER_INFO_SIZE * TRIANGLE_BATCH];
_ALIGN(16) boolean g_RasterRes[TRIANGLE_BATCH];

template<int BUFFER_WIDTH, int BUFFER_HEIGHT>
struct RenderJob : public IPRunnable
{
	const static int JOB_NUM_TOP_TILES = _GET_NUM_TILES(BUFFER_WIDTH, BUFFER_HEIGHT, TOP_TILE_SIZE, TOP_TILE_SIZE);
	const static int JOB_NUM_MID_TILES = JOB_NUM_TOP_TILES * _GET_NUM_TILES(TOP_TILE_SIZE, TOP_TILE_SIZE, MID_TILE_SIZE, MID_TILE_SIZE);

	float startX;
	float startY;
	//byte* pRasterData;

	float topTileFull[JOB_NUM_TOP_TILES * 2];
	float topTilePartial[JOB_NUM_TOP_TILES * 2];
	float midTileFull[JOB_NUM_MID_TILES * 2];
	float midTilePartial[JOB_NUM_MID_TILES * 2];

	//double start;
	//double end;

	void Render()
	{
		//start = g_pPlatform->GetTimer().GetTime();

		AcceptData acceptTopData;
		acceptTopData.pFullData = topTileFull;
		acceptTopData.pPartialData = topTilePartial;
		
		AcceptData acceptMidData;
		acceptMidData.pFullData = midTileFull;
		acceptMidData.pPartialData = midTilePartial;
		
		_LOOPi(TRIANGLE_BATCH)
		{
			//_mm_prefetch((const char*)(g_Raster + (i) * RASTER_INFO_SIZE), _MM_HINT_T0);
			
			acceptTopData.numFull = 0;
			acceptTopData.numPartial = 0;
			acceptMidData.numFull = 0;
			acceptMidData.numPartial = 0;

			if(g_RasterRes[i])
			{

#if NUM_LAYERS == 1
				TilesEdgeCheckWithBB<NUM_INTER, 1, TOP_TILE_SIZE, 0, &AcceptPartial, &AcceptFull>
					(g_Raster + i * RASTER_INFO_SIZE, startX, startY, BUFFER_WIDTH, BUFFER_HEIGHT, &acceptTopData);


				for(int tileIndex=0; tileIndex < acceptTopData.numPartial; tileIndex++)
				{
					TileRasterizeEdgeCheck<NUM_INTER, 1, TOP_TILE_SIZE, _THE_PIXEL_SHADER>
						(g_Raster + i * RASTER_INFO_SIZE, acceptTopData.pPartialData[tileIndex << 1], acceptTopData.pPartialData[(tileIndex << 1) + 1], NULL);
				}

				for(int tileIndex=0; tileIndex < acceptTopData.numFull; tileIndex++)
				{	
					TileRasterizeNoEdgeCheck<NUM_INTER, 1, TOP_TILE_SIZE, _THE_PIXEL_SHADER>
						(g_Raster + i * RASTER_INFO_SIZE, acceptTopData.pFullData[tileIndex << 1], acceptTopData.pFullData[(tileIndex << 1) + 1], NULL);
				}
		

#endif

#if NUM_LAYERS == 2
				TilesEdgeCheckWithBB<NUM_INTER, 2, TOP_TILE_SIZE, 0, &AcceptPartial, &AcceptFull>
						(g_Raster + i * RASTER_INFO_SIZE, startX, startY, BUFFER_WIDTH, BUFFER_HEIGHT, &acceptTopData);

				for(int tileIndex=0; tileIndex < acceptTopData.numPartial; tileIndex++)
				{
					//TilesEdgeCheck2x2<NUM_INTER, 2, MID_TILE_SIZE, 1, &AcceptPartial, &AcceptFull>
					TilesEdgeCheck<NUM_INTER, 2, TOP_TILE_SIZE, TOP_TILE_SIZE, MID_TILE_SIZE, 1, &AcceptPartial, &AcceptFull>
						(g_Raster + i * RASTER_INFO_SIZE, acceptTopData.pPartialData[tileIndex << 1], acceptTopData.pPartialData[(tileIndex << 1) + 1], &acceptMidData);
				}

				for(int tileIndex=0; tileIndex < acceptMidData.numPartial; tileIndex++)
				{
					TileRasterizeEdgeCheck<NUM_INTER, 2, MID_TILE_SIZE, _THE_PIXEL_SHADER>
						(g_Raster + i * RASTER_INFO_SIZE, acceptMidData.pPartialData[tileIndex << 1], acceptMidData.pPartialData[(tileIndex << 1) + 1], NULL);
				}

				for(int tileIndex=0; tileIndex < acceptTopData.numFull; tileIndex++)
				{	
					TileRasterizeNoEdgeCheck<NUM_INTER, 2, TOP_TILE_SIZE, _THE_PIXEL_SHADER>
						(g_Raster + i * RASTER_INFO_SIZE, acceptTopData.pFullData[tileIndex << 1], acceptTopData.pFullData[(tileIndex << 1) + 1], NULL);
				}

				for(int tileIndex=0; tileIndex < acceptMidData.numFull; tileIndex++)
				{
					TileRasterizeNoEdgeCheck<NUM_INTER, 2, MID_TILE_SIZE, _THE_PIXEL_SHADER>
						(g_Raster + i * RASTER_INFO_SIZE, acceptMidData.pFullData[tileIndex << 1], acceptMidData.pFullData[(tileIndex << 1) + 1], NULL);
				}

#endif
				//TilesEdgeCheckWithBB<NUM_INTER, 1, TOP_TILE_SIZE, 0, &AcceptPartial, &AcceptFull>(g_Raster + i * RASTER_INFO_SIZE, startX, startY, BUFFER_WIDTH, BUFFER_HEIGHT, &acceptTopData);

				//for(int tileIndex=0; tileIndex < acceptTopData.numPartial; tileIndex++)
				//{
				//	TileRasterizeEdgeCheck<NUM_INTER, 1, TOP_TILE_SIZE, _THE_PIXEL_SHADER>
				//		(g_Raster + i * RASTER_INFO_SIZE, acceptTopData.pPartialData[tileIndex << 1], acceptTopData.pPartialData[(tileIndex << 1) + 1], NULL);
				//}

				//for(int tileIndex=0; tileIndex < acceptTopData.numFull; tileIndex++)
				//{	
				//	TileRasterizeNoEdgeCheck<NUM_INTER, 1, TOP_TILE_SIZE, _THE_PIXEL_SHADER>
				//		(g_Raster + i * RASTER_INFO_SIZE, acceptTopData.pFullData[tileIndex << 1], acceptTopData.pFullData[(tileIndex << 1) + 1], NULL);
				//}
			}
		}

		//end = (g_pPlatform->GetTimer().GetTime());
	}

	int Run()
	{
		Render();
		return 0;
	}
};

RenderJob<HALF_RASTERIZE_BUFFER_W, HALF_RASTERIZE_BUFFER_H> g_RenderJob1;
RenderJob<HALF_RASTERIZE_BUFFER_W, HALF_RASTERIZE_BUFFER_H> g_RenderJob2;
RenderJob<HALF_RASTERIZE_BUFFER_W, HALF_RASTERIZE_BUFFER_H> g_RenderJob3;
RenderJob<HALF_RASTERIZE_BUFFER_W, HALF_RASTERIZE_BUFFER_H> g_RenderJob4;

struct TransformAndSetupJob : public IPRunnable
{
	const gmtl::MatrixA44f* pCubeProjs;
	uint numCubes;
	byte* pRasters;
	boolean* pRasterRes;

	int Run()
	{
		_LOOP(cubeIndex, numCubes)
		{
			_mm_prefetch((char*)&(pCubeProjs[cubeIndex+1]), _MM_HINT_T0);

			const gmtl::MatrixA44f& cubeWorldViewProj = pCubeProjs[cubeIndex];

			_LOOPi(12)
			{
				uint i1 = g_CubeIndices[i*3];
				uint i2 = g_CubeIndices[i*3+1];
				uint i3 = g_CubeIndices[i*3+2];

				const float* pV1XYZ = g_CubeVertices + i1 * CUBE_VERTEX_STRIDE;
				const float* pV2XYZ = g_CubeVertices + i2 * CUBE_VERTEX_STRIDE;
				const float* pV3XYZ = g_CubeVertices + i3 * CUBE_VERTEX_STRIDE;

				gmtl::VecA4f postV1;
				float v1Inter[3];
				gmtl::VecA4f postV2;
				float v2Inter[3];
				gmtl::VecA4f postV3;
				float v3Inter[3];

				//double cubeStart = g_pPlatform->GetTimer().GetTime();

				CubeVertexTransform(cubeWorldViewProj, pV1XYZ, pV1XYZ + CUBE_TEXUV_OFFSET, &postV1, v1Inter);
				CubeVertexTransform(cubeWorldViewProj, pV2XYZ, pV2XYZ + CUBE_TEXUV_OFFSET, &postV2, v2Inter);
				CubeVertexTransform(cubeWorldViewProj, pV3XYZ, pV3XYZ + CUBE_TEXUV_OFFSET, &postV3, v3Inter);

				//g_VertTransTimeElapsed += (float) (g_pPlatform->GetTimer().GetTime() - cubeStart); 

				byte* pCurrentRaster = pRasters + (RASTER_INFO_SIZE * i) + (RASTER_INFO_SIZE * cubeIndex * 12);
#if NUM_LAYERS == 1
				// 1 layer
				boolean res = TriangleSetup1Layer<NUM_INTER, TOP_TILE_SIZE>(postV3, postV2, postV1, v3Inter, v2Inter, v1Inter, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, pCurrentRaster );
				res = res && TriangleBoundingBox1Layer<NUM_INTER, TOP_TILE_SIZE>(pCurrentRaster, postV3, postV2, postV1);
#endif
#if NUM_LAYERS == 2
				// 2 layer
				boolean res = TriangleSetup2Layer<NUM_INTER, TOP_TILE_SIZE, MID_TILE_SIZE>(postV3, postV2, postV1, v3Inter, v2Inter, v1Inter, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, pCurrentRaster);
				res = res && TriangleBoundingBox2Layer<NUM_INTER, TOP_TILE_SIZE, MID_TILE_SIZE>(pCurrentRaster, postV3, postV2, postV1);
#endif
				pRasterRes[i + cubeIndex * 12] = res;
			}
		}

		return 0;
	}
};

TransformAndSetupJob g_TransformAndSetupJobs[4];

volatile int g_RenderSync;

struct ManualRenderJobDispatch : public IPRunnable
{
	RenderJob<HALF_RASTERIZE_BUFFER_W, HALF_RASTERIZE_BUFFER_H>* pRenderJob;

	int isTerminating;
	int id;

	int Run()
	{
		while(!isTerminating)
		{
			byte& startFlag = ((byte*)(&g_RenderSync))[id];
			//_mm_prefetch((char*)pRenderJob, _MM_HINT_T0);

			if(startFlag)
			{
				//((byte*)(&g_RenderSyncBack))[id] = 0xFF;
				pRenderJob->Render();			
				startFlag = 0;
			}
		}

		return 0;
	}
};

ManualRenderJobDispatch g_Dispatch[NUM_THREADS];

void InitManualJobDispatch()
{
	g_RenderJob1.startX = 0.5f;
	g_RenderJob1.startY = 0.5f;
//	g_RenderJob1.pRasterData = g_Raster;

	g_RenderJob2.startX = HALF_RASTERIZE_BUFFER_W + 0.5f;
	g_RenderJob2.startY = 0.5f;
//	g_RenderJob2.pRasterData = g_Raster;

	g_RenderJob3.startX = 0.5f;
	g_RenderJob3.startY = HALF_RASTERIZE_BUFFER_H + 0.5f;
//	g_RenderJob3.pRasterData = g_Raster;

	g_RenderJob4.startX = HALF_RASTERIZE_BUFFER_W + 0.5f;
	g_RenderJob4.startY = HALF_RASTERIZE_BUFFER_H + 0.5f;
//	g_RenderJob4.pRasterData = g_Raster;

	_LOOPi(NUM_THREADS)
	{
		g_Dispatch[i].isTerminating = FALSE;
		g_Dispatch[i].pRenderJob = NULL;
		g_Dispatch[i].id = i;
	}

	g_Dispatch[0].pRenderJob = &g_RenderJob1;
	g_Dispatch[1].pRenderJob = &g_RenderJob2;
	g_Dispatch[2].pRenderJob = &g_RenderJob3;

	//_LOOPi(NUM_THREADS)
	//{
	//	g_pThreads[i]->Resume(&g_Dispatch[i]);
	//}
}

void ShutdownManualJobDispatch()
{
	_LOOPi(NUM_THREADS)
	{
		g_Dispatch[i].isTerminating = TRUE;
	}
}

void RenderSWCube(const gmtl::MatrixA44f* pCubeWorldViewProj)
{
	int numPerJob = CUBE_BATCH / 4;
	_DEBUG_COMPILE_ASSERT((CUBE_BATCH % 4) == 0);
	
    double swStart = g_pPlatform->GetTimer().GetTime();
//    g_pThreadPool->SetAlwaysActive(TRUE);

	_LOOPi(4)
	{
		g_TransformAndSetupJobs[i].pCubeProjs = (pCubeWorldViewProj + i * numPerJob);
		g_TransformAndSetupJobs[i].pRasters = g_Raster + (i * numPerJob * RASTER_INFO_SIZE * 12);
		g_TransformAndSetupJobs[i].pRasterRes = g_RasterRes + (i * numPerJob * 12);
		g_TransformAndSetupJobs[i].numCubes = numPerJob;
	
		//g_TransformAndSetupJobs[i].Run();
		g_pThreadPool->QueueJobUnsafe(g_TransformAndSetupJobs[i]);
	}

	g_pThreadPool->ProcessJobs();
	
	//g_SWTimeElapsed += (g_pPlatform->GetTimer().GetTime() - swStart);

	//_LOOP(cubeIndex, CUBE_BATCH)
	//{
	//	const gmtl::MatrixA44f& cubeWorldViewProj = pCubeWorldViewProj[cubeIndex];

	//	_LOOPi(12)
	//	{
	//		uint i1 = g_CubeIndices[i*3];
	//		uint i2 = g_CubeIndices[i*3+1];
	//		uint i3 = g_CubeIndices[i*3+2];

	//		const float* pV1XYZ = g_CubeVertices + i1 * CUBE_VERTEX_STRIDE;
	//		const float* pV2XYZ = g_CubeVertices + i2 * CUBE_VERTEX_STRIDE;
	//		const float* pV3XYZ = g_CubeVertices + i3 * CUBE_VERTEX_STRIDE;

	//		gmtl::VecA4f postV1;
	//		float v1Inter[3];
	//		gmtl::VecA4f postV2;
	//		float v2Inter[3];
	//		gmtl::VecA4f postV3;
	//		float v3Inter[3];

	//		double cubeStart = g_pPlatform->GetTimer().GetTime();

	//		CubeVertexTransform(cubeWorldViewProj, pV1XYZ, pV1XYZ + CUBE_TEXUV_OFFSET, &postV1, v1Inter);
	//		CubeVertexTransform(cubeWorldViewProj, pV2XYZ, pV2XYZ + CUBE_TEXUV_OFFSET, &postV2, v2Inter);
	//		CubeVertexTransform(cubeWorldViewProj, pV3XYZ, pV3XYZ + CUBE_TEXUV_OFFSET, &postV3, v3Inter);

	//		g_VertTransTimeElapsed += (float) (g_pPlatform->GetTimer().GetTime() - cubeStart); 

	//		//double swStart = g_pPlatform->GetTimer().GetTime();

	//		//float topTileFull[NUM_TOP_TILES * 2];
	//		//float topTilePartial[NUM_TOP_TILES * 2];
	//		//float midTileFull[NUM_MID_TILES * 2];
	//		//float midTilePartial[NUM_MID_TILES * 2];

	//		//AcceptData acceptTopData;
	//		//acceptTopData.pFullData = topTileFull;
	//		//acceptTopData.numFull = 0;
	//		//acceptTopData.pPartialData = topTilePartial;
	//		//acceptTopData.numPartial = 0;

	//		//AcceptData acceptMidData;
	//		//acceptMidData.pFullData = midTileFull;
	//		//acceptMidData.numFull = 0;
	//		//acceptMidData.pPartialData = midTilePartial;
	//		//acceptMidData.numPartial = 0;
	//	
	//		//boolean res = TriangleSetup1Layer<NUM_INTER, TOP_TILE_SIZE>(postV3, postV2, postV1, v3Inter, v2Inter, v1Inter, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, g_Raster + (RASTER_INFO_SIZE * i) + (RASTER_INFO_SIZE * cubeIndex * 12) );
	//		//res = res && TriangleBoundingBox1Layer<NUM_INTER, TOP_TILE_SIZE>(g_Raster + (RASTER_INFO_SIZE * i) + (RASTER_INFO_SIZE * cubeIndex * 12), postV3, postV2, postV1);
	//		boolean res = TriangleSetup2Layer<NUM_INTER, TOP_TILE_SIZE, MID_TILE_SIZE>(postV3, postV2, postV1, v3Inter, v2Inter, v1Inter, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, g_Raster + (RASTER_INFO_SIZE * i) + (RASTER_INFO_SIZE * cubeIndex * 12) );
	//		res = res && TriangleBoundingBox2Layer<NUM_INTER, TOP_TILE_SIZE, MID_TILE_SIZE>(g_Raster + (RASTER_INFO_SIZE * i) + (RASTER_INFO_SIZE * cubeIndex * 12), postV3, postV2, postV1);
	//		g_RasterRes[i + cubeIndex * 12] = res;

	//		/*if(TriangleSetup2Layer<NUM_INTER, TOP_TILE_SIZE, MID_TILE_SIZE>(postV3, postV2, postV1, v3Inter, v2Inter, v1Inter, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, g_Raster + RASTER_INFO_SIZE * i))
	//		{
	//			if(TriangleBoundingBox2Layer<NUM_INTER, TOP_TILE_SIZE, MID_TILE_SIZE>(g_Raster + RASTER_INFO_SIZE * i, postV3, postV2, postV1))
	//			{
	//				//TilesEdgeCheckWithBB<NUM_INTER, 2, TOP_TILE_SIZE, 0, &AcceptPartial, &AcceptFull>(g_Raster, 0.5f, 0.5f, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, &acceptTopData);

	//				////TileRasterizeEdgeCheck<NUM_INTER, TOP_TILE_SIZE, _THE_PIXEL_SHADER>(raster, (float*) topTilePartial, numPartial);


	//				//for(int tileIndex=0; tileIndex < acceptTopData.numPartial; tileIndex++)
	//				//{
	//				//	TilesEdgeCheck<NUM_INTER, 2, TOP_TILE_SIZE, TOP_TILE_SIZE, MID_TILE_SIZE, 1, &AcceptPartial, &AcceptFull>
	//				//		//TilesEdgeCheck2x2<NUM_INTER, 2, MID_TILE_SIZE, 1, &AcceptPartial, &AcceptFull>
	//				//		(g_Raster, acceptTopData.pPartialData[tileIndex << 1], acceptTopData.pPartialData[(tileIndex << 1) + 1], &acceptMidData);
	//				//}

	//				////DoSomething2<NUM_INTER, 2, TOP_TILE_SIZE, TOP_TILE_SIZE, MID_TILE_SIZE, 1>(raster, (float*) topTilePartial, numPartial, (float*) midTilePartial, &numPartialMid, (float*) midTileFull, &numFullMid);
	//				//for(int tileIndex=0; tileIndex < acceptMidData.numPartial; tileIndex++)
	//				//{
	//				//	TileRasterizeEdgeCheck<NUM_INTER, 2, MID_TILE_SIZE, _THE_PIXEL_SHADER>
	//				//		(g_Raster, acceptMidData.pPartialData[tileIndex << 1], acceptMidData.pPartialData[(tileIndex << 1) + 1], NULL);
	//				//}

	//				//for(int tileIndex=0; tileIndex < acceptTopData.numFull; tileIndex++)
	//				//{	
	//				//	TileRasterizeNoEdgeCheck<NUM_INTER, 2, TOP_TILE_SIZE, _THE_PIXEL_SHADER>
	//				//		(g_Raster, acceptTopData.pFullData[tileIndex << 1], acceptTopData.pFullData[(tileIndex << 1) + 1], NULL);
	//				//}

	//				//for(int tileIndex=0; tileIndex < acceptMidData.numFull; tileIndex++)
	//				//{
	//				//	TileRasterizeNoEdgeCheck<NUM_INTER, 2, MID_TILE_SIZE, _THE_PIXEL_SHADER>
	//				//		(g_Raster, acceptMidData.pFullData[tileIndex << 1], acceptMidData.pFullData[(tileIndex << 1) + 1], NULL);
	//				//}


	//				g_NumTriangles++;
	//			}
	//		}*/

	//		//g_SWTimeElapsed += (float) (g_pPlatform->GetTimer().GetTime() - swStart);
	//	}
	//}

	
	//g_RenderSync = 0x00FFFFFF;
	//g_SWTimeElapsed += (float) (g_pPlatform->GetTimer().GetTime() - swStart);

	//g_RenderJob1.Render();
	//g_RenderJob2.Render();
	//g_RenderJob3.Render();
	//g_RenderJob4.Render();

	//double swStart = g_pPlatform->GetTimer().GetTime();
	//while (g_RenderSync > 0);
	//
	g_pThreadPool->QueueJobUnsafe(g_RenderJob1);
	g_pThreadPool->QueueJobUnsafe(g_RenderJob2);
	g_pThreadPool->QueueJobUnsafe(g_RenderJob3);
	g_pThreadPool->QueueJobUnsafe(g_RenderJob4);

//	g_pThreadPool->SetAlwaysActive(FALSE);

	g_pThreadPool->ProcessJobs();
	
	g_SWTimeElapsed += (g_pPlatform->GetTimer().GetTime() - swStart);
	//g_SWTimeElapsed += (g_RenderJob1.end - g_RenderJob1.start);

}

void RenderQuad()
{
	gmtl::VecA4f pos1;
	pos1[0] = -1.0f; pos1[1] = 1.0f; pos1[2] = 0.0f; pos1[3] = 1.0f;
	gmtl::VecA4f uv1;
	uv1[0] = 0.0f; uv1[1] = 0.0f;
	gmtl::VecA4f pos2;
	pos2[0] = 1.0f; pos2[1] = 1.0f; pos2[2] = 0.0f; pos2[3] = 1.0f;
	gmtl::VecA4f uv2;
	uv2[0] = 1.0f; uv2[1] = 0.0f;
	gmtl::VecA4f pos3;
	pos3[0] = -1.0f; pos3[1] = -1.0f; pos3[2] = 0.0f; pos3[3] = 1.0f;
	gmtl::VecA4f uv3;
	uv3[0] = 0.0f; uv3[1] = 1.0f;
	gmtl::VecA4f pos4;
	pos4[0] = 1.0f; pos4[1] = -1.0f; pos4[2] = 0.0f; pos4[3] = 1.0f;
	gmtl::VecA4f uv4;
	uv4[0] = 1.0f; uv4[1] = 1.0f;

	double timeStart = g_pPlatform->GetTimer().GetTime();

#undef _THE_PIXEL_SHADER
#define _THE_PIXEL_SHADER &CheckerBoardPixelShader

	const static uint TOP_TILE_SIZE = 32;
	const static uint MID_TILE_SIZE = 8;
	const static uint NUM_TOP_TILES = _GET_NUM_TILES(RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, TOP_TILE_SIZE, TOP_TILE_SIZE);
	const static uint NUM_MID_TILES = NUM_TOP_TILES * _GET_NUM_TILES(TOP_TILE_SIZE, TOP_TILE_SIZE, MID_TILE_SIZE, MID_TILE_SIZE);
	const static uint NUM_INTER = 2;

	_ALIGN(16) byte raster[_GET_RASTER_INFO_SIZE(NUM_INTER, 2)];
	//_TRACE(_W("Rastersize: %d\n"), _GET_RASTER_INFO_SIZE(0));

	byte topTileFull[ _GET_TILE_BUFFER_SIZE( NUM_TOP_TILES ) ];
	byte topTilePartial[ _GET_TILE_BUFFER_SIZE( NUM_TOP_TILES ) ];
	byte midTileFull[ _GET_TILE_BUFFER_SIZE( NUM_MID_TILES ) ];
	byte midTilePartial[ _GET_TILE_BUFFER_SIZE( NUM_MID_TILES ) ];

	AcceptData acceptTopData;
	acceptTopData.pFullData = (float*) topTileFull;
	acceptTopData.numFull = 0;
	acceptTopData.pPartialData = (float*) topTilePartial;
	acceptTopData.numPartial = 0;

	AcceptData acceptMidData;
	acceptMidData.pFullData = (float*) midTileFull;
	acceptMidData.numFull = 0;
	acceptMidData.pPartialData = (float*) midTilePartial;
	acceptMidData.numPartial = 0;

	if(TriangleSetup2Layer<NUM_INTER, TOP_TILE_SIZE, MID_TILE_SIZE>(pos3, pos2, pos1, (float*) &uv3, (float*) &uv2, (float*) &uv1, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, raster))
	{
		//TriangleBoundingBox2Layer<NUM_INTER, TOP_TILE_SIZE, MID_TILE_SIZE>(raster, pos3, pos2, pos1);
		TilesEdgeCheck<NUM_INTER, 2, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, TOP_TILE_SIZE, 0, &AcceptPartial, &AcceptFull>(raster, 0.5f, 0.5f, &acceptTopData);

		//TileRasterizeEdgeCheck<NUM_INTER, TOP_TILE_SIZE, _THE_PIXEL_SHADER>(raster, (float*) topTilePartial, numPartial);

		for(int tileIndex=0; tileIndex < acceptTopData.numPartial; tileIndex +=2)
		{
			TilesEdgeCheck<NUM_INTER, 2, TOP_TILE_SIZE, TOP_TILE_SIZE, MID_TILE_SIZE, 1, &AcceptPartial, &AcceptFull>
				(raster, acceptTopData.pPartialData[tileIndex], acceptTopData.pPartialData[tileIndex + 1], &acceptMidData);
		}

		for(int tileIndex=0; tileIndex < acceptMidData.numPartial; tileIndex +=2)
		{
			TileRasterizeEdgeCheck<NUM_INTER, 2, MID_TILE_SIZE, _THE_PIXEL_SHADER>
				(raster, acceptMidData.pPartialData[tileIndex], acceptMidData.pPartialData[tileIndex + 1], NULL);		
		}

		for(int tileIndex=0; tileIndex < acceptTopData.numFull; tileIndex +=2)
		{	
			TileRasterizeNoEdgeCheck<NUM_INTER, 2, TOP_TILE_SIZE, _THE_PIXEL_SHADER>
				(raster, acceptTopData.pFullData[tileIndex], acceptTopData.pFullData[tileIndex + 1], NULL);		
		}

		for(int tileIndex=0; tileIndex < acceptMidData.numFull; tileIndex +=2)
		{
			TileRasterizeNoEdgeCheck<NUM_INTER, 2, MID_TILE_SIZE, _THE_PIXEL_SHADER>
				(raster, acceptMidData.pFullData[tileIndex], acceptMidData.pFullData[tileIndex + 1], NULL);
		}
	}

	if(TriangleSetup2Layer<NUM_INTER, TOP_TILE_SIZE, MID_TILE_SIZE>(pos2, pos3, pos4, (float*) &uv2, (float*) &uv3, (float*) &uv4, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, raster))
	{	
		//TriangleBoundingBox2Layer<NUM_INTER, TOP_TILE_SIZE, MID_TILE_SIZE>(raster, pos2, pos3, pos4);
		TilesEdgeCheck<NUM_INTER, 2, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, TOP_TILE_SIZE, 0, &AcceptPartial, &AcceptFull>(raster, 0.5f, 0.5f, &acceptTopData);

		//TileRasterizeEdgeCheck<NUM_INTER, TOP_TILE_SIZE, _THE_PIXEL_SHADER>(raster, (float*) topTilePartial, numPartial);
		for(int tileIndex=0; tileIndex < acceptTopData.numPartial; tileIndex +=2)
		{
			TilesEdgeCheck<NUM_INTER, 2, TOP_TILE_SIZE, TOP_TILE_SIZE, MID_TILE_SIZE, 1, &AcceptPartial, &AcceptFull>
				(raster, acceptTopData.pPartialData[tileIndex], acceptTopData.pPartialData[tileIndex + 1], &acceptMidData);
		}

		for(int tileIndex=0; tileIndex < acceptMidData.numPartial; tileIndex +=2)
		{
			TileRasterizeEdgeCheck<NUM_INTER, 2, MID_TILE_SIZE, _THE_PIXEL_SHADER>
				(raster, acceptMidData.pPartialData[tileIndex], acceptMidData.pPartialData[tileIndex + 1], NULL);		
		}

		for(int tileIndex=0; tileIndex < acceptTopData.numFull; tileIndex +=2)
		{	
			TileRasterizeNoEdgeCheck<NUM_INTER, 2, TOP_TILE_SIZE, _THE_PIXEL_SHADER>
				(raster, acceptTopData.pFullData[tileIndex], acceptTopData.pFullData[tileIndex + 1], NULL);		
		}

		for(int tileIndex=0; tileIndex < acceptMidData.numFull; tileIndex +=2)
		{
			TileRasterizeNoEdgeCheck<NUM_INTER, 2, MID_TILE_SIZE, _THE_PIXEL_SHADER>
				(raster, acceptMidData.pFullData[tileIndex], acceptMidData.pFullData[tileIndex + 1], NULL);
		}
	}

	//RasterizationInfo<NUM_INTER, 2> rasterInfo;
	//TriangleSetup<NUM_INTER, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, 2>(pos3, pos2, pos1, (float*) &uv3, (float*) &uv2, (float*) &uv1, rasterInfo);
	//RasterizeTileLevel2<NUM_INTER, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, 2, _THE_PIXEL_SHADER>(&rasterInfo, 0.5f, 0.5f);
	//TriangleSetup<NUM_INTER, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, 2>(pos2, pos3, pos4, (float*) &uv2, (float*) &uv3, (float*) &uv4, rasterInfo);
	//RasterizeTileLevel2<NUM_INTER, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, 2, _THE_PIXEL_SHADER>(&rasterInfo, 0.5f, 0.5f);


	//RasterizePixelBlock<NUM_INTER, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, 2, _THE_PIXEL_SHADER>(rasterInfo, 0.5f, 0.5f);

	g_SWQuadTimeElapsed +=  (float) (g_pPlatform->GetTimer().GetTime() - timeStart);
}

void RenderStats()
{
	// Add debug overlay here
	// Frame rate counter
	gmtl::Vec2i fontPos(1, 17);
	g_pVerdanaFontFace->RenderString(ToStringAuto(_W("%d"), m_LastNumOfFrames), fontPos, 0xFFFFFF90);

	const RMetrics& metric = g_pRenderer->GetMetrics();

	//// Renderer	
	fontPos[1] = 17 * 4;
	g_pVerdanaFontFace->RenderString(ToStringAuto(_W("numDrawCalls: %d"), metric.numDrawCalls), fontPos, 0xFF90FFFF);
	fontPos[1] += 17;
	g_pVerdanaFontFace->RenderString(ToStringAuto(_W("numPrimitivesRendered: %d"), metric.numPrimitivesRendered), fontPos, 0xFF90FFFF);
	fontPos[1] += 17;
	g_pVerdanaFontFace->RenderString(ToStringAuto(_W("numVerticesRendered: %d"), metric.numVerticesRendered), fontPos, 0xFF90FFFF);
	fontPos[1] += 17;
	g_pVerdanaFontFace->RenderString(ToStringAuto(_W("numRenderGroups: %d"), metric.numRenderGroups), fontPos, 0xFF90FFFF);
	fontPos[1] += 17;
	g_pVerdanaFontFace->RenderString(ToStringAuto(_W("numBeginPass: %d"), metric.numBeginPass), fontPos, 0xFF90FFFF);
	fontPos[1] += 17;
	g_pVerdanaFontFace->RenderString(ToStringAuto(_W("numBeginTechnique: %d"), metric.numBeginTechnique), fontPos, 0xFF90FFFF);
	fontPos[1] += 17;
	g_pVerdanaFontFace->RenderString(ToStringAuto(_W("numApplyConstantParam: %d"), metric.numApplyConstantParam), fontPos, 0xFF90FFFF);
	fontPos[1] += 17;
	g_pVerdanaFontFace->RenderString(ToStringAuto(_W("numApplyDynamicParam: %d"), metric.numApplyDynamicParam), fontPos, 0xFF90FFFF);

	g_pVerdanaFontFace->Flush();

	const CMMetrics& memMetrics = g_pAllocator->GetMetrics();

	fontPos[1] += 17;
	g_pCourierFontFace->RenderString(ToStringAuto(_W("numChunkAllocs: %d"), memMetrics.numChunkAllocs), fontPos, 0xFFA0F07F);
	fontPos[1] += 17;
	g_pCourierFontFace->RenderString(ToStringAuto(_W("numPageAllocs: %d"), memMetrics.numPageAllocs), fontPos, 0xFFA0F07F);
	fontPos[1] += 17;
	g_pCourierFontFace->RenderString(ToStringAuto(_W("numPagesFree: %d"), memMetrics.numPagesFree), fontPos, 0xFFA0F07F);
	fontPos[1] += 17;
	g_pCourierFontFace->RenderString(ToStringAuto(_W("totalChunkSizeUsed: %d"), memMetrics.totalChunkSizeUsed), fontPos, 0xFFA0F07F);
	fontPos[1] += 17;
	g_pCourierFontFace->RenderString(ToStringAuto(_W("totalHeapSizeUsed: %d"), memMetrics.totalHeapSizeUsed), fontPos, 0xFFA0F07F);
	fontPos[1] += 17;
	g_pCourierFontFace->RenderString(ToStringAuto(_W("totalPageSizeUsed: %d"), memMetrics.totalPageSizeUsed), fontPos, 0xFFA0F07F);
	fontPos[1] += 34;
	g_pCourierFontFace->RenderString(ToStringAuto(_W("Software Raster Time: %f"), g_SWTimeElapsed * 1000.0), fontPos, 0xFFF0207F);
	fontPos[1] += 17;
	g_pCourierFontFace->RenderString(ToStringAuto(_W("Software VS Time: %f"), g_VertTransTimeElapsed * 1000.0f), fontPos, 0xFFF0207F);
	fontPos[1] += 17;
	g_pCourierFontFace->RenderString(ToStringAuto(_W("Num visible tris: %d"), g_NumTriangles), fontPos, 0xFFF0207F);
	fontPos[1] += 17;
	g_pCourierFontFace->RenderString(ToStringAuto(_W("Num pixels processed: %d"), g_NumPixelsProcessed), fontPos, 0xFFF0207F);
	fontPos[1] += 34;
	g_pCourierFontFace->RenderString(ToStringAuto(_W("Software Raster Quad Time: %f"), g_SWQuadTimeElapsed * 1000.0f), fontPos, 0xFF99FF2F);

	g_pCourierFontFace->Flush();	
}

void RenderDebugObjects()
{
	g_pDebugFX->GetParams().SetViewMatrix(*((const gmtl::Matrix44f*)&g_View));
	g_pDebugFX->GetParams().SetProjMatrix(*((const gmtl::Matrix44f*)&g_Proj));

	g_pDebugFX->Flush();
}		

void FastDepthClear()
{
	//_DEBUG_ASSERT((RASTERIZE_BUFFER_W & 0xF) == 0);
	//_DEBUG_ASSERT((RASTERIZE_BUFFER_H & 0xF) == 0);

#ifdef USE_FAST_DEPTH_CLEAR
	//_DEBUG_COMPILE_ASSERT( ((RASTERIZE_BUFFER_W / 2) % 32) == 0 );
	//_DEBUG_COMPILE_ASSERT( (FAST_DEPTH_CLEAR_W % 4) == 0 );

	const static __m128i CLEAR_FLAGS = _mm_set1_epi32(0xFFFFFFFF);
	uint* pCurFastDepthClearBuffer = g_pFastDepthClearBuffer;
	_LOOPi(FAST_DEPTH_CLEAR_H)
	{
		_LOOPj(FAST_DEPTH_CLEAR_W / 4)
		{
			_mm_store_si128((__m128i*) pCurFastDepthClearBuffer, CLEAR_FLAGS); 
			pCurFastDepthClearBuffer += 4;
		}
	}
#else	
	const __m128 static ONE = _mm_set_ps1(1.0f);
	float* pCurDepthBuffer = g_pRasterizeDepthBuffer;

	_LOOPi(RASTERIZE_BUFFER_H)
	{
		_LOOPj(RASTERIZE_BUFFER_W >> 2)
		{
			_mm_store_ps(pCurDepthBuffer, ONE);
			pCurDepthBuffer += 4;
		}
	}
#endif
}

void RenderAll()
{
	g_VertTransTimeElapsed = 0.0f;
	g_SWTimeElapsed = 0.0f;
	g_NumTriangles = 0;
	g_NumPixelsProcessed = 0;
	g_SWQuadTimeElapsed = 0;

	memset(g_pRasterizeBuffer, 0, sizeof(byte) * RASTERIZE_BUFFER_W * RASTERIZE_BUFFER_H);
	FastDepthClear();

	g_CurIndex += g_IncRate * g_TimeDT;
	if(g_CurIndex >= 4.0f)
		g_CurIndex -= 4.0f;

	gmtl::MatrixA44f viewProj;
	gmtl::MatrixA44f invViewProj;
	MatMatMult(&viewProj, &g_Proj, &g_View);
	gmtl::invertFull(invViewProj, viewProj);

	_LOOPi(NUM_CUBES)
	{
		MatMatMult(&g_CubeWorldViewProj[i], &viewProj, &g_CubeWorld[i]);
	}

	if(g_pRenderer->BeginRender())
	{	
		g_pMainClearGroup->Flush();
		g_pMainClearGroup->Reset();

		//g_pHWGroup->AddDrawOp(g_pTriangleVBGroup, g_pTriangleIB, g_pTriangleEffect, NULL, 0, RVarData());	
		_LOOPi(NUM_CUBES)
		{
			REffectParam params[1];
			params[0].SetMatrix44( &(_CAST_MAT44(g_CubeWorldViewProj[i])) );
			g_pHWGroup->AddDrawOp(g_pCubeVBGroup, g_pCubeIB, g_pCubeEffect, params, 0, RVarData());
			g_pHWGroup->Flush();
			g_pHWGroup->Reset();
		}


		// Render software
		_LOOPi(NUM_CUBES / CUBE_BATCH)
		{
			RenderSWCube(&(g_CubeWorldViewProj[i * CUBE_BATCH]));
		}

		//_LOOPi(1)
		//{
		//    RenderQuad();
		//}

		//gmtl::VecA4f pos1;
		//pos1[0] = -1.0f; pos1[1] = 1.0f; pos1[2] = 0.0f; pos1[3] = 1.0f;
		//gmtl::VecA4f pos2;
		//pos2[0] = 1.0f; pos2[1] = 1.0f; pos2[2] = 0.0f; pos2[3] = 1.0f;
		//gmtl::VecA4f pos3;
		//pos3[0] = -1.0f; pos3[1] = -1.0f; pos3[2] = 0.0f; pos3[3] = 1.0f;
		//gmtl::VecA4f pos4;
		//pos4[0] = 0.5f; pos4[1] = -0.5f; pos4[2] = 0.0f; pos4[3] = 1.0f;

		//RasterizationInfo<0, 3> rasterInfo;
		//TriangleSetup<0, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, 3>(pos3, pos2, pos1,/*pos1, pos2, pos3,*/ NULL, NULL, NULL, rasterInfo);
		//RasterizeTileLevel3<0, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, 3, &WhitePixelShader>(&rasterInfo, 0.5f, 0.5f);
		//TriangleSetup<0, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, 3>(pos2, pos3, pos4,/*pos1, pos2, pos3,*/ NULL, NULL, NULL, rasterInfo);
		//RasterizeTileLevel3<0, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, 3, &WhitePixelShader>(&rasterInfo, 0.5f, 0.5f);
		////RasterizePixelBlock<0, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, 3, &WhitePixelShader>(rasterInfo, 0.5f, 0.5f);

		uint pitch = 0;
		byte* pDest = g_pRasterizeTex->LockImmediate(0, pitch);
		_DEBUG_ASSERT(pitch == g_pRasterizeTex->GetWidth(0));
		memcpy(pDest, g_pRasterizeBuffer, sizeof(byte) * RASTERIZE_BUFFER_W * RASTERIZE_BUFFER_H);
		g_pRasterizeTex->UnlockImmediate(0);

		//RasterizationInfo<0, 3> rasterInfo2;
		//TriangleSetup<0, 1024 / 2, 768, 3>(pos2, pos3, pos4,/*pos1, pos2, pos3,*/ NULL, NULL, NULL, rasterInfo2);

		g_pSWGroup->AddDrawOp(g_pQuadVBGroup, g_pQuadIB, g_pQuadTexEffect, NULL, 0, RVarData());
		g_pSWGroup->Flush();
		g_pSWGroup->Reset();

		RenderDebugObjects();
		RenderStats();

		g_pRenderer->EndRender();
	}
}
