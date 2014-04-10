//================================================================================
//
//		AHSceneRenderPhase_Process.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/16/2012
//
//================================================================================

#include "AppHelpers.h"

_NAMESPACE_BEGIN

#if defined(_IS_USE_APPHELPER_LIB_BASIC)

#define __EXEC(pComp)			pComp->Run()
#define __THREAD_EXEC(pComp)	pOwner->GetThreadPool()->QueueJob(*pComp)

#define _NUM_JOBS (pOwner->GetNumOfContexts() << 3)
	
const static uint MAX_NUM_JOBS = 256;

// SceneRenderPLShadowProcessComp 
int AHSceneRenderPhase::SceneRenderPLShadowProcessComp::Run()
{
	uint context = pOwner->GetThreadPool()->GetCurrentThreadIndex();
	IBFXParamPool* pBFXParamPool = pOwner->GetBFXParamPool(context);
	ISHFXParamPool* pSHFXParamPool = pOwner->GetSHFXParamPool(context);
	ISHFXCubePointLight* pSHFXPointLight = pPL->GetShadowPointLight();
	_DEBUG_ASSERT(pSHFXPointLight->GetNumOfContexts() <= pOwner->GetNumOfContexts());
	//float invRange = pPL->GetBFXPointLight().invRange;
	const Sphere& lightSphere = pPL->GetWorldSphere();		

	pSHFXParamPool->SetCasterCubePointLight(pSHFXPointLight, face);

	_LOOPi(numComp)
	{
		// Render point light shadows
		AHRenderComponent* pRenderComp = (AHRenderComponent*) ppComp[i];
		if(pRenderComp->IsMatTypeUsed(SHFX_MAT_CAST_CUBE_POINT_LIGHT_SHADOW))
		{
			const AABox& renderAABox = pRenderComp->GetWorldAABox();
			//float dist = AABoxToPointDist(&renderAABox, (gmtl::VecA3f*) &lightSphere.center);

            boolean isLessThan;
            float dist = AABoxToPointDistCheckLessThan(&renderAABox, (gmtl::VecA3f*) &lightSphere.center, lightSphere.center[3], isLessThan);
			
			//if(AABoxInSphere(&renderAABox, &lightSphere))
            //if(dist < lightSphere.center[3])
            if(isLessThan)
            {
				//float diffMinY = renderAABox.min[1] - lightSphere.center[1];
				//float diffMaxY = renderAABox.max[1] - lightSphere.center[1];

				//// Set the flags if the y-min/y-max is on the back/front hemisphere of the point light
				//uint casterFlag = (diffMinY < 0.0f ? 0x2 : 0x1) | (diffMaxY < 0.0f ? 0x2 : 0x1);
				//pSHFXParamPool->SetPointLights(&pSHFXPointLight, &casterFlag, 1);

				// Find distance
				//gmtl::VecA4f aaboxCenter;
				//VecVecAdd(&aaboxCenter, &_CAST_VECA4(renderAABox.max), &_CAST_VECA4(renderAABox.min));
				//VecScalarMult(&aaboxCenter, &aaboxCenter, 0.5f);

				//gmtl::VecA4f diff;
				//VecVecSub(&diff, &aaboxCenter, &lightSphere.center);
				//float dist;
				//VecLength(&dist, &_CAST_VECA3(diff));

				//pSHFXParamPool->SetZDistanceFromCamera(dist * invRange);		
			
				pSHFXParamPool->SetZDistanceFromCamera(dist);//pBounds[i].min[2]);

				AHSpatialComponent* pSpatial = (AHSpatialComponent*) pRenderComp->GetEntity()->GetComponent(COMP_SPATIAL);
				_DEBUG_ASSERT(pSpatial);
				pBFXParamPool->SetWorldMatrix( _CAST_MAT44(pSpatial->GetWorldMatrix()) );

				pRenderComp->Render(SHFX_MAT_CAST_CUBE_POINT_LIGHT_SHADOW, SHFX_MAT_CAST_CUBE_POINT_LIGHT_SHADOW, pOwner->GetBaseFX(), pOwner->GetRenderContainer(), pOwner->GetParamContainer(context), pOwner->GetMatCreateCallback());
			}
		}
	}

	return 0;
}

