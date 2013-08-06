//==============================================================================
//
//		CBFXPipeline.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/28/2008
//
//==============================================================================

#include "BaseFXFramework.h"

_NAMESPACE_BEGIN

int LightingOpaqueSortLess(const void* pL, const void* pR)
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


int LightingTransSortLess(const void* pL, const void* pR)
{
	const RRenderOp* pLhs = *((const RRenderOp**) pL);
	const RRenderOp* pRhs = *((const RRenderOp**) pR);
	
    // Front to back
    if(pLhs->userData.fVal != pRhs->userData.fVal )
        return (pLhs->userData.fVal < pRhs->userData.fVal) ? 1 : -1;

    return 0;

	// Sort back to front
	//return pLhs->userData.fVal < pRhs->userData.fVal ? 1 : -1;
}

void 
CBFXPipeline::Initialize()
{
	IRResourceMgr& resMgr = m_pBase->GetRenderer().GetRResourceMgr();

	wstring str = _W("[shd]\\BaseFX\\OpaquePipeSetup.fx");
	CRCDataKey id; 
	id.Set(str.c_str(), (uint) str.size() * sizeof(wchar));
	IByteBufferPtr pFileBuffer = m_pBase->GetAppCallback().GetFileData(str.c_str());
	if(pFileBuffer)
	{
		m_pOpaqueSetupEffect = resMgr.LoadEffect(&id, 
			pFileBuffer,
			NULL,
			NULL,
			0);
	}
	
	_DEBUG_ASSERT(m_pOpaqueSetupEffect);

	//str = _W("[shd]\\BaseFX\\AlphaTestedPipeSetup.fx");
	//id.Set(str.c_str(), (uint) str.size() * sizeof(wchar));
	//pFileBuffer = m_pBase->GetAppCallback().GetFileData(str.c_str());
	//if(pFileBuffer)
	//{
	//	m_pAlphaTestedSetupEffect = resMgr.LoadEffect(&id, 
	//		pFileBuffer,
	//		NULL,
	//		NULL,
	//		0);
	//}
	//
	//_DEBUG_ASSERT(m_pAlphaTestedSetupEffect);

	str = _W("[shd]\\BaseFX\\TranslucentPipeSetup.fx");
	id.Set(str.c_str(), (uint) str.size() * sizeof(wchar));
	pFileBuffer = m_pBase->GetAppCallback().GetFileData(str.c_str());
	if(pFileBuffer)
	{
		m_pTransSetupEffect = resMgr.LoadEffect(&id, 
			pFileBuffer,
			NULL,
			NULL,
			0);
	}
	
	_DEBUG_ASSERT(m_pTransSetupEffect);			

	//str = _W("[shd]\\BaseFX\\OpaqueAdditivePipeSetup.fx");
	//id.Set(str.c_str(), (uint) str.size() * sizeof(wchar));
	//pFileBuffer = m_pBase->GetAppCallback().GetFileData(str.c_str());
	//if(pFileBuffer)
	//{
	//	m_pOpaqueAdditiveSetupEffect = resMgr.LoadEffect(&id,
	//		pFileBuffer,
	//		NULL,
	//		NULL,
	//		0);		
	//}
	
	str = _W("[shd]\\BaseFX\\WireFrameSetup.fx");
	id.Set(str.c_str(), (uint) str.size() * sizeof(wchar));
	pFileBuffer = m_pBase->GetAppCallback().GetFileData(str.c_str());
	if(pFileBuffer)
	{
		m_pWireFrameSetupEffect = resMgr.LoadEffect(&id,
			pFileBuffer,
			NULL,
			NULL,
			0);		
	}

	m_pOpaque = resMgr.CreateRenderGroup(LightingOpaqueSortLess);
	m_pAlphaTested = resMgr.CreateRenderGroup(LightingOpaqueSortLess);
	m_pAlphaBlended = resMgr.CreateRenderGroup(LightingTransSortLess);

	AddContext();
}

