//==============================================================================
//
//		CSHFXPointLight.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		16/09/09
//
//==============================================================================

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

void 
CSHFXPointLight::Update(const gmtl::Vec3f& pos)
{
	m_ShadowPLCast[0].posAndFarPlane[0]	= pos[0];
	m_ShadowPLCast[0].posAndFarPlane[1]	= pos[1];
	m_ShadowPLCast[0].posAndFarPlane[2]	= pos[2];

	m_ShadowPLCast[1].posAndFarPlane[0]	= pos[0];
	m_ShadowPLCast[1].posAndFarPlane[1]	= pos[1];
	m_ShadowPLCast[1].posAndFarPlane[2]	= pos[2];
}

void
CSHFXPointLight::SetRange(float range)
{
	m_ShadowPLCast[0].posAndFarPlane[3]	= 1.0f / range;
	m_ShadowPLCast[1].posAndFarPlane[3]	= 1.0f / range;
}

//void 
//CSHFXPointLight::SetSMSize(uint smSize)
//{
//	if(m_pSM && 
//		m_pSM->GetWidth(0) == smSize && 
//		m_pSM->GetHeight(0) == smSize)
//	{
//		return;
//	}
//
//	m_pSM		= m_pShadowFX->GetRenderer().GetRResourceMgr().CreateTextureRT(smSize, smSize, TEXF_G32R32F);
//	m_pBackupSM = m_pShadowFX->GetRenderer().GetRResourceMgr().CreateTextureRT(smSize, smSize, TEXF_G32R32F);
//	m_pDepth	= m_pShadowFX->GetRenderer().GetRResourceMgr().CreateDepthBuffer(smSize, smSize, DF_D24S8, 0);
//
//	m_ScreenMesh.Initialize(smSize, smSize, &m_pShadowFX->GetRenderer());
//}

void 
CSHFXPointLight::Initialize()
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

	//fileName = _W("[shd]\\BaseFXUtils\\ColorFill.fx");
	//id.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));
	//pBuffer = m_pShadowFX->GetAppCallback().GetFileData(fileName.c_str());

	//float expClearColor = gmtl::Math::exp(EXP_SM_PL_K * 1.0f);
	//REffectParam params[1];
	//uint semantics[1];
	//gmtl::Vec4f colorFill;
	//colorFill[0] = colorFill[1] = colorFill[2] = colorFill[3] = expClearColor;
	//params[0].SetVector4(&colorFill);
	//semantics[0] = BFXU_COLOR_FILL;

	//m_pColorFillPLEffect = m_pShadowFX->GetRenderer().GetRResourceMgr().LoadEffect(
	//	&id, 
	//	pBuffer,
	//	params,
	//	semantics,
	//	1);


	//fileName = _W("[shd]\\BaseFXUtils\\ColorChannelEnable.fx");
	//id.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));
	//pBuffer = m_pShadowFX->GetAppCallback().GetFileData(fileName.c_str());
	//m_pColorChannelEffect = m_pShadowFX->GetRenderer().GetRResourceMgr().LoadEffect(
	//	&id, 
	//	pBuffer,
	//	NULL,
	//	NULL,
	//	0);

	//m_pClearGroup = m_pShadowFX->GetRenderer().GetRResourceMgr().CreateRenderGroup(NULL);

	_LOOPi(2)
	{
		m_pCasterGroups[i] = m_pShadowFX->GetRenderer().GetRResourceMgr().CreateRenderGroup(PointLightCasterSortLess);
		
		m_pBlur1Group[i] = m_pShadowFX->GetRenderer().GetRResourceMgr().CreateRenderGroup(NULL, 1);
		m_pBlur2Group[i] = m_pShadowFX->GetRenderer().GetRResourceMgr().CreateRenderGroup(NULL, 1);
	}

	m_pClearFinal = m_pShadowFX->GetRenderer().GetRResourceMgr().CreateRenderGroup(NULL);

	m_pBlur1Effect = NULL;
	m_pBlur2Effect = NULL;

	AddContext();
}

