//==============================================================================
//
//		SeparationBehavior.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/31/2009
//
//==============================================================================

#include "stdafx.h"

void SeparationBehavior::Update(float timeDT, AHComponent* pCurFlocking, AHComponent** ppOthers, uint numOthers, gmtl::VecA3f& newDir)
{
	AHEntity* pCurEntity = pCurFlocking->GetEntity();

	AHSpatialComponent* pSpatComp	= _CAST_COMP_SPATIAL(pCurEntity);
	_DEBUG_ASSERT(pSpatComp);
	const gmtl::MatrixA44f& worldMat = pSpatComp->GetWorldMatrix();
	gmtl::VecA3f pos(worldMat[0][3], worldMat[1][3], worldMat[2][3]);		
	
	gmtl::VecA3f tempDir;
	tempDir.set(0.0f, 0.0f, 0.0f);

	_LOOPi(numOthers)
	{
		FlockingAIComponent* pOtherFlocking = (FlockingAIComponent*) ppOthers[i];
		AHEntity* pOtherEntity = pOtherFlocking->GetEntity();
		if(pOtherEntity != pCurEntity)
		{
			AHSpatialComponent* pOtherSpatComp = _CAST_COMP_SPATIAL(pOtherEntity);
			_DEBUG_ASSERT(pOtherSpatComp);
			
			const gmtl::MatrixA44f& otherWorldMat = pOtherSpatComp->GetWorldMatrix();
			gmtl::VecA3f otherPos(otherWorldMat[0][3], otherWorldMat[1][3], otherWorldMat[2][3]);
			
			gmtl::VecA3f dirToOther;
			VecVecSub(&_CAST_VECA4(dirToOther), &_CAST_VECA4(otherPos), &_CAST_VECA4(pos));
			
			float length;
			NormalizeVecLength(&length, &dirToOther, &dirToOther);
			
			if(length < m_Radius)
			{
				VecVecSub(&tempDir, &tempDir, &dirToOther);
			}
		}
	}
	
	if(gmtl::Math::abs(tempDir[0]) > gmtl::GMTL_EPSILON ||
		gmtl::Math::abs(tempDir[1]) > gmtl::GMTL_EPSILON ||
		gmtl::Math::abs(tempDir[2]) > gmtl::GMTL_EPSILON)
		NormalizeVec(tempDir);

	newDir = tempDir;
}
							