//================================================================================
//
//		AHScenePRTComputePhase.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/23/2012
//
//================================================================================

#pragma once

#if defined(_IS_USE_APPHELPER_LIB_PRTCOMPUTE)

_NAMESPACE_BEGIN

class AHScenePRTComputePhase : public CRefCounted<IRefCounted>
{
	const static uint MAX_FRUSTUM_CULL = 4096;

	// Minimum number of ops to spawn a thread
	const static uint MIN_NUM_OPS = 16;			// For main render
	const static uint MIN_NUM_PL_OPS = 32;		// Point light ops
	const static uint MIN_NUM_DL_OPS = 32;		// Dir light ops

	struct FrustumCullStore
	{
		void* res[MAX_FRUSTUM_CULL];
		AABox ssbounds[MAX_FRUSTUM_CULL];
	};

	struct PLLightGroup
	{
		ISHFXCubePointLight* pShadow;
		IDEFXLightGroup* pLG;
	};

	struct DLLightGroup
	{
		ISHFXCascadedDirLight* pShadow;
		IDEFXLightGroup* pLG;
	};

	struct SceneRenderPLShadowProcessComp : public IPRunnable
	{
		AHScenePRTComputePhase* pOwner;
		AHComponent** ppComp;
		uint numComp;
		AABox* pBounds;

		AHPointLightComponent* pPL;
		eRCubeFace face;

		//UncachedMat* pUncachedMats;
		//uint* pNumUncached;

		int Run();
	};

	struct SceneRenderPLShadowCullComp : public IPRunnable
	{
		AHScenePRTComputePhase* pOwner;
		AHComponent** ppComp;
		uint numComp;

		AHPointLightComponent* pPL;
		const gmtl::MatrixA44f* pPLFrustum;
		eRCubeFace face;
		float nearPlane;
		float farPlane;

		//UncachedMat* pUncachedMats;
		//uint* pNumUncached;

		int Run();
	};

	struct SceneRenderDLShadowProcessComp : public IPRunnable
	{
		AHScenePRTComputePhase* pOwner;
		AHComponent** ppComp;
		AABox* pBounds;
		uint numComp;

		AHDirLightComponent* pDL;
		uint cascadeIndex;

		int Run();
	};

	struct SceneRenderDLShadowCullComp : public IPRunnable
	{
		AHScenePRTComputePhase* pOwner;
		AHDirLightComponent* pDL;
		const gmtl::MatrixA44f* pCascadeFrustum;
		uint cascadeIndex;

		int Run();
	};

	struct SceneRenderProcessComp : public IPRunnable	
	{
		AHScenePRTComputePhase* pOwner;
		AHComponent** ppComp;
		AABox* pBounds;
		uint numComp;

		//UncachedMat* pUncachedMats;
		//uint* pNumUncached;

		void ProcessRenderComp(AHRenderComponent* pRenderComp, AABox& curBounds, uint context);
		void ProcessPointLightComp(AHPointLightComponent* pPointLightComp, uint context);
		void ProcessDirLightComp(AHDirLightComponent* pDirLightComp, uint context);

		int Run();
	};

	struct AllocStoreFrustum
	{
		std::vector<FrustumCullStore*> store;
		uint numInUse;
	};

	//typedef std::vector<FrustumCullStore*> AllocStoreFrustum;
	//typedef ObjectPoolGrowable<FrustumCullStore, 1> AllocStoreFrustum;
	typedef ObjectPoolGrowable<SceneRenderPLShadowProcessComp, 32> AllocStorePLShadowProcessComp;
	typedef ObjectPoolGrowable<SceneRenderDLShadowProcessComp, 32> AllocStoreDLShadowProcessComp;
	typedef ObjectPoolGrowable<SceneRenderDLShadowCullComp, 8> AllocStoreDLShadowCullComp;
	typedef ObjectPoolGrowable<SceneRenderPLShadowCullComp, 8> AllocStorePLShadowCullComp;
	typedef ObjectPoolGrowable<SceneRenderProcessComp, 32> AllocStoreProcessComp;

	typedef ObjectPoolGrowable<gmtl::Matrix44f, 256> AllocStoreLightMatrix;

	typedef std::vector<PLLightGroup> PLList;
	typedef std::vector<DLLightGroup> DLList;

private:
	// All simd/cache aligned members
	FrustumCullStore	m_FrustumStoreMain;

	float				m_FarZ;
	gmtl::VecA4f		m_CamPos;
	const gmtl::MatrixA44f*	m_pParaView;

	// Others
	IPThreadPoolPtr		m_pThreadPool;

	IRRendererPtr		m_pRenderer;
	IBFXBaseFXPtr		m_pBaseFX;
	IDEFXDeferredFXPtr	m_pDeferredFX;
	ISHFXShadowFXPtr	m_pShadowFX;

	IBFXPipelinePtr		m_pBFXPipeline;
	IDEFXPipelinePtr	m_pDEFXPipeline;
	ISHFXPipelinePtr	m_pSHFXPipeline;

