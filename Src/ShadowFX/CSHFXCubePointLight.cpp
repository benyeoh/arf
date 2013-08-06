//================================================================================
//
//		CSHFXCubePointLight.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/14/2012
//
//================================================================================

#include "ShadowFXFramework.h"

_NAMESPACE_BEGIN

static int PointLightCasterSortLess(const void* pL, const void* pR)
{
	const RRenderOp* pLhs = *((const RRenderOp**) pL);
	const RRenderOp* pRhs = *((const RRenderOp**) pR);

	// Sort by constant params
	if( pLhs->pEffect != pRhs->pEffect )
		return (pLhs->pEffect < pRhs->pEffect) ? 1 : -1;

	//if( pLhs->pVBGroup != pRhs->pVBGroup )
	//	return (pLhs->pVBGroup < pRhs->pVBGroup) ? 1 : -1;

	//if( pLhs->pIB != pRhs->pIB )
	//	return (pLhs->pIB < pRhs->pIB) ? 1 : -1; 

	//if( pLhs->opHash != pRhs->opHash )
	//	return (pLhs->opHash < pRhs->opHash) ? 1 : -1;

    // Front to back
    if(pLhs->userData.fVal != pRhs->userData.fVal )
        return (pLhs->userData.fVal > pRhs->userData.fVal) ? 1 : -1;

    return 0;
}

void CSHFXCubePointLight::Initialize()
{
	wstring fileName = _W("[shd]\\BaseFXUtils\\2DPipeSetup.fx");
	CRCDataKey id;
	id.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));
	IByteBufferPtr pBuffer = m_pShadowFX->GetAppCallback().GetFileData(fileName.c_str());
	m_p2DPipeSetupEffect = m_pShadowFX->GetRenderer().GetRResourceMgr().LoadEffect(
		&id, 
		pBuffer,
		NULL,
		NULL,
		0);

	_LOOPi(CF_NUM_FACES)
	{
		m_pCasterGroups[i] = m_pShadowFX->GetRenderer().GetRResourceMgr().CreateRenderGroup(PointLightCasterSortLess);
		m_pNoDrawOpClearGroups[i] = m_pShadowFX->GetRenderer().GetRResourceMgr().CreateRenderGroup(NULL);
	
		m_pBlur1Group[i] = m_pShadowFX->GetRenderer().GetRResourceMgr().CreateRenderGroup(NULL, 1);
		m_pBlur2Group[i] = m_pShadowFX->GetRenderer().GetRResourceMgr().CreateRenderGroup(NULL, 1);
	}

	m_pBlur1Effect = NULL;
	m_pBlur2Effect = NULL;

	AddContext();

	m_ShadowPLCast.posAndInvRange[0] = 0.0f;
	m_ShadowPLCast.posAndInvRange[1] = 0.0f;
	m_ShadowPLCast.posAndInvRange[2] = 0.0f;
	m_ShadowPLCast.posAndInvRange[3] = 1.0f / 10.0f;
}