// DLProcessShadowComp
int AHSceneRenderPhase::SceneRenderDLShadowProcessComp::Run()
{
	uint context = pOwner->GetThreadPool()->GetCurrentThreadIndex();
	IBFXParamPool* pBFXParamPool = pOwner->GetBFXParamPool(context);
	ISHFXParamPool* pSHFXParamPool = pOwner->GetSHFXParamPool(context);
	ISHFXCascadedDirLight* pSHFXDirLight = pDL->GetShadowDirLight();
	_DEBUG_ASSERT(pSHFXDirLight->GetNumOfContexts() <= pOwner->GetNumOfContexts());

	_LOOPi(numComp)
	{
		// Render dir light shadows
		AHRenderComponent* pRenderComp = (AHRenderComponent*) ppComp[i];
		if(pRenderComp->IsMatTypeUsed(SHFX_MAT_CAST_DIR_LIGHT_SHADOW))
		{
            pSHFXParamPool->SetZDistanceFromCamera(pBounds[i].min[2]);
			pSHFXParamPool->SetCascadedCasterFlag(1 << cascadeIndex);
			pSHFXParamPool->SetCascadedDirLight(pSHFXDirLight);

			AHSpatialComponent* pSpatial = (AHSpatialComponent*) pRenderComp->GetEntity()->GetComponent(COMP_SPATIAL);
			_DEBUG_ASSERT(pSpatial);
			pBFXParamPool->SetWorldMatrix( _CAST_MAT44(pSpatial->GetWorldMatrix()) );

			pRenderComp->Render(SHFX_MAT_CAST_DIR_LIGHT_SHADOW, SHFX_MAT_CAST_DIR_LIGHT_SHADOW, pOwner->GetBaseFX(), pOwner->GetRenderContainer(), pOwner->GetParamContainer(context), pOwner->GetMatCreateCallback());
		}
	}

	return 0;
}

// DLShadowCull
int AHSceneRenderPhase::SceneRenderDLShadowCullComp::Run()
{
	uint context = pOwner->GetThreadPool()->GetCurrentThreadIndex();

	// Find culled objects
	AHSceneRenderPhase::FrustumCullStore* pFrustumStore = pOwner->GetFrustumStore(context);
	ISHFXCascadedDirLight* pShadowDL = pDL->GetShadowDirLight();
	_DEBUG_ASSERT(pShadowDL);
	_DEBUG_ASSERT(pShadowDL->GetNumOfContexts() <= pOwner->GetNumOfContexts());

	AABox frustumAA;
	pOwner->GetFrustumAA(*pCascadeFrustum, frustumAA);

	ISDBSceneContainer* pScene = pOwner->GetSceneContainer();
	uint numComp = pScene->QueryInFrustumOrtho((void**) pFrustumStore->res, pFrustumStore->ssbounds, AHSceneRenderPhase::MAX_FRUSTUM_CULL, frustumAA, *pCascadeFrustum, SDB_FILTER_RENDER_COMP);
	_DEBUG_ASSERT(numComp <= AHSceneRenderPhase::MAX_FRUSTUM_CULL);

	// TODO: Impose a minimum screen size on object/ occlusion culling?

	// Split into dir light shadow process
	if(numComp > 0)
	{
		uint numCompPerContext = numComp / _NUM_JOBS;
		if(numCompPerContext < AHSceneRenderPhase::MIN_NUM_DL_OPS)
			numCompPerContext = AHSceneRenderPhase::MIN_NUM_DL_OPS;

		IPRunnable* pRunnable[MAX_NUM_JOBS];
		_DEBUG_ASSERT(_NUM_JOBS < MAX_NUM_JOBS);

		int numToDispatch = (numComp / numCompPerContext) - 1;
		if(numToDispatch < 0)
			numToDispatch = 0;

		_LOOPi(numToDispatch)
		{
			AHSceneRenderPhase::SceneRenderDLShadowProcessComp* pDLShadowProcess = pOwner->GetDLShadowProcessComp(context);
			pDLShadowProcess->pOwner	= pOwner;
			pDLShadowProcess->ppComp	= (AHComponent**) &(pFrustumStore->res[i * numCompPerContext]);
			pDLShadowProcess->numComp	= numCompPerContext;
			pDLShadowProcess->pDL		= pDL;
			pDLShadowProcess->cascadeIndex = cascadeIndex;
			pDLShadowProcess->pBounds	= &(pFrustumStore->ssbounds[i * numCompPerContext]);

			pRunnable[i] = pDLShadowProcess;

			//pDLShadowProcess->Run();
			//pOwner->GetThreadPool()->QueueJob(*pDLShadowProcess);
			//__THREAD_EXEC(pDLShadowProcess);
		}

		if(numToDispatch > 0)
			pOwner->GetThreadPool()->QueueJobs(pRunnable, numToDispatch);

		uint numCompStart = numCompPerContext * numToDispatch;
		uint numCompLeft = numComp - numCompStart;
		AHSceneRenderPhase::SceneRenderDLShadowProcessComp* pDLShadowProcess = pOwner->GetDLShadowProcessComp(context);
		pDLShadowProcess->pOwner	= pOwner;
		pDLShadowProcess->ppComp	= (AHComponent**) &( pFrustumStore->res[numCompStart] );
		pDLShadowProcess->numComp	= numCompLeft;
		pDLShadowProcess->pDL		= pDL;
		pDLShadowProcess->cascadeIndex = cascadeIndex;
		pDLShadowProcess->pBounds	= &(pFrustumStore->ssbounds[numCompStart]);


		//pRunnable[numToDispatch] = pDLShadowProcess;
		//pOwner->GetThreadPool()->QueueJobs(pRunnable, numToDispatch+1);

		__EXEC(pDLShadowProcess);
		//pDLShadowProcess->Run();
	}

	return 0;
}

