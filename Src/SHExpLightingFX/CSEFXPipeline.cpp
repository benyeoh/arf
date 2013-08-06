//==============================================================================
//
//		CSEFXPipeline.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		13/10/2009
//
//==============================================================================

#include "SHExpLightingFXFramework.h"

_NAMESPACE_BEGIN

int SHExpLightSortLess(const RRenderOp* pLhs, const RRenderOp* pRhs)
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

	//if( pLhs->opHash != pRhs->opHash )
	//	return (pLhs->opHash < pRhs->opHash) ? 1 : -1;

    // Front to back
    if(pLhs->userData.fVal != pRhs->userData.fVal )
        return (pLhs->userData.fVal > pRhs->userData.fVal) ? 1 : -1;

    return 0;
}

void
CSEFXPipeline::UpdateScreenSize(uint width, uint height)
{
	m_ScreenMesh.Initialize(width, height, &m_pSHExpLightingFX->GetRenderer(), BUF_DEFAULT, FALSE);

	uint occlWidth	= width / 2;
	uint occlHeight = height / 2;

	m_DownsampleDepthMesh.Initialize(occlWidth, occlHeight, &m_pSHExpLightingFX->GetRenderer());
	m_OcclMesh.Initialize(occlWidth, occlHeight, &m_pSHExpLightingFX->GetRenderer(), BUF_DYNAMIC, FALSE);

	IRResourceMgr& resMgr = m_pSHExpLightingFX->GetRenderer().GetRResourceMgr();
	m_pOccl1	= resMgr.CreateTexture2DRT(occlWidth, occlHeight, 1, TEXF_A16B16G16R16F);
	m_pOccl2	= resMgr.CreateTexture2DRT(occlWidth, occlHeight, 1, TEXF_A16B16G16R16F);
	m_pOccl3	= resMgr.CreateTexture2DRT(occlWidth, occlHeight, 1, TEXF_A16B16G16R16F);
	//m_pOccl4	= resMgr.CreateTexture2DRT(occlWidth, occlHeight, TEXF_A16B16G16R16F);
	m_pOcclDepth	= resMgr.CreateDepthBuffer(occlWidth, occlHeight, DF_D24S8, 0);

	// Create the collapse light material
	const RRendererInfo& info = m_pSHExpLightingFX->GetRenderer().GetRendererInfo();
	REffectParam lightSHParams[32];
	gmtl::Vec2f texSize;
	texSize[0] = (float)occlWidth; 
	texSize[1] = (float)occlHeight;
	lightSHParams[0].SetVector2( &texSize );
	lightSHParams[1].SetTextureRT(m_pOccl1);
	lightSHParams[2].SetTextureRT(m_pOccl2);
	lightSHParams[3].SetTextureRT(m_pOccl3);
	gmtl::Vec2f screenPixelOffset;
	screenPixelOffset[0] = (info.pixelOffset * 2.0f) / width;
	screenPixelOffset[1] = (info.pixelOffset * 2.0f) / height;
	lightSHParams[4].SetVector2(&screenPixelOffset);

	//lightSHParams[4].SetTextureRT(m_pOccl4);
	//lightSHParams[3].SetTextureRT(m_pOccl3);

	uint semantics[32];
	semantics[0] = SEFX_OCCL_TEX_SIZE;
	semantics[1] = SEFX_TEX_OCCL_1;
	semantics[2] = SEFX_TEX_OCCL_2;
	semantics[3] = SEFX_TEX_OCCL_3;
	semantics[4] = BFXU_SCREEN_PIXEL_OFFSET;
	//semantics[4] = SEFX_TEX_OCCL_4;

	CRCDataKey groupKey;
	ComputeBFXMatGroupTemplateDataKey(m_pLightSHMatGroupTemplate, lightSHParams, 5, groupKey);
	m_pLightSHMatGroup = 
		m_pSHExpLightingFX->GetBaseFX().GetResourceMgr().CreateMaterialGroup(
			&groupKey, m_pLightSHMatGroupTemplate, lightSHParams, semantics, 5);

	// Create the occluder accum material
	screenPixelOffset[0] = (info.pixelOffset * 2.0f) / occlWidth;
	screenPixelOffset[1] = (info.pixelOffset * 2.0f) / occlHeight;
	lightSHParams[0].SetVector2( &screenPixelOffset );
	semantics[0] = BFXU_SCREEN_PIXEL_OFFSET;
	ComputeBFXMatGroupTemplateDataKey(m_pOccluderSHMatGroupTemplate, lightSHParams, 1, groupKey);
	m_pOccluderSHMatGroup = m_pSHExpLightingFX->GetBaseFX().GetResourceMgr().CreateMaterialGroup(
			&groupKey, m_pOccluderSHMatGroupTemplate, lightSHParams, semantics, 1);

	// Create the copy-to-depth effect with the accumulated z-pos
	m_pSHExpLightingFX->GetDeferredFX().GetParamPool().GetParam( DEFX_Z_POS_ACCUM, &lightSHParams[0] );
	semantics[0] = BFXU_RT1;
	//lightSHParams[1].SetVector2( &screenPixelOffset );
	//semantics[1] = BFXU_SCREEN_PIXEL_OFFSET;
	m_pCopyToDepthEffect = 
		m_pSHExpLightingFX->GetRenderer().GetRResourceMgr().LoadEffect(m_pCopyToDepthTemplate, lightSHParams, semantics, 1);
}

