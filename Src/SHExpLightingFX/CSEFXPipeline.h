//==============================================================================
//
//		CSEFXPipeline.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		13/10/2009
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CSEFXPipeline : public ISEFXPipeline, public IBFXRenderCallback, public IRRenderCallback
{
	struct OccluderOpParams
	{
		const Sphere* pSpheres;
		uint numSpheres;
		const Sphere* pCoverage;
		const AABox* pScreenSpace;
	};

	typedef ObjectPoolGrowable<OccluderOpParams, 64> OccluderOpPool;

private:
	CSEFXSHExpLightingFX* m_pSHExpLightingFX;

	BFXMesh					m_SphereMesh;

	BFXUScreenSpaceMesh		m_ScreenMesh;
	BFXUScreenSpaceMesh		m_OcclMesh;
	BFXUScreenSpaceMesh		m_DownsampleDepthMesh;

	IBFXMaterialGroupPtr	m_pLightSHMatGroup;
	IBFXMaterialGroupPtr	m_pOccluderSHMatGroup;
	IBFXMaterialGroupPtr	m_pStencilVolumeMatGroup;

	IBFXMaterialGroupTemplatePtr	m_pLightSHMatGroupTemplate;
	IBFXMaterialGroupTemplatePtr	m_pOccluderSHMatGroupTemplate;

	uint	m_CollapseGroup;
	uint	m_OcclGroup;

	BFXMesh*	m_pCurMesh;
	uint		m_CurrentMatIndex;

	IREffectPtr	m_pCopyToDepthEffect;
	IREffectTemplatePtr m_pCopyToDepthTemplate;

	IREffectPtr	m_pOcclAccumPipeSetup;

	IRTextureRTPtr m_pOccl1;
	IRTextureRTPtr m_pOccl2;
	IRTextureRTPtr m_pOccl3;
	IRTextureRTPtr m_pOccl4;

	IRDepthBufferPtr m_pOcclDepth;

	OccluderOpParams*	m_pCurOccluderOpParam;
	OccluderOpPool		m_OpPool;

public:
	CSEFXPipeline()
		: m_pSHExpLightingFX(NULL)
		, m_CollapseGroup(BFX_INVALID)
		, m_pCurMesh(NULL)
		, m_CurrentMatIndex(BFX_INVALID)
		, m_OcclGroup(BFX_INVALID)
	{
	}

	virtual ~CSEFXPipeline()
	{
	}

private:
	void UpdateSpheresTex(const Sphere* pSpheres, uint numSpheres);
	
	void RenderVisibility(IRVertexBufferGroup* pVBGroup, IRIndexBuffer* pIB, IREffect* pEffect, uint tech, REffectParam* pDynamicParams);
	void RenderCollapseLight(IRVertexBufferGroup* pVBGroup, IRIndexBuffer* pIB, IREffect* pEffect, uint tech, REffectParam* pDynamicParams);
	void RenderStencilVolume(IRVertexBufferGroup* pVBGroup, IRIndexBuffer* pIB, IREffect* pEffect, uint tech, REffectParam* pDynamicParams);

public:
	void SetSHExpLightingFX(CSEFXSHExpLightingFX* pSHExpLightingFX) { m_pSHExpLightingFX = pSHExpLightingFX; }

	void UpdateScreenSize(uint width, uint height);
	void Initialize();

	void Render(const Sphere* pSpheres,
				uint numSpheres,
				const Sphere& coverage, 
				const gmtl::MatrixA44f& coverageWorld,
				const AABox& screenCoverage);
	
	void BeginRenderEffect(BFXMesh& mesh);
	void RenderEffect(IREffect* pEffect, uint tech, REffectParam* pDynamicParams, uint numParams);
	void EndRenderEffect(BFXMesh& mesh);

	void BeginRenderGroup(uint groupNum);
	void EndRenderGroup(uint groupNum);

	void BeginRenderOp(const RRenderOp* pSortData);
	void EndRenderOp(const RRenderOp* pSortData);	
};

_NAMESPACE_END
