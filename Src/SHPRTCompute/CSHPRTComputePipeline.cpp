//================================================================================
//
//		CSHPRTComputePipeline.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/28/2012
//
//================================================================================

#include "SHPRTComputeFramework.h"

_NAMESPACE_BEGIN

int SHPRTComputeOpaqueSortLess(const void* pL, const void* pR)
{
	const RRenderOp* pLhs = *((const RRenderOp**) pL);
	const RRenderOp* pRhs = *((const RRenderOp**) pR);

	// Sort by shaders
	if( pLhs->pEffectTemplate != pRhs->pEffectTemplate )
		return (pLhs->pEffectTemplate < pRhs->pEffectTemplate) ? 1 : -1;
	//if( pLhs->technique != pRhs->technique ) 
	//	return (pLhs->technique < pRhs->technique) ? 1 : -1;
	//if( pLhs->pass != pRhs->pass ) 
	//	return pLhs->pass < pRhs->pass;

	// Sort by constant params
	if( pLhs->pEffect != pRhs->pEffect )
		return (pLhs->pEffect < pRhs->pEffect) ? 1 : -1;

	//if( pLhs->pVBGroup != pRhs->pVBGroup )
	//	return pLhs->pVBGroup < pRhs->pVBGroup;

	//if( pLhs->pIB != pRhs->pIB )
	//	return pLhs->pIB < pRhs->pIB; 

	if( pLhs->opHash != pRhs->opHash )
		return (pLhs->opHash < pRhs->opHash) ? 1 : -1;

    // Front to back
    if(pLhs->userData.fVal != pRhs->userData.fVal )
        return (pLhs->userData.fVal > pRhs->userData.fVal) ? 1 : -1;

    return 0;
}

int SHPRTComputeTransSortLess(const void* pL, const void* pR)
{
	const RRenderOp* pLhs = *((const RRenderOp**) pL);
	const RRenderOp* pRhs = *((const RRenderOp**) pR);

    // Sort back to front
    if(pLhs->userData.fVal != pRhs->userData.fVal )
        return (pLhs->userData.fVal < pRhs->userData.fVal) ? 1 : -1;

    return 0;
}

