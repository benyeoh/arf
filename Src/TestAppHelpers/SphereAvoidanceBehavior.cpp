//==============================================================================
//
//		SphereAvoidanceBehavior.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/31/2009
//
//==============================================================================

#include "stdafx.h"

void 
SphereAvoidanceBehavior::Update(float timeDT, AHComponent* pCurFlocking, AHComponent** ppOthers, uint numOthers, gmtl::VecA3f& newDir)
{
	AHEntity* pCurEntity = pCurFlocking->GetEntity();

	AHSpatialComponent* pSpatComp	= _CAST_COMP_SPATIAL(pCurEntity);
	_DEBUG_ASSERT(pSpatComp);
	const gmtl::MatrixA44f& worldMat = pSpatComp->GetWorldMatrix();
	gmtl::VecA3f pos(worldMat[0][3], worldMat[1][3], worldMat[2][3]);		
	
	gmtl::VecA3f diff;
	VecVecSub(&_CAST_VECA4(diff), &_CAST_VECA4(m_Sphere.center), &_CAST_VECA4(pos));
		
	float distance;
	NormalizeVecLength(&distance, &diff, &diff);

	float distanceDiff = m_Sphere.center[3] - distance;
	if(distanceDiff < m_Threshold)
	{
		newDir = diff;
	}
}

	