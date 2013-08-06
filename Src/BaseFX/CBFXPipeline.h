//==============================================================================
//
//		CBFXPipeline.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/28/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CBFXPipeline : public CRefCounted<IBFXPipeline>
{
	struct PipelineContext
	{
		RefCountSmartPointer<BFXDataContext<RRenderOp>> pOpaqueContext;
		RefCountSmartPointer<BFXDataContext<RRenderOp>> pAlphaTestedContext;
		RefCountSmartPointer<BFXDataContext<RRenderOp>> pAlphaBlendedContext;
	};

private:	
	CBFXBaseFX* m_pBase;

	IREffectPtr m_pTransSetupEffect;
	IREffectPtr m_pOpaqueSetupEffect;
	//IREffectPtr m_pAlphaTestedSetupEffect;
	IREffectPtr m_pWireFrameSetupEffect;

	BFXDataPool<RRenderOp> m_OpaquePool;
	IRRenderGroupPtr m_pOpaque;
	BFXDataPool<RRenderOp> m_AlphaTestedPool;
	IRRenderGroupPtr m_pAlphaTested;
	BFXDataPool<RRenderOp> m_AlphaBlendedPool;
	IRRenderGroupPtr m_pAlphaBlended;

	std::vector<PipelineContext> m_Contexts; 

public: 
	CBFXPipeline()
		: m_pBase(NULL)
	{
	}

	virtual ~CBFXPipeline()
	{
	}

public: 
	void SetBaseFX(CBFXBaseFX* pBase) { m_pBase = pBase; }
	void Initialize();

	void SetViewport(uint left, uint top, uint width, uint height);
	void AddRenderTarget(IRTextureRT* pTexRT);
	void ClearRenderTargets();
	void SetDepthBuffer(IRDepthBuffer* pDepth);

	void FlushOpaqueGroup();
	void FlushAlphaTestedGroup();
	void FlushAlphaBlendedGroup();

	void ResetGroup();

	uint AddContext();
	void ResetContext(uint contextID);
	uint GetNumOfContexts();

	//void BeginRenderEffect(BFXMesh& mesh);
	void RenderEffect(BFXRenderContext& renderContext);
	//void EndRenderEffect(BFXMesh& mesh);
};

_NAMESPACE_END
