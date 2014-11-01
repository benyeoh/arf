//==============================================================================
//
//		CSHFXDebug.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/23/2008
//
//==============================================================================

#include "ShadowFXFramework.h"

_NAMESPACE_BEGIN

void
CSHFXDebug::Initialize()
{	
	wstring fileName = _W("[shd]\\ShadowFX\\SMDebug.fx");
	CRCDataKey effectID;
	effectID.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));
	IByteBufferPtr pBuffer = m_pShadowFX->GetAppCallback().GetFileData(fileName.c_str());
	m_pSMDebugEffect = m_pShadowFX->GetRenderer().GetRResourceMgr().LoadEffect(
		&effectID, 
		pBuffer,
		NULL,
		NULL,
		0);
		
	fileName = _W("[shd]\\BaseFXUtils\\2DPipeSetup.fx");
	effectID.Set(fileName.c_str(), (uint) fileName.size() * sizeof(wchar));
	pBuffer = m_pShadowFX->GetAppCallback().GetFileData(fileName.c_str());
	m_p2DPipeSetupEffect = m_pShadowFX->GetRenderer().GetRResourceMgr().LoadEffect(
		&effectID, 
		pBuffer,
		NULL,
		NULL,
		0);
		
	// Initialize quad buffers
	RVertexDesc quadDesc[] =
	{
		{ VDU_POSITION, 0 },
		{ VDU_TEXCOORDF2, 0 },
		{ VDU_END, 0 },
	};

	IRVertexBuffer* pVB = m_pShadowFX->GetRenderer().GetRResourceMgr().CreateVertexBuffer(NULL, quadDesc, 4, BUF_DEFAULT);
	
	m_pQuadVBGroup	= m_pShadowFX->GetRenderer().GetRResourceMgr().CreateVertexBufferGroup(NULL);
	m_pQuadIB		= m_pShadowFX->GetRenderer().GetRResourceMgr().CreateIndexBuffer(NULL, 6, BUF_DEFAULT);
	m_pQuadVBGroup->AddVertexBuffer(pVB);

	byte* pQuadData = pVB->Lock(0, 4);
	_LOOPi(4)
	{
		float x = (float)(i % 2);
		float y = (float)(i / 2);
		
		float xPos = x;// * 2.0f - 1.0f;
		float yPos = y;// * 2.0f - 1.0f;
		
		*((gmtl::Vec3f*)pQuadData) = gmtl::Vec3f(xPos, yPos, 0.0f);
		pQuadData += GetVertexDataSize(VDU_POSITION);
		*((gmtl::Vec2f*)pQuadData) = gmtl::Vec2f(x, 1.0f - y);
		pQuadData += GetVertexDataSize(VDU_TEXCOORDF2);
	}

	pVB->Unlock();

	ushort* pQuadIndices = m_pQuadIB->Lock(0, 6);
	*pQuadIndices = (ushort) 0;		pQuadIndices++;
	*pQuadIndices = (ushort) 2;		pQuadIndices++;
	*pQuadIndices = (ushort) 3;		pQuadIndices++;
	*pQuadIndices = (ushort) 0;		pQuadIndices++;
	*pQuadIndices = (ushort) 3;		pQuadIndices++;
	*pQuadIndices = (ushort) 1;		pQuadIndices++;
	
	m_pQuadIB->Unlock();

	m_pDebugGroup = m_pShadowFX->GetRenderer().GetRResourceMgr().CreateRenderGroup(NULL);
}

void CSHFXDebug::SetViewport(uint left, uint top, uint width, uint height)
{
	m_pDebugGroup->SetViewport(left, top, width, height);
}

void CSHFXDebug::AddRenderTarget(IRTextureRT* pTexRT)
{
	m_pDebugGroup->AddRenderTarget(pTexRT);
}

void CSHFXDebug::ClearRenderTargets()
{
	m_pDebugGroup->ClearRenderTargets();
}

