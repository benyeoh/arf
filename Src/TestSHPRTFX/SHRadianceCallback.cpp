//================================================================================
//
//		SHRadianceCallback.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		8/8/2012
//
//================================================================================

#include "stdafx.h"

struct TransferVBContainer
{
	std::vector<IRVertexBufferPtr> transferVBs;
};

void SHRadianceCallback::Compute()
{
	IByteBufferPtr pMatData = g_AppCallback.GetFileData(_W("[shd]\\TestSHPRTFX\\TestSHPRTFXParaboloidRadianceBRDF.mgp"));
	m_pParaboloidRadianceTemplate = g_pBaseFX->GetResourceMgr().CreateMaterialGroupTemplate(NULL, pMatData, _W("[shd]\\TestSHPRTFX\\TestSHPRTFXParaboloidRadianceBRDF.mgp"));
	//IByteBufferPtr pMatData = g_AppCallback.GetFileData(_W("[shd]\\SHPRTCompute\\ParaboloidOcclusion.mgp"));
	//m_pParaboloidRadianceTemplate = g_pBaseFX->GetResourceMgr().CreateMaterialGroupTemplate(NULL, pMatData, _W("[shd]\\SHPRTCompute\\ParaboloidOcclusion.mgp"));

	pMatData = g_AppCallback.GetFileData(_W("[shd]\\TestSHPRTFX\\TestSHPRTFXParaboloidSimpleSkyDome.mgp"));
	m_pParaboloidSkyDomeTemplate = g_pBaseFX->GetResourceMgr().CreateMaterialGroupTemplate(NULL, pMatData, _W("[shd]\\TestSHPRTFX\\TestSHPRTFXParaboloidSimpleSkyDome.mgp"));

	// Compute SHPRT
	IByteBufferPtr pAdjacency = _NEW CByteBuffer(8192);
	IByteBufferPtr pNumAdjacency = _NEW CByteBuffer(1024);

	//IByteBufferPtr pSubsurface = _NEW CByteBuffer(8192);
	//IByteBufferPtr pNumSubsurface = _NEW CByteBuffer(1024);

	float timeToCompute = 0.0f;

	// Initialize swap materials and zero transfer VB
	_LOOPi(NUM_MESHES)
	{
		m_pSwappedMeshLists[i] = _NEW BFXUMeshList;
		if(g_pMeshes[i])
		{
			_LOOPj(g_pMeshes[i]->meshes.size())
			{
				// Init zero vb
				uint numVertices = g_pMeshes[i]->meshes[j].pVBGroup->GetVertexBuffer(0)->GetNumVertices();
				SHPRTDataEntry* pZeroData = _NEW SHPRTDataEntry[numVertices];
				g_pSHPRTCompute->ZeroTransfer(pZeroData, numVertices);

				IRVertexBuffer* pInitVB = g_pSHPRTCompute->CreateTransferVB(pZeroData, pZeroData, pZeroData, numVertices);
				g_pMeshes[i]->meshes[j].pVBGroup->AddVertexBuffer(pInitVB);

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

				IBFXMaterialGroupPtr pMatGroup = g_pBaseFX->GetResourceMgr().CreateMaterialGroup(NULL, m_pParaboloidRadianceTemplate, params, paramSemantics, numParams);

				BFXMesh swappedMesh;
				swappedMesh.pIB = g_pMeshes[i]->meshes[j].pIB;
				swappedMesh.pVBGroup = g_pMeshes[i]->meshes[j].pVBGroup;
				swappedMesh.pMatGroup = pMatGroup; 
				m_pSwappedMeshLists[i]->meshes.push_back(swappedMesh);

				_DELETE_ARRAY(pZeroData);
			}
		}
	}

	// Skydome swap
	// Exchange with occl mat
	m_pSwappedSkyDome = _NEW BFXUMeshList;

	REffectParam params[16];
	uint paramSemantics[16];
	_LOOPi(g_pSkyDome->meshes.size())
	{
		uint numParams = g_pSkyDome->meshes[i].pMatGroup->GetNumOfParams();
		_DEBUG_ASSERT(numParams < 16);
		_LOOPj(numParams)
		{
			g_pSkyDome->meshes[i].pMatGroup->GetParam(j, params[j]);
			paramSemantics[j] = g_pSkyDome->meshes[i].pMatGroup->GetSemantic(j);
		}

		IBFXMaterialGroupPtr pMatGroup = g_pBaseFX->GetResourceMgr().CreateMaterialGroup(NULL, m_pParaboloidSkyDomeTemplate, params, paramSemantics, numParams);

		BFXMesh swappedMesh;
		swappedMesh.pIB = g_pSkyDome->meshes[i].pIB;
		swappedMesh.pVBGroup = g_pSkyDome->meshes[i].pVBGroup;
		swappedMesh.pMatGroup = pMatGroup; 
		m_pSwappedSkyDome->meshes.push_back(swappedMesh);
	}

	TransferVBContainer vbContainer[NUM_MESHES];

	_LOOP(count, NUM_BOUNCES)
	{
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

					//g_pSHPRTCompute->ZeroTransfer(pRData, numVertices);
					//IRVertexBuffer* pInitVB = g_pSHPRTCompute->CreateTransferVB(pRData, pRData, pRData, numVertices);
					//g_pMeshes[i]->meshes[j].pVBGroup->AddVertexBuffer(pInitVB);

					//_LOOPk(numVertices)
					//{
					//	memcpy(pRData[k].sh, unitSH, sizeof(float) * 9);
					//	memcpy(pGData[k].sh, unitSH, sizeof(float) * 9);
					//	memcpy(pBData[k].sh, unitSH, sizeof(float) * 9);
					//}

					g_pPlatform->GetTimer().BeginSample(); 
					g_pSHPRTCompute->ComputeOccludedTransferSphere(pLocEntries, pRData, pGData, pBData, numVertices, 1.0f, *this, g_pThreadPool);
					g_pPlatform->GetTimer().EndSample(); 
					timeToCompute += (float) g_pPlatform->GetTimer().GetTimeElapsed();

					GetAdjacentIndicesPerVertex(pVB, pIB, pAdjacency, pNumAdjacency, 0.0001f); 

					_LOOPk(NUM_FILTERS)
					{
						g_pSHPRTCompute->FilterVertexTransfer(pLocEntries, pAdjacency, pNumAdjacency, pRData, pGData, pBData, numVertices, DOT_THRESHOLD);
					}

					// Store result vb
					IRVertexBuffer* pTransferVB = g_pSHPRTCompute->CreateTransferVB(pRData, pGData, pBData, numVertices);
					vbContainer[i].transferVBs.push_back(pTransferVB);

					// Finally add result to VB group
					//g_pMeshes[i]->meshes[j].pVBGroup->RemoveVertexBuffer(g_pMeshes[i]->meshes[j].pVBGroup->GetNumOfVertexBuffers() - 1);
					//g_pMeshes[i]->meshes[j].pVBGroup->AddVertexBuffer(pTransferVB);

					_DELETE_ARRAY(pRData);
					_DELETE_ARRAY(pGData);
					_DELETE_ARRAY(pBData);
					_DELETE_ARRAY(pLocEntries);
				}
			}
		}

		_LOOPi(NUM_MESHES)
		{
			if(g_pMeshes[i])
			{
				_LOOPj(g_pMeshes[i]->meshes.size())
				{
					g_pMeshes[i]->meshes[j].pVBGroup->RemoveVertexBuffer(g_pMeshes[i]->meshes[j].pVBGroup->GetNumOfVertexBuffers() - 1);
					g_pMeshes[i]->meshes[j].pVBGroup->AddVertexBuffer(vbContainer[i].transferVBs[j]);
				}
			}

			vbContainer[i].transferVBs.clear();
		}
	}

	_LOOPi(NUM_MESHES)
	{
		m_pSwappedMeshLists[i] = NULL;
	}
	
	m_pSwappedSkyDome = NULL;

	m_pParaboloidRadianceTemplate = NULL;
	m_pParaboloidSkyDomeTemplate = NULL;

	_TRACE(_W("Time to compute SH: %f\n"), timeToCompute);

	// Reset
	g_pBFXPipeline->ClearRenderTargets();
	g_pBFXPipeline->AddRenderTarget(g_pRenderer->GetBackBufferColor());
	g_pBFXPipeline->SetDepthBuffer(g_pRenderer->GetBackBufferDepth());
	uint width = g_pRenderer->GetBackBufferColor()->GetWidth();
	uint height = g_pRenderer->GetBackBufferColor()->GetHeight();
	g_pBFXPipeline->SetViewport(0, 0, width, height);
}

