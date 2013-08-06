//==============================================================================
//
//		CBLFXBloomFX.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/20/2008
//
//==============================================================================

#include "BloomFXFramework.h"

_NAMESPACE_BEGIN

void 
CBLFXBloomFX::UninitializeBloomEffect()
{
	m_pBloomDownSampleSourceMed = NULL;
	m_pBloomRTHMed = NULL;
	m_pBloomRTVMed = NULL;

	m_pGaussianBlurEffectHMed = NULL;
	m_pGaussianBlurEffectVMed = NULL;
	m_pBloomDownSampleEffectMed = NULL;
	m_pBloomUpSampleComposeEffectMed = NULL;
}

void 
CBLFXBloomFX::UninitializeStarGlareEffect()
{
	m_pStarRT1 = NULL;
	m_pStarRT2 = NULL;

	m_pStarBlurEffectH = NULL;
	m_pStarBlurEffectH2 = NULL;
	m_pStarBlurEffectV = NULL;
	m_pStarBlurEffectV2 = NULL;
	m_pPreBlurEffectH = NULL;
	m_pPreBlurEffectV = NULL;
}

void 
CBLFXBloomFX::UninitializePreparationEffects()
{
	m_pRTDownSampledSource = NULL;
	m_pComposition = NULL;
	m_pRTBrightPassSource = NULL;
	
	m_pBrightPassEffect = NULL;
	m_pBlendEffect = NULL;
}

void 
CBLFXBloomFX::SetBloom(boolean isEnabled)
{
	if(isEnabled)
	{
		if(!m_pBloomRTHMed)
		{
			InitializeBloomEffect();
		}
	}
	else
	{
		UninitializeBloomEffect();
	}
}
	
void 
CBLFXBloomFX::SetStarGlare(boolean isEnabled)
{
	if(isEnabled)
	{
		if(!m_pStarRT1)
		{
			InitializeStarGlareEffect();
		}
	}
	else
	{
		UninitializeStarGlareEffect();
	}
}
	
