//==============================================================================
//
//		CDEFXPipeline.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		13/10/2009
//
//==============================================================================

#include "DeferredFXFramework.h"

_NAMESPACE_BEGIN

int AccumSortLess(const void* pL, const void* pR)
{
	const RRenderOp* pLhs = *((const RRenderOp**) pL);
	const RRenderOp* pRhs = *((const RRenderOp**) pR);

	// Front to back
	if(pLhs->userData.fVal != pRhs->userData.fVal)
		return pLhs->userData.fVal > pRhs->userData.fVal ? 1 : -1;

	//// Sort by shaders
	//if( pLhs->pEffectTemplate != pRhs->pEffectTemplate )
	//	return (pLhs->pEffectTemplate < pRhs->pEffectTemplate) ? 1 : -1;
	////if( pLhs->technique != pRhs->technique ) 
	////	return (pLhs->technique < pRhs->technique) ? 1 : -1;
	////if( pLhs->pass != pRhs->pass ) 
	////	return pLhs->pass < pRhs->pass;

	// Sort by constant params
	if( pLhs->pEffect != pRhs->pEffect )
	    return (pLhs->pEffect < pRhs->pEffect) ? 1 : -1;

    return 0;

	////if( pLhs->pVBGroup != pRhs->pVBGroup )
	////	return pLhs->pVBGroup < pRhs->pVBGroup;

	////if( pLhs->pIB != pRhs->pIB )
	////	return pLhs->pIB < pRhs->pIB; 

	//if( pLhs->opHash != pRhs->opHash )
	//	return (pLhs->opHash < pRhs->opHash) ? 1 : -1;

	// Front to back
	//return pLhs->userData.fVal > pRhs->userData.fVal ? 1 : -1;
}


void CDEFXPipeline::SetViewport(uint left, uint top, uint width, uint height)
{
	m_pAccumGroup->SetViewport(left, top, width, height);
	m_pAccumGroupAlphaTested->SetViewport(left, top, width, height);
}

void CDEFXPipeline::AddAccumulationTargets(IRTextureRT* pZPos, IRTextureRT* pNormal, IRTextureRT* pMaterial)
{
	m_pAccumGroup->AddRenderTarget(pZPos);
	m_pAccumGroup->AddRenderTarget(pNormal);
	m_pAccumGroup->AddRenderTarget(pMaterial);

	m_pAccumGroupAlphaTested->AddRenderTarget(pZPos);
	m_pAccumGroupAlphaTested->AddRenderTarget(pNormal);
	m_pAccumGroupAlphaTested->AddRenderTarget(pMaterial);
}

void CDEFXPipeline::AddRenderTarget(IRTextureRT* pRT)
{
	m_pAccumGroup->AddRenderTarget(pRT);
	m_pAccumGroupAlphaTested->AddRenderTarget(pRT);
}

void CDEFXPipeline::ClearRenderTargets()
{
	m_pAccumGroup->ClearRenderTargets();
	m_pAccumGroupAlphaTested->ClearRenderTargets();
}

void CDEFXPipeline::SetDepthBuffer(IRDepthBuffer* pDepth)
{
	m_pAccumGroup->SetDepthBuffer(pDepth);
	m_pAccumGroupAlphaTested->SetDepthBuffer(pDepth);
}

void CDEFXPipeline::Flush()
{
	_LOOPi(m_Contexts.size())
	{
		m_pAccumGroup->AddToList(m_Contexts[i].pAccumContext->GetDataBuffer(), m_Contexts[i].pAccumContext->GetNumUsedData());
		m_pAccumGroupAlphaTested->AddToList(m_Contexts[i].pAccumAlphaTestedContext->GetDataBuffer(), m_Contexts[i].pAccumAlphaTestedContext->GetNumUsedData());
	}

	m_pDeferredStencil->ApplyRenderState(1, NULL);

	if(m_pAccumGroup->GetNumOfDrawOps() > 0)
	{
		m_pAccumGroup->Flush();
	}

	if(m_pAccumGroupAlphaTested->GetNumOfDrawOps() > 0)
	{
		//m_pAlphaTestedSetupEffect->BeginTechnique(0);
		//m_pAlphaTestedSetupEffect->BeginPass(0);

		m_pAccumGroupAlphaTested->Flush();
		
		//m_pAlphaTestedSetupEffect->EndPass();
		//m_pAlphaTestedSetupEffect->EndTechnique();
	}

	m_pDeferredStencil->ResetRenderState();

	m_pAccumGroup->Reset();
	m_pAccumGroupAlphaTested->Reset();

	m_AccumPool.UpdateSize();
	m_AccumAlphaTestedPool.UpdateSize();
	_LOOPi(m_Contexts.size())
	{
		m_Contexts[i].pAccumContext->UpdateAndReset();
		m_Contexts[i].pAccumAlphaTestedContext->UpdateAndReset();
	}
}

