//==============================================================================
//
//		CSAOFXPipeline.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		07/12/2009
//
//==============================================================================

#include "ScreenAmbOcclFXFramework.h"

_NAMESPACE_BEGIN

void CSAOFXPipeline::SetAmbOccl(CSAOFXScreenAmbOcclFX* pAmbOccl)
{
    m_pAmbOcclFX = pAmbOccl;
}

void CSAOFXPipeline::UpdateScreenSize(uint screenWidth, uint screenHeight)
{
    REffectParam downsampleZPos;
    REffectParam downsampleNormal;
    m_pAmbOcclFX->GetDeferredFX().GetParamPool().GetParam(DEFX_Z_POS_ACCUM, &downsampleZPos);
    m_pAmbOcclFX->GetDeferredFX().GetParamPool().GetParam(DEFX_NORMALS_ACCUM, &downsampleNormal);

    uint downsampleWidth    = (screenWidth / 2);
    uint downsampleHeight   = (screenHeight / 2);
    //_DEBUG_ASSERT(downsampleWidth == (screenWidth / 2));
    //_DEBUG_ASSERT(downsampleHeight == (screenHeight / 2));

    IRResourceMgr& resMgr = m_pAmbOcclFX->GetRenderer().GetRResourceMgr();
    m_pAORT     = resMgr.CreateTexture2DRT(downsampleWidth, downsampleHeight, 1, TEXF_A8R8G8B8);
    m_pAORTTemp = resMgr.CreateTexture2DRT(downsampleWidth, downsampleHeight, 1, TEXF_A8R8G8B8);

    m_AOMesh.Initialize(downsampleWidth, downsampleHeight, &m_pAmbOcclFX->GetRenderer());
    m_ScreenMesh.Initialize(screenWidth, screenHeight, &m_pAmbOcclFX->GetRenderer());

    gmtl::Vec2f spreadH(1.0f, 0.0f);
    gmtl::Vec2f spreadV(0.0f, 1.0f);
	gmtl::Vec2f offsetH(-0.0f, 0.0f);
	gmtl::Vec2f offsetV(0.0f, -0.0f);

    m_HBlurParams.InitializeGaussianFilter(2.5f, 11, &spreadH, &offsetH, 1.0f, (float)downsampleWidth, (float)downsampleHeight, FALSE);
    m_VBlurParams.InitializeGaussianFilter(2.5f, 11, &spreadV, &offsetV, 1.0f, (float)downsampleWidth, (float)downsampleHeight, FALSE);

    //const static int NUM_POINTS = 9;
    //gmtl::VecA3f points[NUM_POINTS];
    //int numPoints = 0;

    //points[0][0] = 0.0f;
    //points[0][1] = 0.0f;
    //points[0][2] = 0.0f;

    //srand( 1235 );
    //for(int i=1; i < NUM_POINTS; i++)
    //{
    //    gmtl::VecA3f newPoint;
    //    newPoint[0] = (float(rand()) / float(RAND_MAX)) * 2.0f - 1.0f;
    //    newPoint[1] = (float(rand()) / float(RAND_MAX)) * 2.0f - 1.0f;
    //    newPoint[2] = 0.0f;

    //    points[i] = newPoint;
    //}

    //for(int i=0; i < 50000; i++)
    //{
    //    if((i % 100) == 0)
    //    {
    //        m_pAmbOcclFX->GetAppCallback().Log(ToStringAuto(_W(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>")));
    //        for(int j=0; j < NUM_POINTS; j++)
    //        {
    //            m_pAmbOcclFX->GetAppCallback().Log(ToStringAuto(_W("float2(%ff, %ff),\n"), points[j][0], points[j][1]));
    //        }
    //    }

    //    for(int j=1; j < NUM_POINTS; j++)
    //    {
    //        for(int k=0; k < NUM_POINTS; k++)
    //        {
    //            if(j == k)
    //                continue;

    //            gmtl::VecA3f diff;
    //            diff[0] = points[j][0] - points[k][0];
    //            diff[1] = points[j][1] - points[k][1];
    //            diff[2] = points[j][2] - points[k][2];

    //            float length;
    //            NormalizeVecLength(&length, &diff, &diff);
    //            
    //            points[j][0] = points[j][0] + diff[0] * gmtl::Math::Max((1.0f - length), 0.0f) * 0.02f;
    //            points[j][1] = points[j][1] + diff[1] * gmtl::Math::Max((1.0f - length), 0.0f) * 0.02f;
    //            points[j][2] = points[j][2] + diff[2] * gmtl::Math::Max((1.0f - length), 0.0f) * 0.02f;

    //            gmtl::VecA3f zero;
    //            zero[0] = zero[1] = zero[2] = 0.0f;

    //            float distToEdge = gmtl::length(_CAST_VEC3(points[j]));
    //            if(distToEdge > 1.0f)
    //                NormalizeVec(&points[j], &points[j]);
    //        }
    //    }
    //}

    //for(int i=0; i < NUM_POINTS; i++)
    //{
    //    m_pAmbOcclFX->GetAppCallback().Log(ToStringAuto(_W("float2(%ff, %ff),\n"), points[i][0], points[i][1]));
    // }

    //_LOOPi( 11 )
    //{
    //    m_pAmbOcclFX->GetAppCallback().Log(ToStringAuto(_W("%d uv: %f, %f\n"), i, m_HBlurParams.GetGaussianOffsets()[i][0], m_HBlurParams.GetGaussianOffsets()[i][1] ));
    //}

    //_LOOPi( 11 )
    //{
    //    m_pAmbOcclFX->GetAppCallback().Log(ToStringAuto(_W("%d: %f\n"), i, m_HBlurParams.GetGaussianWeights()[i]));
    //}

    // AO
    REffectParam params[32];
    params[0].SetTextureRT(downsampleZPos.pRT);
    gmtl::Vec2f screenPixelOffset;
    screenPixelOffset[0] = 1.0f / downsampleWidth; screenPixelOffset[1] = 1.0f / downsampleHeight;
    gmtl::Vec2f aoSize;
    aoSize[0] = (float)downsampleZPos.pRT->GetWidth(); aoSize[1] = (float)downsampleZPos.pRT->GetHeight();
    params[1].SetVector2(&screenPixelOffset);
    params[2].SetTexture2D(m_pAONoiseTex);
    params[3].SetVector2(&aoSize);
    params[4].SetTextureRT(downsampleNormal.pRT);
    uint semantics[32];
    semantics[0] = BFXU_RT1;
    semantics[1] = BFXU_SCREEN_PIXEL_OFFSET;
    semantics[2] = BFXU_RT2;
    semantics[3] = BFXU_RT1_SIZE;
    semantics[4] = BFXU_RT3;

    m_pSSAOEffect = m_pAmbOcclFX->GetRenderer().GetRResourceMgr().LoadEffect(
        m_pSSAOEffectTemplate,
        params,
        semantics,
        5);

    // H Blur
	semantics[0] = BFXU_FILTER_TEXTURE_OFFSETS;
	semantics[1] = BFXU_FILTER_WEIGHTS;
	semantics[2] = BFXU_RT1;
	semantics[3] = BFXU_SCREEN_PIXEL_OFFSET;
	semantics[4] = BFXU_RT2;
    semantics[5] = BFXU_RT3;

	params[0].SetValue((void*) m_HBlurParams.GetGaussianOffsets(), 11 * sizeof(gmtl::Vec2f));
	params[1].SetValue((void*) m_HBlurParams.GetGaussianWeights(), 11 * sizeof(float));
	params[2].SetTextureRT(m_pAORT);
	screenPixelOffset[0] = 1.0f / downsampleWidth; screenPixelOffset[1] = 1.0f / downsampleHeight;
	params[3].SetVector2(&screenPixelOffset);
	params[4].SetTextureRT(downsampleZPos.pRT);
    params[5].SetTextureRT(downsampleNormal.pRT);
    m_pBlurHEffect  = m_pAmbOcclFX->GetRenderer().GetRResourceMgr().LoadEffect(m_pBlurEffectTemplate, params, semantics, 6);
    m_BlurHTech     = 1; //GetGaussianBlurEffectTech(m_pBlurHEffect, m_HBlurParams.GetGaussianSize());

	// V Blur
	params[0].SetValue((void*) m_VBlurParams.GetGaussianOffsets(), 11 * sizeof(gmtl::Vec2f));
	params[1].SetValue((void*) m_VBlurParams.GetGaussianWeights(), 11 * sizeof(float));
	params[2].SetTextureRT(m_pAORTTemp);

	m_pBlurVEffect  = m_pAmbOcclFX->GetRenderer().GetRResourceMgr().LoadEffect(m_pBlurEffectTemplate, params, semantics, 6);
    m_BlurVTech     = 1; //GetGaussianBlurEffectTech(m_pBlurVEffect, m_VBlurParams.GetGaussianSize());

    // Upsample
    params[0].SetTextureRT(m_pAORT);
    semantics[0] = BFXU_RT1;
    gmtl::Vec2f texSize;
    texSize[0] = (float) m_pAORT->GetWidth(); texSize[1] = (float) m_pAORT->GetHeight();
    params[1].SetVector2(&texSize);
    semantics[1] = BFXU_RT1_SIZE;
	
    REffectParam zPos;
    m_pAmbOcclFX->GetDeferredFX().GetParamPool().GetParam(DEFX_Z_POS_ACCUM, &zPos);
    params[2].SetTextureRT(zPos.pRT);
    semantics[2] = BFXU_RT2;

	REffectParam albedo;
	m_pAmbOcclFX->GetDeferredFX().GetParamPool().GetParam(DEFX_ALBEDO_ACCUM, &albedo);
	params[3].SetTextureRT(albedo.pRT);
	semantics[3] = BFXU_RT3;

    m_pUpsampleEffect = m_pAmbOcclFX->GetRenderer().GetRResourceMgr().LoadEffect(
        m_pUpsampleEffectTemplate,
        params,
        semantics,
        4);
}

