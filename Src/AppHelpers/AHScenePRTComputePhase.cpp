//================================================================================
//
//		AHScenePRTComputePhase.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/23/2012
//
//================================================================================

#include "AppHelpers.h"

#if defined(_IS_USE_APPHELPER_LIB_PRTCOMPUTE)

_NAMESPACE_BEGIN


#define __EXEC(pComp)			pComp->Run()
#define __THREAD_EXEC(pComp)	GetThreadPool()->QueueJob(*pComp)

AHScenePRTComputePhase::AHScenePRTComputePhase(IRRenderer* pRenderer, IBFXBaseFX* pBaseFX, IDEFXDeferredFX* pDeferredFX, ISHFXShadowFX* pShadowFX, IPThreadPool* pThreadPool,
										IBFXPipeline* pBFXPipeline, IDEFXPipeline* pDEFXPipeline, ISHFXPipeline* pSHFXPipeline, AHPRTComputePipeline* pAHPRTComputePipeline,
										IRTexture2D* pZPos, IRTexture2D* pNormals, IRTexture2D* pAlbedo,
										ISDBSceneContainer* pScene, float viewDistance, AHScenePRTComputeSetup* pSetup)
{
	m_pSetup		= pSetup;

	m_pRenderer		= pRenderer;
	m_pBaseFX		= pBaseFX;
	m_pDeferredFX	= pDeferredFX;
	m_pShadowFX		= pShadowFX;

	m_pThreadPool	= pThreadPool;

	m_pBFXPipeline	= pBFXPipeline;
	m_pDEFXPipeline	= pDEFXPipeline;
	m_pSHFXPipeline	= pSHFXPipeline;
	m_pAHPRTComputePipeline	= pAHPRTComputePipeline;

	m_NumContexts = m_pThreadPool->GetNumThreads() + 1;

	_DEBUG_ASSERT(m_pBFXPipeline->GetNumOfContexts() <= m_NumContexts);
	_DEBUG_ASSERT(m_pDEFXPipeline->GetNumOfContexts() <= m_NumContexts);
	_DEBUG_ASSERT(m_pAHPRTComputePipeline->GetNumOfContexts() <= m_NumContexts);

	//// Create as many contexts as there are threads
	//_LOOPi(m_NumContexts - 1)
	//{
	//	m_pBFXPipeline->AddContext();
	//	m_pDEFXPipeline->AddContext();
	//	//m_pSHFXPipeline->AddContext();
	//}

	m_pMatCreateCallback = _NEW AHMaterialCreateCallbackImmed;

	m_ParamContainers = _NEW BFXParamContainer[m_NumContexts];
	m_pBFXParamPools = _NEW IBFXParamPoolPtr[m_NumContexts];
	m_pDEFXParamPools = _NEW IDEFXParamPoolPtr[m_NumContexts];
	m_pSHFXParamPools = _NEW ISHFXParamPoolPtr[m_NumContexts];
	m_pFrustumStores = _NEW AllocStoreFrustum[m_NumContexts];
	m_pPLShadowProcessCompStores = _NEW AllocStorePLShadowProcessComp[m_NumContexts];
	m_pDLShadowProcessCompStores = _NEW AllocStoreDLShadowProcessComp[m_NumContexts];
	m_pDLShadowCullStores = _NEW AllocStoreDLShadowCullComp[m_NumContexts];
	m_pPLShadowCullStores = _NEW AllocStorePLShadowCullComp[m_NumContexts];
	m_pLightMatrixStores = _NEW AllocStoreLightMatrix[m_NumContexts];

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

	m_FarZ = viewDistance;
	m_CamPos.set(0.0f, 0.0f, 0.0f);

	SetScene(pScene);
}

AHScenePRTComputePhase::~AHScenePRTComputePhase()
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

	_DELETE_ARRAY(m_pLightMatrixStores);
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

void AHScenePRTComputePhase::SetScene(ISDBSceneContainer* pScene)
{
	m_pScene = pScene;
}

void AHScenePRTComputePhase::SetViewDistance(float viewDist)
{
	m_FarZ = viewDist;
}

void AHScenePRTComputePhase::GetFrustumAA(const gmtl::MatrixA44f& viewProj, AABox& dest)
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