void CSHPRTComputePipeline::InitializeGPUAccumulation(IRRenderer* pRenderer, IAppCallback* pCallback)
{
	_LOOPi(SHPRT_COMPUTE_OCCL_RT_LOG_SIZE)
	{
		uint size = 1 << (SHPRT_COMPUTE_OCCL_RT_LOG_SIZE - i - 1);
		m_ScreenMesh[i].Initialize(size, size, pRenderer);

		m_pAccumulationRedGroup[i] = pRenderer->GetRResourceMgr().CreateRenderGroup(NULL);
		m_pAccumulationRedGroup[i]->SetDepthBuffer(NULL);
		m_pAccumulationRedGroup[i]->SetViewport(0, 0, size, size);
		m_pAccumulationRedGroup[i]->SetClearColor(gmtl::Vec4f(0.0f, 0.0f, 0.0f, 0.0f));

		m_pAccumulationGreenGroup[i] = pRenderer->GetRResourceMgr().CreateRenderGroup(NULL);
		m_pAccumulationGreenGroup[i]->SetDepthBuffer(NULL);
		m_pAccumulationGreenGroup[i]->SetViewport(0, 0, size, size);
		m_pAccumulationGreenGroup[i]->SetClearColor(gmtl::Vec4f(0.0f, 0.0f, 0.0f, 0.0f));

		m_pAccumulationBlueGroup[i] = pRenderer->GetRResourceMgr().CreateRenderGroup(NULL);
		m_pAccumulationBlueGroup[i]->SetDepthBuffer(NULL);
		m_pAccumulationBlueGroup[i]->SetViewport(0, 0, size, size);
		m_pAccumulationBlueGroup[i]->SetClearColor(gmtl::Vec4f(0.0f, 0.0f, 0.0f, 0.0f));
	}

	_LOOPi(NUM_LATENCY_BUFFERS)
	{
		_LOOPj(SHPRT_COMPUTE_OCCL_RT_LOG_SIZE - 1)
		{
			uint size = 1 << (SHPRT_COMPUTE_OCCL_RT_LOG_SIZE - j - 1);

			m_pAccumulationOcclBand1RT[i][j] = pRenderer->GetRResourceMgr().CreateTexture2DRT(size, size, 1, TEXF_A32B32G32R32F);
			m_pAccumulationOcclBand2RT[i][j] = pRenderer->GetRResourceMgr().CreateTexture2DRT(size, size, 1, TEXF_A32B32G32R32F);
			m_pAccumulationOcclBandLastRT[i][j] = pRenderer->GetRResourceMgr().CreateTexture2DRT(size, size, 1, TEXF_A32B32G32R32F);
		}		

		// Create 1x1 final RTs
		m_pAccumulationOcclRedBand1RT[i] = pRenderer->GetRResourceMgr().CreateTexture2DRT(1, 1, 1, TEXF_A32B32G32R32F);
		m_pAccumulationOcclRedBand2RT[i] = pRenderer->GetRResourceMgr().CreateTexture2DRT(1, 1, 1, TEXF_A32B32G32R32F);
		m_pAccumulationOcclRedBandLastRT[i] = pRenderer->GetRResourceMgr().CreateTexture2DRT(1, 1, 1, TEXF_A32B32G32R32F);

		m_pAccumulationOcclGreenBand1RT[i] = pRenderer->GetRResourceMgr().CreateTexture2DRT(1, 1, 1, TEXF_A32B32G32R32F);
		m_pAccumulationOcclGreenBand2RT[i] = pRenderer->GetRResourceMgr().CreateTexture2DRT(1, 1, 1, TEXF_A32B32G32R32F);
		m_pAccumulationOcclGreenBandLastRT[i] = pRenderer->GetRResourceMgr().CreateTexture2DRT(1, 1, 1, TEXF_A32B32G32R32F);

		m_pAccumulationOcclBlueBand1RT[i] = pRenderer->GetRResourceMgr().CreateTexture2DRT(1, 1, 1, TEXF_A32B32G32R32F);
		m_pAccumulationOcclBlueBand2RT[i] = pRenderer->GetRResourceMgr().CreateTexture2DRT(1, 1, 1, TEXF_A32B32G32R32F);
		m_pAccumulationOcclBlueBandLastRT[i] = pRenderer->GetRResourceMgr().CreateTexture2DRT(1, 1, 1, TEXF_A32B32G32R32F);
	}

	// Accumulate
	IRResourceMgr& resMgr = pRenderer->GetRResourceMgr();
	wstring str = _W("[shd]\\SHPRTCompute\\AccumulateOccl.fx");
	CRCDataKey id;
	id.Set(str.c_str(), (uint) str.size() * sizeof(wchar));
	IByteBufferPtr pFileBuffer = pCallback->GetFileData(str.c_str());
	_DEBUG_ASSERT(pFileBuffer);
	m_pAccumulationEffect = resMgr.LoadEffect(&id, pFileBuffer, NULL, NULL, 0);

	uint numParams = m_pAccumulationEffect->GetTemplate()->GetNumOfDynamicParams(0);
	memset(m_AccumulateSemanticToIndices, 0xFF, sizeof(m_AccumulateSemanticToIndices));
	_LOOPi(numParams)
	{
		uint semantic = m_pAccumulationEffect->GetTemplate()->GetDynamicParamSemantic(0, i);
		m_AccumulateSemanticToIndices[semantic - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] = i;
	}

	_DEBUG_ASSERT( m_AccumulateSemanticToIndices[SHPRTCOMPUTE_TEX_SIZE_RCP - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] < 0xFF );
	_DEBUG_ASSERT( m_AccumulateSemanticToIndices[SHPRTCOMPUTE_SRC_TEXTURE_1 - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] < 0xFF );
	_DEBUG_ASSERT( m_AccumulateSemanticToIndices[SHPRTCOMPUTE_SRC_TEXTURE_2 - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] < 0xFF );
	_DEBUG_ASSERT( m_AccumulateSemanticToIndices[SHPRTCOMPUTE_SRC_TEXTURE_3 - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] < 0xFF );

	// InitializeRed
	str = _W("[shd]\\SHPRTCompute\\InitializeOcclRed.fx");
	id.Set(str.c_str(), (uint) str.size() * sizeof(wchar));
	pFileBuffer = pCallback->GetFileData(str.c_str());
	_DEBUG_ASSERT(pFileBuffer);
	m_pInitializeRedEffect = resMgr.LoadEffect(&id, pFileBuffer, NULL, NULL, 0);

	numParams = m_pInitializeRedEffect->GetTemplate()->GetNumOfDynamicParams(0);
	memset(m_InitializeRedSemanticToIndices, 0xFF, sizeof(m_InitializeRedSemanticToIndices));
	_LOOPi(numParams)
	{
		uint semantic = m_pInitializeRedEffect->GetTemplate()->GetDynamicParamSemantic(0, i);
		m_InitializeRedSemanticToIndices[semantic - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] = i;
	}

	_DEBUG_ASSERT( m_InitializeRedSemanticToIndices[SHPRTCOMPUTE_WEIGHT_TEXTURE - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] < 0xFF );
	_DEBUG_ASSERT( m_InitializeRedSemanticToIndices[SHPRTCOMPUTE_TEX_SIZE_RCP - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] < 0xFF );
	_DEBUG_ASSERT( m_InitializeRedSemanticToIndices[SHPRTCOMPUTE_SRC_TEXTURE_1 - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] < 0xFF );
	_DEBUG_ASSERT( m_InitializeRedSemanticToIndices[SHPRTCOMPUTE_PARA_VIEW - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] < 0xFF );

	// InitializeGreen
	str = _W("[shd]\\SHPRTCompute\\InitializeOcclGreen.fx");
	id.Set(str.c_str(), (uint) str.size() * sizeof(wchar));
	pFileBuffer = pCallback->GetFileData(str.c_str());
	_DEBUG_ASSERT(pFileBuffer);
	m_pInitializeGreenEffect = resMgr.LoadEffect(&id, pFileBuffer, NULL, NULL, 0);

	numParams = m_pInitializeGreenEffect->GetTemplate()->GetNumOfDynamicParams(0);
	memset(m_InitializeGreenSemanticToIndices, 0xFF, sizeof(m_InitializeGreenSemanticToIndices));
	_LOOPi(numParams)
	{
		uint semantic = m_pInitializeGreenEffect->GetTemplate()->GetDynamicParamSemantic(0, i);
		m_InitializeGreenSemanticToIndices[semantic - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] = i;
	}

	_DEBUG_ASSERT( m_InitializeGreenSemanticToIndices[SHPRTCOMPUTE_WEIGHT_TEXTURE - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] < 0xFF );
	_DEBUG_ASSERT( m_InitializeGreenSemanticToIndices[SHPRTCOMPUTE_TEX_SIZE_RCP - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] < 0xFF );
	_DEBUG_ASSERT( m_InitializeGreenSemanticToIndices[SHPRTCOMPUTE_SRC_TEXTURE_1 - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] < 0xFF );
	_DEBUG_ASSERT( m_InitializeGreenSemanticToIndices[SHPRTCOMPUTE_PARA_VIEW - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] < 0xFF );

	// InitializeBlue
	str = _W("[shd]\\SHPRTCompute\\InitializeOcclBlue.fx");
	id.Set(str.c_str(), (uint) str.size() * sizeof(wchar));
	pFileBuffer = pCallback->GetFileData(str.c_str());
	_DEBUG_ASSERT(pFileBuffer);
	m_pInitializeBlueEffect = resMgr.LoadEffect(&id, pFileBuffer, NULL, NULL, 0);

	numParams = m_pInitializeBlueEffect->GetTemplate()->GetNumOfDynamicParams(0);
	memset(m_InitializeBlueSemanticToIndices, 0xFF, sizeof(m_InitializeBlueSemanticToIndices));
	_LOOPi(numParams)
	{
		uint semantic = m_pInitializeBlueEffect->GetTemplate()->GetDynamicParamSemantic(0, i);
		m_InitializeBlueSemanticToIndices[semantic - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] = i;
	}

	_DEBUG_ASSERT( m_InitializeBlueSemanticToIndices[SHPRTCOMPUTE_WEIGHT_TEXTURE - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] < 0xFF );
	_DEBUG_ASSERT( m_InitializeBlueSemanticToIndices[SHPRTCOMPUTE_TEX_SIZE_RCP - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] < 0xFF );
	_DEBUG_ASSERT( m_InitializeBlueSemanticToIndices[SHPRTCOMPUTE_SRC_TEXTURE_1 - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] < 0xFF );
	_DEBUG_ASSERT( m_InitializeBlueSemanticToIndices[SHPRTCOMPUTE_PARA_VIEW - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] < 0xFF );

	// Weight texture
	m_pBRDFDirTex =	pRenderer->GetRResourceMgr().CreateTexture2D(NULL, SHPRT_COMPUTE_OCCL_RT_SIZE, SHPRT_COMPUTE_OCCL_RT_SIZE, 1, TEXF_R32F, TEXU_DEFAULT);

	uint pitch = 0;
	byte* pBRDFData = m_pBRDFDirTex->Lock(0, pitch, NULL);
	_DEBUG_ASSERT(pitch == (sizeof(float) * SHPRT_COMPUTE_OCCL_RT_SIZE));
	memcpy(pBRDFData, PARABOLOID_PROJ_INTEGRAL_WEIGHTS, SHPRT_COMPUTE_OCCL_RT_SIZE * SHPRT_COMPUTE_OCCL_RT_SIZE * sizeof(float));
	m_pBRDFDirTex->Unlock(0);
}

