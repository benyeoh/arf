//================================================================================
//
//		BackedSMCallback.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		8/8/2012
//
//================================================================================

#include "stdafx.h"

void BakedSMRenderCallback::ResetMaterials()
{
    // Create a new material with the baked SM
    REffectParamStore effectParamsStore[16*NUM_MESHES];
	REffectParam effectParams[16*NUM_MESHES];
    uint semantics[16*NUM_MESHES];
	uint numParams[NUM_MESHES];

	_LOOPi(NUM_MESHES)
	{
		numParams[i] = g_Meshes[i].pMatGroup->GetNumOfParams();
	
		_DEBUG_ASSERT(numParams[i] < 16);
		_LOOPj(numParams[i])
		{
			g_Meshes[i].pMatGroup->GetParam(j, effectParams[i*16+j]);
			effectParamsStore[i*16+j] = effectParams[i*16+j];
			semantics[i*16+j] = g_Meshes[i].pMatGroup->GetSemantic(j);
		}

		g_Meshes[i].pMatGroup = NULL;
	}
	
	const wchar* pFileName = _W("[shd]\\TestBakedSM\\TestBakedSMSH.mgp");
	IFFilePtr pFile = g_pFileSystem->GetFile(pFileName);
	IByteBufferPtr pMatData = _NEW CByteBuffer(pFile->Length());
	AppendData(pMatData, pFile);

	_LOOPi(NUM_MESHES)
	{
		IBFXMaterialGroupTemplatePtr pMatGroupTemplate = g_pBaseFX->GetResourceMgr().CreateMaterialGroupTemplate(NULL, pMatData, pFileName);
		g_Meshes[i].pMatGroup = g_pBaseFX->GetResourceMgr().CreateMaterialGroup(NULL, pMatGroupTemplate, &(effectParams[i*16]), &(semantics[i*16]), numParams[i]);
	}
}