void AHScenePRTComputePhase::ComputePointLightMatrix(gmtl::Matrix44f& dest, const gmtl::MatrixA44f& paraView, AHPointLightComponent* pComp)
{
	gmtl::identity(dest);
	return;

	//// Transform sphere center to paraboloid view space
	//gmtl::VecA3f pos; 
	//pos[0] = pComp->GetWorldSphere().center[0];
	//pos[1] = pComp->GetWorldSphere().center[1];
	//pos[2] = pComp->GetWorldSphere().center[2];

	//TransformVec(&paraView, &pos, &pos);

	//// Find the 'up' and 'right' axis of the point light sphere in relation to its position to the view
	//// NOTE: LHS coordinate system in paraboloid view space
	//gmtl::VecA3f forward;
	//NormalizeVec(&forward, &pos);

	//gmtl::VecA3f right(1.0f, 0.0f, 0.0f);

	//// Singularity check
	//if( forward[0] > 0.99f )
	//{
	//	right[0] = 0.0f;
	//	right[1] = -1.0f;
	//	right[2] = 0.0f;
	//}
	//else if( forward[0] < -0.99f )
	//{
	//	right[0] = 0.0f;
	//	right[1] = 1.0f;
	//	right[2] = 0.0f;
	//}

	//gmtl::VecA3f up;
	//VecCrossNormalize(&up, &forward, &right);
	////gmtl::cross(_CAST_VEC3(up), _CAST_VEC3(forward), _CAST_VEC3(right));
	////gmtl::normalize(_CAST_VEC3(up));

	//VecCrossNormalize(&right, &up, &forward);
	////gmtl::cross(_CAST_VEC3(right), _CAST_VEC3(up), _CAST_VEC3(forward));
	////gmtl::normalize(_CAST_VEC3(right));
	//
	//// Find extents
	//gmtl::VecA3f nearOffset;
	//VecScalarMult(&nearOffset, &forward, -pComp->GetRadius());

	//gmtl::VecA3f upExtent;
	//VecScalarMult(&upExtent, &up, pComp->GetRadius());

	//gmtl::VecA3f rightExtent;
	//VecScalarMult(&rightExtent, &right, pComp->GetRadius());

	//gmtl::VecA3f posNear;
	//VecVecAdd(&posNear, &pos, &nearOffset);

	//gmtl::VecA3f extents[4];
	//VecVecAdd(&extents[0], &posNear, &upExtent);
	//VecVecAdd(&extents[1], &posNear, &rightExtent);
	//VecVecSub(&extents[2], &posNear, &upExtent);
	//VecVecSub(&extents[3], &posNear, &rightExtent);

	//// Project extents using paraboloid projection
	//gmtl::VecA3f min;
	//gmtl::VecA3f max;
	//min.set(9999999999999.0f, 9999999999999.0f, 9999999999999.0f);
	//max.set(-9999999999999.0f, -9999999999999.0f, -9999999999999.0f);

	//_LOOPi(4)
	//{
	//	NormalizeVec(extents[i]);

	//	float proj = extents[i][1] + 1.0f;
	//	if(proj < 0.0001f)
	//		proj = 0.0001f;

	//	VecScalarMult(&extents[i], &extents[i], 1.0f / proj);
	//	VecVecMin(&min, &min, &extents[i]);
	//	VecVecMax(&max, &max, &extents[i]);
	//}

	//// Set the scale for the screen space bounds (from -1 to 1)
	//gmtl::VecA3f scale;
	//scale[0] = (max[0] - min[0]) * 0.5f;
	//scale[2] = (max[2] - min[2]) * 0.5f;

	//gmtl::VecA3f offset;
	//offset[0] = min[0] + scale[0];
	//offset[2] = min[2] + scale[2];

	////gmtl::identity(dest);

	//dest[0][0] = scale[0];
	//dest[0][3] = offset[0];
	//dest[1][1] = 1.0f;//scale[2];
	//dest[1][3] = 0.0f;//offset[2];

	//gmtl::MatrixA44f mat;
	//_CAST_MAT44(mat) = dest; 
	//gmtl::VecA3f points[2];
	//gmtl::VecA3f respoints[2];
	//points[0][0] = -1.0f;
	//points[0][1] = 1.0f;
	//points[0][2] = 0.0f;
	////points[0][3] = 1.0f;

	//points[1][0] = 1.0f;
	//points[1][1] = -1.0f;
	//points[1][2] = 0.0f;
	////points[1][3] = 1.0f;

	//TransformVecW1(&mat, &points[0], &respoints[0]);
	//TransformVecW1(&mat, &points[1], &respoints[1]);

}