void CSHPRTComputePipeline::Initialize(IRRenderer* pRenderer, IAppCallback* pCallback)
{
	m_CurrentLatencyRT = 0;

	_LOOPi(NUM_LATENCY_BUFFERS)
	{
		m_pOcclusionRT[i] = pRenderer->GetRResourceMgr().CreateTexture2DRT(SHPRT_COMPUTE_OCCL_RT_SIZE, SHPRT_COMPUTE_OCCL_RT_SIZE, 1, TEXF_A8R8G8B8);
		m_pOcclusionRTDual[i] = pRenderer->GetRResourceMgr().CreateTexture2DRT(SHPRT_COMPUTE_OCCL_RT_SIZE, SHPRT_COMPUTE_OCCL_RT_SIZE, 1, TEXF_A8R8G8B8);
	}

	m_pOcclusionDB = pRenderer->GetRResourceMgr().CreateDepthBuffer(SHPRT_COMPUTE_OCCL_RT_SIZE, SHPRT_COMPUTE_OCCL_RT_SIZE, DF_D24S8, 0);

	m_pClearGroup = pRenderer->GetRResourceMgr().CreateRenderGroup(NULL);
	m_pClearGroup->SetDepthBuffer(m_pOcclusionDB);
	m_pClearGroup->SetViewport(0, 0, SHPRT_COMPUTE_OCCL_RT_SIZE, SHPRT_COMPUTE_OCCL_RT_SIZE);
	m_pClearGroup->SetClearColor(gmtl::Vec4f(0.0f, 0.0f, 0.0f, 0.0f));
	m_pClearGroup->SetClearDepth(1.0f);

	//m_pComputeOcclusionGroup = pRenderer->GetRResourceMgr().CreateRenderGroup(SHPRTComputeOpaqueSortLess);
	//m_pComputeOcclusionGroup->SetDepthBuffer(m_pOcclusionDB);
	//m_pComputeOcclusionGroup->SetViewport(0, 0, SHPRT_COMPUTE_OCCL_RT_SIZE, SHPRT_COMPUTE_OCCL_RT_SIZE);

	//m_pComputeOcclusionAlphaTestedGroup = pRenderer->GetRResourceMgr().CreateRenderGroup(SHPRTComputeOpaqueSortLess);
	//m_pComputeOcclusionAlphaTestedGroup->SetDepthBuffer(m_pOcclusionDB);
	//m_pComputeOcclusionAlphaTestedGroup->SetViewport(0, 0, SHPRT_COMPUTE_OCCL_RT_SIZE, SHPRT_COMPUTE_OCCL_RT_SIZE);

	//m_pComputeOcclusionTranslucentGroup = pRenderer->GetRResourceMgr().CreateRenderGroup(SHPRTComputeTransSortLess);
	//m_pComputeOcclusionTranslucentGroup->SetDepthBuffer(m_pOcclusionDB);
	//m_pComputeOcclusionTranslucentGroup->SetViewport(0, 0, SHPRT_COMPUTE_OCCL_RT_SIZE, SHPRT_COMPUTE_OCCL_RT_SIZE);

	////InitializeGPUAccumulation(pRenderer, pCallback);

	//AddContext();
}