void
CSEFXPipeline::Initialize()
{
	//m_OpPool.Initialize();

	wstring fileName = _W("[shd]\\SHExpLightingFX\\OcclAccumPipeSetup.fx");
	CRCDataKey id;
	id.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));
	IByteBufferPtr pBuffer = m_pSHExpLightingFX->GetAppCallback().GetFileData(fileName.c_str());
	m_pOcclAccumPipeSetup = m_pSHExpLightingFX->GetRenderer().GetRResourceMgr().LoadEffect(
		&id, 
		pBuffer,
		NULL,
		NULL,
		0);

	//
	//fileName = _W("[shd]\\SHExpLightingFX\\SHExpStencilSetup.fx");
	//id.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));
	//pBuffer = m_pSHExpLightingFX->GetAppCallback().GetFileData(fileName.c_str());
	//m_pSHExpStencilSetup = m_pSHExpLightingFX->GetRenderer().GetRResourceMgr().LoadEffect(
	//	&id, 
	//	pBuffer,
	//	NULL,
	//	NULL,
	//	0);

	// Initialize material group templates
	wstring fileData = _W("[shd]\\SHExpCollapseLighting.mgp");
	pBuffer = m_pSHExpLightingFX->GetAppCallback().GetFileData(fileData.c_str());
	CRCDataKey key;
	key.Set(fileData.c_str(), (uint) (sizeof(wchar) * fileData.length()));
	m_pLightSHMatGroupTemplate = 
		m_pSHExpLightingFX->GetBaseFX().GetResourceMgr().CreateMaterialGroupTemplate(&key, pBuffer, fileData.c_str());

	fileData = _W("[shd]\\SHExpOcclusionAccum.mgp");
	pBuffer = m_pSHExpLightingFX->GetAppCallback().GetFileData(fileData.c_str());
	key.Set(fileData.c_str(), (uint) (sizeof(wchar) * fileData.length()));
	m_pOccluderSHMatGroupTemplate= 
		m_pSHExpLightingFX->GetBaseFX().GetResourceMgr().CreateMaterialGroupTemplate(&key, pBuffer, fileData.c_str());

	// Initialize copy-to-depth effect template
	fileData	= _W("[shd]\\BaseFXUtils\\CopyToDepth.fx");
	pBuffer		= m_pSHExpLightingFX->GetAppCallback().GetFileData(fileData.c_str());
	key.Set(fileData.c_str(), (uint) (sizeof(wchar) * fileData.length()));
	m_pCopyToDepthTemplate = m_pSHExpLightingFX->GetRenderer().GetRResourceMgr().LoadEffectTemplate(&key, pBuffer);

	// Initialize stencil volume for sphere
	fileData = _W("[shd]\\SHExpStencilVolume.mgp");
	pBuffer = m_pSHExpLightingFX->GetAppCallback().GetFileData(fileData.c_str());
	key.Set(fileData.c_str(), (uint) (sizeof(wchar) * fileData.length()));
	IBFXMaterialGroupTemplatePtr pStencilVolumeTemplate = 
		m_pSHExpLightingFX->GetBaseFX().GetResourceMgr().CreateMaterialGroupTemplate(&key, pBuffer, fileData.c_str());
	ComputeBFXMatGroupTemplateDataKey(pStencilVolumeTemplate, NULL, 0, key);
	m_pStencilVolumeMatGroup = 
		m_pSHExpLightingFX->GetBaseFX().GetResourceMgr().CreateMaterialGroup(
		&key, pStencilVolumeTemplate, NULL, NULL, 0);

	IRVertexBufferGroup* pVBGroup = NULL;
	IRIndexBuffer* pIB = NULL;
	CreateSphere(&m_pSHExpLightingFX->GetRenderer(), 
		&pVBGroup,
		&pIB,
		1.0f, 
		16,
		24);
	m_SphereMesh.pVBGroup	= pVBGroup;
	m_SphereMesh.pIB		= pIB;
	m_SphereMesh.pMatGroup	= m_pStencilVolumeMatGroup;

	// Initialize all data that is dependent on screen size
	const RDisplayInfo& info = m_pSHExpLightingFX->GetRenderer().GetDisplayInfo();
	UpdateScreenSize(info.backBufferWidth, info.backBufferHeight);
}

