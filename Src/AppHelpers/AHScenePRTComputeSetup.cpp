//================================================================================
//
//		AHScenePRTComputeSetup.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/24/2012
//
//================================================================================

#include "AppHelpers.h"

#if defined(_IS_USE_APPHELPER_LIB_PRTCOMPUTE)

_NAMESPACE_BEGIN

void AHScenePRTComputeSetup::ResetLightsNoDestroy()
{
	CascadedDirLightMap::Iterator dirLightItr = m_CascadedDirLightMap.BeginIter();
	while(dirLightItr.IsValid())
	{
		// Reset each directional light component with the original shadow light
		CascadeDirLightEntry* pEntry = dirLightItr.GetValue();
		pEntry->pComp->SetShadowDirLight( pEntry->pOrig, pEntry->origCascadeExp, pEntry->origCascadeFarNearSteepness, pEntry->origPosOffset, pEntry->origIsShadowMapShared);
		pEntry->pComp->GetRenderComponent()->SetMeshList( pEntry->pOrigMeshList );
		pEntry->pComp->SetDeferredDirLight(pEntry->pOrigLG, pEntry->pComp->GetRenderComponent());

		dirLightItr.Next();
	}

	CubePointLightMap::Iterator pointLightItr = m_CubePointLightMap.BeginIter();
	while(pointLightItr.IsValid())
	{
		CubePointLightEntry* pEntry = pointLightItr.GetValue();
		pEntry->pComp->SetShadowPointLight( pEntry->pOrig, pEntry->origIsShadowMapShared );
		pEntry->pComp->GetRenderComponent()->SetMeshList( pEntry->pOrigMeshList );
		pEntry->pComp->SetDeferredPointLight(pEntry->pOrigLG, pEntry->pComp->GetRenderComponent());

		pointLightItr.Next();
	}
}

void AHScenePRTComputeSetup::ResetLights()
{
	CascadedDirLightMap::Iterator dirLightItr = m_CascadedDirLightMap.BeginIter();
	while(dirLightItr.IsValid())
	{
		// Reset each directional light component with the original shadow light
		CascadeDirLightEntry* pEntry = dirLightItr.GetValue();
		pEntry->pComp->SetShadowDirLight( pEntry->pOrig, pEntry->origCascadeExp, pEntry->origCascadeFarNearSteepness, pEntry->origPosOffset, pEntry->origIsShadowMapShared);
		pEntry->pComp->GetRenderComponent()->SetMeshList( pEntry->pOrigMeshList );
		pEntry->pComp->SetDeferredDirLight(pEntry->pOrigLG, pEntry->pComp->GetRenderComponent());

		pEntry->pOrigMeshList->Release();
		pEntry->pOrig->Release();
		pEntry->pSubst->Release();

		if(pEntry->pOrigLG)
			pEntry->pOrigLG->Release();
	
		pEntry->pSubstLG->Release();

		dirLightItr.Next();
	}

	m_CascadedDirLightMap.Reset();

	CubePointLightMap::Iterator pointLightItr = m_CubePointLightMap.BeginIter();
	while(pointLightItr.IsValid())
	{
		CubePointLightEntry* pEntry = pointLightItr.GetValue();
		pEntry->pComp->SetShadowPointLight( pEntry->pOrig, pEntry->origIsShadowMapShared );
		pEntry->pComp->GetRenderComponent()->SetMeshList( pEntry->pOrigMeshList );
		pEntry->pComp->SetDeferredPointLight(pEntry->pOrigLG, pEntry->pComp->GetRenderComponent());

		pEntry->pOrigMeshList->Release();
		pEntry->pOrig->Release();
		pEntry->pSubst->Release();
		
		if(pEntry->pOrigLG)
			pEntry->pOrigLG->Release();
		
		pEntry->pSubstLG->Release();

		pointLightItr.Next();
	}

	m_CubePointLightMap.Reset();
}