boolean 
CBLFXBloomFX::InitializeStarGlareEffect()
{
	InitializePreparationEffects();
	
	const RDisplayInfo& info = m_pRenderer->GetDisplayInfo();
	
	// Init texture RTs
	m_pStarRT1 = m_pRenderer->GetRResourceMgr().CreateTexture2DRT(
		(uint) (info.backBufferWidth / 4), (uint) (info.backBufferHeight / 4), 1, TEXF_A8R8G8B8);
	m_pStarRT2 = m_pRTBrightPassSource;//m_pRenderer->GetRResourceMgr().CreateTextureRT(
	//	(uint) (info.backBufferWidth / 4), (uint) (info.backBufferHeight / 4), TEXF_A8R8G8B8);
	
	if(!m_pStarRT1 || !m_pStarRT2)
	{
		m_pCallback->Log(_W("Error: Failed to enable star blur effect"));
		//Disable();
		return FALSE;
	}
	
	gmtl::Vec2f spreadH(1.0f, 0.0f);
	gmtl::Vec2f spreadV(0.0f, 1.0f);

	//uint kernelSizeH = (uint) ((m_pStarRT1->GetWidth(0) / 500.0f) * 13); 
	//kernelSizeH = kernelSizeH > 25 ? 25 : kernelSizeH;	
	//uint kernelSizeV = (uint) ((m_pStarRT2->GetWidth(0) / 500.0f) * 13); 
	//kernelSizeV = kernelSizeV > 25 ? 25 : kernelSizeV;
	
	//m_PreBlurKernelH.InitializeGaussianFilter(
	//	1.0f, kernelSizeH, &spreadH, 1.0f, 
	//	(float)m_pRTBrightPassSource->GetWidth(0), (float)m_pRTBrightPassSource->GetHeight(0), TRUE);
	//m_PreBlurKernelV.InitializeGaussianFilter(
	//	1.0f, kernelSizeV, &spreadV, 1.0f, 
	//	(float)m_pStarRT1->GetWidth(0), (float)m_pStarRT1->GetHeight(0), TRUE);
	
	m_StarQuad.Initialize(m_pStarRT1->GetWidth(), m_pStarRT1->GetHeight(), m_pRenderer);

	wstring fileName = _W("[shd]\\basefxutils\\gaussianblur1d.fx");
	IByteBufferPtr pBuffer = m_pCallback->GetFileData(fileName.c_str());
	CRCDataKey id;
	id.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));
		
	m_pPreBlurEffectH = LoadGaussianEffect(
		&id, pBuffer, m_pRenderer, &m_PreBlurKernelH, m_pRTBrightPassSource);	// Source to RT1
	m_pPreBlurEffectV = LoadGaussianEffect(
		&id, pBuffer, m_pRenderer, &m_PreBlurKernelV, m_pStarRT1);		// RT1 to RT2

	gmtl::Vec2f starBlurH(1.0f, 0.707f);
	gmtl::Vec2f starBlurV(0.707f, -1.0f);
	gmtl::normalize(starBlurH);
	gmtl::normalize(starBlurV);
	
	//float minVal = gmtl::Math::Min(spreadH[0], spreadH[1]);
	//spreadH /= minVal;
	//minVal = gmtl::Math::Min(spreadV[0], spreadV[1]);
	//spreadV /= minVal;
	
	m_StarBlurH.InitializeBlurParams(
		8, 1, starBlurH, 0.935f, 0.16f, m_pStarRT2->GetWidth(), m_pStarRT2->GetHeight()); // RT2 to RT1
	m_StarBlurH2.InitializeBlurParams(
		8, 8, starBlurH, 0.935f, 0.77f, m_pStarRT1->GetWidth(), m_pStarRT1->GetHeight()); // RT1 to Compose
	m_StarBlurV.InitializeBlurParams(
		8, 1, starBlurV, 0.935f, 0.16f, m_pStarRT2->GetWidth(), m_pStarRT2->GetHeight()); // RT2 to RT1
	m_StarBlurV2.InitializeBlurParams(
		8, 8, starBlurV, 0.935f, 0.77f, m_pStarRT1->GetWidth(), m_pStarRT1->GetHeight()); // RT1 to Compose
	
	fileName = _W("[shd]\\BloomFX\\StarBlur.fx");
	pBuffer = m_pCallback->GetFileData(fileName.c_str());
	id.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));
	uint blurSemantics[3];
	blurSemantics[0] = BFXU_FILTER_TEXTURE_OFFSETS;
	blurSemantics[1] = BFXU_FILTER_WEIGHTS;
	blurSemantics[2] = BFXU_RT1;
	REffectParam blurParams[3];
	blurParams[0].SetValue(
		(void*) m_StarBlurH.GetLineOffsets(), BLFX_MAX_STAR_LINE_SIZE * sizeof(gmtl::Vec2f));
	blurParams[1].SetValue(
		(void*) m_StarBlurH.GetWeights(), BLFX_MAX_STAR_LINE_SIZE * sizeof(float));
	blurParams[2].SetTextureRT(m_pStarRT2);
	m_pStarBlurEffectH = m_pRenderer->GetRResourceMgr().LoadEffect(&id, pBuffer, blurParams, blurSemantics, 3);
	
	blurParams[0].SetValue(
		(void*) m_StarBlurH2.GetLineOffsets(), BLFX_MAX_STAR_LINE_SIZE * sizeof(gmtl::Vec2f));
	blurParams[1].SetValue(
		(void*) m_StarBlurH2.GetWeights(), BLFX_MAX_STAR_LINE_SIZE * sizeof(float));
	blurParams[2].SetTextureRT(m_pStarRT1);
	m_pStarBlurEffectH2 = m_pRenderer->GetRResourceMgr().LoadEffect(&id, pBuffer, blurParams, blurSemantics, 3);

	blurParams[0].SetValue(
		(void*) m_StarBlurV.GetLineOffsets(), BLFX_MAX_STAR_LINE_SIZE * sizeof(gmtl::Vec2f));
	blurParams[1].SetValue(
		(void*) m_StarBlurV.GetWeights(), BLFX_MAX_STAR_LINE_SIZE * sizeof(float));
	blurParams[2].SetTextureRT(m_pStarRT2);
	m_pStarBlurEffectV = m_pRenderer->GetRResourceMgr().LoadEffect(&id, pBuffer, blurParams, blurSemantics, 3);

	blurParams[0].SetValue(
		(void*) m_StarBlurV2.GetLineOffsets(), BLFX_MAX_STAR_LINE_SIZE * sizeof(gmtl::Vec2f));
	blurParams[1].SetValue(
		(void*) m_StarBlurV2.GetWeights(), BLFX_MAX_STAR_LINE_SIZE * sizeof(float));
	blurParams[2].SetTextureRT(m_pStarRT1);
	m_pStarBlurEffectV2 = m_pRenderer->GetRResourceMgr().LoadEffect(&id, pBuffer, blurParams, blurSemantics, 3);
	
	return TRUE;
}