void CSHFXCubePointLight::UpdateViewProj()
{
	// Set proj
	gmtl::MatrixA44f proj;
	gmtl::setPerspective(_CAST_MAT44(proj), gmtl::Math::PI / 2.0f, 1.0f, GetViewProjNearPlane(), GetViewProjFarPlane());

	// Face Front (-Z)
	gmtl::MatrixA44f view;
	gmtl::Vec3f at = _CAST_VEC3(m_ShadowPLCast.posAndInvRange);
	at[2] -= 1.0f;
	gmtl::setView(_CAST_MAT44(view), _CAST_VEC3(m_ShadowPLCast.posAndInvRange), at, gmtl::Vec3f(0.0f, 1.0f, 0.0f));
	MatMatMult(&(m_ViewProj[CF_NEG_Z]), &proj, &view);

	// Face Back (+Z)
	at = _CAST_VEC3(m_ShadowPLCast.posAndInvRange);
	at[2] += 1.0f;
	gmtl::setView(_CAST_MAT44(view), _CAST_VEC3(m_ShadowPLCast.posAndInvRange), at, gmtl::Vec3f(0.0f, 1.0f, 0.0f));
	MatMatMult(&(m_ViewProj[CF_POS_Z]), &proj, &view);

	// Face Right (+X)
	at = _CAST_VEC3(m_ShadowPLCast.posAndInvRange);
	at[0] += 1.0f;
	gmtl::setView(_CAST_MAT44(view), _CAST_VEC3(m_ShadowPLCast.posAndInvRange), at, gmtl::Vec3f(0.0f, 1.0f, 0.0f));
	MatMatMult(&(m_ViewProj[CF_POS_X]), &proj, &view);

	// Face Left (-X)
	at = _CAST_VEC3(m_ShadowPLCast.posAndInvRange);
	at[0] -= 1.0f;
	gmtl::setView(_CAST_MAT44(view), _CAST_VEC3(m_ShadowPLCast.posAndInvRange), at, gmtl::Vec3f(0.0f, 1.0f, 0.0f));
	MatMatMult(&(m_ViewProj[CF_NEG_X]), &proj, &view);

	// Face Up (+Y)
	at = _CAST_VEC3(m_ShadowPLCast.posAndInvRange);
	at[1] += 1.0f;
	gmtl::setView(_CAST_MAT44(view), _CAST_VEC3(m_ShadowPLCast.posAndInvRange), at, gmtl::Vec3f(0.0f, 0.0f, 1.0f));
	MatMatMult(&(m_ViewProj[CF_POS_Y]), &proj, &view);

	// Face Down (-Y)
	at = _CAST_VEC3(m_ShadowPLCast.posAndInvRange);
	at[1] -= 1.0f;
	gmtl::setView(_CAST_MAT44(view), _CAST_VEC3(m_ShadowPLCast.posAndInvRange), at, gmtl::Vec3f(0.0f, 0.0f, -1.0f));
	MatMatMult(&(m_ViewProj[CF_NEG_Y]), &proj, &view);
}

void CSHFXCubePointLight::Update(const gmtl::Vec3f& pos)
{
	m_ShadowPLCast.posAndInvRange[0]	= pos[0];
	m_ShadowPLCast.posAndInvRange[1]	= pos[1];
	m_ShadowPLCast.posAndInvRange[2]	= pos[2];
	UpdateViewProj();
}

void CSHFXCubePointLight::SetRange(float range)
{
	SetRangeNoUpdate(range);
	UpdateViewProj();
}

void CSHFXCubePointLight::SetRangeNoUpdate(float range)
{
	m_ShadowPLCast.posAndInvRange[3] = 1.0f / range;
}

IRRenderGroup* CSHFXCubePointLight::GetCasterGroup(eRCubeFace face)
{
	return m_pCasterGroups[face];
}

uint CSHFXCubePointLight::AddContext()
{
	PipelineContext contextToAdd;
	_LOOPi(CF_NUM_FACES)
	{
		contextToAdd.pCasterContext[i]	= _NEW BFXDataContext<RRenderOp>(&m_CasterPool[i]);
	}

	m_Contexts.push_back(contextToAdd);
	return m_Contexts.size() - 1;
}

uint CSHFXCubePointLight::GetNumOfContexts()
{
	return m_Contexts.size();
}

void CSHFXCubePointLight::ResetContext(uint contextID)
{
	_LOOPi(CF_NUM_FACES)
	{
		m_Contexts[contextID].pCasterContext[i]->Dispose();
	}
}

void CSHFXCubePointLight::RenderPointLightCaster(BFXRenderContext& context)
{
	CSHFXParamPool* pPool = (CSHFXParamPool*) context.pContainer->GetParamCallback( _GET_LIB_INDEX(SHFX_EFFECT_PARAM_OFFSET) );
	IBFXParamPool* pBFXPool = (IBFXParamPool*) context.pContainer->GetParamCallback( _GET_LIB_INDEX(BFX_EFFECT_PARAM_OFFSET) );

	uint contextID = pBFXPool->GetCurrentContext();

	eRCubeFace faceIndex = pPool->GetCubePLCasterFace();
	IRRenderGroup* pCasterGroup = m_pCasterGroups[faceIndex];
	RRenderOp* pOp = m_Contexts[contextID].pCasterContext[faceIndex]->Alloc();

	RVarData data;
	data.fVal = pPool->GetZDistanceFromCamera();

	pCasterGroup->FillDrawOp(pOp, context.pMesh->pVBGroup, context.pMesh->pIB, context.pEffect, context.pDynamicParams, context.tech, data);
}

