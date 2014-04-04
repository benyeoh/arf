//================================================================================
//
//		AHScenePRTComputePhase_Process.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/27/2012
//
//================================================================================

#include "AppHelpers.h"

#if defined(_IS_USE_APPHELPER_LIB_PRTCOMPUTE)

_NAMESPACE_BEGIN

#define __EXEC(pComp)			pComp->Run()
#define __THREAD_EXEC(pComp)	pOwner->GetThreadPool()->QueueJob(*pComp)

// SceneRenderPLShadowProcessComp 
int AHScenePRTComputePhase::SceneRenderPLShadowProcessComp::Run()
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

            boolean isLessThan;
            float dist = AABoxToPointDistCheckLessThan(&renderAABox, (gmtl::VecA3f*) &lightSphere.center, lightSphere.center[3], isLessThan);
			//float dist = AABoxToPointDist(&renderAABox, (gmtl::VecA3f*) &lightSphere.center);

			//if(dist < lightSphere.center[3])
            if(isLessThan)
            {
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
int AHScenePRTComputePhase::SceneRenderDLShadowProcessComp::Run()
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
int AHScenePRTComputePhase::SceneRenderDLShadowCullComp::Run()
{
	uint context = pOwner->GetThreadPool()->GetCurrentThreadIndex();

	// Find culled objects
	AHScenePRTComputePhase::FrustumCullStore* pFrustumStore = pOwner->GetFrustumStore(context);
	ISHFXCascadedDirLight* pShadowDL = pDL->GetShadowDirLight();
	_DEBUG_ASSERT(pShadowDL);
	_DEBUG_ASSERT(pShadowDL->GetNumOfContexts() <= pOwner->GetNumOfContexts());

	AABox frustumAA;
	pOwner->GetFrustumAA(*pCascadeFrustum, frustumAA);

	ISDBSceneContainer* pScene = pOwner->GetSceneContainer();
	uint numComp = pScene->QueryInFrustumOrtho((void**) pFrustumStore->res, pFrustumStore->ssbounds, AHScenePRTComputePhase::MAX_FRUSTUM_CULL, frustumAA, *pCascadeFrustum, SDB_FILTER_RENDER_COMP);
	_DEBUG_ASSERT(numComp <= AHScenePRTComputePhase::MAX_FRUSTUM_CULL);

	// TODO: Impose a minimum screen size on object/ occlusion culling?

	// Split into dir light shadow process
	if(numComp > 0)
	{
		uint numCompPerContext = numComp / pOwner->GetNumOfContexts();
		if(numCompPerContext < AHScenePRTComputePhase::MIN_NUM_DL_OPS)
			numCompPerContext = AHScenePRTComputePhase::MIN_NUM_DL_OPS;

		int numToDispatch = (numComp / numCompPerContext) - 1;
		if(numToDispatch < 0)
			numToDispatch = 0;

		_LOOPi(numToDispatch)
		{
			AHScenePRTComputePhase::SceneRenderDLShadowProcessComp* pDLShadowProcess = pOwner->GetDLShadowProcessComp(context);
			pDLShadowProcess->pOwner	= pOwner;
			pDLShadowProcess->ppComp	= (AHComponent**) &(pFrustumStore->res[i * numCompPerContext]);
			pDLShadowProcess->numComp	= numCompPerContext;
			pDLShadowProcess->pDL		= pDL;
			pDLShadowProcess->cascadeIndex = cascadeIndex;
			pDLShadowProcess->pBounds	= &(pFrustumStore->ssbounds[i * numCompPerContext]);

			//pDLShadowProcess->Run();
			//pOwner->GetThreadPool()->QueueJob(*pDLShadowProcess);
			__THREAD_EXEC(pDLShadowProcess);
		}

		uint numCompStart = numCompPerContext * numToDispatch;
		uint numCompLeft = numComp - numCompStart;
		AHScenePRTComputePhase::SceneRenderDLShadowProcessComp* pDLShadowProcess = pOwner->GetDLShadowProcessComp(context);
		pDLShadowProcess->pOwner	= pOwner;
		pDLShadowProcess->ppComp	= (AHComponent**) &( pFrustumStore->res[numCompStart] );
		pDLShadowProcess->numComp	= numCompLeft;
		pDLShadowProcess->pDL		= pDL;
		pDLShadowProcess->cascadeIndex = cascadeIndex;
		pDLShadowProcess->pBounds	= &(pFrustumStore->ssbounds[numCompStart]);

		__EXEC(pDLShadowProcess);
		//pDLShadowProcess->Run();
	}

	return 0;
}

// PLShadowCull
int AHScenePRTComputePhase::SceneRenderPLShadowCullComp::Run()
{
	uint context = pOwner->GetThreadPool()->GetCurrentThreadIndex();

	// Find culled objects
	AHScenePRTComputePhase::FrustumCullStore* pFrustumStore = pOwner->GetFrustumStore(context);
	ISHFXCubePointLight* pShadowPL = pPL->GetShadowPointLight();
	_DEBUG_ASSERT(pShadowPL);
	_DEBUG_ASSERT(pShadowPL->GetNumOfContexts() <= pOwner->GetNumOfContexts());

	AABox frustumAA;
	pOwner->GetFrustumAA(*pPLFrustum, frustumAA);

	ISDBSceneContainer* pScene = pOwner->GetSceneContainer();
	uint numComp = pScene->QueryInFrustumPersp((void**) pFrustumStore->res, NULL, AHScenePRTComputePhase::MAX_FRUSTUM_CULL, frustumAA, *pPLFrustum, nearPlane, farPlane, SDB_FILTER_RENDER_COMP);
	_DEBUG_ASSERT(numComp <= AHScenePRTComputePhase::MAX_FRUSTUM_CULL);

	// TODO: Impose a minimum screen size on object/ occlusion culling?

	// Split into point light shadow process
	if(numComp > 0)
	{
		uint numCompPerContext = numComp / pOwner->GetNumOfContexts();
		if(numCompPerContext < AHScenePRTComputePhase::MIN_NUM_PL_OPS)
			numCompPerContext = AHScenePRTComputePhase::MIN_NUM_PL_OPS;

		int numToDispatch = (numComp / numCompPerContext) - 1;
		if(numToDispatch < 0)
			numToDispatch = 0;

		_LOOPi(numToDispatch)
		{
			AHScenePRTComputePhase::SceneRenderPLShadowProcessComp* pPLShadowProcess = pOwner->GetPLShadowProcessComp(context);
			pPLShadowProcess->pOwner	= pOwner;
			pPLShadowProcess->ppComp	= (AHComponent**) &(pFrustumStore->res[i * numCompPerContext]);
			pPLShadowProcess->numComp	= numCompPerContext;
			pPLShadowProcess->pPL		= pPL;
			pPLShadowProcess->face		= face;
			pPLShadowProcess->pBounds	= &(pFrustumStore->ssbounds[i * numCompPerContext]);

			//pPLShadowProcess->Run();
			//pOwner->GetThreadPool()->QueueJob(*pPLShadowProcess);
			__THREAD_EXEC(pPLShadowProcess);
		}

		uint numCompStart = numCompPerContext * numToDispatch;
		uint numCompLeft = numComp - numCompStart;
		AHScenePRTComputePhase::SceneRenderPLShadowProcessComp* pPLShadowProcess = pOwner->GetPLShadowProcessComp(context);
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
void AHScenePRTComputePhase::SceneRenderProcessComp::ProcessRenderComp(AHRenderComponent* pRenderComp, AABox& curBounds, uint context)
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

	if(pRenderComp->IsMatTypeUsed(SHPRTCOMPUTE_MAT_OCCLUSION))
	{
		pRenderComp->Render(SHPRTCOMPUTE_MAT_OCCLUSION, BFX_MAT_BASE_LIGHTING, pOwner->GetBaseFX(), pOwner->GetRenderContainer(), pOwner->GetParamContainer(context), pOwner->GetMatCreateCallback());
	}

	if(pRenderComp->IsMatTypeUsed(AH_PRTCOMPUTE_MAT_BACKFACE))
	{
		pRenderComp->Render(AH_PRTCOMPUTE_MAT_BACKFACE, AH_PRTCOMPUTE_MAT_BACKFACE, pOwner->GetBaseFX(), pOwner->GetRenderContainer(), pOwner->GetParamContainer(context), pOwner->GetMatCreateCallback());
	}

	if(pRenderComp->IsMatTypeUsed(AH_PRTCOMPUTE_MAT_ACCUM))
	{
		pRenderComp->Render(AH_PRTCOMPUTE_MAT_ACCUM, DEFX_MAT_ACCUM, pOwner->GetBaseFX(), pOwner->GetRenderContainer(), pOwner->GetParamContainer(context), pOwner->GetMatCreateCallback());
	}

}

void AHScenePRTComputePhase::SceneRenderProcessComp::ProcessPointLightComp(AHPointLightComponent* pPointLightComp, uint context)
{
	ISHFXCubePointLight* pShadowPL = pPointLightComp->GetShadowPointLight();
	boolean isRefreshShadow = FALSE;
	if(pShadowPL)
	{
		// Cast shadow

		// Only refresh shadow map if we need to
		AHScenePRTComputeSetup* pSetup = pOwner->GetSetup();
		_DEBUG_ASSERT(pSetup);
		isRefreshShadow = pSetup->CheckUpdateCubePointLightShadow(pPointLightComp);
		if( isRefreshShadow )
		{
			// Split into the 6 frustums
			float nearPlane = pShadowPL->GetViewProjNearPlane();
			float farPlane = pShadowPL->GetViewProjFarPlane();

			_LOOPi(CF_NUM_FACES)
			{
				AHScenePRTComputePhase::SceneRenderPLShadowCullComp* pPLCull = pOwner->GetPLShadowCullComp(context);
				pPLCull->pOwner = pOwner;
				pPLCull->pPL = pPointLightComp;
				pPLCull->pPLFrustum = pShadowPL->GetViewProj((eRCubeFace) i);
				pPLCull->face = (eRCubeFace) i;
				pPLCull->nearPlane = nearPlane;
				pPLCull->farPlane = farPlane;

				//pPLCull->Run();
				//pOwner->GetThreadPool()->QueueJob(*pPLCull);
				__THREAD_EXEC(pPLCull);
			}
		}
	}

	AHRenderComponent* pRenderComp = pPointLightComp->GetRenderComponent();
	if(pRenderComp && pRenderComp->IsMatTypeUsed(AH_PRTCOMPUTE_MAT_LIGHTING))
	{
		pOwner->GetSetup()->UpdatePLLightGroupRT(pPointLightComp, pOwner->GetCurRT(), pOwner->GetCurDB());

		// Deferred style light
		IBFXParamPool* pBFXParamPool = pOwner->GetBFXParamPool(context);
		pBFXParamPool->SetZDistanceFromCamera(0.0f);

		gmtl::Matrix44f& worldMat = pOwner->GetLightMatrix(context);
		pOwner->ComputePointLightMatrix(worldMat, pOwner->GetParaView(), pPointLightComp); 
		//gmtl::identity(worldMat);
		pBFXParamPool->SetWorldMatrix(worldMat);

		IDEFXParamPool* pDEFXParamPool = pOwner->GetDEFXParamPool(context);
		pDEFXParamPool->SetCurrentLightGroup(pPointLightComp->GetLightGroup());
		pDEFXParamPool->SetPointLight(&(pPointLightComp->GetDEFXPointLight()));

		// Set shadowed light
		ISHFXParamPool* pSHFXParamPool = pOwner->GetSHFXParamPool(context);
		//uint casterFlagDummy = 0;
		//pSHFXParamPool->SetPointLights(&pShadowPL, &casterFlagDummy, (pShadowPL != NULL) ? 1 : 0);
		pSHFXParamPool->SetCubePointLights(&pShadowPL, (pShadowPL != NULL) ? 1 : 0);

		pRenderComp->Render(AH_PRTCOMPUTE_MAT_LIGHTING, DEFX_MAT_LIGHTING, pOwner->GetBaseFX(), pOwner->GetRenderContainer(), pOwner->GetParamContainer(context), pOwner->GetMatCreateCallback());
	}

	if(pRenderComp || isRefreshShadow)
	{
		PLLightGroup pl;
		pl.pShadow	= isRefreshShadow ? pShadowPL : NULL;
		pl.pLG		= pPointLightComp->GetLightGroup();
		pOwner->AddPLContext(context, pl);
	}
}

void AHScenePRTComputePhase::SceneRenderProcessComp::ProcessDirLightComp(AHDirLightComponent* pDirLightComp, uint context)
{
	ISHFXCascadedDirLight* pShadowDL = pDirLightComp->GetShadowDirLight();
	boolean isRefreshShadow = FALSE;
	if(pShadowDL)
	{
		// Cast shadow

		// Only refresh shadow map if we need to
		AHScenePRTComputeSetup* pSetup = pOwner->GetSetup();
		_DEBUG_ASSERT(pSetup);
		isRefreshShadow = pSetup->CheckUpdateCascadeDirLightShadow(pDirLightComp, _CAST_VECA3(pOwner->GetCamPos()));
		if( isRefreshShadow )
		{
			// Only render 1 cascade
			// And use the actual proj (since it's a sphere)
			const SHFXCascadedProj& cascadeProj = pShadowDL->GetProj();
			//_LOOPi(4)
			{
				AHScenePRTComputePhase::SceneRenderDLShadowCullComp* pDLCull = pOwner->GetDLShadowCullComp(context);
				pDLCull->pOwner = pOwner;
				pDLCull->pDL = pDirLightComp;
				pDLCull->pCascadeFrustum = &(cascadeProj.cascadedViewProj[0]);
				pDLCull->cascadeIndex = 0;

				//pDLCull->Run();
				//pOwner->GetThreadPool()->QueueJob(*pDLCull);
				__THREAD_EXEC(pDLCull);
			}
		}
	}

	AHRenderComponent* pRenderComp = pDirLightComp->GetRenderComponent();
	if(pRenderComp && pRenderComp->IsMatTypeUsed(AH_PRTCOMPUTE_MAT_LIGHTING))
	{
		pOwner->GetSetup()->UpdateDLLightGroupRT(pDirLightComp, pOwner->GetCurRT(), pOwner->GetCurDB());

		// Deferred style light
		IBFXParamPool* pBFXParamPool = pOwner->GetBFXParamPool(context);
		pBFXParamPool->SetZDistanceFromCamera(0.0f);

		gmtl::Matrix44f& worldMat = pOwner->GetLightMatrix(context);
		gmtl::identity(worldMat);
		pBFXParamPool->SetWorldMatrix(worldMat);

		IDEFXParamPool* pDEFXParamPool = pOwner->GetDEFXParamPool(context);
		pDEFXParamPool->SetCurrentLightGroup(pDirLightComp->GetLightGroup());
		pDEFXParamPool->SetDirLight(&(pDirLightComp->GetDEFXDirLight()));

		// Set shadowed light
		ISHFXParamPool* pSHFXParamPool = pOwner->GetSHFXParamPool(context);
		pSHFXParamPool->SetCascadedDirLight(pShadowDL);

		// This should render the replaced screen mesh
		pRenderComp->Render(AH_PRTCOMPUTE_MAT_LIGHTING, DEFX_MAT_LIGHTING, pOwner->GetBaseFX(), pOwner->GetRenderContainer(), pOwner->GetParamContainer(context), pOwner->GetMatCreateCallback());
	}

	if(pRenderComp || isRefreshShadow)
	{
		DLLightGroup dl;
		dl.pShadow	= isRefreshShadow ? pShadowDL : NULL;
		dl.pLG		= pDirLightComp->GetLightGroup();
		pOwner->AddDLContext(context, dl);
	}
}

int AHScenePRTComputePhase::SceneRenderProcessComp::Run()
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

_NAMESPACE_END

#endif