void AHScenePRTComputePhase::Update(const gmtl::VecA3f& loc, const gmtl::MatrixA44f& paraView, BFXRenderContainer& renderContainer, BFXParamContainer& paramContainer, IRTextureRT* pRT, IRDepthBuffer* pDB)
{
	//// Frustum cull
	//MatMatMult(&m_ViewProj, m_pProj, m_pView);
	//AABox frustumAA;
	//GetFrustumAA(m_ViewProj, frustumAA);

	m_pRT = pRT; 
	m_pDB = pDB;

	m_pParaView = &paraView;
	m_pRenderContainer = &renderContainer;

	renderContainer.SetRenderCallback(m_pBFXPipeline, _GET_LIB_INDEX(BFX_EFFECT_PARAM_OFFSET));
	renderContainer.SetRenderCallback(m_pDEFXPipeline, _GET_LIB_INDEX(DEFX_EFFECT_PARAM_OFFSET));
	renderContainer.SetRenderCallback(m_pSHFXPipeline, _GET_LIB_INDEX(SHFX_EFFECT_PARAM_OFFSET));
	renderContainer.SetRenderCallback(m_pAHPRTComputePipeline, _GET_LIB_INDEX(AH_PRTCOMPUTE_EFFECT_PARAM_OFFSET));

	_LOOPi(m_NumContexts)
	{
		// Set the param callback on our containers
		m_ParamContainers[i].SetParamCallback( paramContainer.GetParamCallback(SHPRTCOMPUTE_LIB_ID), SHPRTCOMPUTE_LIB_ID ); 
	}

	// Camera pos
	m_CamPos[0] = loc[0];
	m_CamPos[1] = loc[1];
	m_CamPos[2] = loc[2];

	//gmtl::VecA4f camPos(0.0f, 0.0f, 0.0f, 0.0f);
	//gmtl::VecA4f camOffsetX;
	//gmtl::VecA4f camAxisX;
	//camAxisX[0] = (*m_pView)[0][0];
	//camAxisX[1] = (*m_pView)[0][1];
	//camAxisX[2] = (*m_pView)[0][2];

	//gmtl::VecA4f camOffsetY;
	//gmtl::VecA4f camAxisY;
	//camAxisY[0] = (*m_pView)[1][0];
	//camAxisY[1] = (*m_pView)[1][1];
	//camAxisY[2] = (*m_pView)[1][2];

	//gmtl::VecA4f camOffsetZ;
	//gmtl::VecA4f camAxisZ;
	//camAxisZ[0] = (*m_pView)[2][0];
	//camAxisZ[1] = (*m_pView)[2][1];
	//camAxisZ[2] = (*m_pView)[2][2];

	//VecScalarMult(&camOffsetX, &camAxisX, -(*m_pView)[0][3]);
	//VecScalarMult(&camOffsetY, &camAxisY, -(*m_pView)[1][3]);
	//VecScalarMult(&camOffsetZ, &camAxisZ, -(*m_pView)[2][3]);

	//VecVecAdd(&camPos, &camPos, &camOffsetX);
	//VecVecAdd(&camPos, &camPos, &camOffsetY);
	//VecVecAdd(&m_CamPos, &camPos, &camOffsetZ);

	//// Far corner vecs
	//gmtl::VecA3f cameraFarCornerVecs[4];
	//gmtl::VecA3f viewFarCornerVecs[4];

	//gmtl::MatrixA44f invViewProj;
	//gmtl::MatrixA44f invProj;
	//gmtl::MatrixA44f invView;

	//gmtl::VecA3f zeroEyePos;
	//zeroEyePos[0] = zeroEyePos[1] = zeroEyePos[2] = 0.0f;

	//gmtl::invertFull(invProj, *m_pProj);
	//invView = *m_pView;
	//gmtl::setTrans(_CAST_MAT44(invView), gmtl::Vec3f(0.0f, 0.0f, 0.0f));
	//gmtl::transpose(_CAST_MAT44(invView));
	//MatMatMult(&invViewProj, &invView, &invProj);
	//ComputeCameraFarPlaneCornerVecs(cameraFarCornerVecs, zeroEyePos, invViewProj);
	//ComputeCameraFarPlaneCornerVecs(viewFarCornerVecs, zeroEyePos, invProj);

	// Set global params
	// NOTE: We are using shared params
	//_LOOPi(m_NumContexts)
	gmtl::MatrixA44f identityMat;
	gmtl::identity(_CAST_MAT44(identityMat));
	IBFXParamPool* pBFXParamPool = GetBFXParamPool(0);
	pBFXParamPool->SetViewMatrix(_CAST_MAT44(identityMat));
	pBFXParamPool->SetProjMatrix(_CAST_MAT44(identityMat));
	pBFXParamPool->SetCameraProperties(_CAST_VEC3(m_CamPos), 0.0f, m_FarZ);
	pBFXParamPool->FinalizeSharedParams();

	//_LOOPi(m_NumContexts)
	//{
	//	IDEFXParamPool* pDEFXParamPool = GetDEFXParamPool(i);
	//	pDEFXParamPool->SetCameraFarPlaneCornerVecs(cameraFarCornerVecs);
	//	pDEFXParamPool->SetViewFarPlaneOffsets((gmtl::Vec3f*) &(viewFarCornerVecs[0]), viewFarCornerVecs[1][0] - viewFarCornerVecs[0][0], viewFarCornerVecs[2][1] - viewFarCornerVecs[0][1]);
	//}

	Sphere querySphere;
	querySphere.center[0] = loc[0];
	querySphere.center[1] = loc[1];
	querySphere.center[2] = loc[2];
	querySphere.center[3] = m_FarZ;

	gmtl::VecA3f span;
	span.set(querySphere.center[3], querySphere.center[3], querySphere.center[3]);

	AABox queryBox;
	VecVecSub(&queryBox.min, &(_CAST_VECA3(querySphere.center)), &span);
	VecVecAdd(&queryBox.max, &(_CAST_VECA3(querySphere.center)), &span);

	uint numComp = m_pScene->QuerySphere(m_FrustumStoreMain.res, MAX_FRUSTUM_CULL, queryBox, querySphere, SDB_FILTER_RENDER_COMP | SDB_FILTER_POINTLIGHT_COMP | SDB_FILTER_DIRLIGHT_COMP);
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

	m_pThreadPool->WaitUntilFinished();
}

