//==============================================================================
//
//		AHScreenSpaceFrustumCuller.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/27/2008
//
//==============================================================================

#include "AppHelpers.h"

_NAMESPACE_BEGIN

void
AHScreenSpaceFrustumCuller::SetViewProjMatrixPersp(const gmtl::MatrixA44f& viewProj, float nearZ, float farZ)
{
	//if(!isOrtho)
	//{
	//	// Derive the near and far plane
	//	m_ZNear = proj[2][3] / proj[2][2];
	//	m_ZFar = proj[2][3] / (1.0f + proj[2][2]);	
	//}

	m_ViewProj = viewProj;
	m_ZNear	= nearZ;
	m_ZFar = farZ;
		
	//MatMatMult(&m_ViewProj, &proj, &view);	
	//m_IsOrtho = isOrtho;
}

void
AHScreenSpaceFrustumCuller::SetViewProjMatrixOrtho(const gmtl::MatrixA44f& viewProj)
{
	//if(!isOrtho)
	//{
	//	// Derive the near and far plane
	//	m_ZNear = proj[2][3] / proj[2][2];
	//	m_ZFar = proj[2][3] / (1.0f + proj[2][2]);	
	//}

	m_ViewProj = viewProj;
		
	//MatMatMult(&m_ViewProj, &proj, &view);	
	//m_IsOrtho = isOrtho;
}

boolean
AHScreenSpaceFrustumCuller::IsInFrustumOrtho(const OOBox& worldSpaceBox, AABox& screenSpaceResult)
{
	TransformOOBoxToAABox(&m_ViewProj, &worldSpaceBox, &screenSpaceResult);
	return AABoxInAABox(&screenSpaceResult, &m_Frustum);
}

boolean
AHScreenSpaceFrustumCuller::IsInFrustumPersp(const OOBox& worldSpaceBox, AABox& screenSpaceResult)
{
	TransformAndProjectOOBoxToAABox(&m_ViewProj, m_ZNear, m_ZFar, &worldSpaceBox, &screenSpaceResult);
	return AABoxInAABox(&screenSpaceResult, &m_Frustum);
}

_NAMESPACE_END