void CSHFXPointLight::SetShadowMapParamsGaussianFilter(IRTexture2D* pSM, IRTexture2D* pBackup, IRTexture2D* pFinal, IRDepthBuffer* pDepth, float stdDev, uint spreadKernelSizes)
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

	_LOOPi(2)
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

		pRT = pFinal->GetTextureRT(0);
		_DEBUG_ASSERT(pRT);
		_DEBUG_ASSERT((pRT->GetWidth() >> 1) == shadowMapSize);

		m_pBlur2Group[i]->ClearRenderTargets();
		m_pBlur2Group[i]->AddRenderTarget(pRT);
		m_pBlur2Group[i]->SetViewport((pRT->GetWidth() >> 1) * (i & 1), 0, pRT->GetWidth() >> 1, pRT->GetHeight());
	}

	m_pClearFinal->ClearRenderTargets();
	m_pClearFinal->AddRenderTarget(pFinal->GetTextureRT(0));
	m_pClearFinal->SetViewport(0, 0, pFinal->GetWidth(0), pFinal->GetHeight(0));
	m_pClearFinal->SetClearColor(gmtl::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));

	m_ScreenMesh.Initialize(pSM->GetWidth(0), pSM->GetHeight(0), &m_pShadowFX->GetRenderer());

	//// Add the clear draw op
	//m_pClearGroup->Reset();
	//m_pClearGroup->AddDrawOp(m_ScreenMesh.GetQuadVertices(), m_ScreenMesh.GetQuadIndices(),
	//	m_pColorFillPLEffect, NULL, 0, RVarData());

	// Set gaussian blur params
	gmtl::Vec2f spreadH(1.0f, 0.0f);
	gmtl::Vec2f spreadV(0.0f, 1.0f);
	gmtl::Vec2f offsetH(0.0f, 0.0f);
	gmtl::Vec2f offsetV(0.0f, 0.0f);

	m_GaussianParamsH->InitializeGaussianFilter(
		(float) stdDev, spreadKernelSizes*2+1, &spreadH, &offsetH, 1.0f, (float)pSM->GetWidth(0), (float)pSM->GetHeight(0), TRUE);

	m_GaussianParamsV->InitializeGaussianFilter(
		(float) stdDev, spreadKernelSizes*2+1, &spreadV, &offsetV, 1.0f, (float)pBackup->GetWidth(0), (float)pBackup->GetHeight(0), TRUE);

	wstring fileName = _W("[shd]\\basefxutils\\gaussianblur1d.fx");
	IByteBufferPtr pBuffer = m_pShadowFX->GetAppCallback().GetFileData(fileName.c_str());
	CRCDataKey id;
	id.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));

	m_pBlur1Effect = LoadGaussianEffect(&id, pBuffer, &m_pShadowFX->GetRenderer(), m_GaussianParamsH, pSM);
	m_pBlur2Effect = LoadGaussianEffect(&id, pBuffer, &m_pShadowFX->GetRenderer(), m_GaussianParamsV, pBackup);	

	// Add gaussian blur draw ops
	_LOOPi(2)
	{
		m_pBlur1Group[i]->Reset();
		m_pBlur1Group[i]->AddDrawOp(m_ScreenMesh.GetQuadVertices(), m_ScreenMesh.GetQuadIndices(),
			m_pBlur1Effect, NULL, GetGaussianBlurEffectTech(m_pBlur1Effect, m_GaussianParamsH->GetGaussianSize()), RVarData());

		m_pBlur2Group[i]->Reset();
		m_pBlur2Group[i]->AddDrawOp(m_ScreenMesh.GetQuadVertices(), m_ScreenMesh.GetQuadIndices(),
			m_pBlur2Effect, NULL, GetGaussianBlurEffectTech(m_pBlur2Effect, m_GaussianParamsV->GetGaussianSize()), RVarData());
	}

	m_pFinal = pFinal;
}

