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

void BakedSMComputeParamPool::GetParam(uint semantic, REffectParam* pToStore)
{
    _DEBUG_COMPILE_ASSERT(2048 == BAKEDSMCOMPUTE_PARABOLOID_VIEW);
    _DEBUG_COMPILE_ASSERT(2049 == BAKEDSMCOMPUTE_FAR_PLANE);
    _DEBUG_COMPILE_ASSERT(2050 == BAKEDSMCOMPUTE_BAKED_SM_EXP);

    _DEBUG_COMPILE_ASSERT(2176 == BAKEDSMCOMPUTE_BAKED_SM_TEX1);
    _DEBUG_COMPILE_ASSERT(2177 == BAKEDSMCOMPUTE_BAKED_SM_TEX2);
    _DEBUG_COMPILE_ASSERT(2178 == BAKEDSMCOMPUTE_BAKED_SM_TEX3);
    _DEBUG_COMPILE_ASSERT(2179 == BAKEDSMCOMPUTE_BAKED_SM_TEX4);

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

    case BAKEDSMCOMPUTE_BAKED_SM_EXP:
        {
            pToStore->SetFloat(BAKED_SM_EXP_K_VAL);
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