void
CSEFXPipeline::Render(const Sphere* pSpheres,
					  uint numSpheres,
					  const Sphere& coverage, 
					  const gmtl::MatrixA44f& coverageWorld,
					  const AABox& screenCoverage)
{
	// Requires deferred pipeline to be already setup
	
	OccluderOpParams* pParam = m_OpPool.Get();
	m_pCurOccluderOpParam = pParam;

	m_pCurOccluderOpParam->pSpheres		= pSpheres;
	m_pCurOccluderOpParam->numSpheres	= numSpheres;
	m_pCurOccluderOpParam->pCoverage	= &coverage;
	m_pCurOccluderOpParam->pScreenSpace	= &screenCoverage;

	// Render the sphere coverage
	m_SphereMesh.pMatGroup->SetCurrentMatType(SEFX_MAT_STENCIL_VOLUME);
	m_pSHExpLightingFX->GetBaseFX().GetParamPool().SetWorldMatrix(_CAST_MAT44(coverageWorld));
	m_pSHExpLightingFX->GetBaseFX().Render(m_SphereMesh, 1.0f);

	// Render the occluder (these will be updated later)
	BFXMesh mesh;
	mesh.pVBGroup	= m_OcclMesh.GetQuadVertices();
	mesh.pIB		= m_OcclMesh.GetQuadIndices();
	mesh.pMatGroup	= m_pOccluderSHMatGroup;
	mesh.pMatGroup->SetCurrentMatType(SEFX_MAT_OCCLUDER_ACCUM);
	m_pSHExpLightingFX->GetCParamPool().SetNumSpheres(numSpheres);
	m_pSHExpLightingFX->GetBaseFX().Render(mesh, 1.0f);

	// Collapse the env light
	if(m_CollapseGroup == BFX_INVALID)
	{
		mesh.pVBGroup	= m_ScreenMesh.GetQuadVertices();
		mesh.pIB		= m_ScreenMesh.GetQuadIndices();
		mesh.pMatGroup	= m_pLightSHMatGroup;
		mesh.pMatGroup->SetCurrentMatType(SEFX_MAT_COLLAPSE_LIGHT);
		m_pSHExpLightingFX->GetBaseFX().Render(mesh, 1.0f);
	}

	//BFXMesh mesh;
	//mesh.pMatGroup	= m_pLightSHMatGroup;

	//// Render visibility / occluders
	//m_pSHExpLightingFX->GetCParamPool().UpdateSpheresTex();

	//mesh.pVBGroup	= m_OcclMesh.GetQuadVertices();
	//mesh.pIB		= m_OcclMesh.GetQuadIndices();
	//m_pLightSHMatGroup->SetCurrentMatType(SEFX_MAT_OCCLUDER_ACCUM);
	//m_pSHExpLightingFX->GetBaseFX().Render(mesh, 1.0f);

	//// Collapse lighting
	//mesh.pVBGroup	= m_ScreenMesh.GetQuadVertices();
	//mesh.pIB		= m_ScreenMesh.GetQuadIndices();
	//m_pLightSHMatGroup->SetCurrentMatType(SEFX_MAT_COLLAPSE_LIGHT);
	//m_pSHExpLightingFX->GetBaseFX().Render(mesh, 1.0f);
}

