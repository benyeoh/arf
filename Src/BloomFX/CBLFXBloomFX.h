//==============================================================================
//
//		CBLFXBloomFX.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/20/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CBLFXBloomFX : public CRefCounted<IBLFXBloomFX>
{
private: 
	//float m_AverageLuminance;
	IAppCallback* m_pCallback;
	IRRendererPtr m_pRenderer;
	CBLFXPipeline m_Pipeline;
	CBLFXParamPool m_ParamPool;
		
	// Star Glare
	IREffectPtr m_pPreBlurEffectH;
	IREffectPtr m_pPreBlurEffectV;
	IREffectPtr m_pStarBlurEffectH;
	IREffectPtr m_pStarBlurEffectH2;
	IREffectPtr m_pStarBlurEffectV;
	IREffectPtr m_pStarBlurEffectV2;
	BFXUScreenSpaceMesh m_StarQuad;
	IRTextureRTPtr m_pStarRT1;
	IRTextureRTPtr m_pStarRT2;
	BLFXStarBlurParams	m_StarBlurH;
	BLFXStarBlurParams	m_StarBlurH2;
	BLFXStarBlurParams	m_StarBlurV;
	BLFXStarBlurParams	m_StarBlurV2;
	BFXUGaussianBlurParams m_PreBlurKernelH;
	BFXUGaussianBlurParams m_PreBlurKernelV;
	
	// Bloom
	IREffectPtr m_pGaussianBlurEffectHMed;
	IREffectPtr m_pGaussianBlurEffectVMed;
	IREffectPtr m_pBloomDownSampleEffectMed;
	IREffectPtr m_pBloomUpSampleComposeEffectMed;
	BFXUScreenSpaceMesh m_BloomQuadMed;
	IRTextureRTPtr m_pBloomRTHMed;
	IRTextureRTPtr m_pBloomDownSampleSourceMed;
	IRTextureRTPtr m_pBloomRTVMed;
	BFXUGaussianBlurParams m_GaussianBlurKernelHMed;
	BFXUGaussianBlurParams m_GaussianBlurKernelVMed;
	
	IREffectPtr m_pGaussianBlurEffectHLarge;
	IREffectPtr m_pGaussianBlurEffectVLarge;
	BFXUScreenSpaceMesh m_BloomQuadLarge;
	IRTextureRTPtr m_pBloomRTHLarge;
	IRTextureRTPtr m_pBloomRTVLarge;
	BFXUGaussianBlurParams m_GaussianBlurKernelHLarge;
	BFXUGaussianBlurParams m_GaussianBlurKernelVLarge;
	
	// Preparation
	IREffectPtr m_pBrightPassEffect;
	IREffectPtr m_pBlendEffect;
	BFXUScreenSpaceMesh m_CompositionQuad;
	BFXUScreenSpaceMesh m_BrightPassQuad;
	BFXUScreenSpaceMesh m_BackBufferQuad;
	IRTextureRTPtr m_pRTDownSampledSource;
	IRTextureRTPtr m_pComposition;
	IRTextureRTPtr m_pRTBrightPassSource;
		
	//float m_GaussianDetailLevel;
	
public: 
	CBLFXBloomFX()
		: m_pRenderer(NULL) //m_AverageLuminance(0.06f) 
		, m_pCallback(NULL)
		//, m_GaussianDetailLevel(0.0f)
	{
	}

	~CBLFXBloomFX()
	{
	}

private:
	boolean InitializeBloomEffect();
	boolean InitializeStarGlareEffect();
	boolean InitializePreparationEffects(boolean resetAll=FALSE);
	
	void UninitializeBloomEffect();
	void UninitializeStarGlareEffect();
	void UninitializePreparationEffects();
	
	void AddPreparationPasses();
	void AddBloomPass();
	void AddStarGlarePass();

public:
	boolean Initialize(IRRenderer& renderer, IAppCallback& callback);
	
	void SetBloom(boolean isEnabled);
	void SetStarGlare(boolean isEnabled);
	
	void Render();
	
	IBLFXParamPool& GetParamPool() { return m_ParamPool; }		
	IRRenderer& GetRenderer() { return *m_pRenderer; }
	IAppCallback& GetAppCallback() { return *m_pCallback; }
	
	IRTextureRT* GetDownSampledSource() { return m_pRTDownSampledSource; }
};

_NAMESPACE_END
