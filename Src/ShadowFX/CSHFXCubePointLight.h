//================================================================================
//
//		CSHFXCubePointLight.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/14/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

struct SHFXCubeShadowPLParams
{
	gmtl::Vec4f	posAndInvRange;
};

class CSHFXCubePointLight : public CRefCounted<ISHFXCubePointLight>
{
	struct PipelineContext
	{
		RefCountSmartPointer<BFXDataContext<RRenderOp>> pCasterContext[CF_NUM_FACES];
	};

private:
	SHFXCubeShadowPLParams	m_ShadowPLCast;
	gmtl::MatrixA44f		m_ViewProj[CF_NUM_FACES];

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
	IREffectPtr m_pCubeBlurEffect[CF_NUM_FACES];

	CSHFXShadowFX* m_pShadowFX;

	BFXDataPool<RRenderOp> m_CasterPool[CF_NUM_FACES];
	IRRenderGroupPtr m_pCasterGroups[CF_NUM_FACES];
	IRRenderGroupPtr m_pNoDrawOpClearGroups[CF_NUM_FACES];
	IRRenderGroupPtr m_pBlur1Group[CF_NUM_FACES];
	IRRenderGroupPtr m_pBlur2Group[CF_NUM_FACES];
	//IRRenderGroupPtr m_pClearGroup;

	std::vector<PipelineContext> m_Contexts;

	IRTextureCubePtr m_pFinal;

public:
	CSHFXCubePointLight(CSHFXShadowFX* pShadowFX)
		: m_pShadowFX(pShadowFX)
		, m_GaussianParamsH(NULL)
		, m_GaussianParamsV(NULL)
		, m_PoissonParams(NULL)
	{
		Initialize();
	}

	virtual ~CSHFXCubePointLight()
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
	void UpdateViewProj();

public:
	void Update(const gmtl::Vec3f& pos);
	void SetRange(float range);
	void SetRangeNoUpdate(float range);

	float GetViewProjNearPlane() { return 0.001f; }
	float GetViewProjFarPlane() { return 1.0f / m_ShadowPLCast.posAndInvRange[3]; }

	SHFXCubeShadowPLParams* GetParam()	{ return &m_ShadowPLCast; }
	gmtl::MatrixA44f* GetViewProj(eRCubeFace face) { return &(m_ViewProj[face]); }

	//IRTextureRT*	GetShadowMap()			{ return m_pCasterGroups[0]->GetRenderTarget(0); }	
	//IRTextureRT*	GetShadowMapBackup()	{ return m_pGaussianHGroup->GetRenderTarget(0); }					
	//IRDepthBuffer*	GetDepthBuffer()		{ return m_pCasterGroups[0]->GetDepthBuffer(); }

	IRTextureCube*	GetShadowMap()		{ return m_pFinal; }					
	IRDepthBuffer*	GetDepthBuffer()	{ return m_pCasterGroups[0]->GetDepthBuffer(); }

	IRRenderGroup* GetCasterGroup(eRCubeFace face);

	uint AddContext();
	uint GetNumOfContexts();
	void ResetContext(uint contextID);

	void RenderPointLightCaster(BFXRenderContext& context);
	void Flush();
	//void FlushCasterGroups();
	//void FlushFilterGroups();

	void SetShadowMapParamsGaussianFilter(IRTexture2D* pSM, IRTexture2D* pBackup, IRTextureCube* pFinal, IRDepthBuffer* pDepth, float stdDev, uint spreadKernelSizes);
	void SetShadowMapParamsPoissonFilter(IRTexture2D* pSM, IRTextureCube* pFinal, IRDepthBuffer* pDepth, eBFXUPoissonBlurSamples blurSamples, float spreadScale, boolean isUseNoiseBlur);
	void SetShadowMapParamsCubePoissonFilter(IRTextureCube* pSM, IRTextureCube* pFinal, IRDepthBuffer* pDepth, eBFXUPoissonBlurSamples blurSamples, float spreadScale, boolean isUseNoiseBlur);
	void SetShadowMapParams(IRTextureCube* pFinal, IRDepthBuffer* pDepth);

	_IMPL_ALIGNED_ALLOCS(16)
};

_DECLARE_SMARTPTR(CSHFXCubePointLight)

_NAMESPACE_END
