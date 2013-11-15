

#include "stdafx.h"
#include "BakedSMParaboloidProjWeights.h"

union FP32
{
    uint u;
    float f;
    struct
    {
        uint Mantissa : 23;
        uint Exponent : 8;
        uint Sign : 1;
    };
};

union FP16
{
    unsigned short u;
    struct
    {
        uint Mantissa : 10;
        uint Exponent : 5;
        uint Sign : 1;
    };
};

static ushort float_to_half_fast3(float fval)
{
    FP32 f;
    f.f = fval;

    FP32 f32infty = { 255 << 23 };
    FP32 f16infty = { 31 << 23 };
    FP32 magic = { 15 << 23 };
    uint sign_mask = 0x80000000u;
    uint round_mask = ~0xfffu; 
    FP16 o = { 0 };

    uint sign = f.u & sign_mask;
    f.u ^= sign;

    // NOTE all the integer compares in this function can be safely
    // compiled into signed compares since all operands are below
    // 0x80000000. Important if you want fast straight SSE2 code
    // (since there's no unsigned PCMPGTD).

    if (f.u >= f32infty.u) // Inf or NaN (all exponent bits set)
        o.u = (f.u > f32infty.u) ? 0x7e00 : 0x7c00; // NaN->qNaN and Inf->Inf
    else // (De)normalized number or zero
    {
        f.u &= round_mask;
        f.f *= magic.f;
        f.u -= round_mask;
        if (f.u > f16infty.u) f.u = f16infty.u; // Clamp to signed infinity if overflowed

        o.u = f.u >> 13; // Take the bits!
    }

    o.u |= sign >> 16;
    return o.u;
}


struct AcceptData
{
    float* pFullData;
    int numFull;

    float* pPartialData;
    int numPartial;
};