void CSHFXPointLight::SetShadowMapParamsPoissonFilter(IRTexture2D* pSM, IRTexture2D* pFinal, IRDepthBuffer* pDepth, eBFXUPoissonBlurSamples blurSamples, float spreadScale, boolean isUseNoiseBlur)
{
	// Initialize all render groups
	_DEBUG_ASSERT(pSM->GetWidth(0) == pSM->GetHeight(0));
	_DEBUG_ASSERT(pDepth->GetWidth() >= pSM->GetHeight(0));

	if(!m_PoissonParams)
		m_PoissonParams = _NEW BFXUPoissonBlurParams;

	uint shadowMapSize = pSM->GetWidth(0);

	_LOOPi(2)
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

		pRT = pFinal->GetTextureRT(0);
		_DEBUG_ASSERT(pRT);
		_DEBUG_ASSERT((pRT->GetWidth() >> 1) == shadowMapSize);

		m_pBlur2Group[i]->ClearRenderTargets();
		m_pBlur2Group[i]->AddRenderTarget(pRT);
		m_pBlur2Group[i]->SetViewport((pRT->GetWidth() >> 1) * (i & 1), 0, pRT->GetWidth() >> 1, pRT->GetHeight());
	}

	m_pClearFinal->ClearRenderTargets();
	m_pClearFinal->AddRenderTarget(pFinal->GetTextureRT(0));
	m_pClearFinal->SetViewport(0, 0, pFinal->GetWidth(0), pFinal->GetHeight(0));
	m_pClearFinal->SetClearColor(gmtl::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));

	m_ScreenMesh.Initialize(pSM->GetWidth(0), pSM->GetHeight(0), &m_pShadowFX->GetRenderer());

	m_PoissonParams->InitializePoissonFilter(blurSamples, &m_pShadowFX->GetRenderer(), spreadScale, (float)pSM->GetWidth(0), (float)pSM->GetHeight(0), isUseNoiseBlur);

	wstring fileName = _W("[shd]\\basefxutils\\poissondiskblur.fx");
	IByteBufferPtr pBuffer = m_pShadowFX->GetAppCallback().GetFileData(fileName.c_str());
	CRCDataKey id;
	id.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));

	m_pBlur1Effect = NULL;
	m_pBlur2Effect = LoadPoissonEffect(&id, pBuffer, &m_pShadowFX->GetRenderer(), m_PoissonParams, pSM);	

	//m_pBlur1Group->Reset();
	//m_pBlur1Group->AddDrawOp(m_ScreenMesh.GetQuadVertices(), m_ScreenMesh.GetQuadIndices(),
	//	m_pBlur1Effect, NULL, GetGaussianBlurEffectTech(m_pBlur1Effect, m_GaussianParamsH.GetGaussianSize()), RVarData());

	_LOOPi(CF_NUM_FACES)
	{
		m_pBlur2Group[i]->Reset();
		m_pBlur2Group[i]->AddDrawOp(m_ScreenMesh.GetQuadVertices(), m_ScreenMesh.GetQuadIndices(),
			m_pBlur2Effect, NULL, GetPoissonBlurEffectTech(m_pBlur2Effect, m_PoissonParams), RVarData());
	}

	m_pFinal = pFinal;
}