void 
CBFXPipeline::RenderEffect(BFXRenderContext& renderContext)
{
	int alphaIndex = renderContext.pEffect->GetTemplate()->GetConstantParamIndex(BFX_ALPHA_MODE);
	int alphaType = 0;
	if(alphaIndex >= 0)
		alphaType = renderContext.pEffect->GetParam(alphaIndex)->intVal;

	RVarData userData;
	CBFXParamPool* pParamPool = (CBFXParamPool*) renderContext.pContainer->GetParamCallback( _GET_LIB_INDEX(BFX_EFFECT_PARAM_OFFSET) );
	userData.fVal = pParamPool->GetZDistanceFromCamera();

	uint context = pParamPool->GetCurrentContext();

	IRRenderGroup* pGroup;
	RRenderOp* pOp;

	// Add multi-pass pipeline
	// TODO: Problem with translucent multi-pass?
	//if(numIter == 0)
	{
		switch(alphaType)
		{
			case BFX_ALPHA_MODE_OPAQUE: 
			{
				pGroup = m_pOpaque;
				pOp = m_Contexts[context].pOpaqueContext->Alloc();
				break;
			}

			case BFX_ALPHA_MODE_ALPHATESTED:
			{
				pGroup = m_pAlphaTested;
				pOp = m_Contexts[context].pAlphaTestedContext->Alloc();
				break;
			}

			case BFX_ALPHA_MODE_TRANSLUCENT:
			case BFX_ALPHA_MODE_ADDITIVE:
			{
				pGroup = m_pAlphaBlended;
				pOp = m_Contexts[context].pAlphaBlendedContext->Alloc();
				break;
			}
			
	
			default: _DEBUG_ASSERT(FALSE); break;
		}
	}
	//else
	//{
	//	switch(alphaType)
	//	{
	//		case 0:
	//		case 1:
	//		case 3:
	//		{
	//			RenderOpaqueAdditive(m_pCurMesh->pVBGroup, m_pCurMesh->pIB, pEffect, tech, pDynamicParams);
	//			break;
	//		}

	//		case 2:
	//		{
	//			RenderTranslucent(m_pCurMesh->pVBGroup, m_pCurMesh->pIB, pEffect, tech, pDynamicParams, 1);
	//			break;
	//		}
	//		
	//		default: _DEBUG_ASSERT(FALSE); break;
	//	}
	//}	

	pGroup->FillDrawOp(pOp, renderContext.pMesh->pVBGroup, renderContext.pMesh->pIB, 
		renderContext.pEffect, renderContext.pDynamicParams, renderContext.tech, userData);
	
	//pGroup->AddDrawOp(renderContext.pMesh->pVBGroup, renderContext.pMesh->pIB, 
	//	renderContext.pEffect, renderContext.pDynamicParams, renderContext.tech, userData);
}

void 
CBFXPipeline::SetViewport(uint left, uint top, uint width, uint height)
{
	m_pOpaque->SetViewport(left, top, width, height);
	m_pAlphaTested->SetViewport(left, top, width, height);
	m_pAlphaBlended->SetViewport(left, top, width, height);
}

void 
CBFXPipeline::AddRenderTarget(IRTextureRT* pTexRT)
{
	m_pOpaque->AddRenderTarget(pTexRT);
	m_pAlphaTested->AddRenderTarget(pTexRT);
	m_pAlphaBlended->AddRenderTarget(pTexRT);
}

void
CBFXPipeline::ClearRenderTargets()
{
	m_pOpaque->ClearRenderTargets();
	m_pAlphaTested->ClearRenderTargets();
	m_pAlphaBlended->ClearRenderTargets();
}

void 
CBFXPipeline::SetDepthBuffer(IRDepthBuffer* pDepth)
{
	m_pOpaque->SetDepthBuffer(pDepth);
	m_pAlphaTested->SetDepthBuffer(pDepth);
	m_pAlphaBlended->SetDepthBuffer(pDepth);
}

void CBFXPipeline::ResetGroup()
{
	m_pOpaque->Reset();
	m_pAlphaTested->Reset();
	m_pAlphaBlended->Reset();
}