boolean
CBLFXBloomFX::InitializePreparationEffects(boolean resetAll)
{
	if(!m_pRTBrightPassSource || resetAll)
	{
		const RDisplayInfo& info = m_pRenderer->GetDisplayInfo();
	
		m_pRTBrightPassSource = m_pRenderer->GetRResourceMgr().CreateTexture2DRT(
			(uint) (info.backBufferWidth / 4), (uint) (info.backBufferHeight / 4), 1, TEXF_A8R8G8B8);
		m_pRTDownSampledSource = m_pRenderer->GetRResourceMgr().CreateTexture2DRT(
			(uint) (info.backBufferWidth / 4), (uint) (info.backBufferHeight / 4), 1, TEXF_A8R8G8B8);
		m_pComposition = m_pRenderer->GetRResourceMgr().CreateTexture2DRT(
			(uint) (info.backBufferWidth / 4), (uint) (info.backBufferHeight / 4), 1, TEXF_A8R8G8B8);
			
		// Get the effect templates
		wstring fileName = _W("[shd]\\bloomfx\\brightpass.fx");
		CRCDataKey id;
		IByteBufferPtr pBuffer = m_pCallback->GetFileData(fileName.c_str());
		id.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));
		uint brightPassSemantics[1];
		brightPassSemantics[0] = BLFX_RT1;
		REffectParam brightPassParams[1];
		brightPassParams[0].SetTextureRT(m_pRTDownSampledSource);
		m_pBrightPassEffect = m_pRenderer->GetRResourceMgr().LoadEffect(&id, pBuffer, brightPassParams, brightPassSemantics, 1);
		
		fileName = _W("[shd]\\bloomfx\\blend.fx");
		pBuffer = m_pCallback->GetFileData(fileName.c_str());
		id.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));
		uint blendSemantics[1];
		blendSemantics[0] = BLFX_RT1;
		REffectParam blendParams[1];
		blendParams[0].SetTextureRT(m_pComposition);
		m_pBlendEffect = m_pRenderer->GetRResourceMgr().LoadEffect(
			&id,
			pBuffer,
			blendParams,
			blendSemantics,
			1);
		
		m_BrightPassQuad.Initialize(m_pRTBrightPassSource->GetWidth(), m_pRTBrightPassSource->GetHeight(), m_pRenderer);
		m_CompositionQuad.Initialize(m_pComposition->GetWidth(), m_pComposition->GetHeight(), m_pRenderer);
		m_BackBufferQuad.Initialize(info.backBufferWidth, info.backBufferHeight, m_pRenderer);
	}
	
	return TRUE;
}

