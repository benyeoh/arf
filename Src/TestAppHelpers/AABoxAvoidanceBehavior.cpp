//==============================================================================
//
//		AABoxAvoidanceBehavior.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/31/2009
//
//==============================================================================

#include "stdafx.h"

void 
AABoxAvoidanceBehavior::Update(float timeDT, AHComponent* pCurFlocking, AHComponent** ppOthers, uint numOthers, gmtl::VecA3f& newDir)
{
	AHEntity* pCurEntity = pCurFlocking->GetEntity();

	AHSpatialComponent* pSpatComp	= _CAST_COMP_SPATIAL(pCurEntity);
	_DEBUG_ASSERT(pSpatComp);
	const gmtl::MatrixA44f& worldMat = pSpatComp->GetWorldMatrix();
	gmtl::VecA3f pos(worldMat[0][3], worldMat[1][3], worldMat[2][3]);		
	
	gmtl::VecA3f minDiff;
	VecVecSub(&_CAST_VECA4(minDiff), &_CAST_VECA4(pos), &_CAST_VECA4(m_AABox.min));
	
	gmtl::VecA3f maxDiff;
	VecVecSub(&_CAST_VECA4(maxDiff), &_CAST_VECA4(m_AABox.max), &_CAST_VECA4(pos));
	
	if(minDiff[0] < m_Threshold)
		newDir[0] = 1.0f;
	else if(maxDiff[0] < m_Threshold)
		newDir[0] = -1.0f;
		
	if(minDiff[1] < m_Threshold)
		newDir[1] = 1.0f;
	else if(maxDiff[1] < m_Threshold)
		newDir[1] = -1.0f;
		
	if(minDiff[2] < m_Threshold)
		newDir[2] = 1.0f;
	else if(maxDiff[2] < m_Threshold)
		newDir[2] = -1.0f;
	
	if(gmtl::Math::abs(newDir[0]) > gmtl::GMTL_EPSILON ||
		gmtl::Math::abs(newDir[1]) > gmtl::GMTL_EPSILON ||
		gmtl::Math::abs(newDir[2]) > gmtl::GMTL_EPSILON)
		NormalizeVec(newDir);	
}
