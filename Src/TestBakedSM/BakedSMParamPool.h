//================================================================================
//
//		BakedSMComputeParamPool.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/28/2012
//
//================================================================================

#pragma once

class BakedSMComputeParamPool : public IBFXParamCallback
{
private:
    const static uint MAX_NUM_SM = 4;

    const gmtl::Matrix44f*	m_pParaboloidView;
    IRTexture2D* m_pBakedSM[MAX_NUM_SM];

public:
    BakedSMComputeParamPool()
        : m_pParaboloidView(NULL)
    {
        _LOOPi(MAX_NUM_SM)
        {
            m_pBakedSM[i] = NULL;
        }
    }

    ~BakedSMComputeParamPool()
    {
    }

public:
    void SetParaboloidViewMatrix(const gmtl::Matrix44f* pMat);
    void SetBakedSM(IRTexture2D* pSM, uint index);

public:
    void GetParam(uint semantic, REffectParam* pToStore);
    boolean IsParamAvailable(uint semantic);
    boolean IsMorePassesRequired();

    void FinishedResolvingEffect();
    void ResetParams();

    // Prevent cache line false sharing
    _IMPL_ALIGNED_ALLOCS(_CACHE_LINE_SIZE)
};