struct PixelShaderData
{
    BakedSMLocationEntry* pEntry;
    uint numEntries;
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

inline void AddToEntryShader(int pixelBlockX, int pixelBlockY, __m128 coverage, __m128 depthVals, __m128* interVals, void* pContext)
{
    PixelShaderData* pShaderData = (PixelShaderData*) pContext;

    int coverageMask = _mm_movemask_ps(coverage);
    float interValStore[6 * 4];
    _mm_store_ps(&interValStore[0], interVals[0]);
    _mm_store_ps(&interValStore[1 * 4], interVals[1]);
    _mm_store_ps(&interValStore[2 * 4], interVals[2]);
    _mm_store_ps(&interValStore[3 * 4], interVals[3]);
    _mm_store_ps(&interValStore[4 * 4], interVals[4]);
    _mm_store_ps(&interValStore[5 * 4], interVals[5]);

    if(coverageMask & 0x1)
    {
        pShaderData->pEntry[pShaderData->numEntries].pos.set(interValStore[0], interValStore[1 * 4], interValStore[2 * 4]);
        pShaderData->pEntry[pShaderData->numEntries].normal.set(interValStore[3 * 4], interValStore[4 * 4], interValStore[5 * 4]);
        pShaderData->pEntry[pShaderData->numEntries].texX = pixelBlockX;
        pShaderData->pEntry[pShaderData->numEntries].texY = pixelBlockY;
        pShaderData->numEntries++;
    }

    if(coverageMask & 0x2)
    {
        pShaderData->pEntry[pShaderData->numEntries].pos.set(interValStore[0 + 1], interValStore[1 * 4 + 1], interValStore[2 * 4 + 1]);
        pShaderData->pEntry[pShaderData->numEntries].normal.set(interValStore[3 * 4 + 1], interValStore[4 * 4 + 1], interValStore[5 * 4 + 1]);
        pShaderData->pEntry[pShaderData->numEntries].texX = pixelBlockX + 1;
        pShaderData->pEntry[pShaderData->numEntries].texY = pixelBlockY;
        pShaderData->numEntries++;
    }

    if(coverageMask & 0x4)
    {
        pShaderData->pEntry[pShaderData->numEntries].pos.set(interValStore[0 + 2], interValStore[1 * 4 + 2], interValStore[2 * 4 + 2]);
        pShaderData->pEntry[pShaderData->numEntries].normal.set(interValStore[3 * 4 + 2], interValStore[4 * 4 + 2], interValStore[5 * 4 + 2]);
        pShaderData->pEntry[pShaderData->numEntries].texX = pixelBlockX;
        pShaderData->pEntry[pShaderData->numEntries].texY = pixelBlockY + 1;
        pShaderData->numEntries++;
    }

    if(coverageMask & 0x8)
    {
        pShaderData->pEntry[pShaderData->numEntries].pos.set(interValStore[0 + 3], interValStore[1 * 4 + 3], interValStore[2 * 4 + 3]);
        pShaderData->pEntry[pShaderData->numEntries].normal.set(interValStore[3 * 4 + 3], interValStore[4 * 4 + 3], interValStore[5 * 4 + 3]);
        pShaderData->pEntry[pShaderData->numEntries].texX = pixelBlockX + 1;
        pShaderData->pEntry[pShaderData->numEntries].texY = pixelBlockY + 1;
        pShaderData->numEntries++;
    }

}

void BakedSMCompute::Initialize(IRRenderer* pRenderer, IAppCallback* pCallback)
{
    m_pRenderer = pRenderer;
    pCallback = pCallback;
}

BakedSMComputeParamPool* BakedSMCompute::GetBakedSMComputeParamPool()
{
    return &m_ParamPool;
}

uint BakedSMCompute::DoComputeUVLocEntries(BakedSMLocationEntry* pLocEntries, const gmtl::VecA4f& v1, const gmtl::VecA4f& v2, const gmtl::VecA4f& v3, 
                                           const float* pV1Inter, const float* pV2Inter, const float* pV3Inter, uint texWidth, uint texHeight)
{
    const static uint TOP_TILE_SIZE = 32;
    const static uint MID_TILE_SIZE = 8;
    const static uint NUM_INTER = 6;
    const static uint RASTER_INFO_SIZE = _GET_RASTER_INFO_SIZE(NUM_INTER, 2);

    uint numTopTiles = _GET_NUM_TILES(texWidth, texHeight, TOP_TILE_SIZE, TOP_TILE_SIZE);
    uint numMidTiles = numTopTiles * _GET_NUM_TILES(TOP_TILE_SIZE, TOP_TILE_SIZE, MID_TILE_SIZE, MID_TILE_SIZE);
    
    _ALIGN(16) byte raster[RASTER_INFO_SIZE];
    //_TRACE(_W("Rastersize: %d\n"), _GET_RASTER_INFO_SIZE(0));

    _DEBUG_ASSERT(numTopTiles < 2048);

    byte topTileFull[ _GET_TILE_BUFFER_SIZE( 2048 ) ];
    byte topTilePartial[ _GET_TILE_BUFFER_SIZE( 2048 ) ];
    byte midTileFull[ _GET_TILE_BUFFER_SIZE( 2048 * (TOP_TILE_SIZE / MID_TILE_SIZE) * (TOP_TILE_SIZE / MID_TILE_SIZE) ) ];
    byte midTilePartial[ _GET_TILE_BUFFER_SIZE( 2048 * (TOP_TILE_SIZE / MID_TILE_SIZE) * (TOP_TILE_SIZE / MID_TILE_SIZE) ) ];

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

    if(TriangleSetup2Layer<NUM_INTER, TOP_TILE_SIZE, MID_TILE_SIZE>(v1, v2, v3, pV1Inter, pV2Inter, pV3Inter, texWidth, texHeight, raster))
    {
        if(TriangleBoundingBox2Layer<NUM_INTER, TOP_TILE_SIZE, MID_TILE_SIZE>(raster, v1, v2, v3))
        {
            TilesEdgeCheckWithBB<NUM_INTER, 2, TOP_TILE_SIZE, 0, &AcceptPartial, &AcceptFull>
                (raster, 0.5f, 0.5f, texWidth, texHeight, &acceptTopData);

            for(int tileIndex=0; tileIndex < acceptTopData.numPartial; tileIndex++)
            {
                //TilesEdgeCheck2x2<NUM_INTER, 2, MID_TILE_SIZE, 1, &AcceptPartial, &AcceptFull>
                TilesEdgeCheck<NUM_INTER, 2, TOP_TILE_SIZE, TOP_TILE_SIZE, MID_TILE_SIZE, 1, &AcceptPartial, &AcceptFull>
                    (raster, acceptTopData.pPartialData[tileIndex << 1], acceptTopData.pPartialData[(tileIndex << 1) + 1], &acceptMidData);
            }

            PixelShaderData shaderData;
            shaderData.pEntry = pLocEntries;
            shaderData.numEntries = 0;
            
            for(int tileIndex=0; tileIndex < acceptMidData.numPartial; tileIndex++)
            {
                TileRasterizeEdgeCheck<NUM_INTER, 2, MID_TILE_SIZE, &AddToEntryShader>
                    (raster, acceptMidData.pPartialData[tileIndex << 1], acceptMidData.pPartialData[(tileIndex << 1) + 1], &shaderData);
            }

            for(int tileIndex=0; tileIndex < acceptTopData.numFull; tileIndex++)
            {	
                TileRasterizeNoEdgeCheck<NUM_INTER, 2, TOP_TILE_SIZE, &AddToEntryShader>
                    (raster, acceptTopData.pFullData[tileIndex << 1], acceptTopData.pFullData[(tileIndex << 1) + 1], &shaderData);
            }

            for(int tileIndex=0; tileIndex < acceptMidData.numFull; tileIndex++)
            {
                TileRasterizeNoEdgeCheck<NUM_INTER, 2, MID_TILE_SIZE, &AddToEntryShader>
                    (raster, acceptMidData.pFullData[tileIndex << 1], acceptMidData.pFullData[(tileIndex << 1) + 1], &shaderData);
            }

            return shaderData.numEntries;
        }
    }

    return 0;
}

uint BakedSMCompute::ComputeUVLocEntries(BakedSMLocationEntry* pLocEntries, const gmtl::MatrixA44f& worldTrans, 
										void* pPos, uint posStride, void* pNormal, uint normalStride,
										void* pUV, uint uvStride, ushort* pIndices, uint numTri, uint texWidth, uint texHeight)
{
	uint numEntries = 0;

	_LOOPi(numTri)
	{
		uint i1 = (uint) pIndices[i*3];
		uint i2 = (uint) pIndices[i*3+1];
		uint i3 = (uint) pIndices[i*3+2];

		gmtl::VecA4f pos1;
		gmtl::VecA4f pos2;
		gmtl::VecA4f pos3;

		// Get the triangle pos to rasterize (which is the uv)
		// TODO: Remove determinant checks for back faces
		gmtl::Vec2f& uv1 = *( (gmtl::Vec2f*) ( ((byte*) pUV) + uvStride * i1 ) );
		gmtl::Vec2f& uv2 = *( (gmtl::Vec2f*) ( ((byte*) pUV) + uvStride * i2 ) );
		gmtl::Vec2f& uv3 = *( (gmtl::Vec2f*) ( ((byte*) pUV) + uvStride * i3 ) );

		pos1[0] = uv1[0] * 2.0f - 1.0f;
		pos1[1] = 1.0f - uv1[1] * 2.0f;
		pos1[2] = 0.0f;
		pos1[3] = 1.0f;

		pos2[0] = uv2[0] * 2.0f - 1.0f;
		pos2[1] = 1.0f - uv2[1] * 2.0f;
		pos2[2] = 0.0f;
		pos2[3] = 1.0f;

		pos3[0] = uv3[0] * 2.0f - 1.0f;
		pos3[1] = 1.0f - uv3[1] * 2.0f;
		pos3[2] = 0.0f;
		pos3[3] = 1.0f;

		// Get the interpolants (which is the world space position/normals)
		gmtl::Vec3f* pPos1 = (gmtl::Vec3f*) ( ((byte*) pPos) + posStride * i1 );
		gmtl::Vec3f* pPos2 = (gmtl::Vec3f*) ( ((byte*) pPos) + posStride * i2 );
		gmtl::Vec3f* pPos3 = (gmtl::Vec3f*) ( ((byte*) pPos) + posStride * i3 );

		gmtl::VecA3f worldPosInter[3];
		worldPosInter[0][0] = (*pPos1)[0];
		worldPosInter[0][1] = (*pPos1)[1];
		worldPosInter[0][2] = (*pPos1)[2];
		worldPosInter[1][0] = (*pPos2)[0];
		worldPosInter[1][1] = (*pPos2)[1];
		worldPosInter[1][2] = (*pPos2)[2];
		worldPosInter[2][0] = (*pPos3)[0];
		worldPosInter[2][1] = (*pPos3)[1];
		worldPosInter[2][2] = (*pPos3)[2];

		gmtl::Vec3f* pNormal1 = (gmtl::Vec3f*) ( ((byte*) pNormal) + normalStride * i1 );
		gmtl::Vec3f* pNormal2 = (gmtl::Vec3f*) ( ((byte*) pNormal) + normalStride * i2 );
		gmtl::Vec3f* pNormal3 = (gmtl::Vec3f*) ( ((byte*) pNormal) + normalStride * i3 );

		gmtl::VecA3f worldNormalInter[3];
		worldNormalInter[0][0] = (*pNormal1)[0];
		worldNormalInter[0][1] = (*pNormal1)[1];
		worldNormalInter[0][2] = (*pNormal1)[2];
		worldNormalInter[1][0] = (*pNormal2)[0];
		worldNormalInter[1][1] = (*pNormal2)[1];
		worldNormalInter[1][2] = (*pNormal2)[2];
		worldNormalInter[2][0] = (*pNormal3)[0];
		worldNormalInter[2][1] = (*pNormal3)[1];
		worldNormalInter[2][2] = (*pNormal3)[2];

		BatchTransformVecW1(&worldTrans, worldPosInter, worldPosInter, 3);
		BatchTransformVec(&worldTrans, worldNormalInter, worldNormalInter, 3);

		float interData1[6];
		interData1[0] = worldPosInter[0][0];
		interData1[1] = worldPosInter[0][1];
		interData1[2] = worldPosInter[0][2];
		interData1[3] = worldNormalInter[0][0];
		interData1[4] = worldNormalInter[0][1];
		interData1[5] = worldNormalInter[0][2];

		float interData2[6];
		interData2[0] = worldPosInter[1][0];
		interData2[1] = worldPosInter[1][1];
		interData2[2] = worldPosInter[1][2];
		interData2[3] = worldNormalInter[1][0];
		interData2[4] = worldNormalInter[1][1];
		interData2[5] = worldNormalInter[1][2];

		float interData3[6];
		interData3[0] = worldPosInter[2][0];
		interData3[1] = worldPosInter[2][1];
		interData3[2] = worldPosInter[2][2];
		interData3[3] = worldNormalInter[2][0];
		interData3[4] = worldNormalInter[2][1];
		interData3[5] = worldNormalInter[2][2];

		numEntries += DoComputeUVLocEntries(&(pLocEntries[numEntries]), pos1, pos2, pos3, interData1, interData2, interData3, texWidth, texHeight);
	}

	return numEntries;
} 

struct ComputeSMTexToSHJob : public IPRunnable
{
    BakedSMCompute* pOwner;
    ObjectPoolAtomic<ComputeSMTexToSHJob, 4>* pOwnerPool;

