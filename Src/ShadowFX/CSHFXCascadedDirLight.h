//==============================================================================
//
//		CSHFXCascadedDirLight.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		8/11/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CSHFXCascadedDirLight : public CRefCounted<ISHFXCascadedDirLight>
{
	struct PipelineContext
	{
		RefCountSmartPointer<BFXDataContext<RRenderOp>> pCasterContext[4];
	};

private: 
	SHFXCascadedProj m_Proj;
	SHFXCascadedProj m_ProjWithTexOffset;
	SHFXCascadedProj m_ProjAABB;

	SHFXCascadedSplitZ m_Split;
	SHFXCascadedSplitZ m_SplitView;

	CSHFXShadowFX* m_pShadowFX;
			
	BFXUScreenSpaceMesh m_CascadedScreenMesh;	

	BFXUGaussianBlurParams* m_GaussianParamsV;
	BFXUGaussianBlurParams* m_GaussianParamsH;
	
	BFXUPoissonBlurParams* m_PoissonParams;

	IREffectPtr m_p2DPipeSetupEffect;
	//IREffectPtr m_pColorFillEffect;
	//IREffectPtr m_pColorChannelEffect;

	IREffectPtr m_pBlur1Effect[4];
	IREffectPtr m_pBlur2Effect[4];
	
	BFXDataPool<RRenderOp> m_CasterPool[4];
	IRRenderGroupPtr m_pCasterGroups[4];
	//IRRenderGroupPtr m_pNoDrawOpClearGroups[4];
	IRRenderGroupPtr m_pBlur1Group[4];
	IRRenderGroupPtr m_pBlur2Group[4];
	//IRRenderGroupPtr m_pClearGroup;

	IRRenderGroupPtr m_pClearFinal;

	std::vector<PipelineContext> m_Contexts; 
	
	IRTexture2DPtr m_pShadowMap;
	gmtl::Vec2f m_TexOffsets;

public: 
	CSHFXCascadedDirLight(CSHFXShadowFX* pShadowFX)
		: m_pShadowFX(pShadowFX)
		, m_PoissonParams(NULL)
		, m_GaussianParamsV(NULL)
		, m_GaussianParamsH(NULL)
	{
		Initialize();
	}

	virtual ~CSHFXCascadedDirLight()
	{
		if(m_GaussianParamsH)
			_DELETE_ARRAY(m_GaussianParamsH);

		if(m_GaussianParamsV)
			_DELETE_ARRAY(m_GaussianParamsV);

		if(m_PoissonParams)
			_DELETE_ARRAY(m_PoissonParams);
	}

private:
	void Initialize();
	void SetCascadedProj(gmtl::VecA4f* frustumPtsWorldSpace, gmtl::MatrixA44f& lightView, gmtl::VecA3f& lightPosViewSpace);
	void GetLightViewParams(const gmtl::Vec3f& lightDir, 
							const gmtl::Vec3f& lightPos, 										
							gmtl::MatrixA44f& outLightView,
							gmtl::VecA3f& outLightViewPos);
	
	void ComputeMinimalBoundingSphere(gmtl::VecA4f& dest, const gmtl::VecA4f* pPoints, uint numPoints);
	void UpdateZSplits(float nearBiasExponent, float nearViewZ, float farViewZ, float farNearRatio);

public:
	void UpdateCascadedFrustumBounds(SHFXCascadedFrustumBounds& dest, float nearBiasExponent, const gmtl::Matrix44f& proj, float expSteepNess);
	//void UpdateCascadedProj(const gmtl::Vec3f& lightDir, 
	//						const gmtl::Vec3f& lightPos, 
	//						const gmtl::Matrix44f& view, 
	//						const gmtl::Matrix44f& proj);
											 
	//void Update(float nearBiasExponent,
	//			const gmtl::Vec3f& lightDir, 
	//			const gmtl::Vec3f& lightPos, 
	//			const gmtl::Matrix44f& view, 
	//			const gmtl::Matrix44f& proj);

	void Update(const SHFXCascadedFrustumBounds& frustum, const gmtl::Vec3f& lightDir, const gmtl::Vec3f& lightPos, const gmtl::Matrix44f& view);

	const SHFXCascadedSplitZ& GetSplitZ() { return m_Split; }
	const SHFXCascadedSplitZ& GetSplitZView() { return m_SplitView; }
	const SHFXCascadedProj& GetProj() { return m_Proj; }
	const SHFXCascadedProj& GetProjCulling() { return m_ProjAABB; }
	const SHFXCascadedProj& GetProjWithTexOffset() { return m_ProjWithTexOffset; }
	
	//IRTextureRT*	GetShadowMap()			{ return m_pCasterGroups[0]->GetRenderTarget(0); }	
	//IRTextureRT*	GetShadowMapBackup()	{ return m_pGaussianHGroup->GetRenderTarget(0); }					
	IRTexture2D*	GetShadowMap()			{ return m_pShadowMap; }					
	IRDepthBuffer*	GetDepthBuffer()		{ return m_pCasterGroups[0]->GetDepthBuffer(); }
												
	IRRenderGroup* GetCasterGroup(uint cascade);

	void RenderCascadeDirLightCaster(BFXRenderContext& context);
	void Flush();

	uint AddContext();
	uint GetNumOfContexts();
	void ResetContext(uint contextID);

	//void FlushCasterGroups();
	//void FlushFilterGroups();

	void SetShadowMapParamsGaussianFilter(IRTexture2D* pSM, IRTexture2D* pBackup, IRTexture2D* pFinal, IRDepthBuffer* pDepth, float* pStdDevs, uint* pSpreadKernelSizes);
	void SetShadowMapParamsPoissonFilter(IRTexture2D* pSM, IRTexture2D* pFinal, IRDepthBuffer* pDepth, eBFXUPoissonBlurSamples* pBlurSamples, float* pSpreadScales, boolean isUseNoiseBlur);
	void SetShadowMapParams(IRTexture2D* pFinal, IRDepthBuffer* pDepth);

	gmtl::Vec2f* GetTexOffsets() { return &m_TexOffsets; }

	_IMPL_ALIGNED_ALLOCS(16)
};

_NAMESPACE_END
