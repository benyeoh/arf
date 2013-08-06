//================================================================================
//
//		AHPRTComputePipeline.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/28/2012
//
//================================================================================

#include "AppHelpers.h"

#if defined(_IS_USE_APPHELPER_LIB_PRTCOMPUTE)

_NAMESPACE_BEGIN

int OpaqueSortLess(const void* pL, const void* pR)
{
	const RRenderOp* pLhs = *((const RRenderOp**) pL);
	const RRenderOp* pRhs = *((const RRenderOp**) pR);

	// Sort by shaders
	if( pLhs->pEffectTemplate != pRhs->pEffectTemplate )
		return (pLhs->pEffectTemplate < pRhs->pEffectTemplate) ? 1 : -1;

	// Sort by constant params
	if( pLhs->pEffect != pRhs->pEffect )
		return (pLhs->pEffect < pRhs->pEffect) ? 1 : -1;

	if( pLhs->opHash != pRhs->opHash )
		return (pLhs->opHash < pRhs->opHash) ? 1 : -1;

    // Front to back
    if(pLhs->userData.fVal != pRhs->userData.fVal )
        return (pLhs->userData.fVal > pRhs->userData.fVal) ? 1 : -1;

    return 0;
}

void AHPRTComputePipeline::Initialize(IRRenderer* pRenderer)
{
	//wstring fileName = _W("[shd]\\BaseFX\\AlphaTestedPipeSetup.fx");
	//CRCDataKey id;
	//id.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));
	//IByteBufferPtr pBuffer = m_pDeferredFX->GetAppCallback().GetFileData(fileName.c_str());
	//if(pBuffer)
	//{
	//	m_pAlphaTestedSetupEffect = m_pDeferredFX->GetRenderer().GetRResourceMgr().LoadEffect(&id, pBuffer, NULL, NULL, 0);
	//}

	//_DEBUG_ASSERT(m_pAlphaTestedSetupEffect);

	m_pBackfaceGroup = pRenderer->GetRResourceMgr().CreateRenderGroup(OpaqueSortLess);
	m_pBackfaceGroupAlphaTested = pRenderer->GetRResourceMgr().CreateRenderGroup(OpaqueSortLess);

	AddContext();
}

void AHPRTComputePipeline::SetViewport(uint left, uint top, uint width, uint height)
{
	m_pBackfaceGroup->SetViewport(left, top, width, height);
	m_pBackfaceGroupAlphaTested->SetViewport(left, top, width, height);
}

void AHPRTComputePipeline::AddRenderTarget(IRTextureRT* pRT)
{
	m_pBackfaceGroup->AddRenderTarget(pRT);
	m_pBackfaceGroupAlphaTested->AddRenderTarget(pRT);
}

void AHPRTComputePipeline::ClearRenderTargets()
{
	m_pBackfaceGroup->ClearRenderTargets();
	m_pBackfaceGroupAlphaTested->ClearRenderTargets();
}

void AHPRTComputePipeline::SetDepthBuffer(IRDepthBuffer* pDepth)
{
	m_pBackfaceGroup->SetDepthBuffer(pDepth);
	m_pBackfaceGroupAlphaTested->SetDepthBuffer(pDepth);
}

void AHPRTComputePipeline::Flush()
{
	_LOOPi(m_Contexts.size())
	{
		m_pBackfaceGroup->AddToList(m_Contexts[i].pBackfaceContext->GetDataBuffer(), m_Contexts[i].pBackfaceContext->GetNumUsedData());
		m_pBackfaceGroupAlphaTested->AddToList(m_Contexts[i].pBackfaceContextAlphaTested->GetDataBuffer(), m_Contexts[i].pBackfaceContextAlphaTested->GetNumUsedData());
	}

	//IREffectTemplate* pTemplate = m_pDeferredStencil->GetTemplate();
	//pTemplate->BeginTechnique(1);
	//pTemplate->BeginPass(0);

	m_pBackfaceGroup->Flush();

	if(m_pBackfaceGroupAlphaTested->GetNumOfDrawOps() > 0)
	{
		//m_pAlphaTestedSetupEffect->BeginTechnique(0);
		//m_pAlphaTestedSetupEffect->BeginPass(0);

		m_pBackfaceGroupAlphaTested->Flush();

		//m_pAlphaTestedSetupEffect->EndPass();
		//m_pAlphaTestedSetupEffect->EndTechnique();
	}

	//pTemplate->EndPass();
	//pTemplate->EndTechnique();

	m_pBackfaceGroup->Reset();
	m_pBackfaceGroupAlphaTested->Reset();

	m_BackfacePool.UpdateSize();
	m_BackfacePoolAlphaTested.UpdateSize();
	_LOOPi(m_Contexts.size())
	{
		m_Contexts[i].pBackfaceContext->UpdateAndReset();
		m_Contexts[i].pBackfaceContextAlphaTested->UpdateAndReset();
	}
}

uint AHPRTComputePipeline::AddContext()
{
	PipelineContext contextToAdd;
	contextToAdd.pBackfaceContext				= _NEW BFXDataContext<RRenderOp>(&m_BackfacePool);
	contextToAdd.pBackfaceContextAlphaTested	= _NEW BFXDataContext<RRenderOp>(&m_BackfacePoolAlphaTested);

	m_Contexts.push_back(contextToAdd);
	return m_Contexts.size() - 1;
}

uint AHPRTComputePipeline::GetNumOfContexts()
{
	return m_Contexts.size();
}

void AHPRTComputePipeline::ResetContext(uint contextID)
{
	m_Contexts[contextID].pBackfaceContext->Dispose();
	m_Contexts[contextID].pBackfaceContextAlphaTested->Dispose();
}

void AHPRTComputePipeline::RenderEffect(BFXRenderContext& renderContext)
{
	IBFXParamPool* pParamPool = (IBFXParamPool*) renderContext.pContainer->GetParamCallback( _GET_LIB_INDEX(BFX_EFFECT_PARAM_OFFSET) );

	uint context = pParamPool->GetCurrentContext();
	switch(renderContext.matType)
	{
	case AH_PRTCOMPUTE_MAT_BACKFACE:
		{
			RVarData userData;
			userData.fVal = pParamPool->GetZDistanceFromCamera();

			RRenderOp* pOp;

			IRRenderGroup* pGroup;
			int alphaIndex = renderContext.pEffect->GetTemplate()->GetConstantParamIndex(BFX_ALPHA_MODE);
			if(alphaIndex >= 0 && (renderContext.pEffect->GetParam(alphaIndex)->intVal == BFX_ALPHA_MODE_ALPHATESTED))
			{
				pGroup = m_pBackfaceGroupAlphaTested;
				pOp = m_Contexts[context].pBackfaceContextAlphaTested->Alloc();
			}
			else
			{
				pGroup = m_pBackfaceGroup;
				pOp = m_Contexts[context].pBackfaceContext->Alloc();
			}

			pGroup->FillDrawOp(pOp, renderContext.pMesh->pVBGroup, renderContext.pMesh->pIB, 
				renderContext.pEffect, renderContext.pDynamicParams, renderContext.tech, userData);

			//pGroup->AddDrawOp(renderContext.pMesh->pVBGroup, renderContext.pMesh->pIB, 
			//	renderContext.pEffect, renderContext.pDynamicParams, renderContext.tech, userData);

			break;
		} 

	default: _DEBUG_ASSERT(FALSE); break;
	}
}

_NAMESPACE_END

#endif