void
CDEFXPipeline::Initialize()
{
	//m_AccumOps.Initialize();

    wstring fileName = _W("[shd]\\DeferredFX\\DeferredPipelineSetup.fx");
    CRCDataKey id;
    id.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));
    IByteBufferPtr pBuffer = m_pDeferredFX->GetAppCallback().GetFileData(fileName.c_str());
    m_pDeferredStencil = m_pDeferredFX->GetRenderer().GetRResourceMgr().LoadEffect(&id, pBuffer, NULL, NULL, 0);

	//fileName = _W("[shd]\\BaseFX\\AlphaTestedPipeSetup.fx");
	//id.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));
	//pBuffer = m_pDeferredFX->GetAppCallback().GetFileData(fileName.c_str());
	//if(pBuffer)
	//{
	//	m_pAlphaTestedSetupEffect = m_pDeferredFX->GetRenderer().GetRResourceMgr().LoadEffect(&id, pBuffer, NULL, NULL, 0);
	//}

	//_DEBUG_ASSERT(m_pAlphaTestedSetupEffect);

	m_pAccumGroup = m_pDeferredFX->GetRenderer().GetRResourceMgr().CreateRenderGroup(AccumSortLess);
	//m_pAccumGroup->SetClearColor(gmtl::Vec4f(0.0f, 0.0f, 0.0f, 0.0f));
	//m_pAccumGroup->SetClearDepth(1.0f);

	m_pAccumGroupAlphaTested = m_pDeferredFX->GetRenderer().GetRResourceMgr().CreateRenderGroup(AccumSortLess);

 //   fileName = _W("[shd]\\DeferredFX\\CopyAccum.fx");
 //   id.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));
 //   pBuffer = m_pDeferredFX->GetAppCallback().GetFileData(fileName.c_str());
 //   m_pCopyAccumEffectTemplate = m_pDeferredFX->GetRenderer().GetRResourceMgr().LoadEffectTemplate(&id, pBuffer);

	//const RDisplayInfo& info = m_pDeferredFX->GetRenderer().GetDisplayInfo();
	//UpdateScreenSize(info.backBufferWidth, info.backBufferHeight);

	AddContext();
}

//void 
//CDEFXPipeline::Enable2XDownsampleAccum()
//{
//    if(!m_2XDownsampleRefs)
//    {
//        const RDisplayInfo& info = m_pDeferredFX->GetRenderer().GetDisplayInfo();
//        Initialize2XDownsample(info.backBufferWidth, info.backBufferHeight);
//    }
//
//    m_2XDownsampleRefs++;
//}

//void
//CDEFXPipeline::Disable2XDownsampleAccum()
//{
//    m_2XDownsampleRefs--;
//
//    if(!m_2XDownsampleRefs)
//    {
//        Uninitialize2XDownsample();
//    }
//}

//void 
//CDEFXPipeline::Render2XDownsample()
//{
//    _DEBUG_ASSERT(m_Downsample2XGroup == BFX_INVALID);
//    _DEBUG_ASSERT(m_pDownsample2XZPos);
//    _DEBUG_ASSERT(m_pDownsample2XNormals);
//
//    IRPipeline& pipeline = m_pDeferredFX->GetRenderer().GetRPipeline();
//
//    // TODO: Add multiple render targets if needed
//    // Add the downsample setup group
//    m_Downsample2XGroup = pipeline.CreateNewRenderGroup(DEFX_GROUP_DOWNSAMPLE, NULL, *this);
//    pipeline.AddRenderTarget(m_Downsample2XGroup, m_pDownsample2XZPos);
//    pipeline.AddRenderTarget(m_Downsample2XGroup, m_pDownsample2XNormals);
//
//    // TODO: Add depth clear and depth target if needed
//    // Clear to the furthest z distance
//    //pipeline.SetClearColor(m_AccumGroup, gmtl::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
//    //pipeline.SetClearDepth(m_AccumGroup, 1.0f);
//
//    pipeline.AddToRenderGroup(m_Downsample2XGroup, 
//        m_Downsample2XMesh.GetQuadVertices(), 
//        m_Downsample2XMesh.GetQuadIndices(), 
//        m_pDownsample2XEffect, NULL, 0, NULL);	
//}

