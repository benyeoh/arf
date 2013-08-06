//================================================================================
//
//		AHScenePRTCompute.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/27/2012
//
//================================================================================

#include "AppHelpers.h"

#if defined(_IS_USE_APPHELPER_LIB_PRTCOMPUTE)

_NAMESPACE_BEGIN

void AHScenePRTCompute::Render(const gmtl::VecA3f& loc, const gmtl::MatrixA44f& paraView, BFXRenderContainer& renderContainer, BFXParamContainer& paramContainer, IRTexture2D* pRT, IRDepthBuffer* pDB)
{
	//g_pBFXParamPool->SetZDistanceFromCamera(1.0f);
	//g_pBFXParamPool->SetCameraProperties(_CAST_VEC3(loc), 0.0f, 1000.0f);

	m_pBFXPipeline->ClearRenderTargets();
	m_pBFXPipeline->AddRenderTarget(pRT->GetTextureRT(0));
	m_pBFXPipeline->SetDepthBuffer(pDB);
	m_pBFXPipeline->SetViewport(0, 0, pRT->GetWidth(0), pRT->GetHeight(0));

	m_pAHPRTComputePipeline->ClearRenderTargets();
	m_pAHPRTComputePipeline->AddRenderTarget(pRT->GetTextureRT(0));
	m_pAHPRTComputePipeline->SetDepthBuffer(pDB);
	m_pAHPRTComputePipeline->SetViewport(0, 0, pRT->GetWidth(0), pRT->GetHeight(0));

	_DEBUG_ASSERT(pRT->GetWidth(0) == m_pZPos->GetWidth(0));
	_DEBUG_ASSERT(pRT->GetHeight(0) == m_pZPos->GetHeight(0));
	m_pDEFXPipeline->ClearRenderTargets();
	m_pDEFXPipeline->AddAccumulationTargets(m_pZPos->GetTextureRT(0), m_pNormal->GetTextureRT(0), m_pAlbedo->GetTextureRT(0));
	m_pDEFXPipeline->AddRenderTarget(pRT->GetTextureRT(0));
	m_pDEFXPipeline->SetDepthBuffer(pDB);
	m_pDEFXPipeline->SetViewport(0, 0, pRT->GetWidth(0), pRT->GetHeight(0));

	m_pComputePhase->Update(loc, paraView, renderContainer, paramContainer, pRT->GetTextureRT(0), pDB);
	m_pComputePhase->Flush();
}

int AHScenePRTCompute::GetTransferVBIndex(IRVertexBufferGroup* pVBGroup)
{
	_LOOPk(pVBGroup->GetNumOfVertexBuffers()-1)
	{
		IRVertexBuffer* pVB = pVBGroup->GetVertexBuffer(k+1);
		const RVertexDesc* pDesc = pVB->GetDescription();
		uint descIndex = 0;
		while(pDesc[descIndex].usage != VDU_END)
		{
			// TODO: Change this to be dependent on SHPRT lib
			if(pDesc[descIndex].usage == VDU_TEXCOORDF4 && pDesc[descIndex].usageIndex == 4)
			{
				return k+1;
			}

			descIndex++;
		}
	}

	return -1;
}