void AHScenePRTComputeSetup::SetupDirLight(AHDirLightComponent* pComp, uint numContexts)
{
	if(pComp->GetShadowDirLight() || pComp->GetLightGroup())
	{
		CascadeDirLightEntry* pCurEntry = m_CascadedDirLightMap.Get((size_t) pComp);
		if(pCurEntry)
		{
			_DEBUG_ASSERT(pCurEntry->pComp == pComp);
			pComp->SetShadowDirLight(pCurEntry->pSubst, 0.0f, 1.0f, pCurEntry->pComp->GetShadowPosOffset(), pCurEntry->pComp->IsShadowMapShared());
			pComp->SetDeferredDirLight(pCurEntry->pSubstLG, pComp->GetRenderComponent());

			// Not initialized
			pCurEntry->lastShadowPos.set(-99999999999.0f, -99999999999.0f, -99999999999.0f);

			pComp->GetRenderComponent()->SetMeshList(m_pDirLightMesh);
		}
		else
		{
			CascadeDirLightEntry entry;
			entry.pComp = pComp;
			entry.pOrig = pComp->GetShadowDirLight();
			entry.pOrig->AddRef();
			entry.origCascadeExp		= entry.pComp->GetShadowCascadeSplitExp();
			entry.origCascadeFarNearSteepness = entry.pComp->GetShadowCascadeFarNearSteepness();
			entry.origIsShadowMapShared	= entry.pComp->IsShadowMapShared();
			entry.origPosOffset			= entry.pComp->GetShadowPosOffset();
			entry.pOrigMeshList			= entry.pComp->GetRenderComponent()->GetMeshList();
			entry.pOrigMeshList->AddRef();

			entry.pOrigLG = entry.pComp->GetLightGroup();
			if(entry.pOrigLG)
				entry.pOrigLG->AddRef();

			entry.pSubstLG = m_pDeferredFX->GetResourceMgr().CreateLightGroup();
			_LOOPi(numContexts)
			{
				entry.pSubstLG->AddContext();
			}
			entry.pSubstLG->AddRef();
			entry.pComp->SetDeferredDirLight(entry.pSubstLG, entry.pComp->GetRenderComponent());

			ISHFXCascadedDirLight* pClone = m_pShadowFX->GetResourceMgr().CreateCascadedDirLight();
			_LOOPi(numContexts)
			{
				pClone->AddContext();
			}

			uint origShadowMapWidth = entry.pOrig->GetShadowMap()->GetWidth(0);
			uint origShadowMapHeight = entry.pOrig->GetShadowMap()->GetHeight(0);
			uint newShadowMapWidth = origShadowMapWidth >> 1;
			if(newShadowMapWidth < 256)
				newShadowMapWidth = 256;

			uint newShadowMapHeight = origShadowMapHeight >> 1;
			if(newShadowMapHeight < 256)
				newShadowMapHeight = 256;

			IRTexture2DPtr pCloneFinal = m_pRenderer->GetRResourceMgr().CreateTexture2DRT(newShadowMapWidth, newShadowMapHeight, 1, m_pShadowFX->GetResourceMgr().GetSMCompatibleFormat());
			IRDepthBufferPtr pCloneDB = m_pRenderer->GetRResourceMgr().CreateDepthBuffer(newShadowMapWidth, newShadowMapHeight, m_pShadowFX->GetResourceMgr().GetSMCompatibleDepthFormat(), 0);
			pClone->SetShadowMapParams(pCloneFinal, pCloneDB);

			entry.pSubst = pClone;
			entry.pSubst->AddRef();

			pComp->SetShadowDirLight(entry.pSubst, 0.0f, 1.0f, entry.pComp->GetShadowPosOffset(), entry.pComp->IsShadowMapShared());

			// Not initialized
			entry.lastShadowPos.set(-99999999999.0f, -99999999999.0f, -99999999999.0f);

			pComp->GetRenderComponent()->SetMeshList(m_pDirLightMesh);

			m_CascadedDirLightMap.Add((size_t)entry.pComp, entry);
		}
	}
}

