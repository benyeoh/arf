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

struct BakedSMAreaLight
{
    gmtl::VecA4f posAndRadius;
    gmtl::VecA4f lightColor;
};

class BakedSMComputeParamPool : public IBFXParamCallback
{
private:
    const static uint MAX_NUM_SM = 4;

    const gmtl::Matrix44f*	m_pParaboloidView;
    IRTexture2D* m_pBakedSM[MAX_NUM_SM];

    float m_TestLightParams[2];
    gmtl::VecA4f m_SphereLight;

    BakedSMAreaLight* m_pBakedSMAreaLights;
    uint m_NumAreaLights;

public:
    BakedSMComputeParamPool()
        : m_pParaboloidView(NULL)
        , m_pBakedSMAreaLights(NULL)
        , m_NumAreaLights(0)
    {
        _LOOPi(MAX_NUM_SM)
        {
            m_pBakedSM[i] = NULL;
        }

        m_TestLightParams[0] = 0.5f;
        m_TestLightParams[1] = 0.5f;

        m_SphereLight.set(0.0f, 8.0f, 0.0f, 1.0f);
    }

    ~BakedSMComputeParamPool()
    {
    }

public:
    void SetParaboloidViewMatrix(const gmtl::Matrix44f* pMat);
    void SetBakedSM(IRTexture2D* pSM, uint index);
    void SetTestLightParams(float size, float dist);
    void SetSphereLight(gmtl::VecA3f& pos, float radius);
    void SetAreaLights(BakedSMAreaLight* pAreaLights, uint numAreaLights);

public:
    void GetParam(uint semantic, REffectParam* pToStore);
    boolean IsParamAvailable(uint semantic);
    boolean IsMorePassesRequired();

    void FinishedResolvingEffect();
    void ResetParams();

    // Prevent cache line false sharing
    _IMPL_ALIGNED_ALLOCS(_CACHE_LINE_SIZE)
};

