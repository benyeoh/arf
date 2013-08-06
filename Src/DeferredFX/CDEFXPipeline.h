//==============================================================================
//
//		CDEFXPipeline.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		13/10/2009
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

struct AccumOp
{
	int		alphaType;
	float	distance;
};

class CDEFXPipeline : public CRefCounted<IDEFXPipeline>
{
	struct PipelineContext
	{
		RefCountSmartPointer<BFXDataContext<RRenderOp>> pAccumContext;
		RefCountSmartPointer<BFXDataContext<RRenderOp>> pAccumAlphaTestedContext;
	};

private:
	CDEFXDeferredFX* m_pDeferredFX;

	//BFXMesh*	m_pCurMesh;
	//uint		m_CurrentMatIndex;

	//typedef ObjectPoolGrowable<AccumOp, 512> AccumOpPool;

	//AccumOpPool m_AccumOps;

	//// Accumulation buffers
	//IRTextureRTPtr m_pZPos;
	//IRTextureRTPtr m_pNormals;
	//IRTextureRTPtr m_pAlbedo;
	//
 //   IRTextureRTPtr m_pDownsample2XZPos;
 //   IRTextureRTPtr m_pDownsample2XNormals;

	//uint m_AccumGroup;
 //   uint m_Downsample2XGroup;

	IREffectPtr	m_pDeferredStencil;
	//IREffectPtr m_pAlphaTestedSetupEffect;
    //IREffectPtr m_pDownsample2XEffect;
    //IREffectTemplatePtr m_pCopyAccumEffectTemplate;

    //BFXUScreenSpaceMesh m_Downsample2XMesh;

    //uint m_2XDownsampleRefs;

	BFXDataPool<RRenderOp> m_AccumPool;
	IRRenderGroupPtr m_pAccumGroup;
	BFXDataPool<RRenderOp> m_AccumAlphaTestedPool;
	IRRenderGroupPtr m_pAccumGroupAlphaTested;

	std::vector<PipelineContext> m_Contexts; 

public:
	CDEFXPipeline()
		: m_pDeferredFX(NULL)
		//, m_pCurMesh(NULL)
		//, m_CurrentMatIndex(0)
		//, m_AccumGroup(BFX_INVALID)
  //      , m_Downsample2XGroup(BFX_INVALID)
  //      , m_2XDownsampleRefs(0)
	{
	}

	virtual ~CDEFXPipeline()
	{
	}

//private:
//    void Uninitialize2XDownsample();
//    void Initialize2XDownsample(uint screenWidth, uint screenHeight);
//    void Render2XDownsample();

public:
	void SetDeferredFX(CDEFXDeferredFX* pDeferredFX) { m_pDeferredFX = pDeferredFX; }
	void Initialize();

	void SetViewport(uint left, uint top, uint width, uint height);
	void AddAccumulationTargets(IRTextureRT* pZPos, IRTextureRT* pNormal, IRTextureRT* pMaterial);
	void GetAccumulationTargets(IRTextureRT** ppZPos, IRTextureRT** ppNormal, IRTextureRT** ppMaterial, IRTextureRT** ppExtra, uint& numExtra);

	void AddRenderTarget(IRTextureRT* pRT);
	void ClearRenderTargets();

	void SetDepthBuffer(IRDepthBuffer* pDepth);

	void Flush();

	uint AddContext();
	uint GetNumOfContexts();
	void ResetContext(uint contextID);

	void RenderEffect(BFXRenderContext& renderContext);

	//void UpdateScreenSize(uint width, uint height);

 //   // For now, only downsample normals
 //   void Enable2XDownsampleAccum();
 //   void Disable2XDownsampleAccum();
 //   
	//void RenderAccum(IRVertexBufferGroup* pVBGroup, IRIndexBuffer* pIB, IREffect* pEffect, uint tech, REffectParam* pDynamicParams, int alphaType);
 //   
	//void BeginRenderEffect(BFXMesh& mesh);
	//void RenderEffect(IREffect* pEffect, uint tech, REffectParam* pDynamicParams, uint numParams);
	//void EndRenderEffect(BFXMesh& mesh);

	//void BeginRenderGroup(uint groupNum);
	//void EndRenderGroup(uint groupNum);

	//void BeginRenderOp(const RRenderOp* pSortData);
	//void EndRenderOp(const RRenderOp* pSortData);		
};

_NAMESPACE_END