	BFXRenderContainer* m_pRenderContainer;
	BFXParamContainer*	m_ParamContainers;
	IBFXParamPoolPtr*	m_pBFXParamPools;
	IDEFXParamPoolPtr*	m_pDEFXParamPools;
	ISHFXParamPoolPtr*	m_pSHFXParamPools;

	ISDBSceneContainerPtr m_pScene;

	IRTextureRT* m_pRT;
	IRDepthBuffer* m_pDB;

	PLList**	m_ppPLContexts;
	DLList**	m_ppDLContexts;
	uint		m_NumContexts;

	// TODO: All these need to be cache aligned
	AllocStoreProcessComp	m_ProcessCompStoreMain;

	AllocStoreFrustum*				m_pFrustumStores;
	AllocStorePLShadowProcessComp*	m_pPLShadowProcessCompStores;
	AllocStoreDLShadowProcessComp*	m_pDLShadowProcessCompStores;
	AllocStoreDLShadowCullComp*		m_pDLShadowCullStores;
	AllocStorePLShadowCullComp*		m_pPLShadowCullStores;
	AllocStoreLightMatrix*			m_pLightMatrixStores;

	AHMaterialCreateCallbackImmedPtr	m_pMatCreateCallback;
	AHScenePRTComputeSetupPtr	m_pSetup;
	AHPRTComputePipelinePtr		m_pAHPRTComputePipeline;

public:
	AHScenePRTComputePhase(IRRenderer* pRenderer, IBFXBaseFX* pBaseFX, IDEFXDeferredFX* pDeferredFX, ISHFXShadowFX* pShadowFX, IPThreadPool* pThreadPool,
		IBFXPipeline* pBFXPipeline, IDEFXPipeline* pDEFXPipeline, ISHFXPipeline* pSHFXPipeline, AHPRTComputePipeline* pAHPRTComputePipeline,
		IRTexture2D* pZPos, IRTexture2D* pNormals, IRTexture2D* pAlbedo,
		ISDBSceneContainer* pScene, float viewDistance, AHScenePRTComputeSetup* pSetup);

	~AHScenePRTComputePhase();

private:
	void GetFrustumAA(const gmtl::MatrixA44f& viewProj, AABox& dest);

	FrustumCullStore* GetFrustumStore(uint context);
	SceneRenderDLShadowProcessComp* GetDLShadowProcessComp(uint context);
	SceneRenderPLShadowProcessComp* GetPLShadowProcessComp(uint context);
	SceneRenderDLShadowCullComp* GetDLShadowCullComp(uint context);
	SceneRenderPLShadowCullComp* GetPLShadowCullComp(uint context);
	SceneRenderProcessComp* GetProcessComp();
	void AddPLContext(uint context, const PLLightGroup& plGroup);
	void AddDLContext(uint context, const DLLightGroup& dlGroup);
	gmtl::Matrix44f& GetLightMatrix(uint context);

	IBFXMaterialCreateCallback* GetMatCreateCallback()	{ return m_pMatCreateCallback; }
	gmtl::VecA4f& GetCamPos()							{ return m_CamPos; }
	const gmtl::MatrixA44f& GetParaView()				{ return *m_pParaView; }
	AHScenePRTComputeSetup* GetSetup()					{ return m_pSetup; }

	void ComputePointLightMatrix(gmtl::Matrix44f& dest, const gmtl::MatrixA44f& paraView, AHPointLightComponent* pComp); 
	
	IRTextureRT* GetCurRT() { return m_pRT; }
	IRDepthBuffer* GetCurDB() { return m_pDB; }

public:
	uint GetNumOfContexts() { return m_NumContexts; }

	IBFXBaseFX* GetBaseFX() { return m_pBaseFX; }
	BFXParamContainer&	GetParamContainer(uint context) { return m_ParamContainers[context]; }
	BFXRenderContainer& GetRenderContainer() { return *m_pRenderContainer; }
	ISDBSceneContainer* GetSceneContainer() { return m_pScene; }
	IPThreadPool* GetThreadPool() { return m_pThreadPool; }

	IBFXParamPool* GetBFXParamPool(uint context) { return m_pBFXParamPools[context]; }
	IDEFXParamPool* GetDEFXParamPool(uint context) { return m_pDEFXParamPools[context]; }
	ISHFXParamPool* GetSHFXParamPool(uint context) { return m_pSHFXParamPools[context]; }

	void SetScene(ISDBSceneContainer* pScene);
	void SetViewDistance(float viewDist);

	void Update(const gmtl::VecA3f& loc, const gmtl::MatrixA44f& paraView, BFXRenderContainer& renderContainer, BFXParamContainer& paramContainer, IRTextureRT* pRT, IRDepthBuffer* pDB);
	void Flush();

	_IMPL_ALIGNED_ALLOCS(16)
};

_DECLARE_SMARTPTR(AHScenePRTComputePhase)

_NAMESPACE_END

#endif