void BakedSMRenderCallback::ComputeOneMesh(uint index, uint lightMapSize, IRTexture2D* pVisSphereTex)
{
    BakedSMLocationEntry* pLocEntries = (BakedSMLocationEntry*) _ALIGNED_MALLOC(16, lightMapSize * lightMapSize * 2 * sizeof(BakedSMLocationEntry));
    uint posOffset = GetVertexOffset(g_Meshes[index].pVBGroup->GetVertexBuffer(0)->GetDescription(), VDU_POSITION, 0);
    uint texOffset = GetVertexOffset(g_Meshes[index].pVBGroup->GetVertexBuffer(0)->GetDescription(), VDU_TEXCOORDF2, 0);
    uint normOffset = GetVertexOffset(g_Meshes[index].pVBGroup->GetVertexBuffer(0)->GetDescription(), VDU_NORMAL, 0);
    byte* pVBReadData = g_Meshes[index].pVBGroup->GetVertexBuffer(0)->Lock(0, 0);
    uint stride = g_Meshes[index].pVBGroup->GetVertexBuffer(0)->GetVertexSize();
    ushort* pIBReadData = g_Meshes[index].pIB->Lock(0, 0);
    uint numEntries = g_pBakedSMCompute->ComputeUVLocEntries(pLocEntries, g_MeshesWorld[index], 
        pVBReadData + posOffset, stride, 
        pVBReadData + normOffset, stride, 
        pVBReadData + texOffset, stride, 
        pIBReadData, 
        g_Meshes[index].pIB->GetNumIndices() / 3, 
        lightMapSize, lightMapSize, 
        0.03f);
    g_Meshes[index].pVBGroup->GetVertexBuffer(0)->Unlock(FALSE);
    g_Meshes[index].pIB->Unlock(FALSE);


    BakedSMSHEntry* pSHEntries = (BakedSMSHEntry*) _ALIGNED_MALLOC(16, lightMapSize * lightMapSize * 2 * sizeof(BakedSMSHEntry));
    g_pPlatform->GetTimer().BeginSample(); 
    g_pBakedSMCompute->ComputeShadowMapSH(pLocEntries, pSHEntries, numEntries, *this, g_pThreadPool);
    g_pPlatform->GetTimer().EndSample(); 
   
    IRTexture2D* pBakedSMTex[4];
    uint numBakedSMTex = g_pBakedSMCompute->CreateBakedSMSH16Bit(pLocEntries, pSHEntries, numEntries, lightMapSize, lightMapSize, pBakedSMTex);
    _DEBUG_ASSERT(numBakedSMTex == 4);

    // Create a new material with the baked SM
    REffectParam effectParams[32];
    uint semantics[32];

    _LOOPi(g_Meshes[index].pMatGroup->GetNumOfParams())
    {
        g_Meshes[index].pMatGroup->GetParam(i, effectParams[i]);
        semantics[i] = g_Meshes[index].pMatGroup->GetSemantic(i);
    }

    _LOOPi(numBakedSMTex)
    {
        semantics[g_Meshes[index].pMatGroup->GetNumOfParams() + i] = BAKEDSMCOMPUTE_BAKED_SM_TEX1 + i;
        effectParams[g_Meshes[index].pMatGroup->GetNumOfParams() + i].SetTexture2D(pBakedSMTex[i]);
    }

    semantics[g_Meshes[index].pMatGroup->GetNumOfParams() + numBakedSMTex] = BAKEDSMCOMPUTE_BAKED_SM_VIS_SPHERE_TABLE_TEX;
    effectParams[g_Meshes[index].pMatGroup->GetNumOfParams() + numBakedSMTex].SetTexture2D(pVisSphereTex);

    semantics[g_Meshes[index].pMatGroup->GetNumOfParams() + numBakedSMTex + 1] = BAKEDSMCOMPUTE_BAKED_SM_EXP;
    effectParams[g_Meshes[index].pMatGroup->GetNumOfParams() + numBakedSMTex + 1].SetFloat(exp(BAKED_SM_EXP_K_VAL));

    g_Meshes[index].pMatGroup = g_pBaseFX->GetResourceMgr().CreateMaterialGroup(NULL, g_Meshes[index].pMatGroup->GetTemplate(), 
        effectParams, semantics, g_Meshes[index].pMatGroup->GetNumOfParams() + numBakedSMTex + 2);

    _ALIGNED_FREE(pSHEntries);

    //{
    //    IRTexture2DPtr pTest = g_pRenderer->GetRResourceMgr().CreateTexture2D(NULL, TEX_SIZE_X, TEX_SIZE_Y, 1, TEXF_A8R8G8B8, TEXU_DEFAULT_STAGING);	
    //    uint pitch;
    //    byte* pRaw = pTest->Lock(0, pitch, NULL);
    //    memset(pRaw, 0, TEX_SIZE_Y * pitch);
    //    _LOOPi(numEntries)
    //    {
    //        _DEBUG_ASSERT(pLocEntries[i].texX <= TEX_SIZE_X);
    //        _DEBUG_ASSERT(pLocEntries[i].texY <= TEX_SIZE_Y);

    //        uint* pToWrite = (uint*)(pRaw + pLocEntries[i].texY * pitch + pLocEntries[i].texX * sizeof(uint));
    //        *pToWrite = (0xFF000000) | (((uint) (pLocEntries[i].normal[0] * 255.0f)) << 16) | (((uint) (pLocEntries[i].normal[1] * 255.0f)) << 8) | (((uint) (pLocEntries[i].normal[2] * 255.0f)));
    //    }

    //    //_LOOPi(50)
    //    //{
    //    //    _LOOPj(50)
    //    //    {
    //    //        pLocEntries[i * 50 + j].texY = i + 50;
    //    //        pLocEntries[i * 50 + j].texX = j + 50;

    //    //        uint* pToWrite = (uint*)(pRaw + (pLocEntries[i * 50 + j].texY) * pitch + (pLocEntries[i * 50 + j].texX) * sizeof(uint));
    //    //        *pToWrite = 0xFF000000 | ((50 + i * 4) << 16) | ((50 + j * 4) << 8);
    //    //    }
    //    //}

    //    //g_pBakedSMCompute->ComputePushPullTex4ChannelByte(pRaw, TEX_SIZE_X, TEX_SIZE_Y, pLocEntries, 50 * 50);

    //    pTest->Unlock(0);
    //    IByteBufferPtr pToSave = _NEW CByteBuffer();

    //    g_pRenderer->GetRResourceMgr().SaveTexture2D(pTest, pToSave);
    //    IFFilePtr pToSaveFile = g_pFileSystem->GetFile(_W("[dat]\\Textures\\TestBakedSM\\test.dds"));
    //    pToSaveFile->Write(pToSave->GetData(), pToSave->GetDataLength());
    //    pToSaveFile->Close();
    //}

    _ALIGNED_FREE(pLocEntries);
}

