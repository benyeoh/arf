//================================================================================
//
//		SHPRTCallback.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		5/22/2012
//
//================================================================================

#include "stdafx.h"

void SHPRTCallback::Compute()
{
	IByteBufferPtr pMatData = g_AppCallback.GetFileData(_W("[shd]\\SHPRTCompute\\ParaboloidOcclusion.mgp"));
	m_pParaboloidOcclTemplate = g_pBaseFX->GetResourceMgr().CreateMaterialGroupTemplate(NULL, pMatData, _W("[shd]\\SHPRTCompute\\ParaboloidOcclusion.mgp"));

	// Compute SHPRT
	IByteBufferPtr pAdjacency = _NEW CByteBuffer(8192);
	IByteBufferPtr pNumAdjacency = _NEW CByteBuffer(1024);
	
	//IByteBufferPtr pSubsurface = _NEW CByteBuffer(8192);
	//IByteBufferPtr pNumSubsurface = _NEW CByteBuffer(1024);

	float timeToCompute = 0.0f;

	_LOOPi(NUM_MESHES)
	{
		m_pSwappedMeshLists[i] = _NEW BFXUMeshList;
		if(g_pMeshes[i])
		{
			_LOOPj(g_pMeshes[i]->meshes.size())
			{
				// Exchange with occl mat
				REffectParam params[16];
				uint paramSemantics[16];
				uint numParams = g_pMeshes[i]->meshes[j].pMatGroup->GetNumOfParams();
				_DEBUG_ASSERT(numParams < 16);
				_LOOPk(numParams)
				{
					g_pMeshes[i]->meshes[j].pMatGroup->GetParam(k, params[k]);
					paramSemantics[k] = g_pMeshes[i]->meshes[j].pMatGroup->GetSemantic(k);
				}

				IBFXMaterialGroupPtr pMatGroup = g_pBaseFX->GetResourceMgr().CreateMaterialGroup(NULL, m_pParaboloidOcclTemplate, params, paramSemantics, numParams);

				BFXMesh swappedMesh;
				swappedMesh.pIB = g_pMeshes[i]->meshes[j].pIB;
				swappedMesh.pVBGroup = g_pMeshes[i]->meshes[j].pVBGroup;
				swappedMesh.pMatGroup = pMatGroup; 
				m_pSwappedMeshLists[i]->meshes.push_back(swappedMesh);
			}
		}
	}

	_LOOPi(NUM_MESHES)
	{
	
		if(g_pMeshes[i])
		{
			_LOOPj(g_pMeshes[i]->meshes.size())
			{
				pAdjacency->SetDataLength(0);
				pNumAdjacency->SetDataLength(0);
				//pSubsurface->SetDataLength(0);
				//pNumSubsurface->SetDataLength(0);

				IRVertexBuffer* pVB = g_pMeshes[i]->meshes[j].pVBGroup->GetVertexBuffer(0);
				IRIndexBuffer* pIB = g_pMeshes[i]->meshes[j].pIB;
				uint numVertices = pVB->GetNumVertices();

				SHPRTLocationEntry* pLocEntries = _NEW SHPRTLocationEntry[numVertices];

				// Compute offsets in the VB
				const RVertexDesc* pDesc = pVB->GetDescription();
				uint vertexSize = GetVertexSize(pDesc);
				uint posOffset = GetVertexOffset(pDesc, VDU_POSITION, 0);
				uint normalOffset = GetVertexOffset(pDesc, VDU_NORMAL, 0);

				// Compute SHPRT location entries
				byte* pVBData = pVB->Lock(0, 0);
				byte* pVBPosData = pVBData + posOffset;
				byte* pVBNormalData = pVBData + normalOffset;
				g_pSHPRTCompute->ComputeVertexLocEntries(pLocEntries, g_MeshesWorld[i], pVBPosData, vertexSize, pVBNormalData, vertexSize, numVertices, POS_BIAS);
				pVB->Unlock(FALSE);

				// Compute occlusion
				SHPRTDataEntry* pRData = _NEW SHPRTDataEntry[numVertices];
				SHPRTDataEntry* pGData = _NEW SHPRTDataEntry[numVertices];
				SHPRTDataEntry* pBData = _NEW SHPRTDataEntry[numVertices];

				//_LOOPk(numVertices)
				//{
				//	memcpy(pRData[k].sh, unitSH, sizeof(float) * 9);
				//	memcpy(pGData[k].sh, unitSH, sizeof(float) * 9);
				//	memcpy(pBData[k].sh, unitSH, sizeof(float) * 9);
				//}

				g_pPlatform->GetTimer().BeginSample(); 
				g_pSHPRTCompute->ComputeOccludedTransfer(pLocEntries, pRData, pGData, pBData, numVertices, *this, g_pThreadPool);
				g_pPlatform->GetTimer().EndSample(); 
				timeToCompute += (float) g_pPlatform->GetTimer().GetTimeElapsed();

				GetAdjacentIndicesPerVertex(pVB, pIB, pAdjacency, pNumAdjacency, 0.0001f); 

				_LOOPk(NUM_FILTERS)
				{
					g_pSHPRTCompute->FilterVertexTransfer(pLocEntries, pAdjacency, pNumAdjacency, pRData, pGData, pBData, numVertices, DOT_THRESHOLD);
				}

				//GetIndicesInRangePerVertex(pVB, pSubsurface, pNumSubsurface, 10.0f); 

				//_LOOPk(1)
				//{
				//	g_pSHPRTCompute->FilterVertexTransfer(pLocEntries, pSubsurface, pNumSubsurface, pRData, pGData, pBData, numVertices, 1.0f);

				//}

				IRVertexBuffer* pTransferVB = g_pSHPRTCompute->CreateTransferVB(pRData, pGData, pBData, numVertices);

				// Finally add result to VB group
				g_pMeshes[i]->meshes[j].pVBGroup->AddVertexBuffer(pTransferVB);

				_DELETE_ARRAY(pRData);
				_DELETE_ARRAY(pGData);
				_DELETE_ARRAY(pBData);
				_DELETE_ARRAY(pLocEntries);
			}
		}
	}

	_LOOPi(NUM_MESHES)
	{
		m_pSwappedMeshLists[i] = NULL;
	}

	m_pParaboloidOcclTemplate = NULL;
 
	_TRACE(_W("Time to compute SH: %f\n"), timeToCompute);

	g_pBFXPipeline->ClearRenderTargets();
	g_pBFXPipeline->AddRenderTarget(g_pRenderer->GetBackBufferColor());
	g_pBFXPipeline->SetDepthBuffer(g_pRenderer->GetBackBufferDepth());
	uint width = g_pRenderer->GetBackBufferColor()->GetWidth();
	uint height = g_pRenderer->GetBackBufferColor()->GetHeight();
	g_pBFXPipeline->SetViewport(0, 0, width, height);
}

