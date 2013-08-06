//================================================================================
//
//		AHSceneRenderPhase.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		10/17/2012
//
//================================================================================

#include "AppHelpers.h"

_NAMESPACE_BEGIN

#if defined(_IS_USE_APPHELPER_LIB_BASIC)

#define __EXEC(pComp)			pComp->Run()
#define __THREAD_EXEC(pComp)	GetThreadPool()->QueueJob(*pComp)

AHSceneRenderPhase::AHSceneRenderPhase(IRRenderer* pRenderer, IBFXBaseFX* pBaseFX, IDEFXDeferredFX* pDeferredFX, ISHFXShadowFX* pShadowFX, IPThreadPool* pThreadPool,
										IBFXPipeline* pBFXPipeline, IDEFXPipeline* pDEFXPipeline, ISHFXPipeline* pSHFXPipeline, IBFXMaterialCreateCallback* pMatCreateCallback,
										IRTexture2D* pZPos, IRTexture2D* pNormals, IRTexture2D* pAlbedo,
										ISDBSceneContainer* pScene, gmtl::MatrixA44f* pView, gmtl::MatrixA44f* pProj, float nearZ, float farZ)
{
	m_pRenderer		= pRenderer;
	m_pBaseFX		= pBaseFX;
	m_pDeferredFX	= pDeferredFX;
	m_pShadowFX		= pShadowFX;

	m_pThreadPool	= pThreadPool;

	m_pBFXPipeline	= pBFXPipeline;
	m_pDEFXPipeline	= pDEFXPipeline;
	m_pSHFXPipeline	= pSHFXPipeline;

	m_pMatCreateCallback = pMatCreateCallback;
	_DEBUG_ASSERT(m_pMatCreateCallback);

	m_RenderContainer.SetRenderCallback(m_pBFXPipeline, _GET_LIB_INDEX(BFX_EFFECT_PARAM_OFFSET));
	m_RenderContainer.SetRenderCallback(m_pDEFXPipeline, _GET_LIB_INDEX(DEFX_EFFECT_PARAM_OFFSET));
	m_RenderContainer.SetRenderCallback(m_pSHFXPipeline, _GET_LIB_INDEX(SHFX_EFFECT_PARAM_OFFSET));

	m_NumContexts = m_pThreadPool->GetNumThreads() + 1;
	
	_DEBUG_ASSERT(m_pBFXPipeline->GetNumOfContexts() <= m_NumContexts);
	_DEBUG_ASSERT(m_pDEFXPipeline->GetNumOfContexts() <= m_NumContexts);

	//// Create as many contexts as there are threads
	//_LOOPi(m_NumContexts - 1)
	//{
	//	m_pBFXPipeline->AddContext();
	//	m_pDEFXPipeline->AddContext();
	//	//m_pSHFXPipeline->AddContext();
	//}

	m_ParamContainers = _NEW BFXParamContainer[m_NumContexts];
	m_pBFXParamPools = _NEW IBFXParamPoolPtr[m_NumContexts];
	m_pDEFXParamPools = _NEW IDEFXParamPoolPtr[m_NumContexts];
	m_pSHFXParamPools = _NEW ISHFXParamPoolPtr[m_NumContexts];
	m_pFrustumStores = _NEW AllocStoreFrustum[m_NumContexts];
	m_pPLShadowProcessCompStores = _NEW AllocStorePLShadowProcessComp[m_NumContexts];
	m_pDLShadowProcessCompStores = _NEW AllocStoreDLShadowProcessComp[m_NumContexts];
	m_pDLShadowCullStores = _NEW AllocStoreDLShadowCullComp[m_NumContexts];
	m_pPLShadowCullStores = _NEW AllocStorePLShadowCullComp[m_NumContexts];

	m_ppPLContexts		= _NEW PLList*[m_NumContexts];
	m_ppDLContexts		= _NEW DLList*[m_NumContexts];

	//IRTexture2D* pZPos;
	//IRTexture2D* pNormals; 
	//IRTexture2D* pAlbedo;
	//uint numExtra = 0;
	//m_pDEFXPipeline->GetAccumulationTargets(&pZPos, &pNormals, &pAlbedo, NULL, numExtra);

	IBFXSharedParamData* pSharedParam = pBaseFX->GetResourceMgr().CreateSharedParamData();

	_LOOPi(m_NumContexts)
	{
		IBFXParamPool* pBFXParamPool = m_pBaseFX->GetResourceMgr().CreateParamPool(pSharedParam);
		IDEFXParamPool* pDEFXParamPool = m_pDeferredFX->GetResourceMgr().CreateParamPool();
		ISHFXParamPool* pSHFXParamPool = m_pShadowFX->GetResourceMgr().CreateParamPool();

		pBFXParamPool->SetCurrentContext(i);
		pDEFXParamPool->SetAccumulationBuffers(pZPos, pNormals, pAlbedo);
	
		m_pBFXParamPools[i] = pBFXParamPool;
		m_pDEFXParamPools[i] = pDEFXParamPool;
		m_pSHFXParamPools[i] = pSHFXParamPool;

		m_ParamContainers[i].SetParamCallback(pBFXParamPool, _GET_LIB_INDEX(BFX_EFFECT_PARAM_OFFSET));
		m_ParamContainers[i].SetParamCallback(pDEFXParamPool, _GET_LIB_INDEX(DEFX_EFFECT_PARAM_OFFSET));
		m_ParamContainers[i].SetParamCallback(pSHFXParamPool, _GET_LIB_INDEX(SHFX_EFFECT_PARAM_OFFSET));

		m_ppPLContexts[i] = _ALIGNED_NEW(_CACHE_LINE_SIZE, PLList) PLList;
		m_ppDLContexts[i] = _ALIGNED_NEW(_CACHE_LINE_SIZE, DLList) DLList;

		m_pFrustumStores[i].store.push_back(_ALIGNED_NEW(16, FrustumCullStore) FrustumCullStore);
		m_pFrustumStores[i].numInUse = 0;
		//m_pFrustumStores[i] = _NEW AllocStoreFrustum();
		//m_pFrustumStores[i]->SetSize(1);

	}

	SetView(pView);
	SetProj(pProj, nearZ, farZ);
	SetCascadeShadowProj(*pProj);
	SetScene(pScene);
}