void CSHFXDebug::SetDepthBuffer(IRDepthBuffer* pDepth)
{
	m_pDebugGroup->SetDepthBuffer(pDepth);
}

void 
CSHFXDebug::RenderCascSM(const gmtl::Vec4f& screenSpacePos, const IRTexture2D* pCascSM)
{
	REffectParam effectParams[RRenderOp::MAX_NUM_PARAMS];
		
	//float width		= (screenSpacePos[2] - screenSpacePos[0]) * 0.5f;
	//float height	= (screenSpacePos[3] - screenSpacePos[1]) * 0.5f;
	//		
	//_LOOPi(4)
	//{
	//	float xStart	= screenSpacePos[0] + (i % 2) * width;
	//	float yStart	= screenSpacePos[1] + (i / 2) * height;
	//	float xEnd		= xStart + width;
	//	float yEnd		= yStart + height;
	//	
	//	m_Offsets[i][0] = xStart;
	//	m_Offsets[i][1] = yStart;
	//	m_Offsets[i][2] = xEnd;
	//	m_Offsets[i][3] = yEnd;
	//	
	//	uint numDynamicParams = m_pCascExpSMDebugEffect->GetTemplate()->GetNumOfDynamicParams(i);
	//	_LOOPj(numDynamicParams)
	//	{
	//		uint semantic = m_pCascExpSMDebugEffect->GetTemplate()->GetDynamicParamSemantic(i, j);
	//		switch(semantic)
	//		{
	//			case SHFX_DEBUG_OFFSET:
	//			{
	//				effectParams[j].SetVector4(&m_Offsets[i]);
	//				break;
	//			}
	//			
	//			case SHFX_DEBUG_SM:
	//			{
	//				effectParams[j].SetTextureRT(pCascExpSM);
	//				break;
	//			}

	//			case SHFX_DEBUG_EXP_AMOUNT:
	//			{
	//				effectParams[j].SetFloat(EXP_SM_K);
	//				break;
	//			}
	//			default: _DEBUG_ASSERT(FALSE);
	//		}
	//	}

	//	m_pDebugGroup->AddDrawOp(m_pQuadVBGroup, m_pQuadIB, m_pCascExpSMDebugEffect, effectParams, i, RVarData());	
	//}


	float width		= (screenSpacePos[2] - screenSpacePos[0]);
	float height	= (screenSpacePos[3] - screenSpacePos[1]);

	float xStart	= screenSpacePos[0];
	float yStart	= screenSpacePos[1];
	float xEnd		= xStart + width;
	float yEnd		= yStart + height;

	m_Offsets[0] = xStart;
	m_Offsets[1] = yStart;
	m_Offsets[2] = xEnd;
	m_Offsets[3] = yEnd;

	uint numDynamicParams = m_pSMDebugEffect->GetTemplate()->GetNumOfDynamicParams(0);
	_LOOPj(numDynamicParams)
	{
		uint semantic = m_pSMDebugEffect->GetTemplate()->GetDynamicParamSemantic(0, j);
		switch(semantic)
		{
		case SHFX_DEBUG_OFFSET:
			{
				effectParams[j].SetVector4(&m_Offsets);
				break;
			}

		case SHFX_DEBUG_SM:
			{
				effectParams[j].SetTexture2D(pCascSM);
				break;
			}

		//case SHFX_DEBUG_EXP_AMOUNT:
		//	{
		//		effectParams[j].SetFloat(EXP_SM_K);
		//		break;
		//	}
		default: _DEBUG_ASSERT(FALSE);
		}
	}

	m_pDebugGroup->AddDrawOp(m_pQuadVBGroup, m_pQuadIB, m_pSMDebugEffect, effectParams, 0, RVarData());	

	// Draw now
	m_p2DPipeSetupEffect->ApplyRenderState(0, NULL);

	m_pDebugGroup->Flush();
	m_pDebugGroup->Reset();

	m_p2DPipeSetupEffect->ResetRenderState();
}