uint AHScenePRTCompute::InitializeMeshes(ISDBSceneContainer* pScene, boolean isForceClear, PRTMeshEntry* pDest, uint bufferSize)
{
	AHComponent** pComp = _NEW AHComponent*[bufferSize];
	uint numComp = pScene->QueryAll((void**) pComp, bufferSize, SDB_FILTER_RENDER_COMP);
	_DEBUG_ASSERT(numComp < bufferSize);

	PRTMeshEntry* pPRTComputeMeshes = pDest;
	uint numPRTComputeMeshes = 0;

	uint matType[2];
	matType[0] = AH_PRTCOMPUTE_MAT_ACCUM;
	matType[1] = SHPRTCOMPUTE_MAT_OCCLUSION;

	_LOOPi(numComp)
	{
		AHRenderComponent* pRenderComp = (AHRenderComponent*) pComp[i];
		BFXUMeshList* pMeshList = pRenderComp->GetMeshList();
		_LOOPj(pMeshList->meshes.size())
		{
			BFXMesh& mesh = pMeshList->meshes[j];
			boolean isRequirePRTData = FALSE;

			// Check if it requires PRT data
			// Loop through each relevant material and see
			// if the effect has the parameter flag	
			_LOOP(matIndex, 2)
			{
				IBFXMaterialGroupTemplate* pTemplate = mesh.pMatGroup->GetTemplate();
				IBFXMaterial* pMat = pTemplate->GetMaterialByType(matType[matIndex]);

				if(pMat)
				{
					uint maxAvailFlag = ( 1 << (pMat->GetNumOfConstantVolatileParams() + pMat->GetNumOfDynamicVolatileParams()) ) - 1;

					// This force loads the effect if not already loaded
					IREffectTemplate* pEffect = pMat->GetEffectTemplate(maxAvailFlag);
					int paramIndex = pEffect->GetConstantParamIndex(AH_PRTCOMPUTE_EFFECT_REQUIRE_PRT);
					if(paramIndex >= 0)
					{
						isRequirePRTData = TRUE;
						break;
					}
				}
			}

			if(isRequirePRTData)
			{
				// During initialization, check if we already have PRT data allocated
				int transferVBIndex = GetTransferVBIndex(mesh.pVBGroup);
				if(transferVBIndex == -1)
				{
					// Create a new unique vb group with no key
					IRVertexBufferGroup* pVBGroup = m_pRenderer->GetRResourceMgr().CreateVertexBufferGroup(NULL);
					_LOOP(vbIndex, mesh.pVBGroup->GetNumOfVertexBuffers())
					{
						pVBGroup->AddVertexBuffer(mesh.pVBGroup->GetVertexBuffer(vbIndex));
					}

					mesh.pVBGroup = pVBGroup;

					isForceClear = TRUE;
				}
				else
				{
					// We already have one allocated
					// Only reset the data if we need to
					if(isForceClear)
					{
						// Remove the data
						mesh.pVBGroup->RemoveVertexBuffer((uint) transferVBIndex);
					}
				}

				if(isForceClear)
				{
					// Init zero vb
					uint numVertices = mesh.pVBGroup->GetVertexBuffer(0)->GetNumVertices();
					SHPRTDataEntry* pZeroData = _NEW SHPRTDataEntry[numVertices];
					m_pSHPRTCompute->ZeroTransfer(pZeroData, numVertices);

					IRVertexBuffer* pInitVB = m_pSHPRTCompute->CreateTransferVB(pZeroData, pZeroData, pZeroData, numVertices);
					mesh.pVBGroup->AddVertexBuffer(pInitVB);

					_DELETE_ARRAY(pZeroData);
				}

				pPRTComputeMeshes[numPRTComputeMeshes].pMesh = &mesh;
				pPRTComputeMeshes[numPRTComputeMeshes].pRenderComp = pRenderComp;
				numPRTComputeMeshes++;
				_DEBUG_ASSERT(numPRTComputeMeshes < bufferSize);
			}
		}
	}

	_DELETE_ARRAY(pComp);

	return numPRTComputeMeshes;
}