// PLShadowCull
int AHSceneRenderPhase::SceneRenderPLShadowCullComp::Run()
{
	uint context = pOwner->GetThreadPool()->GetCurrentThreadIndex();

	// Find culled objects
	AHSceneRenderPhase::FrustumCullStore* pFrustumStore = pOwner->GetFrustumStore(context);
	ISHFXCubePointLight* pShadowPL = pPL->GetShadowPointLight();
	_DEBUG_ASSERT(pShadowPL);
	_DEBUG_ASSERT(pShadowPL->GetNumOfContexts() <= pOwner->GetNumOfContexts());

	AABox frustumAA;
	pOwner->GetFrustumAA(*pPLFrustum, frustumAA);

	ISDBSceneContainer* pScene = pOwner->GetSceneContainer();
	uint numComp = pScene->QueryInFrustumPersp((void**) pFrustumStore->res, NULL, AHSceneRenderPhase::MAX_FRUSTUM_CULL, frustumAA, *pPLFrustum, nearPlane, farPlane, SDB_FILTER_RENDER_COMP);
	_DEBUG_ASSERT(numComp <= AHSceneRenderPhase::MAX_FRUSTUM_CULL);

	// TODO: Impose a minimum screen size on object/ occlusion culling?

	// Split into point light shadow process
	if(numComp > 0)
	{
		uint numCompPerContext = numComp / _NUM_JOBS;
		if(numCompPerContext < AHSceneRenderPhase::MIN_NUM_PL_OPS)
			numCompPerContext = AHSceneRenderPhase::MIN_NUM_PL_OPS;

		IPRunnable* pRunnable[MAX_NUM_JOBS];
		_DEBUG_ASSERT(_NUM_JOBS < MAX_NUM_JOBS);

		int numToDispatch = (numComp / numCompPerContext) - 1;
		if(numToDispatch < 0)
			numToDispatch = 0;

		_LOOPi(numToDispatch)
		{
			AHSceneRenderPhase::SceneRenderPLShadowProcessComp* pPLShadowProcess = pOwner->GetPLShadowProcessComp(context);
			pPLShadowProcess->pOwner	= pOwner;
			pPLShadowProcess->ppComp	= (AHComponent**) &(pFrustumStore->res[i * numCompPerContext]);
			pPLShadowProcess->numComp	= numCompPerContext;
			pPLShadowProcess->pPL		= pPL;
			pPLShadowProcess->face		= face;
			pPLShadowProcess->pBounds	= &(pFrustumStore->ssbounds[i * numCompPerContext]);

			pRunnable[i] = pPLShadowProcess;
			//pPLShadowProcess->Run();
			//pOwner->GetThreadPool()->QueueJob(*pPLShadowProcess);
			//__THREAD_EXEC(pPLShadowProcess);
		}

		if(numToDispatch > 0)
			pOwner->GetThreadPool()->QueueJobs(pRunnable, numToDispatch);

		uint numCompStart = numCompPerContext * numToDispatch;
		uint numCompLeft = numComp - numCompStart;
		AHSceneRenderPhase::SceneRenderPLShadowProcessComp* pPLShadowProcess = pOwner->GetPLShadowProcessComp(context);
		pPLShadowProcess->pOwner	= pOwner;
		pPLShadowProcess->ppComp	= (AHComponent**) &( pFrustumStore->res[numCompStart] );
		pPLShadowProcess->numComp	= numCompLeft;
		pPLShadowProcess->pPL		= pPL;
		pPLShadowProcess->face		= face;
		pPLShadowProcess->pBounds	= &(pFrustumStore->ssbounds[numCompStart]);

		//pPLShadowProcess->Run();
		__EXEC(pPLShadowProcess);
	}

	return 0;
}

