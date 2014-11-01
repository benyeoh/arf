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
            _mm_stream_ps(pCurDepthBuffer, ONE);
            pCurDepthBuffer += 4;
        }
    }
#endif
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

void CubeVertexTransform(const gmtl::MatrixA44f& worldViewProj, const float* pXYZ, gmtl::VecA4f* pXYZWOut)
{
	TransformVecW1((gmtl::MatrixA44f*) &worldViewProj, pXYZ, pXYZWOut);
}

void CubeVertexTransformBatchInt(const gmtl::MatrixA44f& worldViewProj, const float* pXYZ, uint inPosStride, float* pXYZWOut, uint outPosStride, uint numVertices)
{
    __m128 xmm0, xmm1, xmm2, xmm3;

    _DEBUG_ASSERT((outPosStride & 0x3) == 0);

    float* pMatData = (float*) worldViewProj.mData;
    float* pIn = (float*) pXYZ;
    float* pOut = (float*) pXYZWOut;

    __m128 xmm4 = *((__m128*)pMatData);		// col 0
    __m128 xmm5 = *((__m128*)(pMatData + 4));	// col 1
    __m128 xmm6 = *((__m128*)(pMatData + 8));	// col 2
    __m128 xmm7 = *((__m128*)(pMatData + 12));	// col 3

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

    _LOOPi(numVertices)	
    {	
        // Process 2 vertices at a time to take advantage of memory locality
        // Note that the matrix is in column major format and the vertices are in each column
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


#define _THE_PIXEL_SHADER &LinearDepthOutPixelShader
#define NUM_LAYERS 1

const static uint TOP_TILE_SIZE = 8;
const static uint MID_TILE_SIZE = 4;
const static uint NUM_TOP_TILES = _GET_NUM_TILES_DEPTH(RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, TOP_TILE_SIZE, TOP_TILE_SIZE);
const static uint NUM_MID_TILES_IN_TOP_TILE = _GET_NUM_TILES_DEPTH(TOP_TILE_SIZE, TOP_TILE_SIZE, MID_TILE_SIZE, MID_TILE_SIZE);
const static uint NUM_MID_TILES = NUM_TOP_TILES * NUM_MID_TILES_IN_TOP_TILE;
const static uint NUM_INTER = 1;
const static uint RASTER_INFO_SIZE = _GET_RASTER_INFO_SIZE_DEPTH(NUM_LAYERS);
const static int HALF_RASTERIZE_BUFFER_W = (RASTERIZE_BUFFER_W >> 1);
const static int HALF_RASTERIZE_BUFFER_H = (RASTERIZE_BUFFER_H >> 1);
const static int CUBE_BATCH = NUM_CUBES;
const static int TRIANGLE_BATCH = CUBE_BATCH * 12;

_ALIGN(16) byte g_Raster[RASTER_INFO_SIZE * TRIANGLE_BATCH];
_ALIGN(16) boolean g_RasterRes[TRIANGLE_BATCH];

template<int BUFFER_WIDTH, int BUFFER_HEIGHT>
struct RenderJob : public IPRunnable
{
	const static int JOB_NUM_TOP_TILES = _GET_NUM_TILES_DEPTH(BUFFER_WIDTH, BUFFER_HEIGHT, TOP_TILE_SIZE, TOP_TILE_SIZE);
	const static int JOB_NUM_MID_TILES = JOB_NUM_TOP_TILES * _GET_NUM_TILES_DEPTH(TOP_TILE_SIZE, TOP_TILE_SIZE, MID_TILE_SIZE, MID_TILE_SIZE);

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
                const RasterInfoDepth<1>* __restrict pRaster = (RasterInfoDepth<1>*) (g_Raster + i * RASTER_INFO_SIZE);
                const float IN_TILE_X_F         = (float) BUFFER_WIDTH - 1.0f;
                const float IN_TILE_Y_F         = (float) BUFFER_HEIGHT - 1.0f;

                float startXClamped = startX > pRaster->triBBFinal[0] ? startX : pRaster->triBBFinal[0];
                float startYClamped = startY > pRaster->triBBFinal[1] ? startY : pRaster->triBBFinal[1];
                float endX = startX + IN_TILE_X_F;
                float endY = startY + IN_TILE_Y_F;
                float endXClamped = endX < pRaster->triBBFinal[2] ? endX : pRaster->triBBFinal[2];
                float endYClamped = endY < pRaster->triBBFinal[3] ? endY : pRaster->triBBFinal[3];

                _DEBUG_COMPILE_ASSERT( ((BUFFER_WIDTH / (TOP_TILE_SIZE * 2)) & 0x1) == 0 );
                _DEBUG_COMPILE_ASSERT( ((BUFFER_HEIGHT / (TOP_TILE_SIZE * 2)) & 0x1) == 0 );
                
                float area = (endXClamped - startXClamped) * (endYClamped - startYClamped);
                if(area > (IN_TILE_X_F * IN_TILE_Y_F * 0.25f))
                {
                    startXClamped = startX > pRaster->triBB[0] ? startX : pRaster->triBB[0];
                    startYClamped = startY > pRaster->triBB[1] ? startY : pRaster->triBB[1];
                    endXClamped = endX < pRaster->triBB[2] ? endX : pRaster->triBB[2];
                    endYClamped = endY < pRaster->triBB[3] ? endY : pRaster->triBB[3];

                    TilesEdgeCheckWithBB1LayerDepth<TOP_TILE_SIZE>(g_Raster + i * RASTER_INFO_SIZE, g_pRasterizeDepthBuffer, startXClamped, startYClamped, endXClamped, endYClamped);
                }
                else
                    TilesRasterizeEdgeCheckWithBBDepth<TOP_TILE_SIZE>(g_Raster + i * RASTER_INFO_SIZE, g_pRasterizeDepthBuffer, startXClamped, startYClamped, endXClamped, endYClamped);
               
                //TilesEdgeCheckWithBB<NUM_INTER, 1, TOP_TILE_SIZE, 0, &AcceptPartial, &AcceptFull>
				//	(g_Raster + i * RASTER_INFO_SIZE, startX, startY, BUFFER_WIDTH, BUFFER_HEIGHT, &acceptTopData);


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

struct RenderJobInt : public IPRunnable
{
    int binIndex;
    int Run()
    {
        TilesRasterizeDepthUnrollXInt<BIN_WIDTH, BIN_HEIGHT, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, NUM_BIN_CONTEXTS>
            (&(g_pTriBins[binIndex]), &(g_pNumTrisInBins[binIndex]), g_pRasterizeDepthBuffer, binIndex % NUM_BINS_X, binIndex / NUM_BINS_X);

        return 0;
    }
};

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
				gmtl::VecA4f postV2;
				gmtl::VecA4f postV3;

				//double cubeStart = g_pPlatform->GetTimer().GetTime();

				CubeVertexTransform(cubeWorldViewProj, pV1XYZ, &postV1);
				CubeVertexTransform(cubeWorldViewProj, pV2XYZ, &postV2);
				CubeVertexTransform(cubeWorldViewProj, pV3XYZ, &postV3);

				//g_VertTransTimeElapsed += (float) (g_pPlatform->GetTimer().GetTime() - cubeStart); 

				byte* pCurrentRaster = pRasters + (RASTER_INFO_SIZE * i) + (RASTER_INFO_SIZE * cubeIndex * 12);
#if NUM_LAYERS == 1
				// 1 layer
                boolean res = TriangleSetupDepth1Layer<TOP_TILE_SIZE>(postV3, postV2, postV1, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, pCurrentRaster );
                res = res && TriangleBoundingBoxDepth1Layer<TOP_TILE_SIZE>(pCurrentRaster, postV3, postV2, postV1);

				//boolean res = TriangleSetup1Layer<NUM_INTER, TOP_TILE_SIZE>(postV3, postV2, postV1, v3Inter, v2Inter, v1Inter, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, pCurrentRaster );
				//res = res && TriangleBoundingBox1Layer<NUM_INTER, TOP_TILE_SIZE>(pCurrentRaster, postV3, postV2, postV1);
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

struct TransformAndSetupIntJob : public IPRunnable
{
    gmtl::MatrixA44f* pCubeWVP;
    uint numCubes;

    TriangleBin* pTriBins;
    uint* pNumTrisInBins;

    int Run()
    {
		//_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);

        _DEBUG_COMPILE_ASSERT( (RASTERIZE_BUFFER_W % BIN_WIDTH) == 0 );
        _DEBUG_COMPILE_ASSERT( (RASTERIZE_BUFFER_H % BIN_HEIGHT) == 0 );

        _ALIGN(16) float postCubeVertices[4 * NUM_CUBE_VERTICES * NUM_CUBES];
        
        memset(pNumTrisInBins, 0, NUM_BINS_Y * NUM_BINS_X * 1 * sizeof(uint));

        //gmtl::MatrixA44f viewportMat;
        //viewportMat.set(0.5f * RASTERIZE_BUFFER_W, 0.0f, 0.0f, 0.5f * RASTERIZE_BUFFER_W - 0.5f / RASTERIZE_BUFFER_W,
        //                0.0f, 0.5f * RASTERIZE_BUFFER_H, 0.0f, 0.5f * RASTERIZE_BUFFER_H - 0.5f / RASTERIZE_BUFFER_H,
        //                0.0f, 0.0f, 1.0f, 0.0f,
        //                0.0f, 0.0f, 0.0f, 1.0f);

        _LOOPi(numCubes)
        {
            const gmtl::MatrixA44f& cubeWorldViewProj = pCubeWVP[i];

            // Transform also to viewport space
            //gmtl::MatrixA44f wvpViewport;
            // MatMatMult(&wvpViewport, &viewportMat, &cubeWorldViewProj);

            CubeVertexTransformBatchInt(cubeWorldViewProj, g_CubeVertices, CUBE_VERTEX_STRIDE, &postCubeVertices[i * 4 * NUM_CUBE_VERTICES], 4, NUM_CUBE_VERTICES);
            TriangleSetupDepthIntBatch<BIN_WIDTH, BIN_HEIGHT, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H>((gmtl::VecA4f*) &(postCubeVertices[i * 4 * NUM_CUBE_VERTICES]), g_CubeIndices, 12, pTriBins, pNumTrisInBins);
        }

		//_LOOPi(numCubes)
		//{
		//	TriangleSetupDepthIntBatch<BIN_WIDTH, BIN_HEIGHT, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H>((gmtl::VecA4f*) &(postCubeVertices[i * 4 * NUM_CUBE_VERTICES]), g_CubeIndices, 12, pTriBins, pNumTrisInBins);
		//}

        return 0;
    }
};

//TransformAndSetupIntJob g_TransformAndSetupIntJobs[4];

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

void UntileDepthBuffer()
{
    __m128 farPlane = _mm_set_ps1(FAR_PLANE);
    __m128 nearPlane = _mm_set_ps1(NEAR_PLANE);
    __m128 nearMinusFar = _mm_sub_ps(nearPlane, farPlane);

    //memcpy(g_pRasterizeBuffer, g_pRasterizeDepthBuffer, RASTERIZE_BUFFER_W * RASTERIZE_BUFFER_H * sizeof(float));

    _LOOPi(RASTERIZE_BUFFER_H >> 1)
    {
        int row = i * 2;
        
        _LOOPj(RASTERIZE_BUFFER_W >> 2)
        {
            int col = j * 2;
            __m128 quad1 = _mm_load_ps(g_pRasterizeDepthBuffer + row * RASTERIZE_BUFFER_W + (col << 2));
            __m128 quad2 = _mm_load_ps(g_pRasterizeDepthBuffer + row * RASTERIZE_BUFFER_W + ((col+1) << 2));

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

            _mm_store_ps(g_pRasterizeBuffer + row * RASTERIZE_BUFFER_W + (j << 2), line1);
            _mm_store_ps(g_pRasterizeBuffer + (row+1) * RASTERIZE_BUFFER_W + (j << 2), line2);
        }
    }
}

void RenderSWCube(const gmtl::MatrixA44f* pCubeWorldViewProj)
{
	int numPerJob = NUM_CUBES / 4;
    _DEBUG_ASSERT((NUM_CUBES % 4) == 0);
	//_DEBUG_COMPILE_ASSERT((CUBE_BATCH % 4) == 0);
	double swStart = g_pPlatform->GetTimer().GetTime();

	//g_pThreadPool->SetAlwaysActive(TRUE);

	_LOOPi(4)
	{
		g_TransformAndSetupJobs[i].pCubeProjs = (pCubeWorldViewProj + i * numPerJob);
		g_TransformAndSetupJobs[i].pRasters = g_Raster + (i * numPerJob * RASTER_INFO_SIZE * 12);
		g_TransformAndSetupJobs[i].pRasterRes = g_RasterRes + (i * numPerJob * 12);
		g_TransformAndSetupJobs[i].numCubes = numPerJob;
	
		//g_TransformAndSetupJobs[i].Run();
		g_pThreadPool->QueueJob(g_TransformAndSetupJobs[i]);
	}

	g_pThreadPool->WaitUntilFinished();

	g_pThreadPool->QueueJob(g_RenderJob1);
	g_pThreadPool->QueueJob(g_RenderJob2);
	g_pThreadPool->QueueJob(g_RenderJob3);
	g_pThreadPool->QueueJob(g_RenderJob4);

	//g_pThreadPool->SetAlwaysActive(FALSE);

	g_pThreadPool->WaitUntilFinished();

	g_SWTimeElapsed += (g_pPlatform->GetTimer().GetTime() - swStart);
    g_SWTimeElapsedPrevious[g_CurSWTimeIndex % NUM_AVG_TIMES] = g_SWTimeElapsed;
    g_CurSWTimeIndex++;

	//g_SWTimeElapsed += (g_RenderJob1.end - g_RenderJob1.start);

    UntileDepthBuffer();
}

void RenderSWCubesST()
{
	//_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
   
    double swStart = g_pPlatform->GetTimer().GetTime();

    _LOOPi(NUM_CUBES)
    {
        const gmtl::MatrixA44f& cubeWorldViewProj = g_CubeWorldViewProj[i];
        
        const static uint TOP_TILE_SIZE = 16;
        const static uint NUM_TOP_TILES = _GET_NUM_TILES_DEPTH(RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, TOP_TILE_SIZE, TOP_TILE_SIZE);

        _ALIGN(16) byte raster[_GET_RASTER_INFO_SIZE_DEPTH(1)];

        _LOOPj(12)
        {
            uint i1 = g_CubeIndices[j*3];
            uint i2 = g_CubeIndices[j*3+1];
            uint i3 = g_CubeIndices[j*3+2];

            const float* pV1XYZ = g_CubeVertices + i1 * CUBE_VERTEX_STRIDE;
            const float* pV2XYZ = g_CubeVertices + i2 * CUBE_VERTEX_STRIDE;
            const float* pV3XYZ = g_CubeVertices + i3 * CUBE_VERTEX_STRIDE;

            gmtl::VecA4f postV1;
            gmtl::VecA4f postV2;
            gmtl::VecA4f postV3;

            //double cubeStart = g_pPlatform->GetTimer().GetTime();

            CubeVertexTransform(cubeWorldViewProj, pV1XYZ, &postV1);
            CubeVertexTransform(cubeWorldViewProj, pV2XYZ, &postV2);
            CubeVertexTransform(cubeWorldViewProj, pV3XYZ, &postV3);

            if(TriangleSetupDepth1Layer<TOP_TILE_SIZE>(postV3, postV2, postV1, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, raster))
            {
                if(TriangleBoundingBoxDepth1Layer<TOP_TILE_SIZE>(raster, postV3, postV2, postV1))
                {
                    const RasterInfoDepth<1>* __restrict pRaster = (RasterInfoDepth<1>*) (raster);
                    const float IN_TILE_X_F         = (float) RASTERIZE_BUFFER_W - 1.0f;
                    const float IN_TILE_Y_F         = (float) RASTERIZE_BUFFER_H - 1.0f;

                    float startXClamped = 0.5f > pRaster->triBBFinal[0] ? 0.5f : pRaster->triBBFinal[0];
                    float startYClamped = 0.5f > pRaster->triBBFinal[1] ? 0.5f : pRaster->triBBFinal[1];
                    float endX = 0.5f + IN_TILE_X_F;
                    float endY = 0.5f + IN_TILE_Y_F;
                    float endXClamped = endX < pRaster->triBBFinal[2] ? endX : pRaster->triBBFinal[2];
                    float endYClamped = endY < pRaster->triBBFinal[3] ? endY : pRaster->triBBFinal[3];

                    _DEBUG_COMPILE_ASSERT( ((RASTERIZE_BUFFER_W / (TOP_TILE_SIZE * 2)) & 0x1) == 0 );
                    _DEBUG_COMPILE_ASSERT( ((RASTERIZE_BUFFER_H / (TOP_TILE_SIZE * 2)) & 0x1) == 0 );

                    TilesRasterizeEdgeCheckWithBBDepth<TOP_TILE_SIZE>(raster, g_pRasterizeDepthBuffer, startXClamped, startYClamped, endXClamped, endYClamped);

                    //TilesEdgeCheckWithBB1LayerDepth<TOP_TILE_SIZE>(raster, g_pRasterizeDepthBuffer, 0.5f, 0.5f, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H);
                }
            }
        }
    }

    g_SWTimeElapsed += (g_pPlatform->GetTimer().GetTime() - swStart);
    g_SWTimeElapsedPrevious[g_CurSWTimeIndex % NUM_AVG_TIMES] = g_SWTimeElapsed;
    g_CurSWTimeIndex++;

    UntileDepthBuffer();
}

void RenderSWCubeInt()
{
    _DEBUG_COMPILE_ASSERT((NUM_BIN_CONTEXTS >= (NUM_THREADS)));

    int numPerJob = NUM_CUBES / NUM_BIN_CONTEXTS;
    _DEBUG_COMPILE_ASSERT((NUM_CUBES % NUM_BIN_CONTEXTS) == 0);
   
   // g_pThreadPool->SetAlwaysActive(TRUE);
    TransformAndSetupIntJob transformJobs[NUM_BIN_CONTEXTS];

    double swStart = g_pPlatform->GetTimer().GetTime();
	IPRunnable* pRunnables[NUM_BIN_CONTEXTS];
    _LOOPi(NUM_BIN_CONTEXTS)
    {
        transformJobs[i].pCubeWVP = &(g_CubeWorldViewProj[i*numPerJob]);
        transformJobs[i].numCubes = numPerJob;
        transformJobs[i].pTriBins = g_pTriBins + i * NUM_BINS_X * NUM_BINS_Y;
        transformJobs[i].pNumTrisInBins = g_pNumTrisInBins + i * NUM_BINS_X * NUM_BINS_Y;
      
		pRunnables[i] = &transformJobs[i];
    }

	g_pThreadPool->QueueJobs(pRunnables, NUM_BIN_CONTEXTS);
    g_pThreadPool->WaitUntilFinished();

    // Sort bins in order of most triangles to least
    uint binIndices[NUM_BINS_X * NUM_BINS_Y];
    uint totalNumTrianglesBins[NUM_BINS_X * NUM_BINS_Y];
    uint numBins = NUM_BINS_Y * NUM_BINS_X;

    _LOOPi(NUM_BINS_Y)
    {
        _LOOPj(NUM_BINS_X)
        {
            binIndices[i * NUM_BINS_X + j] = i * NUM_BINS_X + j;

            uint totalThisBin = 0;
            _LOOPk(NUM_BIN_CONTEXTS)
            {
                totalThisBin += g_pNumTrisInBins[NUM_BINS_X * NUM_BINS_Y * k + i * NUM_BINS_X + j];
            }

            //if(totalThisBin == 0)
            //    numBins--;

            totalNumTrianglesBins[i * NUM_BINS_X + j] = totalThisBin;
        }
    }

    std::sort(binIndices, binIndices + NUM_BINS_X * NUM_BINS_Y, [&](const uint a, const uint b){ return totalNumTrianglesBins[a] > totalNumTrianglesBins[b]; });

    RenderJobInt rasterJobs[NUM_BINS_Y * NUM_BINS_X];
	IPRunnable* pRasterRunnables[NUM_BINS_X * NUM_BINS_Y];
	_DEBUG_ASSERT(numBins <= (NUM_BINS_Y * NUM_BINS_X));

    // Render all bins
    _LOOPi(numBins)
    {
        rasterJobs[i].binIndex = binIndices[i];
		pRasterRunnables[i] = &rasterJobs[i];
    }

	g_pThreadPool->QueueJobs(pRasterRunnables, numBins);
    g_pThreadPool->WaitUntilFinished();

    g_SWTimeElapsed += (g_pPlatform->GetTimer().GetTime() - swStart);
    g_SWTimeElapsedPrevious[g_CurSWTimeIndex % NUM_AVG_TIMES] = g_SWTimeElapsed;
    g_CurSWTimeIndex++;

    //g_pThreadPool->SetAlwaysActive(FALSE);
  //g_SWTimeElapsed += (g_RenderJob1.end - g_RenderJob1.start);

    UntileDepthBuffer();
}

void RenderSWCubesIntST()
{
   // _mm_setcsr( _mm_getcsr() | 0x8040 );

    _DEBUG_COMPILE_ASSERT( (RASTERIZE_BUFFER_W % BIN_WIDTH) == 0 );
    _DEBUG_COMPILE_ASSERT( (RASTERIZE_BUFFER_H % BIN_HEIGHT) == 0 );

    _ALIGN(16) float postCubeVertices[4 * NUM_CUBE_VERTICES * NUM_CUBES];
    double swStart = g_pPlatform->GetTimer().GetTime();

    memset(g_pNumTrisInBins, 0, NUM_BINS_Y * NUM_BINS_X * 1 * sizeof(uint));

    //gmtl::MatrixA44f viewportMat;
    //viewportMat.set(0.5f * RASTERIZE_BUFFER_W, 0.0f, 0.0f, 0.5f * RASTERIZE_BUFFER_W - 0.5f / RASTERIZE_BUFFER_W,
    //                0.0f, 0.5f * RASTERIZE_BUFFER_H, 0.0f, 0.5f * RASTERIZE_BUFFER_H - 0.5f / RASTERIZE_BUFFER_H,
    //                0.0f, 0.0f, 1.0f, 0.0f,
    //                0.0f, 0.0f, 0.0f, 1.0f);
                    
    _LOOPi(NUM_CUBES)
    {
        const gmtl::MatrixA44f& cubeWorldViewProj = g_CubeWorldViewProj[i];

        // Transform also to viewport space
        //gmtl::MatrixA44f wvpViewport;
       // MatMatMult(&wvpViewport, &viewportMat, &cubeWorldViewProj);

        CubeVertexTransformBatchInt(cubeWorldViewProj, g_CubeVertices, CUBE_VERTEX_STRIDE, &postCubeVertices[i * 4 * NUM_CUBE_VERTICES], 4, NUM_CUBE_VERTICES);
        TriangleSetupDepthIntBatch<BIN_WIDTH, BIN_HEIGHT, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H>((gmtl::VecA4f*) &(postCubeVertices[i * 4 * NUM_CUBE_VERTICES]), g_CubeIndices, 12, g_pTriBins, g_pNumTrisInBins);
    }

    g_NumTriangles = 0;

    _LOOPi(NUM_BINS_Y)
    {
        _LOOPj(NUM_BINS_X)
        {
            TilesRasterizeDepthUnrollXInt<BIN_WIDTH, BIN_HEIGHT, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, 1>(&(g_pTriBins[i*NUM_BINS_X+j]), &(g_pNumTrisInBins[i*NUM_BINS_X+j]), g_pRasterizeDepthBuffer, j, i);
            g_NumTriangles += g_pNumTrisInBins[i*NUM_BINS_X+j];
        }
    }

    g_SWTimeElapsed += (g_pPlatform->GetTimer().GetTime() - swStart);
    g_SWTimeElapsedPrevious[g_CurSWTimeIndex % NUM_AVG_TIMES] = g_SWTimeElapsed;
    g_CurSWTimeIndex++;

    UntileDepthBuffer();
}

void RenderQuad()
{
	gmtl::VecA4f pos1;
	pos1[0] = -1.0f; pos1[1] = 1.0f; pos1[2] = 0.997f; pos1[3] = 1.0f;
	gmtl::VecA4f pos2;
	pos2[0] = 1.0f; pos2[1] = 1.0f; pos2[2] = 0.997f; pos2[3] = 1.0f;
	gmtl::VecA4f pos3;
	pos3[0] = -1.0f; pos3[1] = -1.0f; pos3[2] = 0.997f; pos3[3] = 1.0f;
	gmtl::VecA4f pos4;
	pos4[0] = 1.0f; pos4[1] = -1.0f; pos4[2] = 0.997f; pos4[3] = 1.0f;
	
	double timeStart = g_pPlatform->GetTimer().GetTime();

	const static uint TOP_TILE_SIZE = 8;
	const static uint NUM_TOP_TILES = _GET_NUM_TILES_DEPTH(RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, TOP_TILE_SIZE, TOP_TILE_SIZE);
	
	_ALIGN(16) byte raster[_GET_RASTER_INFO_SIZE_DEPTH(1)];

    if(TriangleSetupDepth1Layer<TOP_TILE_SIZE>(pos3, pos2, pos1, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, raster))
    {
        if(TriangleBoundingBoxDepth1Layer<TOP_TILE_SIZE>(raster, pos3, pos2, pos1))
        {
            //TilesRasterizeEdgeCheckWithBBDepth<TOP_TILE_SIZE>(raster, g_pRasterizeDepthBuffer, 0.5f, 0.5f, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H);

            //TilesEdgeCheckWithBB1LayerDepth<TOP_TILE_SIZE>(raster, g_pRasterizeDepthBuffer, 0.5f, 0.5f, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H);
        }
    }

    if(TriangleSetupDepth1Layer<TOP_TILE_SIZE>(pos3, pos4, pos2, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, raster))
    {
        if(TriangleBoundingBoxDepth1Layer<TOP_TILE_SIZE>(raster, pos3, pos4, pos2))
        {
           // TilesRasterizeEdgeCheckWithBBDepth<TOP_TILE_SIZE>(raster, g_pRasterizeDepthBuffer, 0.5f, 0.5f, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H);

            //TilesEdgeCheckWithBB1LayerDepth<TOP_TILE_SIZE>(raster, g_pRasterizeDepthBuffer, 0.5f, 0.5f, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H);
        }
    }

	g_SWQuadTimeElapsed +=  (float) (g_pPlatform->GetTimer().GetTime() - timeStart);

    UntileDepthBuffer();
}

void RenderQuadInt()
{
	_DEBUG_COMPILE_ASSERT( (RASTERIZE_BUFFER_W % BIN_WIDTH) == 0 );
	_DEBUG_COMPILE_ASSERT( (RASTERIZE_BUFFER_H % BIN_HEIGHT) == 0 );

	double swStart = g_pPlatform->GetTimer().GetTime();

	memset(g_pNumTrisInBins, 0, NUM_BINS_Y * NUM_BINS_X * 1 * sizeof(uint));

	const static float PLANE_SIZE = 120.00f;

    gmtl::VecA4f pos[4];
    pos[0][0] = -PLANE_SIZE; pos[0][1] = -5.0f; pos[0][2] = -PLANE_SIZE; pos[0][3] = 1.0f;
    pos[1][0] = PLANE_SIZE; pos[1][1] = -5.0f; pos[1][2] = -PLANE_SIZE; pos[1][3] = 1.0f;
 	pos[2][0] = PLANE_SIZE; pos[2][1] = -5.0f; pos[2][2] = PLANE_SIZE; pos[2][3] = 1.0f;
    pos[3][0] = -PLANE_SIZE; pos[3][1] = -5.0f; pos[3][2] = PLANE_SIZE; pos[3][3] = 1.0f;

	//gmtl::VecA4f pos[4];
	//pos[0][0] = -1.0f; pos[0][1] = 1.0f; pos[0][2] = 0.9f; pos[0][3] = 1.0f;
	//pos[1][0] = 1.0f; pos[1][1] = 1.0f; pos[1][2] = 0.9f; pos[1][3] = 1.0f;
	//pos[2][0] = 1.0f; pos[2][1] = -1.0f; pos[2][2] = 0.9f; pos[2][3] = 1.0f;
	//pos[3][0] = -1.0f; pos[3][1] = -1.0f; pos[3][2] = 0.9f; pos[3][3] = 1.0f;	

	ushort indices[6] = { 0, 1, 2, 0, 2, 3 };

    //_DEBUG_COMPILE_ASSERT( (RASTERIZE_BUFFER_W % BIN_WIDTH) == 0 );
    //_DEBUG_COMPILE_ASSERT( (RASTERIZE_BUFFER_H % BIN_HEIGHT) == 0 );

    //_ALIGN(16) float postQuadVertices[4 * 4];
    //double swStart = g_pPlatform->GetTimer().GetTime();

    //memset(g_pNumTrisInBins, 0, NUM_BINS_Y * NUM_BINS_X * 1 * sizeof(uint));

    ////gmtl::MatrixA44f viewportMat;
    ////viewportMat.set(0.5f * RASTERIZE_BUFFER_W, 0.0f, 0.0f, 0.5f * RASTERIZE_BUFFER_W - 0.5f / RASTERIZE_BUFFER_W,
    ////    0.0f, 0.5f * RASTERIZE_BUFFER_H, 0.0f, 0.5f * RASTERIZE_BUFFER_H - 0.5f / RASTERIZE_BUFFER_H,
    ////    0.0f, 0.0f, 1.0f, 0.0f,
    ////    0.0f, 0.0f, 0.0f, 1.0f);

    ////MatMatMult(&wvpViewport, &viewportMat, &cubeWorldViewProj);

	gmtl::MatrixA44f viewProj;
	MatMatMult(&viewProj, &g_Proj, &g_View);
	TransformVecW1(&viewProj, &pos[0], &pos[0]);
	TransformVecW1(&viewProj, &pos[1], &pos[1]);
	TransformVecW1(&viewProj, &pos[2], &pos[2]);
	TransformVecW1(&viewProj, &pos[3], &pos[3]);

    TriangleSetupDepthIntBatch<BIN_WIDTH, BIN_HEIGHT, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H>((gmtl::VecA4f*) &(pos[0]), indices, 2, g_pTriBins, g_pNumTrisInBins);

	g_NumTriangles = 0;

	_LOOPi(NUM_BINS_Y)
	{
		_LOOPj(NUM_BINS_X)
		{
			TilesRasterizeDepthUnrollXInt<BIN_WIDTH, BIN_HEIGHT, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, 1>(&(g_pTriBins[i*NUM_BINS_X+j]), &(g_pNumTrisInBins[i*NUM_BINS_X+j]), g_pRasterizeDepthBuffer, j, i);
			g_NumTriangles += g_pNumTrisInBins[i*NUM_BINS_X+j];
		}
	}

	g_SWTimeElapsed += (g_pPlatform->GetTimer().GetTime() - swStart);
	g_SWTimeElapsedPrevious[g_CurSWTimeIndex % NUM_AVG_TIMES] = g_SWTimeElapsed;
	g_CurSWTimeIndex++;

	UntileDepthBuffer();
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
    double swTimeElapsed = 0.0;
    _LOOPi(NUM_AVG_TIMES)
    {
        swTimeElapsed += g_SWTimeElapsedPrevious[i];
    }

    swTimeElapsed /= NUM_AVG_TIMES;
	g_pCourierFontFace->RenderString(ToStringAuto(_W("Software Raster Time: %f"), swTimeElapsed * 1000.0), fontPos, 0xFFF0207F);
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
       // RenderSWCube(&(g_CubeWorldViewProj[0]));
        //RenderSWCubesST();
        RenderSWCubeInt();
       // RenderSWCubesIntST();
        //RenderQuadInt();
		//RenderQuad();
        
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
		_DEBUG_ASSERT(pitch == g_pRasterizeTex->GetWidth(0) * sizeof(float));
		memcpy(pDest, g_pRasterizeBuffer, sizeof(float) * RASTERIZE_BUFFER_W * RASTERIZE_BUFFER_H);
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
