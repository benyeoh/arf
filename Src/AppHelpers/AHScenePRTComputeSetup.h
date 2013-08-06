//================================================================================
//
//		AHScenePRTComputeSetup.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/24/2012
//
//================================================================================

#pragma once

#if defined(_IS_USE_APPHELPER_LIB_PRTCOMPUTE)

_NAMESPACE_BEGIN

class AHScenePRTComputeSetup : public CRefCounted<IRefCounted>
{
	struct MurmurHashSizeTObj
	{
		inline uint operator() (size_t val) const { return MurmurHashSizeT(val); }
	};

	struct CascadeDirLightEntry
	{
		AHDirLightComponent* pComp;
		ISHFXCascadedDirLight* pOrig;
		ISHFXCascadedDirLight* pSubst;

		float origPosOffset;
		float origCascadeExp;
		boolean origIsShadowMapShared;
		BFXUMeshList* pOrigMeshList;
		IDEFXLightGroup* pOrigLG;
		IDEFXLightGroup* pSubstLG;

		gmtl::Vec3f lastShadowPos;
	};

	struct CubePointLightEntry
	{
		AHPointLightComponent* pComp;
		ISHFXCubePointLight* pOrig;
		ISHFXCubePointLight* pSubst;

		boolean origIsShadowMapShared;
		BFXUMeshList* pOrigMeshList;
		IDEFXLightGroup* pOrigLG;
		IDEFXLightGroup* pSubstLG;

		boolean isRendered;
	};

	typedef DenseChainedHashMap<size_t, CascadeDirLightEntry, MurmurHashSizeTObj> CascadedDirLightMap;
	typedef DenseChainedHashMap<size_t, CubePointLightEntry, MurmurHashSizeTObj> CubePointLightMap;

private:
	CascadedDirLightMap		m_CascadedDirLightMap;
	CubePointLightMap		m_CubePointLightMap;

	ISHFXShadowFX* m_pShadowFX;
	IRRenderer* m_pRenderer;
	IDEFXDeferredFX* m_pDeferredFX;

	SHFXCascadedFrustumBounds m_Bounds;
	float m_ViewDistance;

	BFXUMeshListPtr m_pDirLightMesh;
	BFXUMeshListPtr m_pPointLightMesh;

public:
	AHScenePRTComputeSetup(IRRenderer* pRenderer, ISHFXShadowFX* pShadowFX, IDEFXDeferredFX* pDeferredFX, BFXUMeshList* pDirLightMesh, BFXUMeshList* pPointLightMesh)
		: m_pShadowFX(pShadowFX)
		, m_pRenderer(pRenderer)
		, m_pDeferredFX(pDeferredFX)
		, m_ViewDistance(0.0f)
	{
		m_CascadedDirLightMap.Initialize(32, 0.7f);
		m_CubePointLightMap.Initialize(128, 0.7f);

		m_pDirLightMesh = pDirLightMesh;
		m_pPointLightMesh = pPointLightMesh;
	}

	~AHScenePRTComputeSetup()
	{
		ResetLights();
	}

private:
	void SetupPointLight(AHPointLightComponent* pComp, uint numContexts);
	void SetupDirLight(AHDirLightComponent* pComp, uint numContexts);

public:
	void ResetLightsNoDestroy();
	void ResetLights();
	void SetupLightsForPRTCompute(ISDBSceneContainer* pScene, float camViewDistance, float cascadeViewExtra, uint numContexts);

	void UpdatePLLightGroupRT(AHPointLightComponent* pComp, IRTextureRT* pRT, IRDepthBuffer* pDB);
	void UpdateDLLightGroupRT(AHDirLightComponent* pComp, IRTextureRT* pRT, IRDepthBuffer* pDB);

	boolean CheckUpdateCascadeDirLightShadow(AHDirLightComponent* pComp, const gmtl::VecA3f& camPos);
	boolean CheckUpdateCubePointLightShadow(AHPointLightComponent* pComp);
};

_DECLARE_SMARTPTR(AHScenePRTComputeSetup)

_NAMESPACE_END

#endif