void CSHFXCubePointLight::Flush()
{
	boolean isCasterUsed[CF_NUM_FACES];

	_LOOPi(CF_NUM_FACES)
	{
		_LOOPj(m_Contexts.size())
		{
			m_pCasterGroups[i]->AddToList(m_Contexts[j].pCasterContext[i]->GetDataBuffer(), m_Contexts[j].pCasterContext[i]->GetNumUsedData());
		}

		isCasterUsed[i] = m_pCasterGroups[i]->GetNumOfDrawOps() > 0;
		if(isCasterUsed[i])
		{
			m_pCasterGroups[i]->Flush();
			m_pCasterGroups[i]->Reset();

			m_CasterPool[i].UpdateSize();
			_LOOPj(m_Contexts.size())
			{
				m_Contexts[j].pCasterContext[i]->UpdateAndReset();
			}

			if(m_pBlur1Effect || m_pBlur2Effect)
			{
				IREffectTemplate* pTemplate = m_p2DPipeSetupEffect->GetTemplate();
				pTemplate->BeginTechnique(0);
				pTemplate->BeginPass(0);

				if(m_pBlur1Effect)
					m_pBlur1Group[i]->Flush();

				if(m_pBlur2Effect)
					m_pBlur2Group[i]->Flush();
		
				pTemplate->EndPass();
				pTemplate->EndTechnique();
			}
		}
		else
		{
			m_pNoDrawOpClearGroups[i]->Flush();
			m_pNoDrawOpClearGroups[i]->Reset();
		}
	}

	if(m_pCubeBlurEffect[0])
	{
		_LOOPi(CF_NUM_FACES)
		{
			if(isCasterUsed[i])
			{
				IREffectTemplate* pTemplate = m_p2DPipeSetupEffect->GetTemplate();
				pTemplate->BeginTechnique(0);
				pTemplate->BeginPass(0);

				m_pBlur2Group[i]->Flush();

				pTemplate->EndPass();
				pTemplate->EndTechnique();
			}
		}
	}
}

