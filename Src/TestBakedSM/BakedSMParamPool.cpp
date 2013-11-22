//================================================================================
//
//		BakedSMComputeParamPool.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/29/2012
//
//================================================================================

#include "stdafx.h"

void BakedSMComputeParamPool::SetParaboloidViewMatrix(const gmtl::Matrix44f* pMat)
{
    m_pParaboloidView = pMat;
}

void BakedSMComputeParamPool::SetBakedSM(IRTexture2D* pSM, uint index)
{
    _DEBUG_ASSERT(index < MAX_NUM_SM);
    m_pBakedSM[index] = pSM;
}

void BakedSMComputeParamPool::SetTestLightParams(float size, float dist)
{
    m_TestLightParams[0] = size;
    m_TestLightParams[1] = dist;
}

void BakedSMComputeParamPool::SetSphereLight(gmtl::VecA3f& pos, float radius)
{
    m_SphereLight[0] = pos[0];
    m_SphereLight[1] = pos[1];
    m_SphereLight[2] = pos[2];
    m_SphereLight[3] = radius;

}

void BakedSMComputeParamPool::GetParam(uint semantic, REffectParam* pToStore)
{
    _DEBUG_COMPILE_ASSERT(2048 == BAKEDSMCOMPUTE_PARABOLOID_VIEW);
    _DEBUG_COMPILE_ASSERT(2049 == BAKEDSMCOMPUTE_FAR_PLANE);
    _DEBUG_COMPILE_ASSERT(2050 == BAKEDSMCOMPUTE_TEST_LIGHT_SIZE);
    _DEBUG_COMPILE_ASSERT(2051 == BAKEDSMCOMPUTE_TEST_LIGHT_DIST);
    _DEBUG_COMPILE_ASSERT(2052 == BAKEDSMCOMPUTE_SPHERE_LIGHT);    

    _DEBUG_COMPILE_ASSERT(2176 == BAKEDSMCOMPUTE_BAKED_SM_TEX1);
    _DEBUG_COMPILE_ASSERT(2177 == BAKEDSMCOMPUTE_BAKED_SM_TEX2);
    _DEBUG_COMPILE_ASSERT(2178 == BAKEDSMCOMPUTE_BAKED_SM_TEX3);
    _DEBUG_COMPILE_ASSERT(2179 == BAKEDSMCOMPUTE_BAKED_SM_TEX4);
    _DEBUG_COMPILE_ASSERT(2180 == BAKEDSMCOMPUTE_BAKED_SM_VIS_SPHERE_TABLE_TEX);
    _DEBUG_COMPILE_ASSERT(2181 == BAKEDSMCOMPUTE_BAKED_SM_EXP);
    
    switch(semantic)
    {
    case BAKEDSMCOMPUTE_PARABOLOID_VIEW:
        {	
            pToStore->SetMatrix44(m_pParaboloidView);
            break;
        }
    case BAKEDSMCOMPUTE_FAR_PLANE:
        {
            pToStore->SetFloat(BAKED_SM_MAX_FAR_PLANE);
            break;
        }

    case BAKEDSMCOMPUTE_TEST_LIGHT_SIZE:
        {
            pToStore->SetFloat(m_TestLightParams[0]);
            break;
        }

    case BAKEDSMCOMPUTE_TEST_LIGHT_DIST:
        {
            pToStore->SetFloat(m_TestLightParams[1]);
            break;
        }

    case BAKEDSMCOMPUTE_SPHERE_LIGHT:
        {
            pToStore->SetVector4(&_CAST_VEC4(m_SphereLight));
            break;
        }
    }
}

boolean BakedSMComputeParamPool::IsParamAvailable(uint semantic)
{
    return TRUE;
}

boolean BakedSMComputeParamPool::IsMorePassesRequired()
{
    return FALSE;
}

void BakedSMComputeParamPool::FinishedResolvingEffect()
{
}

void BakedSMComputeParamPool::ResetParams()
{
}