boolean
CBLFXBloomFX::InitializeBloomEffect()
{
	InitializePreparationEffects();
	
	const RDisplayInfo& info = m_pRenderer->GetDisplayInfo();
	
	// Init texture RTs
	m_pBloomDownSampleSourceMed = m_pRenderer->GetRResourceMgr().CreateTexture2DRT(
		(uint) (info.backBufferWidth / 8), (uint) (info.backBufferHeight / 8), 1, TEXF_A8R8G8B8);
	m_pBloomRTHMed = m_pRenderer->GetRResourceMgr().CreateTexture2DRT(
		(uint) (info.backBufferWidth / 8), (uint) (info.backBufferHeight / 8), 1, TEXF_A8R8G8B8);
	m_pBloomRTVMed = m_pBloomDownSampleSourceMed;
	
	m_pBloomRTHLarge = m_pRenderer->GetRResourceMgr().CreateTexture2DRT(
		(uint) (info.backBufferWidth / 4), (uint) (info.backBufferHeight / 4), 1, TEXF_A8R8G8B8);
	//m_pBloomRTVLarge = m_pRenderer->GetRResourceMgr().CreateTextureRT(
	//	(uint) (info.backBufferWidth / 4), (uint) (info.backBufferHeight / 4), TEXF_A8R8G8B8);
	
	if(!m_pBloomDownSampleSourceMed || !m_pBloomRTHMed || 
		!m_pBloomRTHLarge)
	{
		m_pCallback->Log(_W("Error: Failed to enable bloom"));
		return FALSE;
	}
	
	gmtl::Vec2f spreadH(1.0f, 0.0f);
	gmtl::Vec2f spreadV(0.0f, 1.0f);
	gmtl::Vec2f offsetH(0.0f, 0.0f);
	gmtl::Vec2f offsetV(0.0f, 0.0f);

	uint kernelSizeH = (uint) ((m_pBloomDownSampleSourceMed->GetWidth() / 230.0f) * 19); 
	kernelSizeH = kernelSizeH > 19 ? 19 : (kernelSizeH < 9 ? 9 : kernelSizeH);	
	uint kernelSizeV = (uint) ((m_pBloomDownSampleSourceMed->GetWidth() / 230.0f) * 19); 
	kernelSizeV = kernelSizeV > 19 ? 19 : (kernelSizeV < 9 ? 9 : kernelSizeV);	
	
	m_GaussianBlurKernelHMed.InitializeGaussianFilter(
		2.55f, kernelSizeH, &spreadH, &offsetH, 2.8f, 
		(float)m_pBloomDownSampleSourceMed->GetWidth(), (float)m_pBloomDownSampleSourceMed->GetHeight(), TRUE);
	m_GaussianBlurKernelVMed.InitializeGaussianFilter(
		2.55f, kernelSizeV, &spreadV, &offsetV, 2.8f, (
		float)m_pBloomRTHMed->GetWidth(), (float)m_pBloomRTHMed->GetHeight(), TRUE);

	m_BloomQuadMed.Initialize(m_pBloomDownSampleSourceMed->GetWidth(), m_pBloomDownSampleSourceMed->GetHeight(), m_pRenderer);
	
	kernelSizeH = (uint) ((m_pBloomRTHLarge->GetWidth() / 450.0f) * 19); 
	kernelSizeH = kernelSizeH > 19 ? 19 : (kernelSizeH < 9 ? 9 : kernelSizeH);	
	kernelSizeV = (uint) ((m_pComposition->GetWidth() / 450.0f) * 19); 
	kernelSizeV = kernelSizeV > 19 ? 19 : (kernelSizeV < 9 ? 9 : kernelSizeV);	
	
	m_GaussianBlurKernelHLarge.InitializeGaussianFilter(
		2.5f, kernelSizeH, &spreadH, &offsetH, 2.1f, 
		(float)m_pRTBrightPassSource->GetWidth(), (float)m_pRTBrightPassSource->GetHeight(), TRUE);
	m_GaussianBlurKernelVLarge.InitializeGaussianFilter(
		2.5f, kernelSizeV, &spreadV, &offsetH, 2.1f, (
		float)m_pBloomRTHLarge->GetWidth(), (float)m_pBloomRTHLarge->GetHeight(), TRUE);

	m_BloomQuadLarge.Initialize(m_pBloomRTHLarge->GetWidth(), m_pBloomRTHLarge->GetHeight(), m_pRenderer);
	
	// Get the effect templates
	wstring fileName = _W("[shd]\\basefxutils\\gaussianblur1d.fx");
	IByteBufferPtr pBuffer = m_pCallback->GetFileData(fileName.c_str());
	CRCDataKey id;
	id.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));
		
	m_pGaussianBlurEffectHMed = LoadGaussianEffect(
		&id, pBuffer, m_pRenderer, &m_GaussianBlurKernelHMed, m_pBloomDownSampleSourceMed);
	m_pGaussianBlurEffectVMed = LoadGaussianEffect(
		&id, pBuffer, m_pRenderer, &m_GaussianBlurKernelVMed, m_pBloomRTHMed);	
	
	//m_pGaussianBlurEffectHLarge = LoadGaussianEffect(
	//	id, pBuffer, m_pRenderer, &m_GaussianBlurKernelHLarge, m_pRTBrightPassSource);
	//m_pGaussianBlurEffectVLarge = LoadGaussianEffect(
	//	id, pBuffer, m_pRenderer, &m_GaussianBlurKernelVLarge, m_pBloomRTHLarge);	
		
	fileName = _W("[shd]\\bloomfx\\blend.fx");
	pBuffer = m_pCallback->GetFileData(fileName.c_str());
	id.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));
	uint blendSemantics[1];
	blendSemantics[0] = BLFX_RT1;
	REffectParam blendParams[1];
	blendParams[0].SetTextureRT(m_pRTBrightPassSource);
	m_pBloomDownSampleEffectMed = m_pRenderer->GetRResourceMgr().LoadEffect(
		&id,
		pBuffer,
		blendParams,
		blendSemantics,
		1);	
	
	blendParams[0].SetTextureRT(m_pBloomRTVMed);
	m_pBloomUpSampleComposeEffectMed = m_pRenderer->GetRResourceMgr().LoadEffect(
		&id,
		pBuffer,
		blendParams,
		blendSemantics,
		1);	
		
	return TRUE;
}