// SceneRenderProcessComp
void AHSceneRenderPhase::SceneRenderProcessComp::ProcessRenderComp(AHRenderComponent* pRenderComp, AABox& curBounds, uint context)
{
	IBFXParamPool* pBFXParamPool = pOwner->GetBFXParamPool(context);
	pBFXParamPool->SetZDistanceFromCamera(curBounds.min[2]);

	AHSpatialComponent* pSpatial = (AHSpatialComponent*) pRenderComp->GetEntity()->GetComponent(COMP_SPATIAL);
	pBFXParamPool->SetWorldMatrix( _CAST_MAT44(pSpatial->GetWorldMatrix()) );

	uint bfxPLSearchFlag = pRenderComp->IsParamSemanticUsed(BFX_POINT_LIGHT1) ? SDB_FILTER_POINTLIGHT_COMP : 0;
	uint bfxDLSearchFlag = pRenderComp->IsParamSemanticUsed(BFX_DIR_LIGHT) ? SDB_FILTER_DIRLIGHT_COMP : 0;
	if(bfxPLSearchFlag || bfxDLSearchFlag)
	{
		ISHFXParamPool* pSHFXParamPool = pOwner->GetSHFXParamPool(context);

		// Search for lights
		const static uint MAX_LIGHTS = 256;
		AHComponent* pCompTemp[MAX_LIGHTS];
		uint numComp = pOwner->GetSceneContainer()->Query((void**) pCompTemp, MAX_LIGHTS, pRenderComp->GetWorldAABox(), bfxPLSearchFlag | bfxDLSearchFlag);
		_DEBUG_ASSERT(numComp < MAX_LIGHTS);

		const static uint MAX_DIR_LIGHTS = 1;
		AHDirLightComponent* pDL[MAX_DIR_LIGHTS];
		uint numDirLights = 0;

		const static uint MAX_POINT_LIGHTS = 128;
		AHPointLightComponent* pPL[MAX_POINT_LIGHTS];
		//BFXPointLight pointLights[MAX_POINT_LIGHTS];
		uint numPointLights = 0;

		_LOOPi(numComp)
		{
			AHComponent* pComp = pCompTemp[i];
			switch(pComp->GetType())
			{
			case COMP_POINTLIGHT:
				{
					_DEBUG_ASSERT(numPointLights < MAX_POINT_LIGHTS);
					pPL[numPointLights] = ((AHPointLightComponent*) pComp); numPointLights++;
					break;
				}
			case COMP_DIRLIGHT:	
				{
					_DEBUG_ASSERT(numDirLights < MAX_DIR_LIGHTS);
					pDL[numDirLights] = ((AHDirLightComponent*) pComp); numDirLights++;
					break;
				}
			}
		}

		const static uint MAX_POINT_LIGHTS_FOR_SHADER = 4; 
		if(bfxPLSearchFlag > 0)
		{
			if((numPointLights > MAX_POINT_LIGHTS_FOR_SHADER))
			{
				float plStrength[MAX_POINT_LIGHTS_FOR_SHADER];
				AHPointLightComponent* pPLClosest[MAX_POINT_LIGHTS_FOR_SHADER];
				float curClosest = 4294967296.0f;
				uint curClosestIndex = 0;
				gmtl::VecA3f pos;
				pSpatial->GetWorldPos(pos);

				_LOOPi(numPointLights)
				{
					gmtl::VecA3f diff;
					gmtl::VecA3f plPos;
					_CAST_VEC3(plPos) = pPL[i]->GetBFXPointLight().pos;
					VecVecSub(&diff, &plPos, &pos);

					float dist;
					VecLength(&dist, &diff);
					plStrength[i] = dist * pPL[i]->GetBFXPointLight().invRange;
				}

				// Search for the 4 closest/strongest lights
				_LOOPi(MAX_POINT_LIGHTS_FOR_SHADER)
				{
					_LOOPj(numPointLights)
					{
						if(plStrength[j] < curClosest)
						{
							curClosest		= plStrength[j];
							curClosestIndex	= j;
						}
					}

					pPLClosest[i]	= pPL[curClosestIndex];
					curClosest		= 4294967296.0f;
					plStrength[curClosestIndex] = 4294967296.0f;
				}

				_LOOPi(MAX_POINT_LIGHTS_FOR_SHADER)
				{
					pPL[i] = pPLClosest[i];
				}

				numPointLights = MAX_POINT_LIGHTS_FOR_SHADER;
			}

			ISHFXCubePointLight* pSHFXPL[MAX_POINT_LIGHTS_FOR_SHADER];
			//uint casterDummy[MAX_POINT_LIGHTS_FOR_SHADER];
			BFXPointLight* pBFXPL[MAX_POINT_LIGHTS_FOR_SHADER];
			_DEBUG_ASSERT(numPointLights < MAX_POINT_LIGHTS_FOR_SHADER);
			_LOOPi(numPointLights)
			{
				if(pPL[i]->GetShadowPointLight() && !pPL[i]->IsShadowMapShared())
					pSHFXPL[i] = pPL[i]->GetShadowPointLight();
				else
					pSHFXPL[i] = NULL;

				pBFXPL[i] = &(pPL[i]->GetBFXPointLight());
			}

			pSHFXParamPool->SetCubePointLights(pSHFXPL, numPointLights);
			pBFXParamPool->SetPointLights((const BFXPointLight**) pBFXPL, numPointLights);
		}

		if(bfxDLSearchFlag > 0)
		{
			// Always set shadow for dir light
			// TODO: For now...
			_DEBUG_ASSERT(numDirLights == 1);
			pSHFXParamPool->SetCascadedDirLight(pDL[0]->GetShadowDirLight());
			pBFXParamPool->SetDirLight(&(pDL[0]->GetBFXDirLight())); 
		}
		else
		{
			pSHFXParamPool->SetCascadedDirLight(NULL);
			pBFXParamPool->SetDirLight(NULL);
		}
	}

	if(pRenderComp->IsMatTypeUsed(BFX_MAT_BASE_LIGHTING))
	{
		pRenderComp->Render(BFX_MAT_BASE_LIGHTING, BFX_MAT_BASE_LIGHTING, pOwner->GetBaseFX(), pOwner->GetRenderContainer(), pOwner->GetParamContainer(context), pOwner->GetMatCreateCallback());
	}

	if(pRenderComp->IsMatTypeUsed(DEFX_MAT_ACCUM))
	{
		pRenderComp->Render(DEFX_MAT_ACCUM, DEFX_MAT_ACCUM, pOwner->GetBaseFX(), pOwner->GetRenderContainer(), pOwner->GetParamContainer(context), pOwner->GetMatCreateCallback());
	}

}

