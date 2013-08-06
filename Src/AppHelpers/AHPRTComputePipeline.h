//================================================================================
//
//		AHPRTComputePipeline.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/28/2012
//
//================================================================================

#pragma once

#if defined(_IS_USE_APPHELPER_LIB_PRTCOMPUTE)

_NAMESPACE_BEGIN

class AHPRTComputePipeline : public CRefCounted<IRefCounted>, public IBFXRenderCallback
{
	struct PipelineContext
	{
		RefCountSmartPointer<BFXDataContext<RRenderOp>> pBackfaceContext;
		RefCountSmartPointer<BFXDataContext<RRenderOp>> pBackfaceContextAlphaTested;
	};

private:
	//IREffectPtr m_pAlphaTestedSetupEffect;

	BFXDataPool<RRenderOp> m_BackfacePool;
	BFXDataPool<RRenderOp> m_BackfacePoolAlphaTested;

	IRRenderGroupPtr m_pBackfaceGroup;
	IRRenderGroupPtr m_pBackfaceGroupAlphaTested;

	std::vector<PipelineContext> m_Contexts; 

public:
	AHPRTComputePipeline()
	{
	}

	~AHPRTComputePipeline()
	{
	}

public:
	void Initialize(IRRenderer* pRenderer);

	void SetViewport(uint left, uint top, uint width, uint height);
	void AddRenderTarget(IRTextureRT* pRT);
	void ClearRenderTargets();

	void SetDepthBuffer(IRDepthBuffer* pDepth);

	void Flush();

	uint AddContext();
	uint GetNumOfContexts();
	void ResetContext(uint contextID);

	void RenderEffect(BFXRenderContext& renderContext);
};

_DECLARE_SMARTPTR(AHPRTComputePipeline)

_NAMESPACE_END

#endif