boolean 
CBLFXBloomFX::Initialize(IRRenderer& renderer, IAppCallback& callback)
{	
	m_pRenderer = &renderer;
	m_pCallback = &callback;
	
	m_Pipeline.SetBloomFX(this);
	m_ParamPool.SetBloomFX(this);
	m_Pipeline.Initialize();
	
	return TRUE;
}

void 
CBLFXBloomFX::AddPreparationPasses()
{
	IRPipeline& pipeline = m_pRenderer->GetRPipeline();
		
	int groupIndex = pipeline.CreateNewRenderGroup(BLFX_GROUP_BRIGHT_PASS_DOWN_SAMPLE, NULL, m_Pipeline);

	REffectParam dynParams[RRenderOp::MAX_NUM_PARAMS];
	m_ParamPool.InitializeEffect(*m_pBrightPassEffect, 1.0f, dynParams);
	pipeline.AddToRenderGroup(
		groupIndex, (m_BrightPassQuad.GetQuadVertices()), (m_BrightPassQuad.GetQuadIndices()),
		 m_pBrightPassEffect, dynParams, 0, NULL);		
	pipeline.AddRenderTarget(groupIndex, m_pRTBrightPassSource);

	groupIndex = pipeline.CreateNewRenderGroup(BLFX_GROUP_PREPARE_COMPOSITION, NULL, m_Pipeline);
	pipeline.SetClearColor(groupIndex, gmtl::Vec4f(0.0f, 0.0f, 0.0f, 0.0f));
	pipeline.AddRenderTarget(groupIndex, m_pComposition);

	groupIndex = pipeline.CreateNewRenderGroup(BLFX_GROUP_BLEND, NULL, m_Pipeline);	
	pipeline.AddToRenderGroup(groupIndex, 
		(m_BackBufferQuad.GetQuadVertices()), (m_BackBufferQuad.GetQuadIndices()), 
		m_pBlendEffect, NULL, 0, NULL);	
}