void CSHFXCubePointLight::SetShadowMapParamsGaussianFilter(IRTexture2D* pSM, IRTexture2D* pBackup, IRTextureCube* pFinal, IRDepthBuffer* pDepth, float stdDev, uint spreadKernelSizes)
{
	// Initialize all render groups
	_DEBUG_ASSERT(pSM->GetWidth(0) == pSM->GetHeight(0));
	_DEBUG_ASSERT(pBackup->GetWidth(0) == pBackup->GetHeight(0));
	_DEBUG_ASSERT(pDepth->GetWidth() >= pSM->GetHeight(0));

	if(!m_GaussianParamsH)
		m_GaussianParamsH = _NEW BFXUGaussianBlurParams;

	if(!m_GaussianParamsV)
		m_GaussianParamsV = _NEW BFXUGaussianBlurParams;

	uint shadowMapSize = pSM->GetWidth(0);

	//m_pClearGroup->ClearRenderTargets();
	//m_pClearGroup->AddRenderTarget(pSM);
	//m_pClearGroup->SetViewport(0, 0, pSM->GetWidth(), pSM->GetHeight());

	_LOOPi(CF_NUM_FACES)
	{
		IRTextureRT* pRT = pSM->GetTextureRT(0);
		_DEBUG_ASSERT(pRT);
		_DEBUG_ASSERT(pRT->GetWidth() == shadowMapSize);
		m_pCasterGroups[i]->ClearRenderTargets();
		m_pCasterGroups[i]->AddRenderTarget(pRT);
		m_pCasterGroups[i]->SetDepthBuffer(pDepth);
		m_pCasterGroups[i]->SetClearDepth(1.0f);
		m_pCasterGroups[i]->SetClearColor(gmtl::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
		m_pCasterGroups[i]->SetViewport(0, 0, shadowMapSize, shadowMapSize);

		pRT = pBackup->GetTextureRT(0);
		_DEBUG_ASSERT(pRT);
		_DEBUG_ASSERT(pRT->GetWidth() == shadowMapSize);
		m_pBlur1Group[i]->ClearRenderTargets();
		m_pBlur1Group[i]->AddRenderTarget(pRT);
		m_pBlur1Group[i]->SetViewport(0, 0, shadowMapSize, shadowMapSize);

		pRT = pFinal->GetTextureRT(0, (eRCubeFace)i);
		_DEBUG_ASSERT(pRT);
		_DEBUG_ASSERT(pRT->GetWidth() == shadowMapSize);
		m_pNoDrawOpClearGroups[i]->ClearRenderTargets();
		m_pNoDrawOpClearGroups[i]->AddRenderTarget(pRT);
		m_pNoDrawOpClearGroups[i]->SetViewport(0, 0, shadowMapSize, shadowMapSize);
		m_pNoDrawOpClearGroups[i]->SetClearColor(gmtl::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));

		m_pBlur2Group[i]->ClearRenderTargets();
		m_pBlur2Group[i]->AddRenderTarget(pRT);
		m_pBlur2Group[i]->SetViewport(0, 0, shadowMapSize, shadowMapSize);
	}

	m_ScreenMesh.Initialize(shadowMapSize, shadowMapSize, &m_pShadowFX->GetRenderer());

	// Set gaussian blur params
	gmtl::Vec2f spreadH(1.0f, 0.0f);
	gmtl::Vec2f spreadV(0.0f, 1.0f);
	gmtl::Vec2f offsetH(0.0f, 0.0f);
	gmtl::Vec2f offsetV(0.0f, 0.0f);

	m_GaussianParamsH->InitializeGaussianFilter(
		(float) stdDev, spreadKernelSizes*2+1, &spreadH, &offsetH, 1.0f, (float)shadowMapSize, (float)shadowMapSize, TRUE);

	m_GaussianParamsV->InitializeGaussianFilter(
		(float) stdDev, spreadKernelSizes*2+1, &spreadV, &offsetV, 1.0f, (float)shadowMapSize, (float)shadowMapSize, TRUE);

	wstring fileName = _W("[shd]\\basefxutils\\gaussianblur1d.fx");
	IByteBufferPtr pBuffer = m_pShadowFX->GetAppCallback().GetFileData(fileName.c_str());
	CRCDataKey id;
	id.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));

	m_pBlur1Effect = LoadGaussianEffect(&id, pBuffer, &m_pShadowFX->GetRenderer(), m_GaussianParamsH, pSM);
	m_pBlur2Effect = LoadGaussianEffect(&id, pBuffer, &m_pShadowFX->GetRenderer(), m_GaussianParamsV, pBackup);	

	// Add gaussian blur draw ops
	_LOOPi(CF_NUM_FACES)
	{
		m_pCubeBlurEffect[i] = NULL;  

		m_pBlur1Group[i]->Reset();
		m_pBlur1Group[i]->AddDrawOp(m_ScreenMesh.GetQuadVertices(), m_ScreenMesh.GetQuadIndices(),
			m_pBlur1Effect, NULL, GetGaussianBlurEffectTech(m_pBlur1Effect, m_GaussianParamsH->GetGaussianSize()), RVarData());

		m_pBlur2Group[i]->Reset();
		m_pBlur2Group[i]->AddDrawOp(m_ScreenMesh.GetQuadVertices(), m_ScreenMesh.GetQuadIndices(),
			m_pBlur2Effect, NULL, GetGaussianBlurEffectTech(m_pBlur2Effect, m_GaussianParamsV->GetGaussianSize()), RVarData());
	}

	m_pFinal = pFinal;
}