void AHScenePRTComputePhase::Flush()
{
	// Flush backface occlusion first if needed
	m_pAHPRTComputePipeline->Flush();
	
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
		m_pLightMatrixStores[i].Reset();

		m_ppPLContexts[i]->clear();
		m_ppDLContexts[i]->clear();
	}
}

AHScenePRTComputePhase::FrustumCullStore* AHScenePRTComputePhase::GetFrustumStore(uint context) 
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

AHScenePRTComputePhase::SceneRenderPLShadowProcessComp* AHScenePRTComputePhase::GetPLShadowProcessComp(uint context)
{
	SceneRenderPLShadowProcessComp* pProcess = m_pPLShadowProcessCompStores[context].Get();

	// Construct vtable
	pProcess = new(pProcess) SceneRenderPLShadowProcessComp;
	return pProcess;
}

AHScenePRTComputePhase::SceneRenderDLShadowProcessComp* AHScenePRTComputePhase::GetDLShadowProcessComp(uint context)
{
	SceneRenderDLShadowProcessComp* pProcess = m_pDLShadowProcessCompStores[context].Get();

	// Construct vtable
	pProcess = new(pProcess) SceneRenderDLShadowProcessComp;
	return pProcess;
}

AHScenePRTComputePhase::SceneRenderDLShadowCullComp* AHScenePRTComputePhase::GetDLShadowCullComp(uint context)
{
	SceneRenderDLShadowCullComp* pProcess = m_pDLShadowCullStores[context].Get();

	// Construct vtable
	pProcess = new(pProcess) SceneRenderDLShadowCullComp;
	return pProcess;
}

AHScenePRTComputePhase::SceneRenderPLShadowCullComp* AHScenePRTComputePhase::GetPLShadowCullComp(uint context)
{
	SceneRenderPLShadowCullComp* pProcess = m_pPLShadowCullStores[context].Get();

	// Construct vtable
	pProcess = new(pProcess) SceneRenderPLShadowCullComp;
	return pProcess;
}

AHScenePRTComputePhase::SceneRenderProcessComp* AHScenePRTComputePhase::GetProcessComp()
{
	SceneRenderProcessComp* pProcess = m_ProcessCompStoreMain.Get();

	// Construct vtable
	pProcess = new(pProcess) SceneRenderProcessComp;
	return pProcess;
}

gmtl::Matrix44f& AHScenePRTComputePhase::GetLightMatrix(uint context)
{
	gmtl::Matrix44f* pMat = m_pLightMatrixStores[context].Get();
	return *pMat;
}

void AHScenePRTComputePhase::AddPLContext(uint context, const PLLightGroup& plGroup)
{
	m_ppPLContexts[context]->push_back(plGroup);
}

void AHScenePRTComputePhase::AddDLContext(uint context, const DLLightGroup& dlGroup)
{
	m_ppDLContexts[context]->push_back(dlGroup);
}

_NAMESPACE_END

#endif