void CSHPRTComputePipeline::RenderAccumulation(const gmtl::Matrix44f& paraView)
{
	// Red
	IRVertexBufferGroup* pVBGroup = m_ScreenMesh[0].GetQuadVertices();
	IRIndexBuffer* pIB = m_ScreenMesh[0].GetQuadIndices();

	REffectParam params[SHPRTCOMPUTE_NUM_DYNAMIC_SEMANTIC];
	params[ m_InitializeRedSemanticToIndices[SHPRTCOMPUTE_WEIGHT_TEXTURE - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] ].SetTexture2D(m_pBRDFDirTex);
	params[ m_InitializeRedSemanticToIndices[SHPRTCOMPUTE_TEX_SIZE_RCP - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] ].SetFloat( 1.0f / (m_pOcclusionRT[m_CurrentLatencyRT]->GetTextureRT(0)->GetWidth()) );
	params[ m_InitializeRedSemanticToIndices[SHPRTCOMPUTE_SRC_TEXTURE_1 - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] ].SetTexture2D(m_pOcclusionRT[m_CurrentLatencyRT]);
	params[ m_InitializeRedSemanticToIndices[SHPRTCOMPUTE_PARA_VIEW - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] ].SetMatrix44(&paraView);

	RVarData defData;
	defData.iVal = 0;
	m_pAccumulationRedGroup[0]->AddDrawOp(pVBGroup, pIB, m_pInitializeRedEffect, params, 0, defData);
	
	_LOOPi(SHPRT_COMPUTE_OCCL_RT_LOG_SIZE - 1)
	{
		pVBGroup = m_ScreenMesh[i+1].GetQuadVertices();
		pIB = m_ScreenMesh[i+1].GetQuadIndices();

		params[ m_AccumulateSemanticToIndices[SHPRTCOMPUTE_TEX_SIZE_RCP - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] ].SetFloat( 1.0f / (m_pAccumulationOcclBand1RT[m_CurrentLatencyRT][i]->GetTextureRT(0)->GetWidth()) );
		params[ m_AccumulateSemanticToIndices[SHPRTCOMPUTE_SRC_TEXTURE_1 - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] ].SetTexture2D( m_pAccumulationOcclBand1RT[m_CurrentLatencyRT][i] );
		params[ m_AccumulateSemanticToIndices[SHPRTCOMPUTE_SRC_TEXTURE_2 - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] ].SetTexture2D( m_pAccumulationOcclBand2RT[m_CurrentLatencyRT][i] );
		params[ m_AccumulateSemanticToIndices[SHPRTCOMPUTE_SRC_TEXTURE_3 - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] ].SetTexture2D( m_pAccumulationOcclBandLastRT[m_CurrentLatencyRT][i] );
		m_pAccumulationRedGroup[i+1]->AddDrawOp(pVBGroup, pIB, m_pAccumulationEffect, params, 0, defData);
	}

	// Green
	pVBGroup = m_ScreenMesh[0].GetQuadVertices();
	pIB = m_ScreenMesh[0].GetQuadIndices();

	params[ m_InitializeGreenSemanticToIndices[SHPRTCOMPUTE_WEIGHT_TEXTURE - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] ].SetTexture2D(m_pBRDFDirTex);
	params[ m_InitializeGreenSemanticToIndices[SHPRTCOMPUTE_TEX_SIZE_RCP - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] ].SetFloat( 1.0f / (m_pOcclusionRT[m_CurrentLatencyRT]->GetTextureRT(0)->GetWidth()) );
	params[ m_InitializeGreenSemanticToIndices[SHPRTCOMPUTE_SRC_TEXTURE_1 - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] ].SetTexture2D(m_pOcclusionRT[m_CurrentLatencyRT]);
	params[ m_InitializeGreenSemanticToIndices[SHPRTCOMPUTE_PARA_VIEW - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] ].SetMatrix44(&paraView);

	m_pAccumulationGreenGroup[0]->AddDrawOp(pVBGroup, pIB, m_pInitializeGreenEffect, params, 0, defData);

	_LOOPi(SHPRT_COMPUTE_OCCL_RT_LOG_SIZE - 1)
	{
		pVBGroup = m_ScreenMesh[i+1].GetQuadVertices();
		pIB = m_ScreenMesh[i+1].GetQuadIndices();

		params[ m_AccumulateSemanticToIndices[SHPRTCOMPUTE_TEX_SIZE_RCP - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] ].SetFloat( 1.0f / (m_pAccumulationOcclBand1RT[m_CurrentLatencyRT][i]->GetTextureRT(0)->GetWidth()) );
		params[ m_AccumulateSemanticToIndices[SHPRTCOMPUTE_SRC_TEXTURE_1 - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] ].SetTexture2D( m_pAccumulationOcclBand1RT[m_CurrentLatencyRT][i] );
		params[ m_AccumulateSemanticToIndices[SHPRTCOMPUTE_SRC_TEXTURE_2 - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] ].SetTexture2D( m_pAccumulationOcclBand2RT[m_CurrentLatencyRT][i] );
		params[ m_AccumulateSemanticToIndices[SHPRTCOMPUTE_SRC_TEXTURE_3 - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] ].SetTexture2D( m_pAccumulationOcclBandLastRT[m_CurrentLatencyRT][i] );
		m_pAccumulationGreenGroup[i+1]->AddDrawOp(pVBGroup, pIB, m_pAccumulationEffect, params, 0, defData);
	}

	// Blue
	pVBGroup = m_ScreenMesh[0].GetQuadVertices();
	pIB = m_ScreenMesh[0].GetQuadIndices();

	params[ m_InitializeBlueSemanticToIndices[SHPRTCOMPUTE_WEIGHT_TEXTURE - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] ].SetTexture2D(m_pBRDFDirTex);
	params[ m_InitializeBlueSemanticToIndices[SHPRTCOMPUTE_TEX_SIZE_RCP - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] ].SetFloat( 1.0f / (m_pOcclusionRT[m_CurrentLatencyRT]->GetTextureRT(0)->GetWidth()) );
	params[ m_InitializeBlueSemanticToIndices[SHPRTCOMPUTE_SRC_TEXTURE_1 - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] ].SetTexture2D(m_pOcclusionRT[m_CurrentLatencyRT]);
	params[ m_InitializeBlueSemanticToIndices[SHPRTCOMPUTE_PARA_VIEW - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] ].SetMatrix44(&paraView);

	m_pAccumulationBlueGroup[0]->AddDrawOp(pVBGroup, pIB, m_pInitializeBlueEffect, params, 0, defData);

	_LOOPi(SHPRT_COMPUTE_OCCL_RT_LOG_SIZE - 1)
	{
		pVBGroup = m_ScreenMesh[i+1].GetQuadVertices();
		pIB = m_ScreenMesh[i+1].GetQuadIndices();

		params[ m_AccumulateSemanticToIndices[SHPRTCOMPUTE_TEX_SIZE_RCP - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] ].SetFloat( 1.0f / (m_pAccumulationOcclBand1RT[m_CurrentLatencyRT][i]->GetTextureRT(0)->GetWidth()) );
		params[ m_AccumulateSemanticToIndices[SHPRTCOMPUTE_SRC_TEXTURE_1 - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] ].SetTexture2D( m_pAccumulationOcclBand1RT[m_CurrentLatencyRT][i] );
		params[ m_AccumulateSemanticToIndices[SHPRTCOMPUTE_SRC_TEXTURE_2 - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] ].SetTexture2D( m_pAccumulationOcclBand2RT[m_CurrentLatencyRT][i] );
		params[ m_AccumulateSemanticToIndices[SHPRTCOMPUTE_SRC_TEXTURE_3 - SHPRTCOMPUTE_DYNAMIC_SEMANTIC_BEGIN - 1] ].SetTexture2D( m_pAccumulationOcclBandLastRT[m_CurrentLatencyRT][i] );
		m_pAccumulationBlueGroup[i+1]->AddDrawOp(pVBGroup, pIB, m_pAccumulationEffect, params, 0, defData);
	}
}