void BakedSMRenderCallback::Compute()
{
    IRTexture2DPtr pVisSphereTex = g_pBakedSMCompute->CreateExpDepthSphereSHTable();

    float timeToCompute = 0.0f;
    ComputeOneMesh(0, 128, pVisSphereTex);
    _LOOPi(24)
    {
        _TRACE(_W("Computing mesh: %d\n"), i+1);
        ComputeOneMesh(i+1, 32, pVisSphereTex);
    }
    timeToCompute += (float) g_pPlatform->GetTimer().GetTimeElapsed();

    _TRACE(_W("Time to compute SH: %f\n"), timeToCompute);

    // Reset
    g_pBFXPipeline->ClearRenderTargets();
    g_pBFXPipeline->AddRenderTarget(g_pRenderer->GetBackBufferColor());
    g_pBFXPipeline->SetDepthBuffer(g_pRenderer->GetBackBufferDepth());
    uint width = g_pRenderer->GetBackBufferColor()->GetWidth();
    uint height = g_pRenderer->GetBackBufferColor()->GetHeight();
    g_pBFXPipeline->SetViewport(0, 0, width, height);
}

void BakedSMRenderCallback::Render(const gmtl::VecA3f& loc, const gmtl::MatrixA44f& paraView, BFXRenderContainer& renderContainer, BFXParamContainer& paramContainer, IRTexture2D* pRT, IRDepthBuffer* pDB)
{
    paramContainer.SetParamCallback(g_pBFXParamPool, _GET_LIB_INDEX(BFX_EFFECT_PARAM_OFFSET));
    renderContainer.SetRenderCallback(g_pBFXPipeline, _GET_LIB_INDEX_FROM_MAT_TYPE(BFX_MATERIAL_TYPE_OFFSET));

    g_pBFXParamPool->SetZDistanceFromCamera(1.0f);
    g_pBFXParamPool->SetCameraProperties(_CAST_VEC3(loc), 0.001f, 100.0f);

    g_pBFXPipeline->ClearRenderTargets();
    g_pBFXPipeline->AddRenderTarget(pRT->GetTextureRT(0));
    g_pBFXPipeline->SetDepthBuffer(pDB);
    g_pBFXPipeline->SetViewport(0, 0, pRT->GetWidth(0), pRT->GetHeight(0));

    _LOOPi(NUM_MESHES)
    {
        g_pBFXParamPool->SetWorldMatrix( _CAST_MAT44(g_MeshesWorld[i]) );
      
        g_pBaseFX->Render(g_Meshes[i], 1.0f, BAKEDSMCOMPUTE_MAT_OCCLUSION, BFX_MAT_BASE_LIGHTING, paramContainer, renderContainer);
    }

    g_pBFXPipeline->FlushOpaqueGroup();
    g_pBFXPipeline->FlushAlphaTestedGroup();
    g_pBFXPipeline->FlushAlphaBlendedGroup();

    g_pBFXParamPool->ResetParams();
}
