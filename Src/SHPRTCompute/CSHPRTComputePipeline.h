//================================================================================
//
//		CSHPRTComputePipeline.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/28/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

class CSHPRTComputePipeline
{
	//struct PipelineContext
	//{
	//	RefCountSmartPointer<BFXDataContext<RRenderOp>> pOpaqueContext;
	//	RefCountSmartPointer<BFXDataContext<RRenderOp>> pAlphaTestedContext;
	//	RefCountSmartPointer<BFXDataContext<RRenderOp>> pAlphaBlendedContext;
	//};

private:
	IRRenderGroupPtr	m_pClearGroup;

	//BFXDataPool<RRenderOp> m_OpaquePool;
	//IRRenderGroupPtr	m_pComputeOcclusionGroup;
	//BFXDataPool<RRenderOp> m_AlphaTestedPool;
	//IRRenderGroupPtr	m_pComputeOcclusionAlphaTestedGroup;
	//BFXDataPool<RRenderOp> m_AlphaBlendedPool;
	//IRRenderGroupPtr	m_pComputeOcclusionTranslucentGroup;

	//std::vector<PipelineContext> m_Contexts; 

	IRTexture2DPtr		m_pOcclusionRT[NUM_LATENCY_BUFFERS];
	IRTexture2DPtr		m_pOcclusionRTDual[NUM_LATENCY_BUFFERS];
	IRDepthBufferPtr	m_pOcclusionDB;

	IRTexture2DPtr		m_pAccumulationOcclBand1RT[NUM_LATENCY_BUFFERS][SHPRT_COMPUTE_OCCL_RT_LOG_SIZE-1];
	IRTexture2DPtr		m_pAccumulationOcclBand2RT[NUM_LATENCY_BUFFERS][SHPRT_COMPUTE_OCCL_RT_LOG_SIZE-1];
	IRTexture2DPtr		m_pAccumulationOcclBandLastRT[NUM_LATENCY_BUFFERS][SHPRT_COMPUTE_OCCL_RT_LOG_SIZE-1];

	IRTexture2DPtr		m_pAccumulationOcclRedBand1RT[NUM_LATENCY_BUFFERS];
	IRTexture2DPtr		m_pAccumulationOcclRedBand2RT[NUM_LATENCY_BUFFERS];
	IRTexture2DPtr		m_pAccumulationOcclRedBandLastRT[NUM_LATENCY_BUFFERS];

	IRTexture2DPtr		m_pAccumulationOcclGreenBand1RT[NUM_LATENCY_BUFFERS];
	IRTexture2DPtr		m_pAccumulationOcclGreenBand2RT[NUM_LATENCY_BUFFERS];
	IRTexture2DPtr		m_pAccumulationOcclGreenBandLastRT[NUM_LATENCY_BUFFERS];

	IRTexture2DPtr		m_pAccumulationOcclBlueBand1RT[NUM_LATENCY_BUFFERS];
	IRTexture2DPtr		m_pAccumulationOcclBlueBand2RT[NUM_LATENCY_BUFFERS];
	IRTexture2DPtr		m_pAccumulationOcclBlueBandLastRT[NUM_LATENCY_BUFFERS];

	BFXUScreenSpaceMesh	m_ScreenMesh[SHPRT_COMPUTE_OCCL_RT_LOG_SIZE];
	IRRenderGroupPtr	m_pAccumulationRedGroup[SHPRT_COMPUTE_OCCL_RT_LOG_SIZE];
	IRRenderGroupPtr	m_pAccumulationGreenGroup[SHPRT_COMPUTE_OCCL_RT_LOG_SIZE];
	IRRenderGroupPtr	m_pAccumulationBlueGroup[SHPRT_COMPUTE_OCCL_RT_LOG_SIZE];

	IREffectPtr			m_pAccumulationEffect;
	uint				m_AccumulateSemanticToIndices[SHPRTCOMPUTE_NUM_DYNAMIC_SEMANTIC];
	IREffectPtr			m_pInitializeRedEffect;
	uint				m_InitializeRedSemanticToIndices[SHPRTCOMPUTE_NUM_DYNAMIC_SEMANTIC];
	IREffectPtr			m_pInitializeGreenEffect;
	uint				m_InitializeGreenSemanticToIndices[SHPRTCOMPUTE_NUM_DYNAMIC_SEMANTIC];
	IREffectPtr			m_pInitializeBlueEffect;
	uint				m_InitializeBlueSemanticToIndices[SHPRTCOMPUTE_NUM_DYNAMIC_SEMANTIC];

	IRTexture2DPtr		m_pBRDFDirTex;
	uint				m_CurrentLatencyRT;

public:
	CSHPRTComputePipeline()
	{
	}

	~CSHPRTComputePipeline()
	{
	}

private:
	void InitializeGPUAccumulation(IRRenderer* pRenderer, IAppCallback* pCallback);

public:
	void Initialize(IRRenderer* pRenderer, IAppCallback* pCallback);
	//void RenderEffect(BFXRenderContext& renderContext);

	void RenderAccumulation(const gmtl::Matrix44f& paraView);
	void Clear();
	//void Flush();

	uint AddContext();
	uint GetNumOfContexts();
	void ResetContext(uint contextID);

	void SetOcclusionRT(uint index);
	void SetOcclusionRTDual(uint index);
	IRTexture2D* GetOcclusionRT(uint index);
	IRTexture2D* GetOcclusionRTDual(uint index);
	IRDepthBuffer* GetOcclusionDB();

	IRTextureRT* GetAccumulatedRedBand1RT(uint index);
	IRTextureRT* GetAccumulatedRedBand2RT(uint index);
	IRTextureRT* GetAccumulatedRedBandLastRT(uint index);

	IRTextureRT* GetAccumulatedGreenBand1RT(uint index);
	IRTextureRT* GetAccumulatedGreenBand2RT(uint index);
	IRTextureRT* GetAccumulatedGreenBandLastRT(uint index);

	IRTextureRT* GetAccumulatedBlueBand1RT(uint index);
	IRTextureRT* GetAccumulatedBlueBand2RT(uint index);
	IRTextureRT* GetAccumulatedBlueBandLastRT(uint index);
};

_NAMESPACE_END