void CSHFXCubePointLight::SetShadowMapParamsCubePoissonFilter(IRTextureCube* pSM, IRTextureCube* pFinal, IRDepthBuffer* pDepth, eBFXUPoissonBlurSamples blurSamples, float spreadScale, boolean isUseNoiseBlur)
{
	_DEBUG_ASSERT(pSM->GetSize(0) == pFinal->GetSize(0));
	_DEBUG_ASSERT(pDepth->GetWidth() >= pSM->GetSize(0));
	_DEBUG_ASSERT(pDepth->GetHeight() >= pSM->GetSize(0));

	if(!m_PoissonParams)
		m_PoissonParams = _NEW BFXUPoissonBlurParams;

	_LOOPi(CF_NUM_FACES)
	{
		IRTextureRT* pRT = pSM->GetTextureRT(0, (eRCubeFace) i);
		_DEBUG_ASSERT(pRT);
		_DEBUG_ASSERT(pRT->GetWidth() == pRT->GetHeight());
		m_pCasterGroups[i]->ClearRenderTargets();
		m_pCasterGroups[i]->AddRenderTarget(pRT);
		m_pCasterGroups[i]->SetDepthBuffer(pDepth);
		m_pCasterGroups[i]->SetClearDepth(1.0f);
		m_pCasterGroups[i]->SetClearColor(gmtl::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
		m_pCasterGroups[i]->SetViewport(0, 0, pRT->GetWidth(), pRT->GetHeight());

		pRT = pFinal->GetTextureRT(0, (eRCubeFace)i);
		_DEBUG_ASSERT(pRT);
		_DEBUG_ASSERT(pRT->GetWidth() == pRT->GetHeight());
		m_pNoDrawOpClearGroups[i]->ClearRenderTargets();
		m_pNoDrawOpClearGroups[i]->AddRenderTarget(pRT);
		m_pNoDrawOpClearGroups[i]->SetViewport(0, 0, pRT->GetWidth(), pRT->GetHeight());
		m_pNoDrawOpClearGroups[i]->SetClearColor(gmtl::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));

		m_pBlur2Group[i]->ClearRenderTargets();
		m_pBlur2Group[i]->AddRenderTarget(pRT);
		m_pBlur2Group[i]->SetViewport(0, 0, pRT->GetWidth(), pRT->GetHeight());
	}

	m_ScreenMesh.Initialize(pFinal->GetSize(0), pFinal->GetSize(0), &m_pShadowFX->GetRenderer());
	
	m_PoissonParams->InitializePoissonFilter(blurSamples, &m_pShadowFX->GetRenderer(), spreadScale, (float)pFinal->GetSize(0), (float)pFinal->GetSize(0), isUseNoiseBlur);

	wstring fileName = _W("[shd]\\basefxutils\\poissondiskcubeblur.fx");
	IByteBufferPtr pBuffer = m_pShadowFX->GetAppCallback().GetFileData(fileName.c_str());
	CRCDataKey id;
	id.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));

	m_pBlur1Effect = NULL;
	m_pBlur2Effect = NULL;
	
	_LOOPi(CF_NUM_FACES)
	{
		m_pCubeBlurEffect[i] = LoadPoissonCubeEffect(&id, pBuffer, &m_pShadowFX->GetRenderer(), m_PoissonParams, pSM, (eRCubeFace) i);

		m_pBlur2Group[i]->Reset();
		m_pBlur2Group[i]->AddDrawOp(m_ScreenMesh.GetQuadVertices(), m_ScreenMesh.GetQuadIndices(),
			m_pCubeBlurEffect[i], NULL, GetPoissonBlurEffectTech(m_pCubeBlurEffect[i], m_PoissonParams), RVarData());
	}

	m_pFinal = pFinal;
}