void CSAOFXPipeline::Uninitialize()
{
    m_pAmbOcclFX->GetDeferredFX().GetPipeline().Disable2XDownsampleAccum();
}

void CSAOFXPipeline::Initialize()
{
    wstring fileName = _W("[shd]\\BaseFXUtils\\2DPipeSetup.fx");
    CRCDataKey id;
    id.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));
    IByteBufferPtr pBuffer = m_pAmbOcclFX->GetAppCallback().GetFileData(fileName.c_str());

    m_p2DEffect = m_pAmbOcclFX->GetRenderer().GetRResourceMgr().LoadEffect(
        &id, 
        pBuffer,
        NULL,
        NULL,
        0);

    fileName = _W("[shd]\\ScreenAmbOcclFX\\SSAOFilterDepth.fx");
    pBuffer = m_pAmbOcclFX->GetAppCallback().GetFileData(fileName.c_str());
    CRCDataKey key;
    key.Set(fileName.c_str(), (uint) (sizeof(wchar) * fileName.length()));
    m_pSSAOEffectTemplate = 
        m_pAmbOcclFX->GetRenderer().GetRResourceMgr().LoadEffectTemplate(&key, pBuffer);

    fileName = _W("[shd]\\ScreenAmbOcclFX\\Upsample.fx");
    pBuffer = m_pAmbOcclFX->GetAppCallback().GetFileData(fileName.c_str());
    key.Set(fileName.c_str(), (uint) (sizeof(wchar) * fileName.length()));
    m_pUpsampleEffectTemplate =
        m_pAmbOcclFX->GetRenderer().GetRResourceMgr().LoadEffectTemplate(&key, pBuffer);

    fileName = _W("[shd]\\ScreenAmbOcclFX\\BlurAODepth.fx");
    pBuffer = m_pAmbOcclFX->GetAppCallback().GetFileData(fileName.c_str());
    key.Set(fileName.c_str(), (uint) (sizeof(wchar) * fileName.length()));
    m_pBlurEffectTemplate =
        m_pAmbOcclFX->GetRenderer().GetRResourceMgr().LoadEffectTemplate(&key, pBuffer);

    // Create the noise texture
    fileName = _W("[tex]\\small_noise.dds");
    pBuffer = m_pAmbOcclFX->GetAppCallback().GetFileData(fileName.c_str());
    key.Set(fileName.c_str(), (uint) (sizeof(wchar) * fileName.length()));
    m_pAONoiseTex = m_pAmbOcclFX->GetRenderer().GetRResourceMgr().LoadTexture2D(&key, pBuffer);

    // Enable 2X downsample for z-pos from deferred pipeline
    m_pAmbOcclFX->GetDeferredFX().GetPipeline().Enable2XDownsampleAccum();

    const RDisplayInfo& info = m_pAmbOcclFX->GetRenderer().GetDisplayInfo();
    UpdateScreenSize(info.backBufferWidth, info.backBufferHeight);
}

