//================================================================================
//
//		CSHPRTComputeParamPool.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/29/2012
//
//================================================================================

#include "SHPRTComputeFramework.h"

_NAMESPACE_BEGIN

void CSHPRTComputeParamPool::SetParaboloidViewMatrix(const gmtl::Matrix44f* pMat)
{
	m_pParaboloidView = pMat;
}

void CSHPRTComputeParamPool::GetParam(uint semantic, REffectParam* pToStore)
{
	switch(semantic)
	{
	case SHPRTCOMPUTE_PARABOLOID_VIEW:
		{	
			pToStore->SetMatrix44(m_pParaboloidView);
			break;
		}
	}
}

boolean CSHPRTComputeParamPool::IsParamAvailable(uint semantic)
{
	return TRUE;
}

boolean CSHPRTComputeParamPool::IsMorePassesRequired()
{
	return FALSE;
}

void CSHPRTComputeParamPool::FinishedResolvingEffect()
{
}

void CSHPRTComputeParamPool::ResetParams()
{
}

_NAMESPACE_END