AHSceneRenderPhase::~AHSceneRenderPhase()
{
	_LOOPi(m_NumContexts)
	{
		m_pBFXParamPools[i] = NULL;
		m_pDEFXParamPools[i] = NULL;
		m_pSHFXParamPools[i] = NULL;

		//_DELETE(m_pFrustumStores[i]);
		_ALIGNED_DELETE(m_ppPLContexts[i], PLList);
		_ALIGNED_DELETE(m_ppDLContexts[i], DLList);

		_LOOPj(m_pFrustumStores[i].store.size())
		{
			_ALIGNED_DELETE(m_pFrustumStores[i].store[j], FrustumCullStore);
		}
	}

	_DELETE_ARRAY(m_ppPLContexts);
	_DELETE_ARRAY(m_ppDLContexts);
	_DELETE_ARRAY(m_pPLShadowProcessCompStores);
	_DELETE_ARRAY(m_pDLShadowProcessCompStores);
	_DELETE_ARRAY(m_pDLShadowCullStores);
	_DELETE_ARRAY(m_pPLShadowCullStores);
	_DELETE_ARRAY(m_pFrustumStores);
	_DELETE_ARRAY(m_pBFXParamPools);
	_DELETE_ARRAY(m_pDEFXParamPools);
	_DELETE_ARRAY(m_pSHFXParamPools);
	_DELETE_ARRAY(m_ParamContainers);
}