void SHRadianceCallback::Render(const gmtl::VecA3f& loc, const gmtl::MatrixA44f& paraView, BFXRenderContainer& renderContainer, BFXParamContainer& paramContainer, IRTexture2D* pRT, IRDepthBuffer* pDB)
{
	paramContainer.SetParamCallback(g_pBFXParamPool, _GET_LIB_INDEX(BFX_EFFECT_PARAM_OFFSET));
	renderContainer.SetRenderCallback(g_pBFXPipeline, _GET_LIB_INDEX_FROM_MAT_TYPE(BFX_MATERIAL_TYPE_OFFSET));

	g_pBFXParamPool->SetZDistanceFromCamera(1.0f);
	g_pBFXParamPool->SetCameraProperties(_CAST_VEC3(loc), 0.0f, 1000.0f);

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
				//	m_pSwappedMeshLists[i]->meshes[j].pMatGroup->CreateEffect(SHPRTCOMPUTE_MAT_OCCLUSION, availabilityFlag);
				//}

				//g_pMeshes[i]->meshes[j].pMatGroup->SetCurrentMatType(DEFX_MAT_ACCUM);
	
				g_pBaseFX->Render(m_pSwappedMeshLists[i]->meshes[j], 1.0f, SHPRTCOMPUTE_MAT_OCCLUSION, BFX_MAT_BASE_LIGHTING, paramContainer, renderContainer);
			}
		}
	}

	gmtl::identity(m_ParaboloidSkyDomeMat);
	gmtl::Vec4f trans;
	trans[0] = loc[0];
	trans[1] = loc[1];
	trans[2] = loc[2];
	trans[3] = 1.0f;
	gmtl::setTrans(m_ParaboloidSkyDomeMat, trans);
	g_pBFXParamPool->SetWorldMatrix(m_ParaboloidSkyDomeMat);

	// Render skydome here
	_LOOPi(g_pSkyDome->meshes.size())
	{
		g_pBaseFX->Render(m_pSwappedSkyDome->meshes[i], 1.0f, SHPRTCOMPUTE_MAT_OCCLUSION, BFX_MAT_BASE_LIGHTING, paramContainer, renderContainer);
	}

	g_pBFXPipeline->FlushOpaqueGroup();
	g_pBFXPipeline->FlushAlphaTestedGroup();
	g_pBFXPipeline->FlushAlphaBlendedGroup();

	g_pBFXParamPool->ResetParams();
}