//void
//CDEFXPipeline::RenderAccum(IRVertexBufferGroup* pVBGroup, IRIndexBuffer* pIB, IREffect* pEffect, uint tech, REffectParam* pDynamicParams, int alphaType)
//{
//	IRPipeline& pipeline = m_pDeferredFX->GetRenderer().GetRPipeline();
//	
//	if(m_AccumGroup == BFX_INVALID)
//	{
//		// Disable z-buffer clear
//		// We clear here instead
//		m_pDeferredFX->GetBaseFX().GetPipeline().DisableMainGroupDepthClearVolatile();
//
//		// Add the accumulation setup group
//        m_AccumGroup = pipeline.CreateNewRenderGroup(DEFX_GROUP_CLEAR_ACCUM1, AccumSortLess, *this);
//        pipeline.AddRenderTarget(m_AccumGroup, m_pZPos);
//        pipeline.AddRenderTarget(m_AccumGroup, m_pAlbedo);
//        pipeline.SetClearColor(m_AccumGroup, gmtl::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
//        pipeline.SetClearDepth(m_AccumGroup, 1.0f);
//
//        m_AccumGroup = pipeline.CreateNewRenderGroup(DEFX_GROUP_CLEAR_ACCUM2, AccumSortLess, *this);
//        pipeline.AddRenderTarget(m_AccumGroup, m_pNormals);
//        pipeline.SetClearColor(m_AccumGroup, gmtl::Vec4f(0.5f, 0.5f, 0.5f, 0.0f));
//
//        m_AccumGroup = pipeline.CreateNewRenderGroup(DEFX_GROUP_ACCUM, AccumSortLess, *this);
//		pipeline.AddRenderTarget(m_AccumGroup, m_pZPos);
//        pipeline.AddRenderTarget(m_AccumGroup, m_pNormals);
//        pipeline.AddRenderTarget(m_AccumGroup, m_pAlbedo);
//
//		// Clear to the furthest z distance
//		//pipeline.SetClearColor(m_AccumGroup, gmtl::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
//		
//        if(m_2XDownsampleRefs > 0)
//            Render2XDownsample();
//	}
//
//	AccumOp* pOp	= m_AccumOps.Get();
//	pOp->alphaType	= alphaType;
//	pOp->distance	= m_pDeferredFX->GetBaseFX().GetParamPool().GetZDistanceFromCamera();
//
//	pipeline.AddToRenderGroup(m_AccumGroup, pVBGroup, pIB, pEffect, pDynamicParams, tech, pOp);	
//}

//void 
//CDEFXPipeline::BeginRenderEffect(BFXMesh& mesh)
//{
//	m_pCurMesh = &mesh;
//	m_CurrentMatIndex = m_pCurMesh->pMatGroup->GetCurrentMatType();
//}
//
//void 
//CDEFXPipeline::RenderEffect(IREffect* pEffect, uint tech, REffectParam* pDynamicParams, uint numParams)
//{
//	int alphaType = 0;
//	int alphaIndex = pEffect->GetTemplate()->GetConstantParamIndex(BFX_ALPHA_MODE);
//	if(alphaIndex >= 0)
//		alphaType = pEffect->GetParam(alphaIndex)->intVal;
//
//	_DEBUG_ASSERT(m_CurrentMatIndex == DEFX_MAT_ACCUM);
//	_DEBUG_ASSERT(alphaType >= 0 && alphaType <= 1);	// Opaque and alpha tested-only
//
//	RenderAccum(m_pCurMesh->pVBGroup, m_pCurMesh->pIB, pEffect, tech, pDynamicParams, alphaType);
//
//	//switch(m_CurrentMatIndex)
//	//{
//	//case DEFX_MAT_ACCUM:
//	//	{
//	//		RenderAccum(m_pCurMesh->pVBGroup, m_pCurMesh->pIB, pEffect, tech, pDynamicParams);
//	//		break;
//	//	}
//
//	//default: _DEBUG_ASSERT(FALSE); break;
//	//}
//
//}
//
//void 
//CDEFXPipeline::EndRenderEffect(BFXMesh& mesh)
//{
//	m_pCurMesh = NULL;
//	m_CurrentMatIndex = BFX_INVALID;
//}
//
//void 
//CDEFXPipeline::BeginRenderGroup(uint groupNum)
//{
//	switch(groupNum)
//	{
//        case DEFX_GROUP_CLEAR_ACCUM1:
//        case DEFX_GROUP_CLEAR_ACCUM2:
//        case DEFX_GROUP_CLEAR_ACCUM3:
//        {
//            break;
//        }
//
//		case DEFX_GROUP_ACCUM:
//		{
//			IREffectTemplate* pTemplate = m_pDeferredStencil->GetTemplate();
//			pTemplate->BeginTechnique(1);
//			pTemplate->BeginPass(0);
//
//			break;
//		}
//
//        case DEFX_GROUP_DOWNSAMPLE:
//        {
//            break;
//        }
//
//		default: _DEBUG_ASSERT(FALSE);
//	}
//}