void AHSceneRenderPhase::SceneRenderProcessComp::ProcessPointLightComp(AHPointLightComponent* pPointLightComp, uint context)
{
	ISHFXCubePointLight* pShadowPL = pPointLightComp->GetShadowPointLight();
	if(pShadowPL)
	{
		// Cast shadow

		// Split into the 6 frustums
		float nearPlane = pShadowPL->GetViewProjNearPlane();
		float farPlane = pShadowPL->GetViewProjFarPlane();

		IPRunnable* pRunnable[CF_NUM_FACES];
		_LOOPi(CF_NUM_FACES)
		{
			AHSceneRenderPhase::SceneRenderPLShadowCullComp* pPLCull = pOwner->GetPLShadowCullComp(context);
			pPLCull->pOwner = pOwner;
			pPLCull->pPL = pPointLightComp;
			pPLCull->pPLFrustum = pShadowPL->GetViewProj((eRCubeFace) i);
			pPLCull->face = (eRCubeFace) i;
			pPLCull->nearPlane = nearPlane;
			pPLCull->farPlane = farPlane;

			pRunnable[i] = pPLCull;
			//pPLCull->Run();
			//pOwner->GetThreadPool()->QueueJob(*pPLCull);
			//__THREAD_EXEC(pPLCull);
		}

		pOwner->GetThreadPool()->QueueJobs(pRunnable, (uint) CF_NUM_FACES);

		//// Find culled objects
		//AHSceneRenderPhase::FrustumCullStore* pFrustumStore = pOwner->GetFrustumStore(context);

		//ISDBSceneContainer* pScene = pOwner->GetSceneContainer();
		//uint numCompPL = pScene->QuerySphere((void**) pFrustumStore->res, AHSceneRenderPhase::MAX_FRUSTUM_CULL, pPointLightComp->GetWorldAABox(), pPointLightComp->GetWorldSphere(), SDB_FILTER_RENDER_COMP);
		//_DEBUG_ASSERT(numCompPL <= AHSceneRenderPhase::MAX_FRUSTUM_CULL);

		//// Split into pointlight shadow process
		//if(numCompPL > 0)
		//{
		//	uint numCompPerContext = numCompPL / pOwner->GetNumOfContexts();
		//	if(numCompPerContext < MIN_NUM_PL_OPS)
		//		numCompPerContext = MIN_NUM_PL_OPS;

		//	int numToDispatch = (numCompPL / numCompPerContext) - 1;
		//	if(numToDispatch < 0)
		//		numToDispatch = 0;

		//	_LOOPi(numToDispatch)
		//	{
		//		AHSceneRenderPhase::SceneRenderPLShadowProcessComp* pPLShadowProcess = pOwner->GetPLShadowProcessComp(context);
		//		pPLShadowProcess->pOwner	= pOwner;
		//		pPLShadowProcess->ppComp	= (AHComponent**) &(pFrustumStore->res[i * numCompPerContext]);
		//		pPLShadowProcess->numComp	= numCompPerContext;
		//		pPLShadowProcess->pPL		= pPointLightComp;
		//		//pPLShadowProcess->pUncachedMats	= pUncachedMats;
		//		//pPLShadowProcess->pNumUncached	= pNumUncached;

		//		//pPLShadowProcess->Run();
		//		pOwner->GetThreadPool()->QueueJob(*pPLShadowProcess);
		//	}

		//	uint numCompStart = numCompPerContext * numToDispatch;
		//	uint numCompPLLeft = numCompPL - numCompStart;
		//	AHSceneRenderPhase::SceneRenderPLShadowProcessComp* pPLShadowProcess = pOwner->GetPLShadowProcessComp(context);
		//	pPLShadowProcess->pOwner	= pOwner;
		//	pPLShadowProcess->ppComp	= (AHComponent**) &( pFrustumStore->res[numCompStart] );
		//	pPLShadowProcess->numComp	= numCompPLLeft;
		//	pPLShadowProcess->pPL		= pPointLightComp;
		//	//pPLShadowProcess->pUncachedMats	= pUncachedMats;
		//	//pPLShadowProcess->pNumUncached	= pNumUncached;

		//	if(numCompPLLeft >= MIN_NUM_PL_OPS)
		//		pOwner->GetThreadPool()->QueueJob(*pPLShadowProcess);
		//	else
		//		pPLShadowProcess->Run();
		//}
	}

	AHRenderComponent* pRenderComp = pPointLightComp->GetRenderComponent();
	if(pRenderComp && pRenderComp->IsMatTypeUsed(DEFX_MAT_LIGHTING))
	{
		// Deferred style light
		IBFXParamPool* pBFXParamPool = pOwner->GetBFXParamPool(context);
		pBFXParamPool->SetZDistanceFromCamera(0.0f);

		pBFXParamPool->SetWorldMatrix( _CAST_MAT44(pPointLightComp->GetRenderWorldMat()) );

		IDEFXParamPool* pDEFXParamPool = pOwner->GetDEFXParamPool(context);
		pDEFXParamPool->SetCurrentLightGroup(pPointLightComp->GetLightGroup());
		pDEFXParamPool->SetPointLight(&(pPointLightComp->GetDEFXPointLight()));

		// Set shadowed light
		ISHFXParamPool* pSHFXParamPool = pOwner->GetSHFXParamPool(context);
		//uint casterFlagDummy = 0;
		//pSHFXParamPool->SetPointLights(&pShadowPL, &casterFlagDummy, (pShadowPL != NULL) ? 1 : 0);
		pSHFXParamPool->SetCubePointLights(&pShadowPL, (pShadowPL != NULL) ? 1 : 0);

		pRenderComp->Render(DEFX_MAT_LIGHTING, DEFX_MAT_LIGHTING, pOwner->GetBaseFX(), pOwner->GetRenderContainer(), pOwner->GetParamContainer(context), pOwner->GetMatCreateCallback());
	}

	if(pRenderComp || pShadowPL)
	{
		PLLightGroup pl;
		pl.pShadow	= pShadowPL;
		pl.pLG		= pPointLightComp->GetLightGroup();
		pOwner->AddPLContext(context, pl);
	}
}

