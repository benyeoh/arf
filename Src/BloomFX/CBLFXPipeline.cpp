//==============================================================================
//
//		CBLFXPipeline.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/20/2008
//
//==============================================================================

#include "BloomFXFramework.h"

_NAMESPACE_BEGIN

//void 
//CBFXPipeline::RenderAlphaTested(IRVertexBufferGroup& vbGroup, 
//								IRIndexBuffer& indexBuffer,
//								IREffect& effect,
//								float detailLevel)
//{
//	BFXPipelineOp op;
//	op.distance = ((CBFXParamPool*) &m_pBase->GetParamPool())->GetZDistanceFromCamera();
//
//	m_Opaque.push_back( op );
//
//	IRPipeline& pipeline = m_pBase->GetRenderer().GetRPipeline();
//	if(m_OpaqueGroupIndex == BFX_INVALID)
//	{
//		m_OpaqueGroupIndex = pipeline.CreateNewRenderGroup(BFX_GROUP_OPAQUE, LightingOpaqueSortLess, *this);
//	}
//
//	pipeline.AddToRenderGroup(m_OpaqueGroupIndex, m_pCurMesh->pVBGroup, m_pCurMesh->pIB, pEffect, pDynamicParams, tech, &(m_Opaque.back()));	
//
//	RFXRenderFXOp op;
//	op.distance = m_pRenderFX->GetCRFXCommon().GetZDistanceFromCamera();
//	//	op.pIB = &indexBuffer;
//	//	op.pVBGroup = &vbGroup;
//
//	m_AlphaTested.push_back( op );
//	m_pRenderFX->AddToPipeline(m_AlphaTestedGroupIndex, 
//		RFX_GROUP_LIGHTING_ALPHA_TESTED,
//		vbGroup, 
//		indexBuffer, 
//		effect, 
//		detailLevel, 
//		LightingOpaqueSortLess, 
//		&(m_AlphaTested.back()));
//}

void 
CBLFXPipeline::Initialize()
{
	wstring fileName = _W("[shd]\\BloomFX\\AdditivePipeSetup.fx");
	IByteBufferPtr pBuffer = m_pBloomFX->GetAppCallback().GetFileData(fileName.c_str());
	CRCDataKey id;
	id.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));
	m_pAdditivePipeSetupEffect = m_pBloomFX->GetRenderer().GetRResourceMgr().LoadEffect(
		&id, 
		pBuffer,
		NULL,
		NULL,
		0);	
	
	fileName = _W("[shd]\\BaseFXUtils\\2DPipeSetup.fx");
	id.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));
	pBuffer = m_pBloomFX->GetAppCallback().GetFileData(fileName.c_str());
	m_p2DPipeSetupEffect = m_pBloomFX->GetRenderer().GetRResourceMgr().LoadEffect(
		&id, 
		pBuffer,
		NULL,
		NULL,
		0);
}

void 
CBLFXPipeline::BeginRenderGroup(uint groupNum)
{
	switch(groupNum)
	{
		case BLFX_GROUP_BRIGHT_PASS_DOWN_SAMPLE:
		{
			IREffectTemplate* pEffectTemplate = m_p2DPipeSetupEffect->GetTemplate();
			pEffectTemplate->BeginTechnique(0);
			pEffectTemplate->BeginPass(0);	
			
			// Do a blit here
			m_pBloomFX->GetRenderer().GetBackBuffer(m_pBloomFX->GetDownSampledSource());
			break;
		}
		
		case BLFX_GROUP_PREPARE_COMPOSITION:
		{
			break;
		}
		
		case BLFX_GROUP_BLOOM_FILTER_H_LARGE:
		case BLFX_GROUP_BLOOM_MED_DOWN_SAMPLE:
		case BLFX_GROUP_BLOOM_FILTER_H_MED:
		case BLFX_GROUP_BLOOM_FILTER_V_MED:
		case BLFX_GROUP_STAR_PRE_FILTER_H:
		case BLFX_GROUP_STAR_PRE_FILTER_V:
		case BLFX_GROUP_STAR_H:
		case BLFX_GROUP_STAR_V:
			break;
			
		case BLFX_GROUP_BLEND:
		{
			//IREffectTemplate* pEffectTemplate = m_pAdditivePipeSetupEffect->GetTemplate();
			//pEffectTemplate->BeginTechnique(0);
			//pEffectTemplate->BeginPass(0);
			break;
		}
		
		case BLFX_GROUP_BLOOM_FILTER_V_LARGE:
		case BLFX_GROUP_BLOOM_UP_SAMPLE_COMPOSE_MED:
		case BLFX_GROUP_STAR_V_COMPOSE:
		case BLFX_GROUP_STAR_H_COMPOSE:
		{
			IREffectTemplate* pEffectTemplate = m_pAdditivePipeSetupEffect->GetTemplate();
			pEffectTemplate->BeginTechnique(0);
			pEffectTemplate->BeginPass(0);
			break;
		}
				
		default:
		{
			_DEBUG_ASSERT(FALSE);
			break;
		}
	}
}

void 
CBLFXPipeline::EndRenderGroup(uint groupNum)
{
	switch(groupNum)
	{
		case BLFX_GROUP_BRIGHT_PASS_DOWN_SAMPLE:
		{
			break;
		}
		
		case BLFX_GROUP_PREPARE_COMPOSITION:
		{
			break;
		}
		
		case BLFX_GROUP_BLOOM_FILTER_H_LARGE:
		case BLFX_GROUP_BLOOM_MED_DOWN_SAMPLE:
		case BLFX_GROUP_BLOOM_FILTER_H_MED:
		case BLFX_GROUP_BLOOM_FILTER_V_MED:
		case BLFX_GROUP_STAR_PRE_FILTER_H:
		case BLFX_GROUP_STAR_PRE_FILTER_V:
		case BLFX_GROUP_STAR_H:
		case BLFX_GROUP_STAR_V:
			break;
			
		case BLFX_GROUP_BLOOM_FILTER_V_LARGE:
		case BLFX_GROUP_BLOOM_UP_SAMPLE_COMPOSE_MED:
		case BLFX_GROUP_STAR_V_COMPOSE:
		case BLFX_GROUP_STAR_H_COMPOSE:
		{
			IREffectTemplate* pEffectTemplate = m_pAdditivePipeSetupEffect->GetTemplate();
			pEffectTemplate->EndPass();
			pEffectTemplate->EndTechnique();
			break;
		}

		case BLFX_GROUP_BLEND:
		{
			//IREffectTemplate* pEffectTemplate = m_pAdditivePipeSetupEffect->GetTemplate();
			//pEffectTemplate->EndPass();
			//pEffectTemplate->EndTechnique();
			
			IREffectTemplate* pEffectTemplate = m_p2DPipeSetupEffect->GetTemplate();
			pEffectTemplate->EndPass();
			pEffectTemplate->EndTechnique();
			break;
		}
						
		default:
		{
			_DEBUG_ASSERT(FALSE);
			break;
		}
	}
}

void 
CBLFXPipeline::BeginRenderOp(const RRenderOp* pSortData)
{	
	// TODO: Copy into dynamic buffers here
	// or callback to app??
	//switch(pOp->type)
	//{
	//	
	//}
}

void 
CBLFXPipeline::EndRenderOp(const RRenderOp* pSortData)
{
	// TODO: Copy into dynamic buffers here
	// or callback to app??
	//switch(pOp->type)
	//{

	//}
}

_NAMESPACE_END