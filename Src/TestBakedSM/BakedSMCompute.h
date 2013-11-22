//================================================================================
//
//		BakedSMCompute.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/11/2013
//
//================================================================================

#pragma once

struct BakedSMLocationEntry
{
    gmtl::VecA3f pos;
    gmtl::VecA3f normal;
    uint texX;
    uint texY;

    _IMPL_ALIGNED_ALLOCS(16)
};

//struct BakedSMDataEntry
//{
//    const static uint BAKED_SM_SIZE = 4;
//    float shadowMapMean[BAKED_SM_SIZE*BAKED_SM_SIZE];
//    float shadowMapMeanSq[BAKED_SM_SIZE*BAKED_SM_SIZE];
//
//    _IMPL_ALIGNED_ALLOCS(16)
//};

struct BakedSMSHEntry
{
    float sh[BAKED_SM_NUM_SH_BANDS*BAKED_SM_NUM_SH_BANDS];
};

struct IBakedSMComputeRenderCallback
{
    // During Render, the application should set up all necessary parameters to correctly render the object
    // IE, loading the correct material for SHPRTCompute, setting up parameters and render containers, rendering the object with any relevant material types etc...
    _PURE( void Render(const gmtl::VecA3f& loc, const gmtl::MatrixA44f& paraView, BFXRenderContainer& renderContainer, BFXParamContainer& paramContainer, IRTexture2D* pRT, IRDepthBuffer* pDB) )
};

class BakedSMCompute : public CRefCounted<IRefCounted>
{   
private:
    IRRendererPtr m_pRenderer;
    //IBFXBaseFXPtr m_pBaseFX;

    IAppCallback* m_pAppCallback;

    BakedSMComputePipeline m_Pipeline;
    BakedSMComputeParamPool m_ParamPool;

private:
    uint DoComputeUVLocEntries(BakedSMLocationEntry* pLocEntries, const gmtl::VecA4f& v1, const gmtl::VecA4f& v2, const gmtl::VecA4f& v3, 
                               const float* pV1Inter, const float* pV2Inter, const float* pV3Inter, uint texWidth, uint texHeight);

    void ComputePushPullTex4ChannelFloat(float* pRWData, uint texWidth, uint texHeight, const BakedSMLocationEntry* pLocEntriesInitial, uint numInitial);

public:
    void Initialize(IRRenderer* pRenderer, IAppCallback* pCallback);
    
    // TODO: This should be removed
    BakedSMComputeParamPool* GetBakedSMComputeParamPool();

    void ComputePushPullTex4ChannelByte(byte* pRWData, uint texWidth, uint texHeight, const BakedSMLocationEntry* pLocEntriesInitial, uint numInitial);

    void CompressSMTexDataToSH(float* pParaTexData, BakedSMSHEntry* pDataEntryDest, const gmtl::Matrix44f& paraView);

    uint ComputeUVLocEntries(BakedSMLocationEntry* pLocEntries, const gmtl::MatrixA44f& worldTrans, 
                             void* pPos, uint posStride, void* pNormal, uint normalStride,
                             void* pUV, uint uvStride, ushort* pIndices, uint numTri, uint texWidth, uint texHeight, float posBias);

    void ComputeShadowMapSH(const BakedSMLocationEntry* pLocEntries, BakedSMSHEntry* pDataEntries, uint numEntries, IBakedSMComputeRenderCallback& occlCallback, IPThreadPool* pThreadPool);
    uint CreateBakedSMSH16Bit(const BakedSMLocationEntry* pLocEntries, BakedSMSHEntry* pDataEntries, uint numEntries, uint texWidth, uint texHeight, IRTexture2D** ppDest);

    IRTexture2D* CreateVisibilitySphereSHTable();
    IRTexture2D* CreateExpDepthSphereSHTable();
};

_DECLARE_SMARTPTR(BakedSMCompute)