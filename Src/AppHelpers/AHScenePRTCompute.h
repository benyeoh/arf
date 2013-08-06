//================================================================================
//
//		AHScenePRTCompute.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/27/2012
//
//================================================================================

#pragma once

#if defined(_IS_USE_APPHELPER_LIB_PRTCOMPUTE)

_NAMESPACE_BEGIN

class AHScenePRTCompute : public CRefCounted<IRefCounted>, public ISHPRTComputeOccludedCallback
{
	struct PRTMeshEntry
	{
		BFXMesh* pMesh;
		AHRenderComponent* pRenderComp;

		IRVertexBuffer* pTempVB;
	};

private:
	AHScenePRTComputePhasePtr m_pComputePhase;
	AHScenePRTComputeSetupPtr m_pComputeSetup;

	IRRendererPtr		m_pRenderer;
	IBFXBaseFXPtr		m_pBaseFX;
	IDEFXDeferredFXPtr	m_pDeferredFX;
	ISHFXShadowFXPtr	m_pShadowFX;
	ISHPRTComputePtr	m_pSHPRTCompute;

	IPThreadPoolPtr		m_pRenderPhaseThreadPool;
	IPThreadPoolPtr		m_pProcessPRTThreadPool;

	IBFXPipelinePtr		m_pBFXPipeline;
	IDEFXPipelinePtr	m_pDEFXPipeline;
	ISHFXPipelinePtr	m_pSHFXPipeline;
	AHPRTComputePipelinePtr m_pAHPRTComputePipeline;

	IRTexture2DPtr	m_pZPos;
	IRTexture2DPtr	m_pNormal;
	IRTexture2DPtr	m_pAlbedo;

public:
	AHScenePRTCompute(IRRenderer* pRenderer, IBFXBaseFX* pBaseFX, IDEFXDeferredFX* pDeferredFX, ISHFXShadowFX* pShadowFX, ISHPRTCompute* pSHPRTCompute, 
					IPThreadPool* pRenderPhaseThreadPool, IPThreadPool* pProcessPRTThreadPool, BFXUMeshList* pDirLightMesh, BFXUMeshList* pPointLightMesh)
	{
		m_pRenderer		= pRenderer;
		m_pBaseFX		= pBaseFX;
		m_pDeferredFX	= pDeferredFX;
		m_pShadowFX		= pShadowFX;
		m_pSHPRTCompute	= pSHPRTCompute;
		m_pRenderPhaseThreadPool	= pRenderPhaseThreadPool;
		m_pProcessPRTThreadPool		= pProcessPRTThreadPool;

		m_pBFXPipeline	= m_pBaseFX->GetResourceMgr().CreatePipeline();
		m_pDEFXPipeline	= m_pDeferredFX->GetResourceMgr().CreatePipeline();
		m_pSHFXPipeline = m_pShadowFX->GetResourceMgr().CreatePipeline();
		m_pAHPRTComputePipeline = _NEW AHPRTComputePipeline();
		m_pAHPRTComputePipeline->Initialize(m_pRenderer);

		_LOOPi(m_pRenderPhaseThreadPool->GetNumThreads())
		{
			m_pBFXPipeline->AddContext();
			m_pDEFXPipeline->AddContext();
			m_pAHPRTComputePipeline->AddContext();
		}

		m_pZPos = m_pRenderer->GetRResourceMgr().CreateTexture2DRT(SHPRT_COMPUTE_OCCL_RT_SIZE, SHPRT_COMPUTE_OCCL_RT_SIZE, 1, TEXF_R32F);
		m_pNormal = m_pRenderer->GetRResourceMgr().CreateTexture2DRT(SHPRT_COMPUTE_OCCL_RT_SIZE, SHPRT_COMPUTE_OCCL_RT_SIZE, 1, TEXF_A8R8G8B8);
		m_pAlbedo = m_pRenderer->GetRResourceMgr().CreateTexture2DRT(SHPRT_COMPUTE_OCCL_RT_SIZE, SHPRT_COMPUTE_OCCL_RT_SIZE, 1, TEXF_A8R8G8B8);

		m_pComputeSetup = _NEW AHScenePRTComputeSetup(pRenderer, pShadowFX, pDeferredFX, pDirLightMesh, pPointLightMesh);
		m_pComputePhase = _NEW AHScenePRTComputePhase(pRenderer, pBaseFX, pDeferredFX, pShadowFX, pRenderPhaseThreadPool, 
													m_pBFXPipeline, m_pDEFXPipeline, m_pSHFXPipeline, m_pAHPRTComputePipeline, 
													m_pZPos, m_pNormal, m_pAlbedo, NULL, 200.0f, m_pComputeSetup);
	}

	~AHScenePRTCompute()
	{
	}

private:
	int GetTransferVBIndex(IRVertexBufferGroup* pVBGroup);
	uint InitializeMeshes(ISDBSceneContainer* pScene, boolean isForceClear, PRTMeshEntry* pDest, uint bufferSize);

public:
	void Render(const gmtl::VecA3f& loc, const gmtl::MatrixA44f& paraView, BFXRenderContainer& renderContainer, BFXParamContainer& paramContainer, IRTexture2D* pRT, IRDepthBuffer* pDB);

	void Compute(ISDBSceneContainer* pScene, float rayViewDistance, float cascadeShadowDistBuffer, uint numBounces, float posBias, float selfOcclScale, uint numFilter, float dotThreshold, boolean isForceClear);
	IRTexture2D* ComputeDebug(ISDBSceneContainer* pScene, const gmtl::VecA3f& pos, const gmtl::VecA3f& dir, float rayViewDistance, float cascadeShadowDistBuffer);
};

_DECLARE_SMARTPTR(AHScenePRTCompute)

_NAMESPACE_END

#endif