//void CSHPRTComputePipeline::RenderEffect(BFXRenderContext& renderContext)
//{
//	int alphaIndex = renderContext.pEffect->GetTemplate()->GetConstantParamIndex(BFX_ALPHA_MODE);
//	int alphaType = 0;
//	if(alphaIndex >= 0)
//		alphaType = renderContext.pEffect->GetParam(alphaIndex)->intVal;
//
//	RVarData data;
//	data.fVal = 1.0f;
//
//	IBFXParamPool* pPool = (IBFXParamPool*) renderContext.pContainer->GetParamCallback(BFX_LIB_ID);
//	if(pPool)
//		data.fVal = pPool->GetZDistanceFromCamera();	
//
//	uint context = pPool->GetCurrentContext();
//
//	IRRenderGroup* pGroup;
//	RRenderOp* pOp;
//	switch(alphaType)
//	{
//	case BFX_ALPHA_MODE_OPAQUE:
//		{
//			pGroup = m_pComputeOcclusionGroup;
//			pOp = m_Contexts[context].pOpaqueContext->Alloc();
//			break;
//		}
//	case BFX_ALPHA_MODE_ALPHATESTED:
//		{
//			pGroup = m_pComputeOcclusionAlphaTestedGroup;
//			pOp = m_Contexts[context].pAlphaTestedContext->Alloc();
//			break;
//		}
//
//	case BFX_ALPHA_MODE_TRANSLUCENT:
//		{
//			pGroup = m_pComputeOcclusionTranslucentGroup;
//			pOp = m_Contexts[context].pAlphaBlendedContext->Alloc();
//			break;
//		}
//
//	default: _DEBUG_ASSERT(FALSE); break;
//	}
//
//	pGroup->FillDrawOp(pOp, renderContext.pMesh->pVBGroup, renderContext.pMesh->pIB, renderContext.pEffect, renderContext.pDynamicParams, renderContext.tech, data);
//	//if(pGroup)
//	//{
//	//	pGroup->AddDrawOp(renderContext.pMesh->pVBGroup, renderContext.pMesh->pIB, renderContext.pEffect, renderContext.pDynamicParams, renderContext.tech, data);
//	//}
//}