void AHSceneRenderPhase::SceneRenderProcessComp::ProcessDirLightComp(AHDirLightComponent* pDirLightComp, uint context)
{
	ISHFXCascadedDirLight* pShadowDL = pDirLightComp->GetShadowDirLight();
	if(pShadowDL)
	{
		// Cast shadow

		// Update the bounds if it has not already been updated
		pDirLightComp->UpdateCascadeShadow(_CAST_VECA3(pOwner->GetCamPos()), pOwner->GetView(), pOwner->GetCascadeShadowProj(), (size_t) &(pOwner->GetCascadeShadowProj()));

		IPRunnable* pRunnable[4];

		// Split into the 4 cascades
		// We can't use the tighter bounding volume because we are selecting cascades based on the projection (and not the z-splits)
		const SHFXCascadedProj& cascadeProj = pShadowDL->GetProj();//pShadowDL->GetProjCulling();
		_LOOPi(4)
		{
			AHSceneRenderPhase::SceneRenderDLShadowCullComp* pDLCull = pOwner->GetDLShadowCullComp(context);
			pDLCull->pOwner = pOwner;
			pDLCull->pDL = pDirLightComp;
			pDLCull->pCascadeFrustum = &(cascadeProj.cascadedViewProj[i]);
			pDLCull->cascadeIndex = i;

			pRunnable[i] = pDLCull;
			//pDLCull->Run();
			//pOwner->GetThreadPool()->QueueJob(*pDLCull);
			//__THREAD_EXEC(pDLCull);
		}

		pOwner->GetThreadPool()->QueueJobs(pRunnable, 4);
	}

	AHRenderComponent* pRenderComp = pDirLightComp->GetRenderComponent();
	if(pRenderComp && pRenderComp->IsMatTypeUsed(DEFX_MAT_LIGHTING))
	{
		// Deferred style light
		IBFXParamPool* pBFXParamPool = pOwner->GetBFXParamPool(context);
		pBFXParamPool->SetZDistanceFromCamera(0.0f);

		pBFXParamPool->SetWorldMatrix( _CAST_MAT44(pDirLightComp->GetRenderWorldMat()) );

		IDEFXParamPool* pDEFXParamPool = pOwner->GetDEFXParamPool(context);
		pDEFXParamPool->SetCurrentLightGroup(pDirLightComp->GetLightGroup());
		pDEFXParamPool->SetDirLight(&(pDirLightComp->GetDEFXDirLight()));

		// Set shadowed light
		ISHFXParamPool* pSHFXParamPool = pOwner->GetSHFXParamPool(context);
		pSHFXParamPool->SetCascadedDirLight(pShadowDL);

		pRenderComp->Render(DEFX_MAT_LIGHTING, DEFX_MAT_LIGHTING, pOwner->GetBaseFX(), pOwner->GetRenderContainer(), pOwner->GetParamContainer(context), pOwner->GetMatCreateCallback());
	}

	if(pRenderComp || pShadowDL)
	{
		DLLightGroup dl;
		dl.pShadow	= pShadowDL;
		dl.pLG		= pDirLightComp->GetLightGroup();
		pOwner->AddDLContext(context, dl);
	}
}

int AHSceneRenderPhase::SceneRenderProcessComp::Run()
{
	uint context = pOwner->GetThreadPool()->GetCurrentThreadIndex();

	//IBFXParamPool* pBFXParamPool = pOwner->GetBFXParamPool(contextIndex);
	//pBFXParamPool->SetCurrentContext(contextIndex);

	_LOOPi(numComp)
	{
		switch(ppComp[i]->GetType())
		{
		case COMP_RENDER:		ProcessRenderComp( (AHRenderComponent*) ppComp[i], pBounds[i], context );	break;
		case COMP_POINTLIGHT:	ProcessPointLightComp( (AHPointLightComponent*) ppComp[i], context );	break;
		case COMP_DIRLIGHT:		ProcessDirLightComp( (AHDirLightComponent*) ppComp[i], context );  break;
		default: _DEBUG_ASSERT(FALSE); break;
		}
	}

	return 0;
}

#endif

_NAMESPACE_END