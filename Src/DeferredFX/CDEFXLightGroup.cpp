//================================================================================
//
//		CDEFXLightGroup.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/5/2012
//
//================================================================================

#include "DeferredFXFramework.h"

_NAMESPACE_BEGIN

void CDEFXLightGroup::Initialize(CDEFXDeferredFX* pDeferred)
{
	wstring fileName = _W("[shd]\\DeferredFX\\DeferredPipelineSetup.fx");
	CRCDataKey id;
	id.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));
	m_pDeferredStencil = pDeferred->GetRenderer().GetRResourceMgr().LoadEffect(
		&id, 
		NULL,
		NULL,
		NULL,
		0);

	if(!m_pDeferredStencil)
	{
		IByteBufferPtr pBuffer = pDeferred->GetAppCallback().GetFileData(fileName.c_str());
		m_pDeferredStencil = pDeferred->GetRenderer().GetRResourceMgr().LoadEffect(
			&id, 
			pBuffer,
			NULL,
			NULL,
			0);
	}

	m_pGroup = pDeferred->GetRenderer().GetRResourceMgr().CreateRenderGroup(NULL);

	AddContext();
}

void CDEFXLightGroup::SetViewport(uint left, uint top, uint width, uint height)
{
	m_pGroup->SetViewport(left, top, width, height);
}

void CDEFXLightGroup::AddRenderTarget(IRTextureRT* pRT)
{
	m_pGroup->AddRenderTarget(pRT);
}

void CDEFXLightGroup::ClearRenderTargets()
{
	m_pGroup->ClearRenderTargets();
}

void CDEFXLightGroup::SetDepthBuffer(IRDepthBuffer* pDepth)
{
	m_pGroup->SetDepthBuffer(pDepth);
}

//uint CDEFXLightGroup::GetNumOfRenderTargets()
//{
//	return m_pGroup->GetNumOfRenderTargets();
//}
//
//IRTextureRT* CDEFXLightGroup::GetRenderTarget(uint rtIndex)
//{
//	return m_pGroup->GetRenderTarget(rtIndex);
//}

void CDEFXLightGroup::Flush()
{
	_LOOPi(m_Contexts.size())
	{
		m_pGroup->AddToList(m_Contexts[i].pContext->GetDataBuffer(), m_Contexts[i].pContext->GetNumUsedData());
	}

	m_pDeferredStencil->GetTemplate()->BeginTechnique(2);
	m_pDeferredStencil->GetTemplate()->BeginPass(0);

	m_pGroup->Flush();

	m_pDeferredStencil->GetTemplate()->EndPass();
	m_pDeferredStencil->GetTemplate()->EndTechnique();

	m_pGroup->Reset();

	m_Pool.UpdateSize();
	_LOOPi(m_Contexts.size())
	{
		m_Contexts[i].pContext->UpdateAndReset();
	}
}

void CDEFXLightGroup::RenderEffect(BFXRenderContext& renderContext, uint context)
{
	RVarData userData;
	userData.iVal = 0;

	RRenderOp* pOp = m_Contexts[context].pContext->Alloc();
	m_pGroup->FillDrawOp(pOp, renderContext.pMesh->pVBGroup, renderContext.pMesh->pIB, 
		renderContext.pEffect, renderContext.pDynamicParams, renderContext.tech, userData);

	//m_pGroup->AddDrawOp(renderContext.pMesh->pVBGroup, renderContext.pMesh->pIB, 
	//	renderContext.pEffect, renderContext.pDynamicParams, renderContext.tech, userData);
}

uint CDEFXLightGroup::AddContext()
{
	PipelineContext contextToAdd;
	contextToAdd.pContext	= _NEW BFXDataContext<RRenderOp>(&m_Pool);

	m_Contexts.push_back(contextToAdd);
	return m_Contexts.size() - 1;
}

uint CDEFXLightGroup::GetNumOfContexts()
{
	return m_Contexts.size();
}

void CDEFXLightGroup::ResetContext(uint contextID)
{
	m_Contexts[contextID].pContext->Dispose();
}

_NAMESPACE_END