void CSHPRTComputePipeline::SetOcclusionRT(uint index)
{
	m_CurrentLatencyRT = index;

	m_pClearGroup->ClearRenderTargets();
	//m_pComputeOcclusionGroup->ClearRenderTargets();
	//m_pComputeOcclusionAlphaTestedGroup->ClearRenderTargets();
	//m_pComputeOcclusionTranslucentGroup->ClearRenderTargets();

	m_pClearGroup->AddRenderTarget(m_pOcclusionRT[index]->GetTextureRT(0));
	//m_pComputeOcclusionGroup->AddRenderTarget(m_pOcclusionRT[index]->GetTextureRT(0));
	//m_pComputeOcclusionAlphaTestedGroup->AddRenderTarget(m_pOcclusionRT[index]->GetTextureRT(0));
	//m_pComputeOcclusionTranslucentGroup->AddRenderTarget(m_pOcclusionRT[index]->GetTextureRT(0));

	//// Red
	//_LOOPi(SHPRT_COMPUTE_OCCL_RT_LOG_SIZE - 1)
	//{
	//	m_pAccumulationRedGroup[i]->ClearRenderTargets();
	//	m_pAccumulationRedGroup[i]->AddRenderTarget(m_pAccumulationOcclBand1RT[index][i]);
	//	m_pAccumulationRedGroup[i]->AddRenderTarget(m_pAccumulationOcclBand2RT[index][i]);
	//	m_pAccumulationRedGroup[i]->AddRenderTarget(m_pAccumulationOcclBandLastRT[index][i]);
	//}

	//m_pAccumulationRedGroup[SHPRT_COMPUTE_OCCL_RT_LOG_SIZE - 1]->ClearRenderTargets();
	//m_pAccumulationRedGroup[SHPRT_COMPUTE_OCCL_RT_LOG_SIZE - 1]->AddRenderTarget(m_pAccumulationOcclRedBand1RT[index]);
	//m_pAccumulationRedGroup[SHPRT_COMPUTE_OCCL_RT_LOG_SIZE - 1]->AddRenderTarget(m_pAccumulationOcclRedBand2RT[index]);
	//m_pAccumulationRedGroup[SHPRT_COMPUTE_OCCL_RT_LOG_SIZE - 1]->AddRenderTarget(m_pAccumulationOcclRedBandLastRT[index]);

	//// Green
	//_LOOPi(SHPRT_COMPUTE_OCCL_RT_LOG_SIZE - 1)
	//{
	//	m_pAccumulationGreenGroup[i]->ClearRenderTargets();
	//	m_pAccumulationGreenGroup[i]->AddRenderTarget(m_pAccumulationOcclBand1RT[index][i]);
	//	m_pAccumulationGreenGroup[i]->AddRenderTarget(m_pAccumulationOcclBand2RT[index][i]);
	//	m_pAccumulationGreenGroup[i]->AddRenderTarget(m_pAccumulationOcclBandLastRT[index][i]);
	//}

	//m_pAccumulationGreenGroup[SHPRT_COMPUTE_OCCL_RT_LOG_SIZE - 1]->ClearRenderTargets();
	//m_pAccumulationGreenGroup[SHPRT_COMPUTE_OCCL_RT_LOG_SIZE - 1]->AddRenderTarget(m_pAccumulationOcclGreenBand1RT[index]);
	//m_pAccumulationGreenGroup[SHPRT_COMPUTE_OCCL_RT_LOG_SIZE - 1]->AddRenderTarget(m_pAccumulationOcclGreenBand2RT[index]);
	//m_pAccumulationGreenGroup[SHPRT_COMPUTE_OCCL_RT_LOG_SIZE - 1]->AddRenderTarget(m_pAccumulationOcclGreenBandLastRT[index]);

	//// Blue
	//_LOOPi(SHPRT_COMPUTE_OCCL_RT_LOG_SIZE - 1)
	//{
	//	m_pAccumulationBlueGroup[i]->ClearRenderTargets();
	//	m_pAccumulationBlueGroup[i]->AddRenderTarget(m_pAccumulationOcclBand1RT[index][i]);
	//	m_pAccumulationBlueGroup[i]->AddRenderTarget(m_pAccumulationOcclBand2RT[index][i]);
	//	m_pAccumulationBlueGroup[i]->AddRenderTarget(m_pAccumulationOcclBandLastRT[index][i]);
	//}

	//m_pAccumulationBlueGroup[SHPRT_COMPUTE_OCCL_RT_LOG_SIZE - 1]->ClearRenderTargets();
	//m_pAccumulationBlueGroup[SHPRT_COMPUTE_OCCL_RT_LOG_SIZE - 1]->AddRenderTarget(m_pAccumulationOcclBlueBand1RT[index]);
	//m_pAccumulationBlueGroup[SHPRT_COMPUTE_OCCL_RT_LOG_SIZE - 1]->AddRenderTarget(m_pAccumulationOcclBlueBand2RT[index]);
	//m_pAccumulationBlueGroup[SHPRT_COMPUTE_OCCL_RT_LOG_SIZE - 1]->AddRenderTarget(m_pAccumulationOcclBlueBandLastRT[index]);
}