void AHSceneRenderPhase::SetScene(ISDBSceneContainer* pScene)
{
	m_pScene = pScene;
}

void AHSceneRenderPhase::GetFrustumAA(const gmtl::MatrixA44f& viewProj, AABox& dest)
{
	gmtl::MatrixA44f invViewProj;
	gmtl::invert(invViewProj, viewProj);

	gmtl::VecA4f screenSpaceBounds[8];
	screenSpaceBounds[0][0] = -1.0f; screenSpaceBounds[0][1] = -1.0f; screenSpaceBounds[0][2] = 0.0f; screenSpaceBounds[0][3] = 1.0f; // near bottom left
	screenSpaceBounds[1][0] = -1.0f; screenSpaceBounds[1][1] = 1.0f; screenSpaceBounds[1][2] = 0.0f; screenSpaceBounds[1][3] = 1.0f; // near top left
	screenSpaceBounds[2][0] = 1.0f; screenSpaceBounds[2][1] = -1.0f; screenSpaceBounds[2][2] = 0.0f; screenSpaceBounds[2][3] = 1.0f; // near bottom right
	screenSpaceBounds[3][0] = 1.0f; screenSpaceBounds[3][1] = 1.0f; screenSpaceBounds[3][2] = 0.0f; screenSpaceBounds[3][3] = 1.0f; // near top right

	screenSpaceBounds[4][0] = -1.0f; screenSpaceBounds[4][1] = -1.0f; screenSpaceBounds[4][2] = 1.0f; screenSpaceBounds[4][3] = 1.0f; // far bottom left
	screenSpaceBounds[5][0] = -1.0f; screenSpaceBounds[5][1] = 1.0f; screenSpaceBounds[5][2] = 1.0f; screenSpaceBounds[5][3] = 1.0f; // far top left
	screenSpaceBounds[6][0] = 1.0f; screenSpaceBounds[6][1] = -1.0f; screenSpaceBounds[6][2] = 1.0f; screenSpaceBounds[6][3] = 1.0f; // far bottom right
	screenSpaceBounds[7][0] = 1.0f; screenSpaceBounds[7][1] = 1.0f; screenSpaceBounds[7][2] = 1.0f; screenSpaceBounds[7][3] = 1.0f; // far top right

	gmtl::VecA4f frustumPtsWorldSpace[8];
	BatchTransformAndProjectVecW1(&invViewProj, screenSpaceBounds, frustumPtsWorldSpace, 8);

	PointsToAABox((gmtl::VecA3f*) frustumPtsWorldSpace, 8, &dest);
}