void CSHFXCubePointLight::SetShadowMapParamsPoissonFilter(IRTexture2D* pSM, IRTextureCube* pFinal, IRDepthBuffer* pDepth, eBFXUPoissonBlurSamples blurSamples, float spreadScale, boolean isUseNoiseBlur)
{
	// Initialize all render groups
	_DEBUG_ASSERT(pSM->GetWidth(0) == pSM->GetHeight(0));
	_DEBUG_ASSERT(pDepth->GetWidth() >= pSM->GetHeight(0));

	if(!m_PoissonParams)
		m_PoissonParams = _NEW BFXUPoissonBlurParams;

	uint shadowMapSize = pSM->GetWidth(0);

	_LOOPi(CF_NUM_FACES)
	{
		IRTextureRT* pRT = pSM->GetTextureRT(0);
		_DEBUG_ASSERT(pRT);
		_DEBUG_ASSERT(pRT->GetWidth() == shadowMapSize);
		m_pCasterGroups[i]->ClearRenderTargets();
		m_pCasterGroups[i]->AddRenderTarget(pRT);
		m_pCasterGroups[i]->SetDepthBuffer(pDepth);
		m_pCasterGroups[i]->SetClearDepth(1.0f);
		m_pCasterGroups[i]->SetClearColor(gmtl::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
		m_pCasterGroups[i]->SetViewport(0, 0, shadowMapSize, shadowMapSize);

		pRT = pFinal->GetTextureRT(0, (eRCubeFace)i);
		_DEBUG_ASSERT(pRT);
		_DEBUG_ASSERT(pRT->GetWidth() == shadowMapSize);
		m_pNoDrawOpClearGroups[i]->ClearRenderTargets();
		m_pNoDrawOpClearGroups[i]->AddRenderTarget(pRT);
		m_pNoDrawOpClearGroups[i]->SetViewport(0, 0, shadowMapSize, shadowMapSize);
		m_pNoDrawOpClearGroups[i]->SetClearColor(gmtl::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));

		m_pBlur2Group[i]->ClearRenderTargets();
		m_pBlur2Group[i]->AddRenderTarget(pRT);
		m_pBlur2Group[i]->SetViewport(0, 0, shadowMapSize, shadowMapSize);
	}

	m_ScreenMesh.Initialize(shadowMapSize, shadowMapSize, &m_pShadowFX->GetRenderer());

	m_PoissonParams->InitializePoissonFilter(blurSamples, &m_pShadowFX->GetRenderer(), spreadScale, (float)shadowMapSize, (float)shadowMapSize, isUseNoiseBlur);

	wstring fileName = _W("[shd]\\basefxutils\\poissondiskblur.fx");
	IByteBufferPtr pBuffer = m_pShadowFX->GetAppCallback().GetFileData(fileName.c_str());
	CRCDataKey id;
	id.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));

	m_pBlur1Effect = NULL;
	m_pBlur2Effect = LoadPoissonEffect(&id, pBuffer, &m_pShadowFX->GetRenderer(), m_PoissonParams, pSM);	

	_LOOPi(CF_NUM_FACES)
	{
		m_pCubeBlurEffect[i] = NULL;  

		m_pBlur2Group[i]->Reset();
		m_pBlur2Group[i]->AddDrawOp(m_ScreenMesh.GetQuadVertices(), m_ScreenMesh.GetQuadIndices(),
			m_pBlur2Effect, NULL, GetPoissonBlurEffectTech(m_pBlur2Effect, m_PoissonParams), RVarData());
	}

	m_pFinal = pFinal;
}

void CSHFXCubePointLight::SetShadowMapParams(IRTextureCube* pFinal, IRDepthBuffer* pDepth)
{
	_LOOPi(CF_NUM_FACES)
	{
		IRTextureRT* pRT = pFinal->GetTextureRT(0, (eRCubeFace)i);
		_DEBUG_ASSERT(pRT);
		_DEBUG_ASSERT(pRT->GetWidth() <= pDepth->GetWidth());
		_DEBUG_ASSERT(pRT->GetHeight() <= pDepth->GetHeight());

		m_pCasterGroups[i]->ClearRenderTargets();
		m_pCasterGroups[i]->AddRenderTarget(pRT);
		m_pCasterGroups[i]->SetDepthBuffer(pDepth);
		m_pCasterGroups[i]->SetClearDepth(1.0f);
		m_pCasterGroups[i]->SetClearColor(gmtl::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
		m_pCasterGroups[i]->SetViewport(0, 0, pRT->GetWidth(), pRT->GetHeight());

		m_pNoDrawOpClearGroups[i]->ClearRenderTargets();
		m_pNoDrawOpClearGroups[i]->AddRenderTarget(pRT);
		m_pNoDrawOpClearGroups[i]->SetViewport(0, 0, pRT->GetWidth(), pRT->GetHeight());
		m_pNoDrawOpClearGroups[i]->SetClearColor(gmtl::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));

		m_pCubeBlurEffect[i] = NULL;  
	}

	m_pBlur1Effect = NULL;
	m_pBlur2Effect = NULL;

	m_pFinal = pFinal;
}

_NAMESPACE_END