void 
CSAOFXPipeline::Render()
{
    IRPipeline& pipeline = m_pAmbOcclFX->GetRenderer().GetRPipeline();

    _DEBUG_ASSERT(m_AOGroup == BFX_INVALID);

    if(m_AOGroup == BFX_INVALID)
    {
        // AO
        m_AOGroup = pipeline.CreateNewRenderGroup(SAOFX_GROUP_AO, NULL, *this);
        pipeline.AddRenderTarget(m_AOGroup, m_pAORT);  

        // Get parameter from deferred renderer
        REffectParam param[32];
        m_pAmbOcclFX->GetBaseFX().GetDynamicParams(m_pSSAOEffect, 0, param);
        pipeline.AddToRenderGroup(m_AOGroup, 
            m_AOMesh.GetQuadVertices(), m_AOMesh.GetQuadIndices(), m_pSSAOEffect, param, 0, NULL);	

        // H Blur
        m_pAmbOcclFX->GetBaseFX().GetDynamicParams(m_pBlurHEffect, 0, param);
        m_BlurHGroup = pipeline.CreateNewRenderGroup(SAOFX_GROUP_BLURH, NULL, *this);
        pipeline.AddRenderTarget(m_BlurHGroup, m_pAORTTemp);
        pipeline.AddToRenderGroup(m_BlurHGroup, 
            m_AOMesh.GetQuadVertices(), m_AOMesh.GetQuadIndices(), m_pBlurHEffect, param, m_BlurHTech, NULL);	

        // V Blur
        m_pAmbOcclFX->GetBaseFX().GetDynamicParams(m_pBlurVEffect, 0, param);
        m_BlurVGroup = pipeline.CreateNewRenderGroup(SAOFX_GROUP_BLURV, NULL, *this);
        pipeline.AddRenderTarget(m_BlurVGroup, m_pAORT);
        pipeline.AddToRenderGroup(m_BlurVGroup, 
            m_AOMesh.GetQuadVertices(), m_AOMesh.GetQuadIndices(), m_pBlurVEffect, param, m_BlurVTech, NULL);	

        // Upsample
        _DEBUG_ASSERT(m_UpsampleGroup == BFX_INVALID);
        m_UpsampleGroup = pipeline.CreateNewRenderGroup(SAOFX_GROUP_UPSAMPLE, NULL, *this);
        pipeline.AddToRenderGroup(m_UpsampleGroup, 
            m_ScreenMesh.GetQuadVertices(), m_ScreenMesh.GetQuadIndices(), m_pUpsampleEffect, NULL, 0, NULL);
    }

}