void AHSceneRenderPhase::Update()
{
	// Frustum cull
	MatMatMult(&m_ViewProj, m_pProj, m_pView);
	AABox frustumAA;
	GetFrustumAA(m_ViewProj, frustumAA);

	// Camera pos
	gmtl::VecA4f camPos(0.0f, 0.0f, 0.0f, 0.0f);
	gmtl::VecA4f camOffsetX;
	gmtl::VecA4f camAxisX;
	camAxisX[0] = (*m_pView)[0][0];
	camAxisX[1] = (*m_pView)[0][1];
	camAxisX[2] = (*m_pView)[0][2];

	gmtl::VecA4f camOffsetY;
	gmtl::VecA4f camAxisY;
	camAxisY[0] = (*m_pView)[1][0];
	camAxisY[1] = (*m_pView)[1][1];
	camAxisY[2] = (*m_pView)[1][2];

	gmtl::VecA4f camOffsetZ;
	gmtl::VecA4f camAxisZ;
	camAxisZ[0] = (*m_pView)[2][0];
	camAxisZ[1] = (*m_pView)[2][1];
	camAxisZ[2] = (*m_pView)[2][2];
	
	VecScalarMult(&camOffsetX, &camAxisX, -(*m_pView)[0][3]);
	VecScalarMult(&camOffsetY, &camAxisY, -(*m_pView)[1][3]);
	VecScalarMult(&camOffsetZ, &camAxisZ, -(*m_pView)[2][3]);

	VecVecAdd(&camPos, &camPos, &camOffsetX);
	VecVecAdd(&camPos, &camPos, &camOffsetY);
	VecVecAdd(&m_CamPos, &camPos, &camOffsetZ);

	// Far corner vecs
	gmtl::VecA3f cameraFarCornerVecs[4];
	gmtl::VecA3f viewFarCornerVecs[4];

	gmtl::MatrixA44f invViewProj;
	gmtl::MatrixA44f invProj;
	gmtl::MatrixA44f invView;

	gmtl::VecA3f zeroEyePos;
	zeroEyePos[0] = zeroEyePos[1] = zeroEyePos[2] = 0.0f;

	//MatMatMult(&viewProj, &g_Proj, &g_View);
	//gmtl::invertFull(invViewProj, viewProj);
	gmtl::invertFull(invProj, *m_pProj);
	invView = *m_pView;
	gmtl::setTrans(_CAST_MAT44(invView), gmtl::Vec3f(0.0f, 0.0f, 0.0f));
	gmtl::transpose(_CAST_MAT44(invView));
	MatMatMult(&invViewProj, &invView, &invProj);
	ComputeCameraFarPlaneCornerVecs(cameraFarCornerVecs, zeroEyePos, invViewProj);
	ComputeCameraFarPlaneCornerVecs(viewFarCornerVecs, zeroEyePos, invProj);

	// Set global params
	// NOTE: We are using shared params
	//_LOOPi(m_NumContexts)
	IBFXParamPool* pBFXParamPool = GetBFXParamPool(0);
	pBFXParamPool->SetViewMatrix(_CAST_MAT44(*m_pView));
	pBFXParamPool->SetProjMatrix(_CAST_MAT44(*m_pProj));
	pBFXParamPool->SetCameraProperties(_CAST_VEC3(m_CamPos), m_NearZ, m_FarZ);
	//pBFXParamPool->SetHemiLight(const BFXHemiLight& hemiLight) )	
	pBFXParamPool->FinalizeSharedParams();

	_LOOPi(m_NumContexts)
	{
		IDEFXParamPool* pDEFXParamPool = GetDEFXParamPool(i);
		pDEFXParamPool->SetCameraFarPlaneCornerVecs(cameraFarCornerVecs);
		pDEFXParamPool->SetViewFarPlaneOffsets((gmtl::Vec3f*) &(viewFarCornerVecs[0]), viewFarCornerVecs[1][0] - viewFarCornerVecs[0][0], viewFarCornerVecs[2][1] - viewFarCornerVecs[0][1]);
	}

	uint numComp = m_pScene->QueryInFrustumPersp(m_FrustumStoreMain.res, m_FrustumStoreMain.ssbounds, MAX_FRUSTUM_CULL, frustumAA, m_ViewProj, m_NearZ, m_FarZ, SDB_FILTER_RENDER_COMP | SDB_FILTER_POINTLIGHT_COMP | SDB_FILTER_DIRLIGHT_COMP);
	_DEBUG_ASSERT(numComp <= MAX_FRUSTUM_CULL);

	// Split frustum culled results to multiple threads
	if(numComp > 0)
	{		
		uint numCompPerContext = numComp / GetNumOfContexts();
		if(numCompPerContext < MIN_NUM_OPS)
			numCompPerContext = MIN_NUM_OPS;

		//uint numToDispatch = GetNumOfContexts() - 1;
		//if(numComp < (numCompPerContext * GetNumOfContexts()))
		//	numToDispatch = numComp / numCompPerContext;

		int numToDispatch = (numComp / numCompPerContext) - 1;
		if(numToDispatch < 0)
			numToDispatch = 0;

		_LOOPi(numToDispatch)
		{
			SceneRenderProcessComp* pProcessComp = GetProcessComp();
			pProcessComp->pOwner	= this;
			pProcessComp->ppComp	= (AHComponent**) &(m_FrustumStoreMain.res[i * numCompPerContext]);
			pProcessComp->numComp	= numCompPerContext;
			pProcessComp->pBounds	= &(m_FrustumStoreMain.ssbounds[i * numCompPerContext]);;
			
			//pProcessComp->Run();
			//GetThreadPool()->QueueJob(*pProcessComp);
			__THREAD_EXEC(pProcessComp);
		}

		uint numCompStart = numCompPerContext * numToDispatch;
		uint numCompLeft = numComp - numCompStart;
		SceneRenderProcessComp* pProcessComp = GetProcessComp();
		pProcessComp->pOwner	= this;
		pProcessComp->ppComp	= (AHComponent**) &(m_FrustumStoreMain.res[numCompStart]);
		pProcessComp->numComp	= numCompLeft;
		pProcessComp->pBounds	= &(m_FrustumStoreMain.ssbounds[numCompStart]);;
		
		__EXEC(pProcessComp);

		//pProcessComp->Run();
		//GetThreadPool()->QueueJob(*pProcessComp);
	}

	while(m_pThreadPool->GetNumJobsPending() > 0)
	{
		m_pThreadPool->ProcessJob();
	}
}

