//==============================================================================
//
//		CSHFXPipeline.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		8/12/2008
//
//==============================================================================

#include "ShadowFXFramework.h"

_NAMESPACE_BEGIN

void CSHFXPipeline::Initialize()
{	
}

void CSHFXPipeline::RenderCascadeDirLightCaster(BFXRenderContext& context)
{	
	CSHFXParamPool* pPool = (CSHFXParamPool*) context.pContainer->GetParamCallback( _GET_LIB_INDEX(SHFX_EFFECT_PARAM_OFFSET) );
	CSHFXCascadedDirLight* pDirLight = (CSHFXCascadedDirLight*) pPool->GetCascadedDirLight();
	_DEBUG_ASSERT(pDirLight);

	pDirLight->RenderCascadeDirLightCaster(context);

	//if(m_CastersIndex[cascadeIndex] == BFX_INVALID)
	//{
	//	
	//	// Insert a prep group
	//	if(m_CascadedDirLightPrepIndex == BFX_INVALID)
	//	{
	//		m_CascadedDirLightPrepIndex = pipeline.CreateNewRenderGroup(
	//			SHFX_GROUP_PREPARE_CASCADED_DIR_LIGHT, NULL, *this);
	//		pipeline.AddRenderTarget(m_CascadedDirLightPrepIndex, pDirLight->GetShadowMap());
	//		pipeline.SetDepthBuffer(m_CascadedDirLightPrepIndex, pDirLight->GetDepthBuffer());

	//		// Clear to exp max color
	//		pipeline.AddToRenderGroup(m_CascadedDirLightPrepIndex,f
	//			m_ScreenMesh.GetQuadVertices(), m_ScreenMesh.GetQuadIndices(),
	//			m_pColorFillEffect, NULL, 0, NULL);

	//		// Add the gaussian filtering also
	//		BFXUScreenSpaceMesh& mesh = pDirLight->GetScreenMesh();

	//		uint gaussianH = pipeline.CreateNewRenderGroup(SHFX_GROUP_GAUSSIAN_H_CASCADED_DIR_LIGHT, NULL, *this);
	//		IREffect* pGaussianH = pDirLight->GetGaussianBlurEffectH();
	//		pipeline.AddRenderTarget(gaussianH, pDirLight->GetShadowMapBackup());
	//		pipeline.SetDepthBuffer(gaussianH, pDirLight->GetDepthBuffer());
	//		pipeline.AddToRenderGroup(gaussianH, 
	//			mesh.GetQuadVertices(), 
	//			mesh.GetQuadIndices(), 
	//			pGaussianH, 
	//			NULL,
	//			GetGaussianBlurEffectTech(pGaussianH, pDirLight->GetGaussianParamsH().GetGaussianSize()),
	//			NULL);

	//		uint gaussianV = pipeline.CreateNewRenderGroup(SHFX_GROUP_GAUSSIAN_V_CASCADED_DIR_LIGHT, NULL, *this);
	//		IREffect* pGaussianV = pDirLight->GetGaussianBlurEffectV();
	//		pipeline.AddRenderTarget(gaussianV, pDirLight->GetShadowMap());		
	//		pipeline.SetDepthBuffer(gaussianV, pDirLight->GetDepthBuffer());
	//		pipeline.AddToRenderGroup(gaussianV, 
	//			mesh.GetQuadVertices(), 
	//			mesh.GetQuadIndices(), 
	//			pGaussianV, 
	//			NULL,
	//			GetGaussianBlurEffectTech(pGaussianV, pDirLight->GetGaussianParamsV().GetGaussianSize()),
	//			NULL);
	//	}

	//	uint cascadeGroup = BFX_INVALID;
	//	switch(cascadeIndex)
	//	{
	//	case 0 : cascadeGroup = SHFX_GROUP_SHADOW_CAST_CASCADED_DIR_LIGHT_1; break;
	//	case 1 : cascadeGroup = SHFX_GROUP_SHADOW_CAST_CASCADED_DIR_LIGHT_2; break;
	//	case 2 : cascadeGroup = SHFX_GROUP_SHADOW_CAST_CASCADED_DIR_LIGHT_3; break;
	//	case 3 : cascadeGroup = SHFX_GROUP_SHADOW_CAST_CASCADED_DIR_LIGHT_4; break;
	//	}

	//	_DEBUG_ASSERT(cascadeGroup != BFX_INVALID);

	//	m_CastersIndex[cascadeIndex] =
	//		pipeline.CreateNewRenderGroup(cascadeGroup, CasterSortLess, *this);
	//	//pipeline.SetDepthBuffer(m_CastersIndex, )
	//	pipeline.AddRenderTarget(m_CastersIndex[cascadeIndex], pDirLight->GetShadowMap());
	//	pipeline.SetDepthBuffer(m_CastersIndex[cascadeIndex], pDirLight->GetDepthBuffer());
	//	pipeline.SetClearDepth(m_CastersIndex[cascadeIndex], 1.0f);
	//}

	//pipeline.AddToRenderGroup(m_CastersIndex[cascadeIndex], pVBGroup, pIB, pEffect, pDynamicParams, tech, &(m_Casters[cascadeIndex].back()));	
}