void 
CSAOFXPipeline::BeginRenderEffect(BFXMesh& mesh)
{

}

void 
CSAOFXPipeline::RenderEffect(IREffect* pEffect, uint tech, REffectParam* pDynamicParams, uint numParams)
{

}

void 
CSAOFXPipeline::EndRenderEffect(BFXMesh& mesh)
{

}

void 
CSAOFXPipeline::BeginRenderGroup(uint groupNum)
{
    switch(groupNum)
    {
        case SAOFX_GROUP_AO:
        {
            IREffectTemplate* pTemplate = m_p2DEffect->GetTemplate();
            pTemplate->BeginTechnique(0);
            pTemplate->BeginPass(0);     
            break;
        }

        case SAOFX_GROUP_BLURH:
        {
            break;
        }

        case SAOFX_GROUP_BLURV:
        {
            break;
        }

        case SAOFX_GROUP_UPSAMPLE:
        {
            break;
        }

        default: _DEBUG_ASSERT(FALSE);
    }
}

void 
CSAOFXPipeline::EndRenderGroup(uint groupNum)
{
    switch(groupNum)
    {
        case SAOFX_GROUP_AO:
        {
            m_AOGroup = BFX_INVALID;
            break;
        }

        case SAOFX_GROUP_BLURH:
        {
            m_BlurHGroup = BFX_INVALID;
            break;
        }

        case SAOFX_GROUP_BLURV:
        {
            m_BlurVGroup = BFX_INVALID;
            break;
        }

        case SAOFX_GROUP_UPSAMPLE:
        {            
            IREffectTemplate* pTemplate = m_p2DEffect->GetTemplate();
            pTemplate->EndPass();
            pTemplate->EndTechnique();

            m_UpsampleGroup = BFX_INVALID;
            break;
        }

        default: _DEBUG_ASSERT(FALSE);
    }

}

void 
CSAOFXPipeline::BeginRenderOp(const RRenderOp* pSortData)
{

}

void 
CSAOFXPipeline::EndRenderOp(const RRenderOp* pSortData)
{

}

_NAMESPACE_END