void AHSceneRenderPhase::Flush()
{
	// Flush deferred pipeline
	m_pDEFXPipeline->Flush();

	// Loop through all shadow/light groups per context and flush
	_LOOPi(m_NumContexts)
	{
		DLList& dlGroups = *(m_ppDLContexts[i]);
		_LOOPj(dlGroups.size())
		{
			if(dlGroups[j].pShadow)
				dlGroups[j].pShadow->Flush();
			if(dlGroups[j].pLG)
				dlGroups[j].pLG->Flush();
		}

		PLList& plGroups = *(m_ppPLContexts[i]);
		_LOOPj(plGroups.size())
		{
			if(plGroups[j].pShadow)
				plGroups[j].pShadow->Flush();
			if(plGroups[j].pLG)
				plGroups[j].pLG->Flush();
		}
	}

	// NOTE: Must either clear to black or otherwise to have separate
	// ambient pass
	// Flush the base pipeline
	m_pBFXPipeline->FlushOpaqueGroup();
	m_pBFXPipeline->FlushAlphaTestedGroup();
	m_pBFXPipeline->FlushAlphaBlendedGroup();

	// Reset
	m_ProcessCompStoreMain.Reset();

	_LOOPi(m_NumContexts)
	{
		m_pBFXParamPools[i]->ResetParams();
		m_pSHFXParamPools[i]->ResetParams();
		m_pDEFXParamPools[i]->ResetParams();

		m_pFrustumStores[i].numInUse = 0;
		//m_pFrustumStores[i].Reset();
		m_pPLShadowProcessCompStores[i].Reset();
		m_pDLShadowProcessCompStores[i].Reset();
		m_pDLShadowCullStores[i].Reset();
		m_pPLShadowCullStores[i].Reset();

		m_ppPLContexts[i]->clear();
		m_ppDLContexts[i]->clear();
	}
}

AHSceneRenderPhase::FrustumCullStore* AHSceneRenderPhase::GetFrustumStore(uint context) 
{ 
	if(m_pFrustumStores[context].numInUse >= m_pFrustumStores[context].store.size())
	{
		m_pFrustumStores[context].store.push_back( _ALIGNED_NEW(16, FrustumCullStore) FrustumCullStore );
	}

	FrustumCullStore* pToReturn = m_pFrustumStores[context].store[m_pFrustumStores[context].numInUse];
	m_pFrustumStores[context].numInUse++;
	return pToReturn;

	//return m_pFrustumStores[context].Get(); 
}

