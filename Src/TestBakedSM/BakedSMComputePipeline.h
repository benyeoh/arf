//================================================================================
//
//		BakedSMComputePipeline.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/28/2012
//
//================================================================================

#pragma once

class BakedSMComputePipeline
{
private:
    IRRenderGroupPtr	m_pClearGroup;

    IRTexture2DPtr		m_pOcclusionRT[BAKED_SM_NUM_LATENCY_BUFFERS];
    IRDepthBufferPtr	m_pOcclusionDB;

    uint				m_CurrentLatencyRT;

public:
    BakedSMComputePipeline()
    {
    }

    ~BakedSMComputePipeline()
    {
    }

public:
    void Initialize(IRRenderer* pRenderer, IAppCallback* pCallback);
    //void RenderEffect(BFXRenderContext& renderContext);

    void Clear();
    //void Flush();

    void SetOcclusionRT(uint index);
    IRTexture2D* GetOcclusionRT(uint index);
    IRDepthBuffer* GetOcclusionDB();
};