void 
CBFXPipeline::FlushOpaqueGroup()
{
	_LOOPi(m_Contexts.size())
	{
		m_pOpaque->AddToList(m_Contexts[i].pOpaqueContext->GetDataBuffer(), m_Contexts[i].pOpaqueContext->GetNumUsedData());
	}

	if(m_pOpaque->GetNumOfDrawOps() > 0)
	{
		//IREffectTemplate* pEffectTemplate = m_pOpaqueSetupEffect->GetTemplate();
		//pEffectTemplate->BeginTechnique(0);
		//pEffectTemplate->BeginPass(0);

		m_pOpaque->Flush();

		//pEffectTemplate->EndPass();
		//pEffectTemplate->EndTechnique();
	}

	m_pOpaque->Reset();

	m_OpaquePool.UpdateSize();
	_LOOPi(m_Contexts.size())
	{
		m_Contexts[i].pOpaqueContext->UpdateAndReset();
	}
}

void 
CBFXPipeline::FlushAlphaTestedGroup()
{
	_LOOPi(m_Contexts.size())
	{
		m_pAlphaTested->AddToList(m_Contexts[i].pAlphaTestedContext->GetDataBuffer(), m_Contexts[i].pAlphaTestedContext->GetNumUsedData());
	}

	if(m_pAlphaTested->GetNumOfDrawOps() > 0)
	{
		// TODO: Shader uses clip instead
		//	IREffectTemplate* pEffectTemplate = m_pAlphaTestedSetupEffect->GetTemplate();
		//	pEffectTemplate->BeginTechnique(0);
		//	pEffectTemplate->BeginPass(0);
	
		m_pAlphaTested->Flush();

		//	pEffectTemplate->EndPass();
		//	pEffectTemplate->EndTechnique();
	}

	m_pAlphaTested->Reset();

	m_AlphaTestedPool.UpdateSize();
	_LOOPi(m_Contexts.size())
	{
		m_Contexts[i].pAlphaTestedContext->UpdateAndReset();
	}
}

void 
CBFXPipeline::FlushAlphaBlendedGroup()
{
	_LOOPi(m_Contexts.size())
	{
		m_pAlphaBlended->AddToList(m_Contexts[i].pAlphaBlendedContext->GetDataBuffer(), m_Contexts[i].pAlphaBlendedContext->GetNumUsedData());
	}

	if(m_pAlphaBlended->GetNumOfDrawOps() > 0)
	{
		IREffectTemplate* pEffectTemplate = m_pTransSetupEffect->GetTemplate();
		pEffectTemplate->BeginTechnique(0);
		pEffectTemplate->BeginPass(0);

		m_pAlphaBlended->Flush();

		pEffectTemplate->EndPass();
		pEffectTemplate->EndTechnique();
	}

	m_pAlphaBlended->Reset();

	m_AlphaBlendedPool.UpdateSize();
	_LOOPi(m_Contexts.size())
	{
		m_Contexts[i].pAlphaBlendedContext->UpdateAndReset();
	}
}

uint CBFXPipeline::AddContext()
{
	PipelineContext contextToAdd;
	contextToAdd.pOpaqueContext			= _NEW BFXDataContext<RRenderOp>(&m_OpaquePool);
	contextToAdd.pAlphaTestedContext	= _NEW BFXDataContext<RRenderOp>(&m_AlphaTestedPool);
	contextToAdd.pAlphaBlendedContext	= _NEW BFXDataContext<RRenderOp>(&m_AlphaBlendedPool);

	m_Contexts.push_back(contextToAdd);
	return m_Contexts.size() - 1;
}

uint CBFXPipeline::GetNumOfContexts()
{
	return m_Contexts.size();
}

void CBFXPipeline::ResetContext(uint contextID)
{
	m_Contexts[contextID].pOpaqueContext->Dispose();
	m_Contexts[contextID].pAlphaTestedContext->Dispose();
	m_Contexts[contextID].pAlphaBlendedContext->Dispose();
}

_NAMESPACE_END