void 
CBLFXBloomFX::AddBloomPass()
{
	IRPipeline& pipeline = m_pRenderer->GetRPipeline();
		
	uint groupIndex = pipeline.CreateNewRenderGroup(BLFX_GROUP_BLOOM_MED_DOWN_SAMPLE, NULL, m_Pipeline);
	pipeline.AddToRenderGroup(groupIndex, (m_BloomQuadMed.GetQuadVertices()),
		 (m_BloomQuadMed.GetQuadIndices()), m_pBloomDownSampleEffectMed, NULL, 1, NULL);
	pipeline.AddRenderTarget(groupIndex, m_pBloomDownSampleSourceMed);
	
	groupIndex = pipeline.CreateNewRenderGroup(BLFX_GROUP_BLOOM_FILTER_H_MED, NULL, m_Pipeline);
	uint tech = GetGaussianBlurEffectTech(m_pGaussianBlurEffectHMed, m_GaussianBlurKernelHMed.GetGaussianSize());
	pipeline.AddToRenderGroup(groupIndex, (m_BloomQuadMed.GetQuadVertices()),
		 (m_BloomQuadMed.GetQuadIndices()), m_pGaussianBlurEffectHMed, NULL, tech, NULL);
	pipeline.AddRenderTarget(groupIndex, m_pBloomRTHMed);
	
	groupIndex = pipeline.CreateNewRenderGroup(BLFX_GROUP_BLOOM_FILTER_V_MED, NULL, m_Pipeline);
	tech = GetGaussianBlurEffectTech(m_pGaussianBlurEffectVMed, m_GaussianBlurKernelVMed.GetGaussianSize()); //(uint)(m_pGaussianBlurEffectHMed->GetTemplate()->GetNumOfTechs() * m_GaussianDetailLevel + 0.5f);
	pipeline.AddToRenderGroup(groupIndex, (m_BloomQuadMed.GetQuadVertices()), 
		(m_BloomQuadMed.GetQuadIndices()), m_pGaussianBlurEffectVMed, NULL, tech, NULL);
	pipeline.AddRenderTarget(groupIndex, m_pBloomRTVMed);
	
	groupIndex = pipeline.CreateNewRenderGroup(BLFX_GROUP_BLOOM_UP_SAMPLE_COMPOSE_MED, NULL, m_Pipeline);
	pipeline.AddToRenderGroup(groupIndex, (m_CompositionQuad.GetQuadVertices()), 
		(m_CompositionQuad.GetQuadIndices()), m_pBloomUpSampleComposeEffectMed, NULL, 1, NULL);
	pipeline.AddRenderTarget(groupIndex, m_pComposition);
	
	//groupIndex = pipeline.CreateNewRenderGroup(BLFX_GROUP_BLOOM_FILTER_H_LARGE, NULL, m_Pipeline);
	//tech = GetGaussianBlurEffectTech(m_pGaussianBlurEffectHLarge, m_GaussianBlurKernelHLarge.GetGaussianSize());
	//pipeline.AddToRenderGroup(groupIndex, (m_BloomQuadLarge.GetQuadVertices()),
	//	 (m_BloomQuadLarge.GetQuadIndices()), m_pGaussianBlurEffectHLarge, NULL, tech, NULL);
	//pipeline.AddRenderTarget(groupIndex, m_pBloomRTHLarge);
	//
	//groupIndex = pipeline.CreateNewRenderGroup(BLFX_GROUP_BLOOM_FILTER_V_LARGE, NULL, m_Pipeline);
	//tech = GetGaussianBlurEffectTech(m_pGaussianBlurEffectHLarge, m_GaussianBlurKernelVLarge.GetGaussianSize());
	//pipeline.AddToRenderGroup(groupIndex, (m_CompositionQuad.GetQuadVertices()), 
	//	(m_CompositionQuad.GetQuadIndices()), m_pGaussianBlurEffectVLarge, NULL, tech, NULL);
	//pipeline.AddRenderTarget(groupIndex, m_pComposition);
}