void CSHFXPointLight::SetShadowMapParams(IRTexture2D* pFinal, IRDepthBuffer* pDepth)
{
	_LOOPi(2)
	{
		IRTextureRT* pRT = pFinal->GetTextureRT(0);
		_DEBUG_ASSERT(pRT);
		_DEBUG_ASSERT(pRT->GetWidth() <= pDepth->GetWidth());
		_DEBUG_ASSERT(pRT->GetHeight() <= pDepth->GetHeight());

		m_pCasterGroups[i]->ClearRenderTargets();
		m_pCasterGroups[i]->AddRenderTarget(pRT);
		m_pCasterGroups[i]->SetDepthBuffer(pDepth);

		m_pCasterGroups[i]->SetViewport((pRT->GetWidth() >> 1) * (i & 1), 0, pRT->GetWidth() >> 1, pRT->GetHeight());
	}

	m_pClearFinal->ClearRenderTargets();
	m_pClearFinal->AddRenderTarget(pFinal->GetTextureRT(0));
	m_pClearFinal->SetViewport(0, 0, pFinal->GetWidth(0), pFinal->GetHeight(0));
	m_pClearFinal->SetClearColor(gmtl::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
	m_pClearFinal->SetClearDepth(1.0f);

	m_pBlur1Effect = NULL;
	m_pBlur2Effect = NULL;

	m_pFinal = pFinal;
}

IRRenderGroup* CSHFXPointLight::GetCasterGroup(uint face)
{
	return m_pCasterGroups[face];
}

void CSHFXPointLight::RenderPointLightCaster(BFXRenderContext& context)
{
	CSHFXParamPool* pPool = (CSHFXParamPool*) context.pContainer->GetParamCallback( _GET_LIB_INDEX(SHFX_EFFECT_PARAM_OFFSET) );
	IBFXParamPool* pBFXPool = (IBFXParamPool*) context.pContainer->GetParamCallback( _GET_LIB_INDEX(BFX_EFFECT_PARAM_OFFSET) );

	uint contextID = pBFXPool->GetCurrentContext();
	
	uint faceIndex = pPool->GetCurrentPLCasterFace();
	IRRenderGroup* pCasterGroup = m_pCasterGroups[faceIndex];
	RRenderOp* pOp = m_Contexts[contextID].pCasterContext[faceIndex]->Alloc();

	RVarData data;
	data.fVal = pPool->GetZDistanceFromCamera();

	pCasterGroup->FillDrawOp(pOp, context.pMesh->pVBGroup, context.pMesh->pIB, context.pEffect, 
		context.pDynamicParams, context.tech, data);
}

uint CSHFXPointLight::AddContext()
{
	PipelineContext contextToAdd;
	_LOOPi(2)
	{
		contextToAdd.pCasterContext[i]	= _NEW BFXDataContext<RRenderOp>(&m_CasterPool[i]);
	}

	m_Contexts.push_back(contextToAdd);
	return m_Contexts.size() - 1;
}

uint CSHFXPointLight::GetNumOfContexts()
{
	return m_Contexts.size();
}

void CSHFXPointLight::ResetContext(uint contextID)
{
	_LOOPi(2)
	{
		m_Contexts[contextID].pCasterContext[i]->Dispose();
	}
}

void CSHFXPointLight::Flush()
{
	m_pClearFinal->Flush();

	_LOOPi(2)
	{
		_LOOPj(m_Contexts.size())
		{
			m_pCasterGroups[i]->AddToList(m_Contexts[j].pCasterContext[i]->GetDataBuffer(), m_Contexts[j].pCasterContext[i]->GetNumUsedData());
		}

		if(m_pCasterGroups[i]->GetNumOfDrawOps() > 0)
		{
			m_pCasterGroups[i]->Flush();
			m_pCasterGroups[i]->Reset();
		
			m_CasterPool[i].UpdateSize();
			_LOOPj(m_Contexts.size())
			{
				m_Contexts[j].pCasterContext[i]->UpdateAndReset();
			}

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

	//// Clear the shadow map
	////m_pClearGroup->Flush();

	//m_pCasterGroups[0]->Flush();
	//m_pCasterGroups[0]->Reset();
	//IREffectTemplate* pTemplate = m_p2DPipeSetupEffect->GetTemplate();
	//pTemplate->BeginTechnique(0);
	//pTemplate->BeginPass(0);
	//m_pBlur1Group->Flush();
	//IRTextureRT* pFinal = m_pGaussianVGroup->GetRenderTarget(0);
	//m_pGaussianVGroup->SetViewport(0, 0, pFinal->GetWidth() >> 1, pFinal->GetHeight());
	//m_pGaussianVGroup->Flush();
	//pTemplate->EndPass();
	//pTemplate->EndTechnique();

	//// Clear the shadow map
	////m_pClearGroup->Flush();

	//m_pCasterGroups[1]->Flush();
	//m_pCasterGroups[1]->Reset();
	//pTemplate->BeginTechnique(0);
	//pTemplate->BeginPass(0);
	//m_pBlur1Group->Flush();
	//m_pGaussianVGroup->SetViewport(pFinal->GetWidth() >> 1, 0, pFinal->GetWidth() >> 1, pFinal->GetHeight());
	//m_pGaussianVGroup->Flush();
	//pTemplate->EndPass();
	//pTemplate->EndTechnique();
}

//void CSHFXPointLight::FlushCasterGroups()
//{
//	// Clear the shadow map
//	m_pClearGroup->Flush();
//
//	_LOOPi(2)
//	{
//		// Only render to the appropriate color channels for each cascade
//		IREffectTemplate* pTemplate = m_pColorChannelEffect->GetTemplate();
//		pTemplate->BeginTechnique(i);
//		pTemplate->BeginPass(0);
//
//		m_pCasterGroups[i]->Flush();
//		m_pCasterGroups[i]->Reset();
//
//		pTemplate->EndPass();
//		pTemplate->EndTechnique();
//	}
//}
//
//void CSHFXPointLight::FlushFilterGroups()
//{
//	IREffectTemplate* pTemplate = m_p2DPipeSetupEffect->GetTemplate();
//	pTemplate->BeginTechnique(0);
//	pTemplate->BeginPass(0);
//
//	m_pBlur1Group->Flush();
//	m_pGaussianVGroup->Flush();
//
//	pTemplate->EndPass();
//	pTemplate->EndTechnique();
//}

_NAMESPACE_END