AHSceneRenderPhase::SceneRenderPLShadowProcessComp* AHSceneRenderPhase::GetPLShadowProcessComp(uint context)
{
	SceneRenderPLShadowProcessComp* pProcess = m_pPLShadowProcessCompStores[context].Get();

	// Construct vtable
	pProcess = new(pProcess) SceneRenderPLShadowProcessComp;
	return pProcess;
}

AHSceneRenderPhase::SceneRenderDLShadowProcessComp* AHSceneRenderPhase::GetDLShadowProcessComp(uint context)
{
	SceneRenderDLShadowProcessComp* pProcess = m_pDLShadowProcessCompStores[context].Get();

	// Construct vtable
	pProcess = new(pProcess) SceneRenderDLShadowProcessComp;
	return pProcess;
}

AHSceneRenderPhase::SceneRenderDLShadowCullComp* AHSceneRenderPhase::GetDLShadowCullComp(uint context)
{
	SceneRenderDLShadowCullComp* pProcess = m_pDLShadowCullStores[context].Get();

	// Construct vtable
	pProcess = new(pProcess) SceneRenderDLShadowCullComp;
	return pProcess;
}

AHSceneRenderPhase::SceneRenderPLShadowCullComp* AHSceneRenderPhase::GetPLShadowCullComp(uint context)
{
	SceneRenderPLShadowCullComp* pProcess = m_pPLShadowCullStores[context].Get();

	// Construct vtable
	pProcess = new(pProcess) SceneRenderPLShadowCullComp;
	return pProcess;
}

AHSceneRenderPhase::SceneRenderProcessComp* AHSceneRenderPhase::GetProcessComp()
{
	SceneRenderProcessComp* pProcess = m_ProcessCompStoreMain.Get();

	// Construct vtable
	pProcess = new(pProcess) SceneRenderProcessComp;
	return pProcess;
}

void AHSceneRenderPhase::AddPLContext(uint context, const PLLightGroup& plGroup)
{
	m_ppPLContexts[context]->push_back(plGroup);
}

void AHSceneRenderPhase::AddDLContext(uint context, const DLLightGroup& dlGroup)
{
	m_ppDLContexts[context]->push_back(dlGroup);
}

void AHSceneRenderPhase::SetView(gmtl::MatrixA44f* pView)
{
	m_pView = pView;

	// NOTE: We are using shared params so the change is reflected in all contexts
	//_LOOPi(m_NumContexts)
	{
		IBFXParamPool* pBFXParamPool = GetBFXParamPool(0);
		pBFXParamPool->SetViewMatrix(_CAST_MAT44(*m_pView));
	}
}

void AHSceneRenderPhase::SetProj(gmtl::MatrixA44f* pProj, float nearZ, float farZ)
{
	m_pProj = pProj;

	m_NearZ = nearZ;
	m_FarZ	= farZ;
	
	// NOTE: We are using shared params so the change is reflected in all contexts
	//_LOOPi(m_NumContexts)
	{
		IBFXParamPool* pBFXParamPool = GetBFXParamPool(0);
		pBFXParamPool->SetProjMatrix(_CAST_MAT44(*m_pProj));
	}
}

void AHSceneRenderPhase::SetCascadeShadowProj(gmtl::MatrixA44f& cascadeProj)
{
	m_CascadeProj = cascadeProj;

	if(m_pScene)
	{
		AHComponent* dirLightComp[16];
		uint numDirLightComp = m_pScene->QueryAll((void**)dirLightComp, 16, SDB_FILTER_DIRLIGHT_COMP);
		_LOOPi(numDirLightComp)
		{
			_DEBUG_ASSERT(dirLightComp[i]->GetType() == COMP_DIRLIGHT);
			((AHDirLightComponent*) dirLightComp)->UpdateCascadeBounds(m_CascadeProj, (size_t) &m_CascadeProj);
		}
	}
}

#endif

_NAMESPACE_END