IRDepthBuffer* CSHPRTComputePipeline::GetOcclusionDB()
{
	return m_pOcclusionDB;
}

IRTexture2D* CSHPRTComputePipeline::GetOcclusionRT(uint index)
{
	return m_pOcclusionRT[index];
}

void CSHPRTComputePipeline::SetOcclusionRTDual(uint index)
{
	m_pClearGroup->ClearRenderTargets();
	//m_pComputeOcclusionGroup->ClearRenderTargets();
	//m_pComputeOcclusionAlphaTestedGroup->ClearRenderTargets();
	//m_pComputeOcclusionTranslucentGroup->ClearRenderTargets();

	m_pClearGroup->AddRenderTarget(m_pOcclusionRTDual[index]->GetTextureRT(0));
	//m_pComputeOcclusionGroup->AddRenderTarget(m_pOcclusionRTDual[index]->GetTextureRT(0));
	//m_pComputeOcclusionAlphaTestedGroup->AddRenderTarget(m_pOcclusionRTDual[index]->GetTextureRT(0));
	//m_pComputeOcclusionTranslucentGroup->AddRenderTarget(m_pOcclusionRTDual[index]->GetTextureRT(0));
}

IRTexture2D* CSHPRTComputePipeline::GetOcclusionRTDual(uint index)
{
	return m_pOcclusionRTDual[index];
}

void CSHPRTComputePipeline::Clear()
{
	m_pClearGroup->Flush();
}