//void 
//CDEFXPipeline::EndRenderGroup(uint groupNum)
//{
//	switch(groupNum)
//	{
//        case DEFX_GROUP_CLEAR_ACCUM1:
//        case DEFX_GROUP_CLEAR_ACCUM2:
//        case DEFX_GROUP_CLEAR_ACCUM3:
//        {
//            break;
//        }
//
//		case DEFX_GROUP_ACCUM:
//		{
//			IREffectTemplate* pTemplate = m_pDeferredStencil->GetTemplate();
//			pTemplate->EndPass();
//			pTemplate->EndTechnique();
//			
//			m_AccumOps.Reset();
//			m_AccumGroup = BFX_INVALID;
//
//            // NOTE: Workaround for NVIDIA point filtering bug
//            // Downsample
//            //if(m_2XDownsampleRefs > 0)
//           //     m_pDownsample2XZPos->CopyFromTextureRT(m_pZPos);
//
//			break;
//		}
//
//        case DEFX_GROUP_DOWNSAMPLE:
//        {
//            m_Downsample2XGroup = BFX_INVALID;
//            break;
//        }
//
//		default: _DEBUG_ASSERT(FALSE);
//	}
//}

void CDEFXPipeline::RenderEffect(BFXRenderContext& renderContext)
{
	IBFXParamPool* pParamPool = (IBFXParamPool*) renderContext.pContainer->GetParamCallback( _GET_LIB_INDEX(BFX_EFFECT_PARAM_OFFSET) );

	uint context = pParamPool->GetCurrentContext();
	switch(renderContext.matType)
	{
		case DEFX_MAT_ACCUM:
		{
			RVarData userData;
			userData.fVal = pParamPool->GetZDistanceFromCamera();

			RRenderOp* pOp;

			IRRenderGroup* pGroup;
			int alphaIndex = renderContext.pEffect->GetTemplate()->GetConstantParamIndex(BFX_ALPHA_MODE);
			if(alphaIndex >= 0 && (renderContext.pEffect->GetParam(alphaIndex)->intVal == BFX_ALPHA_MODE_ALPHATESTED))
			{
				pGroup = m_pAccumGroupAlphaTested;
				pOp = m_Contexts[context].pAccumAlphaTestedContext->Alloc();
			}
			else
			{
				pGroup = m_pAccumGroup;
				pOp = m_Contexts[context].pAccumContext->Alloc();
			}

			pGroup->FillDrawOp(pOp, renderContext.pMesh->pVBGroup, renderContext.pMesh->pIB, 
				renderContext.pEffect, renderContext.pDynamicParams, renderContext.tech, userData);

			//pGroup->AddDrawOp(renderContext.pMesh->pVBGroup, renderContext.pMesh->pIB, 
			//	renderContext.pEffect, renderContext.pDynamicParams, renderContext.tech, userData);

			break;
		} 

		case DEFX_MAT_LIGHTING:
		{
			CDEFXParamPool* pDEFXParamPool = (CDEFXParamPool*) renderContext.pContainer->GetParamCallback( _GET_LIB_INDEX(DEFX_EFFECT_PARAM_OFFSET) );		
			CDEFXLightGroup* pLightGroup = (CDEFXLightGroup*) pDEFXParamPool->GetCurrentLight();
			pLightGroup->RenderEffect(renderContext, context);
			break;
		}

		default: _DEBUG_ASSERT(FALSE); break;
	}

}

uint CDEFXPipeline::AddContext()
{
	PipelineContext contextToAdd;
	contextToAdd.pAccumContext				= _NEW BFXDataContext<RRenderOp>(&m_AccumPool);
	contextToAdd.pAccumAlphaTestedContext	= _NEW BFXDataContext<RRenderOp>(&m_AccumAlphaTestedPool);

	m_Contexts.push_back(contextToAdd);
	return m_Contexts.size() - 1;
}

uint CDEFXPipeline::GetNumOfContexts()
{
	return m_Contexts.size();
}

void CDEFXPipeline::ResetContext(uint contextID)
{
	m_Contexts[contextID].pAccumContext->Dispose();
	m_Contexts[contextID].pAccumAlphaTestedContext->Dispose();
}

void CDEFXPipeline::GetAccumulationTargets(IRTextureRT** ppZPos, IRTextureRT** ppNormal, IRTextureRT** ppMaterial, IRTextureRT** ppExtra, uint& numExtra)
{
	*ppZPos		= m_pAccumGroup->GetRenderTarget(0);
	*ppNormal	= m_pAccumGroup->GetRenderTarget(1);
	*ppMaterial = m_pAccumGroup->GetRenderTarget(2);

	uint numExtraRT = m_pAccumGroup->GetNumOfRenderTargets() - 3;
	if(numExtraRT < numExtra)
		numExtra = numExtraRT;

	_LOOPi(numExtra)
	{
		ppExtra[i] = m_pAccumGroup->GetRenderTarget(3 + i);
	}
}


_NAMESPACE_END