void SHPRTCallback::Render(const gmtl::VecA3f& loc, const gmtl::MatrixA44f& paraView, BFXRenderContainer& renderContainer, BFXParamContainer& paramContainer, IRTexture2D* pRT, IRDepthBuffer* pDB)
{
	paramContainer.SetParamCallback(g_pBFXParamPool, _GET_LIB_INDEX(BFX_EFFECT_PARAM_OFFSET));
	renderContainer.SetRenderCallback(g_pBFXPipeline, _GET_LIB_INDEX_FROM_MAT_TYPE(BFX_MATERIAL_TYPE_OFFSET));

	g_pBFXParamPool->SetZDistanceFromCamera(1.0f);

	g_pBFXPipeline->ClearRenderTargets();
	g_pBFXPipeline->AddRenderTarget(pRT->GetTextureRT(0));
	g_pBFXPipeline->SetDepthBuffer(pDB);
	g_pBFXPipeline->SetViewport(0, 0, pRT->GetWidth(0), pRT->GetHeight(0));

	_LOOPi(NUM_MESHES)
	{
		g_pBFXParamPool->SetWorldMatrix( _CAST_MAT44(g_MeshesWorld[i]) );
		if(g_pMeshes[i])
		{
			_LOOPj(g_pMeshes[i]->meshes.size())
			{
				// Determine if we need to load any effects first
				// TODO: This is not completely failsafe if there's multiple passes
				// Better to pre-load all combinations of volatile params
				//uint availabilityFlag = m_pSwappedMeshLists[i]->meshes[j].pMatGroup->GetParamAvailability(SHPRTCOMPUTE_MAT_OCCLUSION, paramContainer);
				//IREffect* pEffect = m_pSwappedMeshLists[i]->meshes[j].pMatGroup->GetEffect(SHPRTCOMPUTE_MAT_OCCLUSION, availabilityFlag);

				//if(!pEffect)
				//{
				//	 m_pSwappedMeshLists[i]->meshes[j].pMatGroup->CreateEffect(SHPRTCOMPUTE_MAT_OCCLUSION, availabilityFlag);
				//}

				//g_pMeshes[i]->meshes[j].pMatGroup->SetCurrentMatType(DEFX_MAT_ACCUM);
				g_pBaseFX->Render(m_pSwappedMeshLists[i]->meshes[j], 1.0f, SHPRTCOMPUTE_MAT_OCCLUSION, BFX_MAT_BASE_LIGHTING, paramContainer, renderContainer);
			}
		}
	}

	// Render skydome here
	g_pBFXPipeline->FlushOpaqueGroup();
	g_pBFXPipeline->FlushAlphaTestedGroup();
	g_pBFXPipeline->FlushAlphaBlendedGroup();

	g_pBFXParamPool->ResetParams();
}