//void CSHPRTComputePipeline::Flush()
//{
//	_LOOPi(m_Contexts.size())
//	{
//		m_pComputeOcclusionGroup->AddToList(m_Contexts[i].pOpaqueContext->GetDataBuffer(), m_Contexts[i].pOpaqueContext->GetNumUsedData());
//		m_pComputeOcclusionAlphaTestedGroup->AddToList(m_Contexts[i].pAlphaTestedContext->GetDataBuffer(), m_Contexts[i].pAlphaTestedContext->GetNumUsedData());
//		m_pComputeOcclusionTranslucentGroup->AddToList(m_Contexts[i].pAlphaBlendedContext->GetDataBuffer(), m_Contexts[i].pAlphaBlendedContext->GetNumUsedData());
//	}
//
//	m_pClearGroup->Flush();
//	m_pComputeOcclusionGroup->Flush();
//	m_pComputeOcclusionAlphaTestedGroup->Flush();
//	m_pComputeOcclusionTranslucentGroup->Flush();
//
//	m_pComputeOcclusionGroup->Reset();
//	m_pComputeOcclusionAlphaTestedGroup->Reset();
//	m_pComputeOcclusionTranslucentGroup->Reset();
//
//	m_OpaquePool.UpdateSize();
//	m_AlphaTestedPool.UpdateSize();
//	m_AlphaBlendedPool.UpdateSize();
//	_LOOPi(m_Contexts.size())
//	{
//		m_Contexts[i].pOpaqueContext->UpdateAndReset();
//		m_Contexts[i].pAlphaTestedContext->UpdateAndReset();
//		m_Contexts[i].pAlphaBlendedContext->UpdateAndReset();
//	}
//
//	//_LOOPi(SHPRT_COMPUTE_OCCL_RT_LOG_SIZE)
//	//{
//	//	m_pAccumulationRedGroup[i]->Flush();
//	//	m_pAccumulationRedGroup[i]->Reset();
//	//}
//
//	//_LOOPi(SHPRT_COMPUTE_OCCL_RT_LOG_SIZE)
//	//{
//	//	m_pAccumulationGreenGroup[i]->Flush();
//	//	m_pAccumulationGreenGroup[i]->Reset();
//	//}
//
//	//_LOOPi(SHPRT_COMPUTE_OCCL_RT_LOG_SIZE)
//	//{
//	//	m_pAccumulationBlueGroup[i]->Flush();
//	//	m_pAccumulationBlueGroup[i]->Reset();
//	//}
//}

//uint CSHPRTComputePipeline::AddContext()
//{
//	PipelineContext contextToAdd;
//	contextToAdd.pOpaqueContext			= _NEW BFXDataContext<RRenderOp>(&m_OpaquePool);
//	contextToAdd.pAlphaTestedContext	= _NEW BFXDataContext<RRenderOp>(&m_AlphaTestedPool);
//	contextToAdd.pAlphaBlendedContext	= _NEW BFXDataContext<RRenderOp>(&m_AlphaBlendedPool);
//
//	m_Contexts.push_back(contextToAdd);
//	return m_Contexts.size() - 1;
//}
//
//uint CSHPRTComputePipeline::GetNumOfContexts()
//{
//	return m_Contexts.size();
//}
//
//void CSHPRTComputePipeline::ResetContext(uint contextID)
//{
//	m_Contexts[contextID].pOpaqueContext->Dispose();
//	m_Contexts[contextID].pAlphaTestedContext->Dispose();
//	m_Contexts[contextID].pAlphaBlendedContext->Dispose();
//}

IRTextureRT* CSHPRTComputePipeline::GetAccumulatedRedBand1RT(uint index)
{
	return m_pAccumulationOcclRedBand1RT[index]->GetTextureRT(0);
}

IRTextureRT* CSHPRTComputePipeline::GetAccumulatedRedBand2RT(uint index)
{
	return m_pAccumulationOcclRedBand2RT[index]->GetTextureRT(0);
}

IRTextureRT* CSHPRTComputePipeline::GetAccumulatedRedBandLastRT(uint index)
{
	return m_pAccumulationOcclRedBandLastRT[index]->GetTextureRT(0);
}

IRTextureRT* CSHPRTComputePipeline::GetAccumulatedGreenBand1RT(uint index)
{
	return m_pAccumulationOcclGreenBand1RT[index]->GetTextureRT(0);
}

IRTextureRT* CSHPRTComputePipeline::GetAccumulatedGreenBand2RT(uint index)
{
	return m_pAccumulationOcclGreenBand2RT[index]->GetTextureRT(0);
}

IRTextureRT* CSHPRTComputePipeline::GetAccumulatedGreenBandLastRT(uint index)
{
	return m_pAccumulationOcclGreenBandLastRT[index]->GetTextureRT(0);
}

IRTextureRT* CSHPRTComputePipeline::GetAccumulatedBlueBand1RT(uint index)
{
	return m_pAccumulationOcclBlueBand1RT[index]->GetTextureRT(0);
}

IRTextureRT* CSHPRTComputePipeline::GetAccumulatedBlueBand2RT(uint index)
{
	return m_pAccumulationOcclBlueBand2RT[index]->GetTextureRT(0);
}

IRTextureRT* CSHPRTComputePipeline::GetAccumulatedBlueBandLastRT(uint index)
{
	return m_pAccumulationOcclBlueBandLastRT[index]->GetTextureRT(0);
}

_NAMESPACE_END