void 
CBLFXBloomFX::AddStarGlarePass()
{
	IRPipeline& pipeline = m_pRenderer->GetRPipeline();
		
	//REffectParam dynParams[256];

	//int groupIndex = pipeline.CreateNewRenderGroup(BLFX_GROUP_STAR_PRE_FILTER_H, NULL, m_Pipeline);
	//uint tech = GetGaussianBlurEffectTech(m_pPreBlurEffectH, m_PreBlurKernelH.GetGaussianSize());
	//pipeline.AddToRenderGroup(groupIndex, 
	//	(m_StarQuad.GetQuadVertices()), (m_StarQuad.GetQuadIndices()), 
	//	m_pPreBlurEffectH, NULL, tech, NULL);
	//pipeline.AddRenderTarget(groupIndex, m_pStarRT1);
	//
	//groupIndex = pipeline.CreateNewRenderGroup(BLFX_GROUP_STAR_PRE_FILTER_V, NULL, m_Pipeline);
	//tech = GetGaussianBlurEffectTech(m_pPreBlurEffectV, m_PreBlurKernelV.GetGaussianSize()); //(uint)(m_pGaussianBlurEffectHMed->GetTemplate()->GetNumOfTechs() * m_GaussianDetailLevel + 0.5f);
	//pipeline.AddToRenderGroup(groupIndex, 
	//	(m_StarQuad.GetQuadVertices()), (m_StarQuad.GetQuadIndices()),
	//	 m_pPreBlurEffectV, NULL, tech, NULL);
	//pipeline.AddRenderTarget(groupIndex, m_pStarRT2);
	//
	int groupIndex = pipeline.CreateNewRenderGroup(BLFX_GROUP_STAR_H, NULL, m_Pipeline);
	uint tech = 4;// (uint)(m_pGaussianBlurEffectHMed->GetTemplate()->GetNumOfTechs() * m_GaussianDetailLevel + 0.5f);
	pipeline.AddToRenderGroup(groupIndex, 
		(m_StarQuad.GetQuadVertices()), (m_StarQuad.GetQuadIndices()), 
		m_pStarBlurEffectH, NULL, tech, NULL);
	pipeline.AddRenderTarget(groupIndex, m_pStarRT1);
	
	groupIndex = pipeline.CreateNewRenderGroup(BLFX_GROUP_STAR_H_COMPOSE, NULL, m_Pipeline);
	pipeline.AddToRenderGroup(groupIndex, 
		(m_CompositionQuad.GetQuadVertices()), (m_CompositionQuad.GetQuadIndices()),
		 m_pStarBlurEffectH2, NULL, tech, NULL);
	pipeline.AddRenderTarget(groupIndex, m_pComposition);
	
	groupIndex = pipeline.CreateNewRenderGroup(BLFX_GROUP_STAR_V, NULL, m_Pipeline); //RFX_INVALID;
	pipeline.AddToRenderGroup(groupIndex, 
		(m_StarQuad.GetQuadVertices()), (m_StarQuad.GetQuadIndices()),
		 m_pStarBlurEffectV, NULL, tech, NULL);	
	pipeline.AddRenderTarget(groupIndex, m_pStarRT1);
	
	groupIndex = pipeline.CreateNewRenderGroup(BLFX_GROUP_STAR_V_COMPOSE, NULL, m_Pipeline);
	pipeline.AddToRenderGroup(groupIndex, 
		(m_CompositionQuad.GetQuadVertices()), (m_CompositionQuad.GetQuadIndices()), 
		m_pStarBlurEffectV2, NULL, tech, NULL);	
	pipeline.AddRenderTarget(groupIndex, m_pComposition);		
}

void
CBLFXBloomFX::Render()
{
	const RDisplayInfo& info = m_pRenderer->GetDisplayInfo();
	
	int backBufferQuadWidth = m_BackBufferQuad.GetScreenWidth();
	int backBufferQuadHeight = m_BackBufferQuad.GetScreenHeight();
	
	// The screen res has changed	
	if(backBufferQuadWidth != info.backBufferWidth ||
		backBufferQuadHeight != info.backBufferHeight )
	{
		InitializePreparationEffects(TRUE);
		
		if(m_pBloomRTHMed)
		{
			InitializeBloomEffect();
		}
		
		if(m_pStarRT1)		
		{
			InitializeStarGlareEffect();
		}
	}				
	
	AddPreparationPasses();
		
	if(m_pBloomRTHMed)
		AddBloomPass();
		
	if(m_pStarRT1)
		AddStarGlarePass();
}

_NAMESPACE_END