void CSHFXPipeline::RenderCubePointLightCaster(BFXRenderContext& context)
{
	CSHFXParamPool* pPool = (CSHFXParamPool*) context.pContainer->GetParamCallback( _GET_LIB_INDEX(SHFX_EFFECT_PARAM_OFFSET) );
	CSHFXCubePointLight* pPointLight = pPool->GetCubePLCaster();
	_DEBUG_ASSERT(pPointLight);

	pPointLight->RenderPointLightCaster(context);
}

void CSHFXPipeline::RenderPointLightCaster(BFXRenderContext& context)
{
	CSHFXParamPool* pPool = (CSHFXParamPool*) context.pContainer->GetParamCallback( _GET_LIB_INDEX(SHFX_EFFECT_PARAM_OFFSET) );
	CSHFXPointLight* pPointLight = pPool->GetCurrentPLCaster();
	_DEBUG_ASSERT(pPointLight);
	
	pPointLight->RenderPointLightCaster(context);

	//CSHFXParamPool* pPool = (CSHFXParamPool*) &m_pShadowFX->GetParamPool();
	
	//SHFXPipelineOp op;
	//op.distance = pPool->GetZDistanceFromCamera();
	//pPointLight->AddPipelineOp(op);

	//IRPipeline& pipeline = m_pShadowFX->GetRenderer().GetRPipeline();
	//
	//SHFXShadowPLCastIndices& castIndices = pPointLight->GetCastIndices();

	//if(castIndices.groupNumStart == BFX_INVALID)
	//{
	//	_DEBUG_ASSERT(m_NumPointLights < BFX_MAX_POINT_LIGHTS);

	//	uint startGroupOffset = SHFX_GROUP_SHADOW_CAST_PL_START + SHFX_GROUP_SHADOW_CAST_PL_OFFSET_SIZE * m_NumPointLights;			
	//	castIndices.groupNumStart = startGroupOffset;

	//	uint groupIndex =  pipeline.CreateNewRenderGroup(
	//		startGroupOffset + SHFX_GROUP_SHADOW_CAST_PL_PREPARE, NULL, *this);
	//	pipeline.AddRenderTarget(groupIndex, pPointLight->GetShadowMap());
	//	pipeline.SetDepthBuffer(groupIndex, pPointLight->GetDepthBuffer());
	//
	//	// Clear to exp max color
	//	pipeline.AddToRenderGroup(groupIndex,
	//		m_ScreenMesh.GetQuadVertices(), m_ScreenMesh.GetQuadIndices(),
	//		m_pColorFillPLEffect, NULL, 0, NULL);

	//	//castIndices.prepare	= groupIndex;

	//	// Add the gaussian filtering also
	//	BFXUScreenSpaceMesh& mesh = pPointLight->GetScreenMesh();

	//	uint gaussianH = pipeline.CreateNewRenderGroup(startGroupOffset + SHFX_GROUP_GAUSSIAN_H_PL_OFFSET, NULL, *this);
	//	IREffect* pGaussianH = pPointLight->GetGaussianBlurEffectH();
	//	pipeline.AddRenderTarget(gaussianH, pPointLight->GetShadowMapBackup());
	//	pipeline.SetDepthBuffer(gaussianH, pPointLight->GetDepthBuffer());
	//	pipeline.AddToRenderGroup(gaussianH, 
	//		mesh.GetQuadVertices(), 
	//		mesh.GetQuadIndices(), 
	//		pGaussianH, 
	//		NULL,
	//		GetGaussianBlurEffectTech(pGaussianH, pPointLight->GetGaussianParamsH().GetGaussianSize()),
	//		NULL);

	//	//castIndices.blurH = gaussianH;

	//	uint gaussianV = pipeline.CreateNewRenderGroup(startGroupOffset + SHFX_GROUP_GAUSSIAN_V_PL_OFFSET, NULL, *this);
	//	IREffect* pGaussianV = pPointLight->GetGaussianBlurEffectV();
	//	pipeline.AddRenderTarget(gaussianV, pPointLight->GetShadowMap());		
	//	pipeline.SetDepthBuffer(gaussianV, pPointLight->GetDepthBuffer());
	//	pipeline.AddToRenderGroup(gaussianV, 
	//		mesh.GetQuadVertices(), 
	//		mesh.GetQuadIndices(), 
	//		pGaussianV, 
	//		NULL,
	//		GetGaussianBlurEffectTech(pGaussianV, pPointLight->GetGaussianParamsV().GetGaussianSize()),
	//		NULL);

	//	//castIndices.blurV = gaussianV;

	//	_DEBUG_ASSERT( m_pPointLights[m_NumPointLights] == NULL );
	//	m_pPointLights[m_NumPointLights] = pPointLight;
	//	m_NumPointLights++;
	//}

	//uint groupIndex = BFX_INVALID;

	//if(faceIndex == 0)
	//{
	//	// Front face
	//	if(castIndices.frontCast == BFX_INVALID)
	//	{
	//		uint startGroupOffset = castIndices.groupNumStart;			
	//		
	//		castIndices.frontCast =  pipeline.CreateNewRenderGroup(
	//			startGroupOffset + SHFX_GROUP_SHADOW_CAST_PL_FRONT_OFFSET, CasterSortLess, *this);
	//		pipeline.AddRenderTarget(castIndices.frontCast, pPointLight->GetShadowMap());
	//		pipeline.SetDepthBuffer(castIndices.frontCast, pPointLight->GetDepthBuffer());
	//		pipeline.SetClearDepth(castIndices.frontCast, 1.0f);
	//	}

	//	groupIndex = castIndices.frontCast;
	//}
	//else
	//{
	//	if(castIndices.backCast == BFX_INVALID)
	//	{
	//		uint startGroupOffset = castIndices.groupNumStart;			

	//		castIndices.backCast =  pipeline.CreateNewRenderGroup(
	//			startGroupOffset + SHFX_GROUP_SHADOW_CAST_PL_BACK_OFFSET, CasterSortLess, *this);
	//		pipeline.AddRenderTarget(castIndices.backCast, pPointLight->GetShadowMap());
	//		pipeline.SetDepthBuffer(castIndices.backCast, pPointLight->GetDepthBuffer());
	//		pipeline.SetClearDepth(castIndices.backCast, 1.0f);
	//	}

	//	groupIndex = castIndices.backCast;
	//}

	//pipeline.AddToRenderGroup(groupIndex, pVBGroup, pIB, pEffect, pDynamicParams, tech, &(pPointLight->GetLastPipelineOp()));	
}

void CSHFXPipeline::RenderEffect(BFXRenderContext& renderContext)
{
	switch(renderContext.matType)
	{
	case SHFX_MAT_CAST_DIR_LIGHT_SHADOW:
		{
			RenderCascadeDirLightCaster(renderContext);
			break;
		}
	
	case SHFX_MAT_CAST_POINT_LIGHT_SHADOW:
		{
			RenderPointLightCaster(renderContext);
			break;
		}

	case SHFX_MAT_CAST_CUBE_POINT_LIGHT_SHADOW:
		{
			RenderCubePointLightCaster(renderContext);
			break;
		}

	default: _DEBUG_ASSERT(FALSE); break;
	}
}

_NAMESPACE_END