void 
CSEFXPipeline::RenderVisibility(IRVertexBufferGroup* pVBGroup, IRIndexBuffer* pIB, IREffect* pEffect, uint tech, REffectParam* pDynamicParams)
{
	IRPipeline& pipeline = m_pSHExpLightingFX->GetRenderer().GetRPipeline();

	_DEBUG_ASSERT(m_OcclGroup != BFX_INVALID);
	pipeline.AddToRenderGroup(m_OcclGroup, pVBGroup, pIB, pEffect, pDynamicParams, tech, m_pCurOccluderOpParam);
}

void 
CSEFXPipeline::RenderStencilVolume(IRVertexBufferGroup* pVBGroup, IRIndexBuffer* pIB, IREffect* pEffect, uint tech, REffectParam* pDynamicParams)
{
	IRPipeline& pipeline = m_pSHExpLightingFX->GetRenderer().GetRPipeline();

	if(m_OcclGroup == BFX_INVALID)
	{
		// Copy to depth texture
		// TODO: Fix viewports and remove dummy render target
		// TODO: Add stencil clear specifically
		uint depthCopyGroup = pipeline.CreateNewRenderGroup(SEFX_GROUP_DEPTH_COPY, NULL, *this);
		pipeline.AddRenderTarget(depthCopyGroup, m_pOccl1);	
		pipeline.SetDepthBuffer(depthCopyGroup, m_pOcclDepth);
		pipeline.SetClearDepth(depthCopyGroup, 1.0f);
		pipeline.AddToRenderGroup(depthCopyGroup, m_DownsampleDepthMesh.GetQuadVertices(), m_DownsampleDepthMesh.GetQuadIndices(), 
			m_pCopyToDepthEffect, NULL, 0, NULL);

		// Add the accumulation setup group
		m_OcclGroup = pipeline.CreateNewRenderGroup(SEFX_GROUP_OCCLUDER_ACCUM, NULL, *this);
		pipeline.AddRenderTarget(m_OcclGroup, m_pOccl1);
		pipeline.AddRenderTarget(m_OcclGroup, m_pOccl2);
		pipeline.AddRenderTarget(m_OcclGroup, m_pOccl3);
		pipeline.SetDepthBuffer(m_OcclGroup, m_pOcclDepth);

		pipeline.SetClearColor(m_OcclGroup, gmtl::Vec4f(0.0f, 0.0f, 0.0f, 0.0f));
	}

	pipeline.AddToRenderGroup(m_OcclGroup, pVBGroup, pIB, pEffect, pDynamicParams, tech, NULL);
}

void
CSEFXPipeline::RenderCollapseLight(IRVertexBufferGroup* pVBGroup, IRIndexBuffer* pIB, IREffect* pEffect, uint tech, REffectParam* pDynamicParams)
{
	IRPipeline& pipeline = m_pSHExpLightingFX->GetRenderer().GetRPipeline();

	if(m_CollapseGroup == BFX_INVALID)
	{
		// Add the accumulation setup group
		m_CollapseGroup = pipeline.CreateNewRenderGroup(SEFX_GROUP_COLLAPSE_LIGHT, NULL, *this);
	}

	pipeline.AddToRenderGroup(m_CollapseGroup, m_ScreenMesh.GetQuadVertices(),
		m_ScreenMesh.GetQuadIndices(), pEffect, pDynamicParams, tech, NULL);	
}

void 
CSEFXPipeline::BeginRenderEffect(BFXMesh& mesh)
{
	m_pCurMesh = &mesh;
	m_CurrentMatIndex = m_pCurMesh->pMatGroup->GetCurrentMatType();
}