void AHScenePRTComputeSetup::SetupPointLight(AHPointLightComponent* pComp, uint numContexts)
{
	if(pComp->GetShadowPointLight() || pComp->GetLightGroup())
	{
		CubePointLightEntry* pCurEntry = m_CubePointLightMap.Get((size_t) pComp);
		if(pCurEntry)
		{
			_DEBUG_ASSERT(pCurEntry->pComp == pComp);
			pComp->SetShadowPointLight(pCurEntry->pSubst, pCurEntry->pComp->IsShadowMapShared());
			pComp->SetDeferredPointLight(pCurEntry->pSubstLG, pComp->GetRenderComponent());

			// Not initialized
			pCurEntry->isRendered = FALSE;

			pComp->GetRenderComponent()->SetMeshList(m_pPointLightMesh);
		}
		else
		{
			CubePointLightEntry entry;
			entry.pComp = pComp;
			entry.pOrig = pComp->GetShadowPointLight();
			entry.pOrig->AddRef();
			entry.origIsShadowMapShared = entry.pComp->IsShadowMapShared();
			entry.pOrigMeshList			= entry.pComp->GetRenderComponent()->GetMeshList();
			entry.pOrigMeshList->AddRef();

			entry.pOrigLG = entry.pComp->GetLightGroup();
			if(entry.pOrigLG)
				entry.pOrigLG->AddRef();

			entry.pSubstLG = m_pDeferredFX->GetResourceMgr().CreateLightGroup();
			_LOOPi(numContexts)
			{
				entry.pSubstLG->AddContext();
			}
			entry.pSubstLG->AddRef();
			entry.pComp->SetDeferredPointLight(entry.pSubstLG, entry.pComp->GetRenderComponent());

			ISHFXCubePointLight* pClone = m_pShadowFX->GetResourceMgr().CreateCubePointLight();
			_LOOPi(numContexts)
			{
				pClone->AddContext();
			}

			uint origShadowMapSize = entry.pOrig->GetShadowMap()->GetSize(0);
			uint newShadowMapSize = origShadowMapSize >> 1;
			if(newShadowMapSize < 64)
				newShadowMapSize = 64;

			IRTextureCubePtr pCloneFinal = m_pRenderer->GetRResourceMgr().CreateTextureCubeRT(newShadowMapSize, m_pShadowFX->GetResourceMgr().GetSMCompatibleFormat());
			IRDepthBufferPtr pCloneDB = m_pRenderer->GetRResourceMgr().CreateDepthBuffer(newShadowMapSize , newShadowMapSize , m_pShadowFX->GetResourceMgr().GetSMCompatibleDepthFormat(), 0);
			pClone->SetShadowMapParams(pCloneFinal, pCloneDB);

			entry.pSubst = pClone;
			entry.pSubst->AddRef();

			pComp->SetShadowPointLight(entry.pSubst, entry.pComp->IsShadowMapShared());
		
			entry.isRendered = FALSE;
		
			pComp->GetRenderComponent()->SetMeshList(m_pPointLightMesh);

			m_CubePointLightMap.Add((size_t) entry.pComp, entry);
		}
	}
}

void AHScenePRTComputeSetup::SetupLightsForPRTCompute(ISDBSceneContainer* pScene, float camViewDistance, float cascadeViewExtra, uint numContexts)
{
	const static uint MAX_NUM_LIGHTS = 8192;
	ResetLights();
	
	AHComponent* pComp[MAX_NUM_LIGHTS];
	uint numComp = pScene->QueryAll((void**) pComp, MAX_NUM_LIGHTS, SDB_FILTER_DIRLIGHT_COMP | SDB_FILTER_POINTLIGHT_COMP);

	// Find all dir light shadow and replace
	_LOOPi(numComp)
	{
		uint compType = pComp[i]->GetType();
		switch(compType)
		{
		case COMP_DIRLIGHT:
			{
				SetupDirLight((AHDirLightComponent*) pComp[i], numContexts);				
				break;
			}

		case COMP_POINTLIGHT:
			{
				SetupPointLight((AHPointLightComponent*) pComp[i], numContexts);				
				break;
			}
		}
	}

	m_ViewDistance = camViewDistance;
	float totalDist = cascadeViewExtra + camViewDistance;

	_LOOPi(4)
	{
		// Set the shadow bounds to be the same for all cascades
		// IE, a sphere around the camera
		m_Bounds.bounds[i][0] = 0.0f;
		m_Bounds.bounds[i][1] = 0.0f;
		m_Bounds.bounds[i][2] = 0.0f;
		m_Bounds.bounds[i][3] = totalDist;
	}

	// These are used to compute the tightfit AABB (which we won't use anyway)
	m_Bounds.frustumSplits[0][0] = 0.0f;
	m_Bounds.frustumSplits[0][1] = 0.0f;
	m_Bounds.frustumSplits[0][2] = 0.0f;
	m_Bounds.frustumSplits[0][3] = 1.0f;

	m_Bounds.frustumSplits[1][0] = 0.0f;
	m_Bounds.frustumSplits[1][1] = 0.0f;
	m_Bounds.frustumSplits[1][2] = 0.0f;
	m_Bounds.frustumSplits[1][3] = 1.0f;

	m_Bounds.frustumSplits[2][0] = 0.0f;
	m_Bounds.frustumSplits[2][1] = 0.0f;
	m_Bounds.frustumSplits[2][2] = 0.0f;
	m_Bounds.frustumSplits[2][3] = 1.0f;

	m_Bounds.frustumSplits[3][0] = 0.0f;
	m_Bounds.frustumSplits[3][1] = 0.0f;
	m_Bounds.frustumSplits[3][2] = 0.0f;
	m_Bounds.frustumSplits[3][3] = 1.0f;

	_LOOPi(4)
	{
		m_Bounds.frustumSplits[4+i*4][0] = -totalDist;
		m_Bounds.frustumSplits[4+i*4][1] = -totalDist;
		m_Bounds.frustumSplits[4+i*4][2] = -totalDist;
		m_Bounds.frustumSplits[4+i*4][3] = 1;

		m_Bounds.frustumSplits[4+i*4+1][0] = -totalDist;
		m_Bounds.frustumSplits[4+i*4+1][1] = totalDist;
		m_Bounds.frustumSplits[4+i*4+1][2] = -totalDist;
		m_Bounds.frustumSplits[4+i*4+1][3] = 1;

		m_Bounds.frustumSplits[4+i*4+2][0] = totalDist;
		m_Bounds.frustumSplits[4+i*4+2][1] = -totalDist;
		m_Bounds.frustumSplits[4+i*4+2][2] = -totalDist;
		m_Bounds.frustumSplits[4+i*4+2][3] = 1;

		m_Bounds.frustumSplits[4+i*4+3][0] = totalDist;
		m_Bounds.frustumSplits[4+i*4+3][1] = totalDist;
		m_Bounds.frustumSplits[4+i*4+3][2] = -totalDist;
		m_Bounds.frustumSplits[4+i*4+3][3] = 1;
	}
}

