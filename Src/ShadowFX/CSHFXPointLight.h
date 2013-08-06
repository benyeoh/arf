//==============================================================================
//
//		CSHFXPointLight.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		16/09/09
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

struct SHFXShadowPLParams
{
	gmtl::Vec4f		posAndFarPlane;
	float			yDir;
};

class CSHFXPointLight : public CRefCounted<ISHFXPointLight>
{
	struct PipelineContext
	{
		RefCountSmartPointer<BFXDataContext<RRenderOp>> pCasterContext[2];
	};

private:
	SHFXShadowPLParams		m_ShadowPLCast[2];
	
	IREffectPtr m_p2DPipeSetupEffect;
	//IREffectPtr m_pColorFillPLEffect;
	//IREffectPtr m_pColorChannelEffect;

	// Blur Filter
	BFXUScreenSpaceMesh m_ScreenMesh;	

	BFXUGaussianBlurParams* m_GaussianParamsV;
	BFXUGaussianBlurParams* m_GaussianParamsH;
	BFXUPoissonBlurParams* m_PoissonParams;

	IREffectPtr m_pBlur1Effect;
	IREffectPtr m_pBlur2Effect;

	CSHFXShadowFX* m_pShadowFX;

	BFXDataPool<RRenderOp> m_CasterPool[2];
	IRRenderGroupPtr m_pCasterGroups[2];
	IRRenderGroupPtr m_pBlur1Group[2];
	IRRenderGroupPtr m_pBlur2Group[2];
	//IRRenderGroupPtr m_pClearGroup;
	IRRenderGroupPtr m_pClearFinal;

	std::vector<PipelineContext> m_Contexts;

	IRTexture2DPtr m_pFinal;

public:
	CSHFXPointLight(CSHFXShadowFX* pShadowFX)
		: m_pShadowFX(pShadowFX)
		, m_GaussianParamsH(NULL)
		, m_GaussianParamsV(NULL)
		, m_PoissonParams(NULL)
	{
		Initialize();

		// For now, fix to this always
		// Front face = +y
		m_ShadowPLCast[0].yDir	= 1.0f;
		m_ShadowPLCast[1].yDir	= -1.0f;
	}

	virtual ~CSHFXPointLight()
	{
		if(m_GaussianParamsV)
			_DELETE(m_GaussianParamsV);

		if(m_GaussianParamsH)
			_DELETE(m_GaussianParamsH);

		if(m_PoissonParams)
			_DELETE(m_PoissonParams);
	}

private:
	//void SetSMSize(uint smSize);
	//void SetSMFilter(uint size);
	void Initialize();

public:
	void Update(const gmtl::Vec3f& pos);
	void SetRange(float range);

	SHFXShadowPLParams* GetFaceParam(uint index)	{ return &m_ShadowPLCast[index]; }
	
	//IRTextureRT*	GetShadowMap()			{ return m_pCasterGroups[0]->GetRenderTarget(0); }	
	//IRTextureRT*	GetShadowMapBackup()	{ return m_pGaussianHGroup->GetRenderTarget(0); }					
	//IRDepthBuffer*	GetDepthBuffer()		{ return m_pCasterGroups[0]->GetDepthBuffer(); }

	IRTexture2D*	GetShadowMap()		{ return m_pFinal; }					
	IRDepthBuffer*	GetDepthBuffer()	{ return m_pCasterGroups[0]->GetDepthBuffer(); }

	IRRenderGroup* GetCasterGroup(uint face);

	uint AddContext();
	uint GetNumOfContexts();
	void ResetContext(uint contextID);

	void RenderPointLightCaster(BFXRenderContext& context);
	void Flush();
	//void FlushCasterGroups();
	//void FlushFilterGroups();

	void SetShadowMapParamsGaussianFilter(IRTexture2D* pSM, IRTexture2D* pBackup, IRTexture2D* pFinal, IRDepthBuffer* pDepth, float stdDev, uint spreadKernelSizes);
	void SetShadowMapParamsPoissonFilter(IRTexture2D* pSM, IRTexture2D* pFinal, IRDepthBuffer* pDepth, eBFXUPoissonBlurSamples blurSamples, float spreadScale, boolean isUseNoiseBlur);
	void SetShadowMapParams(IRTexture2D* pFinal, IRDepthBuffer* pDepth);

	_IMPL_ALIGNED_ALLOCS(16)
};

_DECLARE_SMARTPTR(CSHFXPointLight)

_NAMESPACE_END