    BakedSMSHEntry* pData;
    gmtl::Matrix44f paraView;
    float texData[BAKED_SM_COMPUTE_SM_RT_SIZE * BAKED_SM_COMPUTE_SM_RT_SIZE];

    int Run()
    {
        pOwner->CompressSMTexDataToSH(texData, pData, paraView);
        pOwnerPool->Dispose(this);

        return 0;
    }
};

void BakedSMCompute::CompressSMTexDataToSH(float* pParaTexData, BakedSMSHEntry* pDataEntryDest, const gmtl::Matrix44f& paraView)
{
    // TODO: Perform least squares projection to the hemisphere instead
    const float SM_RT_SIZE_RCP =  1.0f / BAKED_SM_COMPUTE_SM_RT_SIZE;

    gmtl::MatrixA44f invParaViewRot;
    invParaViewRot.set( paraView[0][0], paraView[1][0], paraView[2][0],
        paraView[0][1], paraView[1][1], paraView[2][1],
        paraView[0][2], paraView[1][2], paraView[2][2] );

    // Zero SH
    BakedSMSHEntry depth;
    
    _LOOPi(BAKED_SM_NUM_SH_BANDS * BAKED_SM_NUM_SH_BANDS)
    {
        depth.sh[i] = 0.0f;
    }

    _LOOPi(BAKED_SM_COMPUTE_SM_RT_SIZE)
    {
        _LOOPj(BAKED_SM_COMPUTE_SM_RT_SIZE)
        {
            uint index = i * BAKED_SM_COMPUTE_SM_RT_SIZE + j;			

            // Derive local hemisphere direction from tex coord of the paraboloid projection
            float u = ((j + 0.5f) * SM_RT_SIZE_RCP * 2.0f) - 1.0f;
            float v = ((1.0f - (i + 0.5f) * SM_RT_SIZE_RCP) * 2.0f) - 1.0f;

            gmtl::VecA3f dir;
            dir[0] = (2.0f * u) / (v * v + u * u + 1.0f);
            dir[1] = -(v * v + u * u - 1.0f) / (v * v + u * u + 1.0f);
            dir[2] = (2.0f * v) / (v * v + u * u + 1.0f);

            // Then transform the dir with respect to the world space
            NormalizeVec(dir);
            TransformVec(&invParaViewRot, &dir, &dir);

            // Weight the occlusion amount, taking into account the other side of the sphere
            // as well as the probability distribution of the texture sample
            // Then evaluate the SH for all bands in that hemisphere direction
            // Final SH coeff: 
            // 4PI/2N * SUM(2N, Sh(nInSphere) * Occl(nInSphere)) = 
            // 4PI/2N * SUM(N, Sh(nInHemi) * Occl(nInHemi)) =
            // SUM( N, Sh(nInHemi) * (4PI/2N * Occl(nInHemi)) )

            float depthVal = pParaTexData[index];
            float expDepth = depthVal; //exp(BAKED_SM_EXP_K_VAL * depthVal);
            expDepth *= PARABOLOID_PROJ_INTEGRAL_WEIGHTS[index];

            _LOOPk(BAKED_SM_NUM_SH_BANDS * BAKED_SM_NUM_SH_BANDS)
            {
                float ylm = EvaluateSHFastCartesian(k, _CAST_VEC3(dir));             
                depth.sh[k] += (ylm * expDepth);
            }
        }
    }

    _LOOPi(BAKED_SM_NUM_SH_BANDS * BAKED_SM_NUM_SH_BANDS)
    {
        // Rescale to linear space
        pDataEntryDest->sh[i] = depth.sh[i];//log(depth.sh[i]) / BAKED_SM_EXP_K_VAL;
    }
}

void BakedSMCompute::ComputeShadowMapSH(const BakedSMLocationEntry* pLocEntries, BakedSMSHEntry* pDataEntries, uint numEntries, IBakedSMComputeRenderCallback& occlCallback, IPThreadPool* pThreadPool)
{
    // TODO: Should let the app allocate mem
    ObjectPoolAtomic<ComputeSMTexToSHJob, 4> compressJobPool;
    ObjectPoolAtomic<ComputeSMTexToSHJob, 4>* pCompressJobPool = &compressJobPool;
    //if(pThreadPool)
    //	pCompressJobPool = _NEW ObjectPoolAtomic<CompressOcclTexToSHJob, 7>;

    //ObjectPoolAtomic<CompressOcclTexToSHJob, 5> compressJobPool;

    int readBackRTIndex = -((int)BAKED_SM_NUM_LATENCY_BUFFERS);

    uint curDataEntry = 0;
    gmtl::MatrixA44f paraViewStore[BAKED_SM_NUM_LATENCY_BUFFERS];

    _LOOPi(numEntries)
    {		
        if(m_pRenderer->BeginRender())
        {
            readBackRTIndex++;

            // For each location entry, render the scene and set
            // the appropriate render states

            BFXRenderContainer renderContainer;
            BFXParamContainer paramContainer;
            //renderContainer.SetRenderCallback((IBFXRenderCallback*) &m_Pipeline, SHPRTCOMPUTE_LIB_ID);
            paramContainer.SetParamCallback((IBFXParamCallback*) &m_ParamPool, SHPRTCOMPUTE_LIB_ID);

            // Derive and set the paraboloid view matrix for the current location
            gmtl::MatrixA44f paraView;
            gmtl::VecA3f x(1.0f, 0.0f, 0.0f);

            // Singularity check
            if( pLocEntries[i].normal[0] > 0.99f )
            {
                x[0] = 0.0f;
                x[1] = -1.0f;
                x[2] = 0.0f;
            }
            else if( pLocEntries[i].normal[0] < -0.99f )
            {
                x[0] = 0.0f;
                x[1] = 1.0f;
                x[2] = 0.0f;
            }

            gmtl::VecA3f z;
            gmtl::cross(_CAST_VEC3(z), _CAST_VEC3(x), _CAST_VEC3(pLocEntries[i].normal));
            gmtl::normalize(_CAST_VEC3(z));

            gmtl::cross(_CAST_VEC3(x), _CAST_VEC3(pLocEntries[i].normal), _CAST_VEC3(z));
            gmtl::normalize(_CAST_VEC3(x));

            gmtl::setViewWithAxis(paraView, pLocEntries[i].pos, x, pLocEntries[i].normal, z); 

            m_ParamPool.SetParaboloidViewMatrix(&_CAST_MAT44(paraView));

            // Then set the current render target
            int curRTIndex = i % BAKED_SM_NUM_LATENCY_BUFFERS;
            m_Pipeline.SetOcclusionRT(curRTIndex);
            m_Pipeline.Clear();

            // Begin app callback for render pipeline
            occlCallback.Render(pLocEntries[i].pos, paraView, renderContainer, paramContainer, m_Pipeline.GetOcclusionRT(curRTIndex), m_Pipeline.GetOcclusionDB());

            m_ParamPool.ResetParams();
            m_pRenderer->EndRender();

            paraViewStore[curRTIndex] = paraView;

            // Read back from GPU and perform integral
            if(readBackRTIndex >= 0)
            {
                readBackRTIndex = readBackRTIndex % BAKED_SM_NUM_LATENCY_BUFFERS;
                IRTextureRT* pOcclusionRT = m_Pipeline.GetOcclusionRT(readBackRTIndex)->GetTextureRT(0);
                pOcclusionRT->LockReadOnlyPrepare();

                uint pitch;
                byte* pToRead = pOcclusionRT->LockReadOnly(pitch);
                _DEBUG_ASSERT( (pitch * pOcclusionRT->GetHeight()) == (sizeof(float) * BAKED_SM_COMPUTE_SM_RT_SIZE  * BAKED_SM_COMPUTE_SM_RT_SIZE) );

                if(pThreadPool)
                {
                    ComputeSMTexToSHJob* pJob = pCompressJobPool->Get();
                    if(!pJob)
                    {
                        pThreadPool->ProcessJob();
                        pJob = pCompressJobPool->Get();
                        _DEBUG_ASSERT(pJob);
                    }

                    memcpy(pJob->texData, pToRead, sizeof(float) * BAKED_SM_COMPUTE_SM_RT_SIZE * BAKED_SM_COMPUTE_SM_RT_SIZE);
                    pJob->pOwner = this;
                    pJob->pData = &pDataEntries[curDataEntry];
                    pJob->paraView = _CAST_MAT44(paraViewStore[readBackRTIndex]);
                    pJob->pOwnerPool = pCompressJobPool;

                    pThreadPool->QueueJob(*pJob);
                }
                else
                {
                    // Integrate occlusion amount from texture and store in the PRT data entry
                    CompressSMTexDataToSH((float*) pToRead, &pDataEntries[curDataEntry], _CAST_MAT44(paraViewStore[readBackRTIndex]));
                }

                pOcclusionRT->UnlockReadOnly();

                curDataEntry++;
            }
        }
        else
        {
            i--;
        }

    }

    _LOOPi(BAKED_SM_NUM_LATENCY_BUFFERS-1)
    {
        readBackRTIndex++;

        if(readBackRTIndex >= 0)
        {
            readBackRTIndex = readBackRTIndex % BAKED_SM_NUM_LATENCY_BUFFERS;
            IRTextureRT* pOcclusionRT = m_Pipeline.GetOcclusionRT(readBackRTIndex)->GetTextureRT(0);
            pOcclusionRT->LockReadOnlyPrepare();

            // TODO: This portion should be threaded
            uint pitch;
            byte* pToRead = pOcclusionRT->LockReadOnly(pitch);
            _DEBUG_ASSERT( (pitch * pOcclusionRT->GetHeight()) == (sizeof(float) * BAKED_SM_COMPUTE_SM_RT_SIZE  * BAKED_SM_COMPUTE_SM_RT_SIZE) );

            if(pThreadPool)
            {
                ComputeSMTexToSHJob* pJob = pCompressJobPool->Get();
                if(!pJob)
                {
                    pThreadPool->ProcessJob();
                    pJob = pCompressJobPool->Get();
                    _DEBUG_ASSERT(pJob);
                }

                memcpy(pJob->texData, pToRead, sizeof(float) * BAKED_SM_COMPUTE_SM_RT_SIZE * BAKED_SM_COMPUTE_SM_RT_SIZE);
                pJob->pOwner = this;
                pJob->pData = &pDataEntries[curDataEntry];
                pJob->paraView = _CAST_MAT44(paraViewStore[readBackRTIndex]);
                pJob->pOwnerPool = pCompressJobPool;

                pThreadPool->QueueJob(*pJob);
            }
            else
            {
                // Integrate occlusion amount from texture and store in the PRT data entry
                CompressSMTexDataToSH((float*) pToRead, &pDataEntries[curDataEntry], _CAST_MAT44(paraViewStore[readBackRTIndex]));
            }

            pOcclusionRT->UnlockReadOnly();

            curDataEntry++;
        }
    }

    if(pThreadPool)
    {
        // Finish processing pending jobs if any
        while(pThreadPool->GetNumJobsPending() > 0)
        {
            pThreadPool->ProcessJob();
        }

        //_DELETE(pCompressJobPool);
    }
}

void BakedSMCompute::ComputePushPullTex4Channel(float* pRWData, uint texWidth, uint texHeight, const BakedSMLocationEntry* pLocEntriesInitial, uint numInitial)
{
    ushort* pPendingXY      = (ushort*) _ALIGNED_MALLOC(16, texWidth * texHeight * sizeof(ushort) * 2);
    ushort* pPendingXYTemp  = (ushort*) _ALIGNED_MALLOC(16, texWidth * texHeight * sizeof(ushort) * 2);
    byte* pFilledMask   = (byte*) _ALIGNED_MALLOC(16, texWidth * texHeight);
    memset(pFilledMask, 0, texWidth * texHeight);

    // Set filled mask for initial entries
    _LOOPi(numInitial)
    {
        pFilledMask[pLocEntriesInitial[i].texY * texWidth + pLocEntriesInitial[i].texX] = 0xFF;       
    }

    // Initialize pendingXY
    uint numLeft = 0;
    _LOOPi(texHeight)
    {
        _LOOPj(texWidth)
        {
            if(pFilledMask[i * texWidth + j] == 0)
            {
                pPendingXY[i * texWidth + j] = j;
                pPendingXY[i * texWidth + j + 1] = i;
                numLeft++;
            }
        }
    }
    
    // Start push-pull
    const static int NUM_OFFSETS = 8;
    const static int XY_OFFSETS[NUM_OFFSETS*2] = 
    {
        -1, 0,
        -1, -1,
        0, -1,
        1, -1,
        1, 0,
        1, 1,
        0, 1,
        -1, 1,
    };

    while(numLeft > 0)
    {
        uint curNumLeft = numLeft;
        numLeft = 0;
        _LOOPi(curNumLeft)
        {
            boolean isFound = FALSE;
            int x = pPendingXY[i*2];
            int y = pPendingXY[i*2+1];

            // Pull an offset if one is found
            _LOOPj(NUM_OFFSETS)
            {
                int curX = x + XY_OFFSETS[j];
                int curY = y + XY_OFFSETS[j+1];        
                if(curX >= 0 && curY >= 0 &&
                   curX < (int) texWidth && curY < (int) texHeight)
                {
                    if(pFilledMask[curY * texWidth + curX] != 0)
                    {
                        pRWData[(y * texWidth + x) * 4]     = pRWData[(curY * texWidth + curX) * 4];
                        pRWData[(y * texWidth + x) * 4 + 1] = pRWData[(curY * texWidth + curX) * 4 + 1];
                        pRWData[(y * texWidth + x) * 4 + 2] = pRWData[(curY * texWidth + curX) * 4 + 2];
                        pRWData[(y * texWidth + x) * 4 + 3] = pRWData[(curY * texWidth + curX) * 4 + 3];
                        isFound = TRUE;
                        break;
                    }
                }               
            }

            if(isFound)
            {
                // Make sure the mask is set to filled
                pFilledMask[y * texWidth + x] = 0xFF;
            }
            else
            {
                // Re-add this point to the queue to process
                pPendingXYTemp[numLeft*2] = x;
                pPendingXYTemp[numLeft*2+1] = y;
                numLeft++;
            }
        }

        ushort* pTemp = pPendingXY;
        pPendingXY = pPendingXYTemp;
        pPendingXYTemp = pTemp;
    }

    _ALIGNED_FREE(pPendingXY);
    _ALIGNED_FREE(pPendingXYTemp);
    _ALIGNED_FREE(pFilledMask);
}

uint BakedSMCompute::CreateBakedSMSH16Bit(const BakedSMLocationEntry* pLocEntries, BakedSMSHEntry* pDataEntries, uint numEntries, uint texWidth, uint texHeight, IRTexture2D** ppDest)
{ 
    // TODO: Have to determine these values empirically
    const static float BIAS_DATA[BAKED_SM_NUM_SH_BANDS * BAKED_SM_NUM_SH_BANDS]   = { 0.0f, 
                                                                   -2.0f, -2.0f, -2.0f, 
                                                                   -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 
                                                                   -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, - 1.0f };
                                                                   
    const static float SCALE_DATA[BAKED_SM_NUM_SH_BANDS * BAKED_SM_NUM_SH_BANDS]  = { 2.0f, 
                                                                    4.0f, 4.0f, 4.0f, 
                                                                    2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 
                                                                    2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f };

    // Store 4 bands in 4 textures
    float* pRawData[4];

    _LOOPi(4)
    {
        //ppDest[i] = m_pRenderer->GetRResourceMgr().CreateTexture2D(NULL, texWidth, texHeight, 1, TEXF_A16B16G16R16, TEXU_DEFAULT);
        //_DEBUG_ASSERT(ppDest[i]);

        pRawData[i] = (float*) _ALIGNED_MALLOC(16, sizeof(float) * 4 * texWidth * texHeight);
    }
 
    float shMax[BAKED_SM_NUM_SH_BANDS * BAKED_SM_NUM_SH_BANDS];
    float shMin[BAKED_SM_NUM_SH_BANDS * BAKED_SM_NUM_SH_BANDS];

    _LOOPi(BAKED_SM_NUM_SH_BANDS * BAKED_SM_NUM_SH_BANDS)
    {
        shMax[i] = -99999999.0f;
        shMin[i] = 99999999.0f;
    }

    // Fill raw data buffer from location entries
    _LOOPi(numEntries)
    {
        uint offset = 0;
        _LOOPj(4)
        {
            // Store 4 coeffs at a time to the appropriate texture
            float* pChannel = pRawData[j] + (pLocEntries[i].texY * sizeof(float) * 4 * texWidth) + (pLocEntries[i].texX * sizeof(float) * 4);
             _LOOPk(4)
            {
                float shVal = pDataEntries[i].sh[k+offset];
                if(shMax[k+offset] < shVal) shMax[k+offset] = shVal;
                if(shMin[k+offset] > shVal) shMin[k+offset] = shVal;

               // float rangeVal = (shVal - BIAS_DATA[k+offset]) / SCALE_DATA[k+offset];
                //_DEBUG_ASSERT(rangeVal >= 0.0f && rangeVal <= 1.0f);
                pChannel[k] = shVal;//(ushort) rangeVal * 65535.0f;
            }

            offset += 4;
        }
    }

    // Compute push-pull to fill sparse areas in the buffer
    _LOOPi(4)
    {
        ComputePushPullTex4Channel(pRawData[i], texWidth, texHeight, pLocEntries, numEntries);
    }

    // Create the actual texture and copy and also do some compression
    uint offset = 0;
    _LOOPi(4)
    {
        ppDest[i] = m_pRenderer->GetRResourceMgr().CreateTexture2D(NULL, texWidth, texHeight, 1, TEXF_A16B16G16R16, TEXU_DEFAULT);
        
        uint pitch;
        byte* pDestData = ppDest[i]->Lock(0, pitch, NULL);

        _LOOP(y, texHeight)
        {
            _LOOP(x, texWidth)
            {
                _LOOPk(4)
                {
                    float rawSHVal = pRawData[i][(y * texWidth * + x) * 4 + k];
                    float rangeVal = (rawSHVal - BIAS_DATA[k+offset]) / SCALE_DATA[k+offset];
                    _DEBUG_ASSERT(rangeVal >= 0.0f && rangeVal <= 1.0f);
                    ushort* pToWrite = (ushort*) (pDestData + y * pitch + (x * 4 + k) * sizeof(ushort));
                    *pToWrite = (ushort) (rangeVal * 65535.0f);
                }
            }
        }

        ppDest[i]->Unlock(0);
        offset += 4;
    }

    _LOOPi(4)
    {
        _ALIGNED_FREE(pRawData[i]);
    }

    return 4;
}
