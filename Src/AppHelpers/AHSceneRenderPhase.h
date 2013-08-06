//================================================================================
//
//		AHSceneRenderPhase.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		10/17/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

#if defined(_IS_USE_APPHELPER_LIB_BASIC)

class AHSceneRenderPhase : public CRefCounted<IRefCounted>
{
	const static uint MAX_FRUSTUM_CULL = 32768;
	
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
		AHSceneRenderPhase* pOwner;
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
		AHSceneRenderPhase* pOwner;
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
		AHSceneRenderPhase* pOwner;
		AHComponent** ppComp;
		AABox* pBounds;
		uint numComp;

		AHDirLightComponent* pDL;
		uint cascadeIndex;

		int Run();
	};

	struct SceneRenderDLShadowCullComp : public IPRunnable
	{
		AHSceneRenderPhase* pOwner;
		AHDirLightComponent* pDL;
		const gmtl::MatrixA44f* pCascadeFrustum;
		uint cascadeIndex;

		int Run();
	};

	struct SceneRenderProcessComp : public IPRunnable	
	{
		AHSceneRenderPhase* pOwner;
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

	typedef std::vector<PLLightGroup> PLList;
	typedef std::vector<DLLightGroup> DLList;

private:
	// All simd/cache aligned members
	FrustumCullStore	m_FrustumStoreMain;
	
	gmtl::MatrixA44f*	m_pView;
	gmtl::MatrixA44f*	m_pProj;
	gmtl::MatrixA44f	m_ViewProj;
	gmtl::MatrixA44f	m_CascadeProj;
	float				m_NearZ;
	float				m_FarZ;
	gmtl::VecA4f		m_CamPos;

	// Others
	IPThreadPoolPtr		m_pThreadPool;

	IRRendererPtr		m_pRenderer;
	IBFXBaseFXPtr		m_pBaseFX;
	IDEFXDeferredFXPtr	m_pDeferredFX;
	ISHFXShadowFXPtr	m_pShadowFX;

	BFXRenderContainer	m_RenderContainer;
	IBFXPipelinePtr		m_pBFXPipeline;
	IDEFXPipelinePtr	m_pDEFXPipeline;
	ISHFXPipelinePtr	m_pSHFXPipeline;
	
	BFXParamContainer*	m_ParamContainers;
	IBFXParamPoolPtr*	m_pBFXParamPools;
	IDEFXParamPoolPtr*	m_pDEFXParamPools;
	ISHFXParamPoolPtr*	m_pSHFXParamPools;

	ISDBSceneContainerPtr m_pScene;

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

	IBFXMaterialCreateCallback* m_pMatCreateCallback;

public:
	AHSceneRenderPhase(IRRenderer* pRenderer, IBFXBaseFX* pBaseFX, IDEFXDeferredFX* pDeferredFX, ISHFXShadowFX* pShadowFX, IPThreadPool* pThreadPool,
						IBFXPipeline* pBFXPipeline, IDEFXPipeline* pDEFXPipeline, ISHFXPipeline* pSHFXPipeline, IBFXMaterialCreateCallback* pMatCreateCallback,
						IRTexture2D* pZPos, IRTexture2D* pNormals, IRTexture2D* pAlbedo,
						ISDBSceneContainer* pScene, gmtl::MatrixA44f* pView, gmtl::MatrixA44f* pProj, float nearZ, float farZ);

	~AHSceneRenderPhase();

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

	IBFXMaterialCreateCallback* GetMatCreateCallback() { return m_pMatCreateCallback; }

	gmtl::MatrixA44f& GetView()					{ return *m_pView; }
	gmtl::MatrixA44f& GetProj()					{ return *m_pProj; }
	gmtl::MatrixA44f& GetCascadeShadowProj()	{ return m_CascadeProj; }
	gmtl::VecA4f& GetCamPos()					{ return m_CamPos; }

public:
	uint GetNumOfContexts() { return m_NumContexts; }

	IBFXBaseFX* GetBaseFX() { return m_pBaseFX; }
	BFXParamContainer&	GetParamContainer(uint context) { return m_ParamContainers[context]; }
	BFXRenderContainer& GetRenderContainer() { return m_RenderContainer; }
	ISDBSceneContainer* GetSceneContainer() { return m_pScene; }
	IPThreadPool* GetThreadPool() { return m_pThreadPool; }

	IBFXParamPool* GetBFXParamPool(uint context) { return m_pBFXParamPools[context]; }
	IDEFXParamPool* GetDEFXParamPool(uint context) { return m_pDEFXParamPools[context]; }
	ISHFXParamPool* GetSHFXParamPool(uint context) { return m_pSHFXParamPools[context]; }

	void SetScene(ISDBSceneContainer* pScene);
	void SetView(gmtl::MatrixA44f* pView);
	void SetProj(gmtl::MatrixA44f* pProj, float nearZ, float farZ);
	void SetCascadeShadowProj(gmtl::MatrixA44f& cascadeProj);

	void Update();
	void Flush();

	_IMPL_ALIGNED_ALLOCS(16)
};

_DECLARE_SMARTPTR(AHSceneRenderPhase)

#endif

_NAMESPACE_END