void AHScenePRTComputeSetup::UpdateDLLightGroupRT(AHDirLightComponent* pComp, IRTextureRT* pRT, IRDepthBuffer* pDB)
{
	CascadeDirLightEntry* pEntry = m_CascadedDirLightMap.Get((size_t) pComp);
	_DEBUG_ASSERT(pEntry);
	//if(pEntry)
	{
		_DEBUG_ASSERT(pEntry->pComp == pComp);
		
		IDEFXLightGroup* pLG = pEntry->pComp->GetLightGroup();
		_DEBUG_ASSERT(pLG);
		pLG->ClearRenderTargets();
		pLG->AddRenderTarget(pRT);
		pLG->SetDepthBuffer(pDB);
		pLG->SetViewport(0, 0, pRT->GetWidth(), pRT->GetHeight());
	}
}

void AHScenePRTComputeSetup::UpdatePLLightGroupRT(AHPointLightComponent* pComp, IRTextureRT* pRT, IRDepthBuffer* pDB)
{
	CubePointLightEntry* pEntry = m_CubePointLightMap.Get((size_t) pComp);
	_DEBUG_ASSERT(pEntry);
	//if(pEntry)
	{
		_DEBUG_ASSERT(pEntry->pComp == pComp);

		IDEFXLightGroup* pLG = pEntry->pComp->GetLightGroup();
		_DEBUG_ASSERT(pLG);
		pLG->ClearRenderTargets();
		pLG->AddRenderTarget(pRT);
		pLG->SetDepthBuffer(pDB);
		pLG->SetViewport(0, 0, pRT->GetWidth(), pRT->GetHeight());
	}
}

boolean AHScenePRTComputeSetup::CheckUpdateCascadeDirLightShadow(AHDirLightComponent* pComp, const gmtl::VecA3f& camPos)
{
	CascadeDirLightEntry* pEntry = m_CascadedDirLightMap.Get((size_t) pComp);
	_DEBUG_ASSERT(pEntry);
	//if(pEntry)
	{
		_DEBUG_ASSERT(pEntry->pComp == pComp);

		gmtl::VecA3f diff;
		gmtl::VecA3f lastShadowPos;
		lastShadowPos.set(pEntry->lastShadowPos[0], pEntry->lastShadowPos[1], pEntry->lastShadowPos[2]);
		VecVecSub(&diff, &camPos, &lastShadowPos);

		float length;
		VecLength(&length, &diff);

		if((length + m_ViewDistance) > m_Bounds.bounds[0][3])
		{
			// Then we need to regenerate the shadows
			gmtl::MatrixA44f view;
			gmtl::identity(_CAST_MAT44(view));
			gmtl::setTrans(_CAST_MAT44(view), _CAST_VEC3(camPos));
			pComp->UpdateCascadeShadow(camPos, view, m_Bounds);
		
			pEntry->lastShadowPos = _CAST_VEC3(camPos);
			return TRUE;
		}
	}

	// We can skip generating the shadows because the shadow frustum fits the current view
	return FALSE;
}

boolean AHScenePRTComputeSetup::CheckUpdateCubePointLightShadow(AHPointLightComponent* pComp)
{
	CubePointLightEntry* pEntry = m_CubePointLightMap.Get((size_t) pComp);
	_DEBUG_ASSERT(pEntry);
	//if(pEntry)
	{
		_DEBUG_ASSERT(pEntry->pComp == pComp);

		if(!pEntry->isRendered)
		{
			// We need to regenerate shadows
			pEntry->isRendered = TRUE;
			return TRUE;
		}
	}

	return FALSE;
}

_NAMESPACE_END

#endif