void 
CSHFXDebug::RenderPLSM(const gmtl::Vec4f& screenSpacePos, const IRTexture2D* pPLSM)
{
	REffectParam effectParams[RRenderOp::MAX_NUM_PARAMS];
	
	//float width		= (screenSpacePos[2] - screenSpacePos[0]) * 0.5f;
	//float height	= (screenSpacePos[3] - screenSpacePos[1]);
	//
	//_LOOPi(2)
	//{
	//	float xStart	= screenSpacePos[0] + i * width;
	//	float yStart	= screenSpacePos[1];
	//	float xEnd		= xStart + width;
	//	float yEnd		= yStart + height;

	//	m_Offsets[i][0] = xStart;
	//	m_Offsets[i][1] = yStart;
	//	m_Offsets[i][2] = xEnd;
	//	m_Offsets[i][3] = yEnd;

	//	uint numDynamicParams = m_pCascExpSMDebugEffect->GetTemplate()->GetNumOfDynamicParams(i);
	//	_LOOPj(numDynamicParams)
	//	{
	//		uint semantic = m_pCascExpSMDebugEffect->GetTemplate()->GetDynamicParamSemantic(i, j);
	//				
	//		switch(semantic)
	//		{
	//			case SHFX_DEBUG_OFFSET:
	//			{
	//				effectParams[j].SetVector4(&m_Offsets[i]);
	//				break;
	//			}
	//			
	//			case SHFX_DEBUG_SM:
	//			{
	//				effectParams[j].SetTextureRT(pPLExpSM);
	//				break;
	//			}

	//			case SHFX_DEBUG_EXP_AMOUNT:
	//			{
	//				effectParams[j].SetFloat(EXP_SM_PL_K);
	//				break;
	//			}
	//			default: _DEBUG_ASSERT(FALSE);
	//		}
	//	}

	//	m_pDebugGroup->AddDrawOp(m_pQuadVBGroup, m_pQuadIB, m_pCascExpSMDebugEffect, effectParams, i, RVarData());
	//}

	float width		= (screenSpacePos[2] - screenSpacePos[0]);
	float height	= (screenSpacePos[3] - screenSpacePos[1]);

	float xStart	= screenSpacePos[0];
	float yStart	= screenSpacePos[1];
	float xEnd		= xStart + width;
	float yEnd		= yStart + height;

	m_Offsets[0] = xStart;
	m_Offsets[1] = yStart;
	m_Offsets[2] = xEnd;
	m_Offsets[3] = yEnd;

	uint numDynamicParams = m_pSMDebugEffect->GetTemplate()->GetNumOfDynamicParams(0);
	_LOOPi(numDynamicParams)
	{
		uint semantic = m_pSMDebugEffect->GetTemplate()->GetDynamicParamSemantic(0, i);

		switch(semantic)
		{
		case SHFX_DEBUG_OFFSET:
			{
				effectParams[i].SetVector4(&m_Offsets);
				break;
			}

		case SHFX_DEBUG_SM:
			{
				effectParams[i].SetTexture2D(pPLSM);
				break;
			}

		//case SHFX_DEBUG_EXP_AMOUNT:
		//	{
		//		effectParams[i].SetFloat(EXP_SM_PL_K);
		//		break;
		//	}
		default: _DEBUG_ASSERT(FALSE);
		}
	}

	m_pDebugGroup->AddDrawOp(m_pQuadVBGroup, m_pQuadIB, m_pSMDebugEffect, effectParams, 0, RVarData());

	// Draw now
	m_p2DPipeSetupEffect->ApplyRenderState(0, NULL);

	m_pDebugGroup->Flush();
	m_pDebugGroup->Reset();

	m_p2DPipeSetupEffect->ResetRenderState();
}


_NAMESPACE_END