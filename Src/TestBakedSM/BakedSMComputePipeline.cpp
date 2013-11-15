//================================================================================
//
//		CSHPRTComputePipeline.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/28/2012
//
//================================================================================

#include "stdafx.h"

void BakedSMComputePipeline::Initialize(IRRenderer* pRenderer, IAppCallback* pCallback)
{
    _DEBUG_COMPILE_ASSERT(BAKEDSMCOMPUTE_MAT_OCCLUSION == 32);

    m_CurrentLatencyRT = 0;

    _LOOPi(BAKED_SM_NUM_LATENCY_BUFFERS)
    {
        m_pOcclusionRT[i] = pRenderer->GetRResourceMgr().CreateTexture2DRT(BAKED_SM_COMPUTE_SM_RT_SIZE, BAKED_SM_COMPUTE_SM_RT_SIZE, 1, TEXF_R32F);
    }

    m_pOcclusionDB = pRenderer->GetRResourceMgr().CreateDepthBuffer(BAKED_SM_COMPUTE_SM_RT_SIZE, BAKED_SM_COMPUTE_SM_RT_SIZE, DF_D24S8, 0);

    m_pClearGroup = pRenderer->GetRResourceMgr().CreateRenderGroup(NULL);
    m_pClearGroup->SetDepthBuffer(m_pOcclusionDB);
    m_pClearGroup->SetViewport(0, 0, BAKED_SM_COMPUTE_SM_RT_SIZE, BAKED_SM_COMPUTE_SM_RT_SIZE);
    m_pClearGroup->SetClearColor(gmtl::Vec4f(1.0f, 1.0f, 1.0f, 1.0f));
    m_pClearGroup->SetClearDepth(1.0f);
}

void BakedSMComputePipeline::SetOcclusionRT(uint index)
{
    m_CurrentLatencyRT = index;

    m_pClearGroup->ClearRenderTargets();
    m_pClearGroup->AddRenderTarget(m_pOcclusionRT[index]->GetTextureRT(0));
}

IRDepthBuffer* BakedSMComputePipeline::GetOcclusionDB()
{
    return m_pOcclusionDB;
}

IRTexture2D* BakedSMComputePipeline::GetOcclusionRT(uint index)
{
    return m_pOcclusionRT[index];
}

void BakedSMComputePipeline::Clear()
{
    m_pClearGroup->Flush();
}
