//================================================================================
//
//		CDEFXLightGroup.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/5/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

class CDEFXLightGroup : public CRefCounted<IDEFXLightGroup>
{
	struct PipelineContext
	{
		RefCountSmartPointer<BFXDataContext<RRenderOp>> pContext;
	};

private:
	BFXDataPool<RRenderOp> m_Pool;
	IRRenderGroupPtr m_pGroup;
	IREffectPtr	m_pDeferredStencil;

	std::vector<PipelineContext> m_Contexts; 

public:
	CDEFXLightGroup()
	{
	}

	~CDEFXLightGroup()
	{
	}

public:
	void Initialize(CDEFXDeferredFX* pDeferred);

	void SetViewport(uint left, uint top, uint width, uint height);
	void AddRenderTarget(IRTextureRT* pRT);
	void ClearRenderTargets();
	//uint GetNumOfRenderTargets();
	//IRTextureRT* GetRenderTarget(uint rtIndex);

	void SetDepthBuffer(IRDepthBuffer* pDepth);

	void Flush();

	void RenderEffect(BFXRenderContext& renderContext, uint context);

	uint AddContext();
	uint GetNumOfContexts();
	void ResetContext(uint contextID);
};

_NAMESPACE_END	