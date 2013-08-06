//==============================================================================
//
//		CSAOFXPipeline.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		07/12/2009
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CSAOFXPipeline : public ISAOFXPipeline, public IBFXRenderCallback, public IRRenderCallback
{
private:
    CSAOFXScreenAmbOcclFX*  m_pAmbOcclFX;
    
    IREffectPtr             m_p2DEffect;
    IREffectTemplatePtr     m_pSSAOEffectTemplate;
    IREffectPtr             m_pSSAOEffect;
    IREffectTemplatePtr     m_pUpsampleEffectTemplate;
    IREffectPtr             m_pUpsampleEffect;
    IREffectTemplatePtr     m_pBlurEffectTemplate;
    IREffectPtr             m_pBlurHEffect;
    IREffectPtr             m_pBlurVEffect;

    BFXUGaussianBlurParams  m_HBlurParams;
    BFXUGaussianBlurParams  m_VBlurParams;

    BFXUScreenSpaceMesh     m_AOMesh;
    BFXUScreenSpaceMesh     m_ScreenMesh;
    
    IRTextureRTPtr          m_pAORTTemp;
    IRTextureRTPtr          m_pAORT;
    IRTexture2DPtr          m_pAONoiseTex;

    uint    m_AOGroup;
    uint    m_BlurHGroup;
    uint    m_BlurVGroup;
    uint    m_UpsampleGroup;
    uint    m_BlurHTech;
    uint    m_BlurVTech;

public:
	CSAOFXPipeline()
        : m_AOGroup(BFX_INVALID)
        , m_BlurHGroup(BFX_INVALID)
        , m_BlurVGroup(BFX_INVALID)
        , m_UpsampleGroup(BFX_INVALID)
	{
	}

    virtual ~CSAOFXPipeline()
    {
        Uninitialize();
    }

private:
    void Uninitialize();

public:
    void SetAmbOccl(CSAOFXScreenAmbOcclFX* pAmbOccl);

    void UpdateScreenSize(uint screenWidth, uint screenHeight);
    void Initialize();

    void Render();

    void BeginRenderEffect(BFXMesh& mesh);
    void RenderEffect(IREffect* pEffect, uint tech, REffectParam* pDynamicParams, uint numParams);
    void EndRenderEffect(BFXMesh& mesh);

    void BeginRenderGroup(uint groupNum);
    void EndRenderGroup(uint groupNum);

    void BeginRenderOp(const RRenderOp* pSortData);
    void EndRenderOp(const RRenderOp* pSortData);	
};

_NAMESPACE_END