void AHScenePRTCompute::Compute(ISDBSceneContainer* pScene, float rayViewDistance, float cascadeShadowDistBuffer, uint numBounces, float posBias, float selfOcclScale, uint numFilter, float dotThreshold, boolean isForceClear)
{
	// Find all relevant meshes that requires PRT data and initialize them

	const static uint MAX_NUM_RENDER = 16384;

	//AHComponent* pComp[MAX_NUM_RENDER];
	//uint numComp = pScene->QueryAll(pComp, MAX_NUM_RENDER, SDB_FILTER_RENDER_COMP);
	//_DEBUG_ASSERT(numComp < MAX_NUM_RENDER);

	PRTMeshEntry pPRTComputeMeshes[MAX_NUM_RENDER];
	uint numPRTComputeMeshes = InitializeMeshes(pScene, isForceClear, pPRTComputeMeshes, MAX_NUM_RENDER);

	// Setup parameters
	m_pComputeSetup->SetupLightsForPRTCompute(pScene, rayViewDistance, cascadeShadowDistBuffer, m_pRenderPhaseThreadPool->GetNumThreads());
	m_pComputePhase->SetScene(pScene);
	m_pComputePhase->SetViewDistance(rayViewDistance);

	// Now compute the PRT
	IByteBufferPtr pAdjacency = _NEW CByteBuffer(8192);
	IByteBufferPtr pNumAdjacency = _NEW CByteBuffer(1024);

	_LOOP(count, numBounces)
	{
		_LOOPi(numPRTComputeMeshes)
		{
			pAdjacency->SetDataLength(0);
			pNumAdjacency->SetDataLength(0);
			//pSubsurface->SetDataLength(0);
			//pNumSubsurface->SetDataLength(0);

			IRVertexBuffer* pVB = pPRTComputeMeshes[i].pMesh->pVBGroup->GetVertexBuffer(0);
			IRIndexBuffer* pIB = pPRTComputeMeshes[i].pMesh->pIB;
			uint numVertices = pVB->GetNumVertices();

			SHPRTLocationEntry* pLocEntries = _NEW SHPRTLocationEntry[numVertices];

			// Compute offsets in the VB
			const RVertexDesc* pDesc = pVB->GetDescription();
			uint vertexSize = GetVertexSize(pDesc);
			uint posOffset = GetVertexOffset(pDesc, VDU_POSITION, 0);
			uint normalOffset = GetVertexOffset(pDesc, VDU_NORMAL, 0);
			_DEBUG_ASSERT(normalOffset > 0);

			// Compute SHPRT location entries
			AHSpatialComponent* pSpatial = (AHSpatialComponent*) pPRTComputeMeshes[i].pRenderComp->GetEntity()->GetComponent(COMP_SPATIAL);
			
			byte* pVBData = pVB->Lock(0, 0);
			byte* pVBPosData = pVBData + posOffset;
			byte* pVBNormalData = pVBData + normalOffset;
			m_pSHPRTCompute->ComputeVertexLocEntries(pLocEntries,  pSpatial->GetWorldMatrix(), pVBPosData, vertexSize, pVBNormalData, vertexSize, numVertices, posBias);
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

			//g_pPlatform->GetTimer().BeginSample(); 
			if(selfOcclScale == 0.0f)
				m_pSHPRTCompute->ComputeOccludedTransfer(pLocEntries, pRData, pGData, pBData, numVertices, *this, m_pProcessPRTThreadPool);
			else
				m_pSHPRTCompute->ComputeOccludedTransferSphere(pLocEntries, pRData, pGData, pBData, numVertices, selfOcclScale, *this, m_pProcessPRTThreadPool);
	
			//g_pPlatform->GetTimer().EndSample(); 
			//timeToCompute += (float) g_pPlatform->GetTimer().GetTimeElapsed();

			GetAdjacentIndicesPerVertex(pVB, pIB, pAdjacency, pNumAdjacency, 0.0001f); 

			_LOOPk(numFilter)
			{
				m_pSHPRTCompute->FilterVertexTransfer(pLocEntries, pAdjacency, pNumAdjacency, pRData, pGData, pBData, numVertices, dotThreshold);
			}

			// Store result vb
			IRVertexBuffer* pTransferVB = m_pSHPRTCompute->CreateTransferVB(pRData, pGData, pBData, numVertices);
			pPRTComputeMeshes[i].pTempVB = pTransferVB;
			pPRTComputeMeshes[i].pTempVB->AddRef();

			_DELETE_ARRAY(pRData);
			_DELETE_ARRAY(pGData);
			_DELETE_ARRAY(pBData);
			_DELETE_ARRAY(pLocEntries);
		}

		_LOOPi(numPRTComputeMeshes)
		{
			// Remove last result
			pPRTComputeMeshes[i].pMesh->pVBGroup->RemoveVertexBuffer(pPRTComputeMeshes[i].pMesh->pVBGroup->GetNumOfVertexBuffers() - 1);

			// Add new result
			pPRTComputeMeshes[i].pMesh->pVBGroup->AddVertexBuffer(pPRTComputeMeshes[i].pTempVB);

			pPRTComputeMeshes[i].pTempVB->Release();
		}
	}

	//m_pSHPRTCompute->ComputeVertexLocEntries(

	// Reset parameters
	//_DELETE_ARRAY(pPRTComputeMeshes);
	
	m_pComputeSetup->ResetLights();
}

IRTexture2D* AHScenePRTCompute::ComputeDebug(ISDBSceneContainer* pScene, const gmtl::VecA3f& pos, const gmtl::VecA3f& dir, float rayViewDistance, float cascadeShadowDistBuffer)
{
	// Find all relevant meshes that requires PRT data and initialize them

	const static uint MAX_NUM_RENDER = 16384;
	PRTMeshEntry pPRTComputeMeshes[MAX_NUM_RENDER];
	uint numPRTComputeMeshes = InitializeMeshes(pScene, FALSE, pPRTComputeMeshes, MAX_NUM_RENDER);

	// Setup parameters
	m_pComputeSetup->SetupLightsForPRTCompute(pScene, rayViewDistance, cascadeShadowDistBuffer, m_pRenderPhaseThreadPool->GetNumThreads());
	m_pComputePhase->SetScene(pScene);
	m_pComputePhase->SetViewDistance(rayViewDistance);

	// Now compute the PRT
	SHPRTLocationEntry locEntry;
	locEntry.pos = pos;
	locEntry.normal = dir;

	IRTexture2D* pTex = m_pSHPRTCompute->ComputeOccludedTransferDebug(locEntry, *this);
	
	m_pComputeSetup->ResetLightsNoDestroy();

	return pTex;
}

_NAMESPACE_END

#endif