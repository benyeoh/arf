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

void BakedSMComputeParamPool::SetAreaLights(BakedSMAreaLight* pAreaLights, uint numAreaLights)
{
    m_pBakedSMAreaLights = pAreaLights;
    m_NumAreaLights = numAreaLights;
}

void BakedSMComputeParamPool::GetParam(uint semantic, REffectParam* pToStore)
{
    _DEBUG_COMPILE_ASSERT(2048 == BAKEDSMCOMPUTE_PARABOLOID_VIEW);
    _DEBUG_COMPILE_ASSERT(2049 == BAKEDSMCOMPUTE_FAR_PLANE);
    _DEBUG_COMPILE_ASSERT(2050 == BAKEDSMCOMPUTE_TEST_LIGHT_SIZE);
    _DEBUG_COMPILE_ASSERT(2051 == BAKEDSMCOMPUTE_TEST_LIGHT_DIST);
    _DEBUG_COMPILE_ASSERT(2052 == BAKEDSMCOMPUTE_SPHERE_LIGHT);    
    _DEBUG_COMPILE_ASSERT(2053 == BAKEDSMCOMPUTE_AREA_LIGHTS);    
    _DEBUG_COMPILE_ASSERT(2054 == BAKEDSMCOMPUTE_AREA_LIGHT1);
    _DEBUG_COMPILE_ASSERT(2055 == BAKEDSMCOMPUTE_AREA_LIGHT2);
    _DEBUG_COMPILE_ASSERT(2056 == BAKEDSMCOMPUTE_AREA_LIGHT3);
    _DEBUG_COMPILE_ASSERT(2057 == BAKEDSMCOMPUTE_AREA_LIGHT4);
    _DEBUG_COMPILE_ASSERT(2058 == BAKEDSMCOMPUTE_AREA_LIGHT5);
    _DEBUG_COMPILE_ASSERT(2059 == BAKEDSMCOMPUTE_AREA_LIGHT6);
    _DEBUG_COMPILE_ASSERT(2060 == BAKEDSMCOMPUTE_AREA_LIGHT7);
    _DEBUG_COMPILE_ASSERT(2061 == BAKEDSMCOMPUTE_AREA_LIGHT8);
    _DEBUG_COMPILE_ASSERT(2062 == BAKEDSMCOMPUTE_AREA_LIGHT9);
    _DEBUG_COMPILE_ASSERT(2063 == BAKEDSMCOMPUTE_AREA_LIGHT10);
    _DEBUG_COMPILE_ASSERT(2064 == BAKEDSMCOMPUTE_AREA_LIGHT11);
    _DEBUG_COMPILE_ASSERT(2065 == BAKEDSMCOMPUTE_AREA_LIGHT12);
    _DEBUG_COMPILE_ASSERT(2066 == BAKEDSMCOMPUTE_AREA_LIGHT13);
    _DEBUG_COMPILE_ASSERT(2067 == BAKEDSMCOMPUTE_AREA_LIGHT14);
    _DEBUG_COMPILE_ASSERT(2068 == BAKEDSMCOMPUTE_AREA_LIGHT15);
    _DEBUG_COMPILE_ASSERT(2069 == BAKEDSMCOMPUTE_AREA_LIGHT16);

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

    case BAKEDSMCOMPUTE_AREA_LIGHTS:
        {
            pToStore->SetValue(m_pBakedSMAreaLights, sizeof(BakedSMAreaLight)*16);
            break;
        }

    case BAKEDSMCOMPUTE_AREA_LIGHT1:
    case BAKEDSMCOMPUTE_AREA_LIGHT2:
    case BAKEDSMCOMPUTE_AREA_LIGHT3:
    case BAKEDSMCOMPUTE_AREA_LIGHT4:
    case BAKEDSMCOMPUTE_AREA_LIGHT5:
    case BAKEDSMCOMPUTE_AREA_LIGHT6:
    case BAKEDSMCOMPUTE_AREA_LIGHT7:
    case BAKEDSMCOMPUTE_AREA_LIGHT8:
    case BAKEDSMCOMPUTE_AREA_LIGHT9:
    case BAKEDSMCOMPUTE_AREA_LIGHT10:
    case BAKEDSMCOMPUTE_AREA_LIGHT11:
    case BAKEDSMCOMPUTE_AREA_LIGHT12:
    case BAKEDSMCOMPUTE_AREA_LIGHT13:
    case BAKEDSMCOMPUTE_AREA_LIGHT14:
    case BAKEDSMCOMPUTE_AREA_LIGHT15:
    case BAKEDSMCOMPUTE_AREA_LIGHT16:
        {
            pToStore->SetValue(m_pBakedSMAreaLights + (semantic - BAKEDSMCOMPUTE_AREA_LIGHT1), sizeof(BakedSMAreaLight));
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