void 
CSEFXPipeline::RenderEffect(IREffect* pEffect, uint tech, REffectParam* pDynamicParams, uint numParams)
{
	switch(m_CurrentMatIndex)
	{
		case SEFX_MAT_STENCIL_VOLUME:
		{
			RenderStencilVolume(m_pCurMesh->pVBGroup, m_pCurMesh->pIB, pEffect, tech, pDynamicParams);
			break;
		}

		case SEFX_MAT_OCCLUDER_ACCUM:
		{
			RenderVisibility(m_pCurMesh->pVBGroup, m_pCurMesh->pIB, pEffect, tech, pDynamicParams);
			break;
		}

		case SEFX_MAT_COLLAPSE_LIGHT:
		{
			RenderCollapseLight(m_pCurMesh->pVBGroup, m_pCurMesh->pIB, pEffect, tech, pDynamicParams);
			break;
		}

		default: _DEBUG_ASSERT(FALSE); break;
	}

}

void 
CSEFXPipeline::EndRenderEffect(BFXMesh& mesh)
{
	m_pCurMesh = NULL;
	m_CurrentMatIndex = BFX_INVALID;
}

void 
CSEFXPipeline::BeginRenderGroup(uint groupNum)
{
	switch(groupNum)
	{
		case SEFX_GROUP_DEPTH_COPY:
		{
			break;
		}

		case SEFX_GROUP_OCCLUDER_ACCUM:
		{
			IREffectTemplate* pTemplate = m_pOcclAccumPipeSetup->GetTemplate();
			pTemplate->BeginTechnique(0);
			pTemplate->BeginPass(0);
			break;
		}

		case SEFX_GROUP_COLLAPSE_LIGHT:
		{
			break;
		}

		default: _DEBUG_ASSERT(FALSE);
	}
}

void 
CSEFXPipeline::EndRenderGroup(uint groupNum)
{
	switch(groupNum)
	{
		case SEFX_GROUP_DEPTH_COPY:
		{
			break;
		}

		case SEFX_GROUP_OCCLUDER_ACCUM:
		{
			IREffectTemplate* pTemplate = m_pOcclAccumPipeSetup->GetTemplate();
			pTemplate->EndPass();
			pTemplate->EndTechnique();

			m_OcclGroup	= BFX_INVALID;
			m_OpPool.Reset();
			break;
		}

		case SEFX_GROUP_COLLAPSE_LIGHT:
		{
			m_CollapseGroup = BFX_INVALID;
			break;
		}

		default: _DEBUG_ASSERT(FALSE);
	}
}

void
CSEFXPipeline::UpdateSpheresTex(const Sphere* pSpheres, uint numSpheres)
{
	// Update sphere attribute texture (since DX9 has no constant buffers)
	uint pitch;
	IRTexture2D* pTex = m_pSHExpLightingFX->GetCParamPool().GetDynamicSphereTex();

	byte* pDest = pTex->LockImmediate(0, pitch);
	_DEBUG_ASSERT(pitch == 4 * sizeof(float) * 128);
	memcpy(pDest, pSpheres, numSpheres * sizeof(Sphere));
	pTex->UnlockImmediate(0);
}


void 
CSEFXPipeline::BeginRenderOp(const RRenderOp* pSortData)
{
	if(pSortData->pSortingData && pSortData->pass == 0)
	{
		//gmtl::Vec2f bottomLeft(-1.0f, -1.0f);
		//gmtl::Vec2f topRight(1.0f, 1.0f);

		//gmtl::Vec2f bottomLeft(-0.7f, -0.7f);
		//gmtl::Vec2f topRight(0.65f, 0.8f);

		OccluderOpParams* pParams = (OccluderOpParams*) pSortData->pSortingData;
		m_OcclMesh.UpdateMesh(&m_pSHExpLightingFX->GetRenderer(), 
			m_pOccl1->GetWidth(), m_pOccl1->GetHeight(),
			_CAST_VEC2(pParams->pScreenSpace->min), _CAST_VEC2(pParams->pScreenSpace->max), FALSE);
		UpdateSpheresTex(pParams->pSpheres, pParams->numSpheres);
	}
}

void 
CSEFXPipeline::EndRenderOp(const RRenderOp* pSortData)
{

}

_NAMESPACE_END
