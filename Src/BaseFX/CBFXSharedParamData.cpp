//================================================================================
//
//		CBFXSharedParamData.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/17/2012
//
//================================================================================

#include "BaseFXFramework.h"

_NAMESPACE_BEGIN

void CBFXSharedParamData::SetViewMatrix(const gmtl::Matrix44f& view)
{
	m_pView = &view;

	m_DirtyViewProjMat = TRUE;
}

const gmtl::Matrix44f& CBFXSharedParamData::GetViewMatrix()
{	
	return *m_pView;
}

void CBFXSharedParamData::SetProjMatrix(const gmtl::Matrix44f& proj)
{
	m_pProj = &proj;

	// Derive the far plane
	//m_CameraPosWithViewFar[3] = proj[2][3] / (1.0f + proj[2][2]);	

	//m_DirtyCamPosViewFar = TRUE;
	//m_DirtyCamPosViewNear = TRUE;
	m_DirtyViewProjMat = TRUE;
}

const gmtl::Matrix44f& CBFXSharedParamData::GetProjMatrix()
{
	return *m_pProj;
}

const gmtl::Matrix44f& CBFXSharedParamData::GetViewProjMatrix()
{
	if(m_DirtyViewProjMat)
	{
		gmtl::Matrix44f* pMat = m_ViewProjStack.Get();
		MatMatMult(pMat, m_pProj, m_pView);
		m_DirtyViewProjMat = FALSE;
	}

	return *(m_ViewProjStack.GetLast());
}

void CBFXSharedParamData::SetCameraProperties(const gmtl::Vec3f& cameraPos, float nearPlane, float farPlane)
{
	m_CameraPos[0] = cameraPos[0];
	m_CameraPos[1] = cameraPos[1];
	m_CameraPos[2] = cameraPos[2];

	m_Near = nearPlane;
	m_Far = farPlane;

	m_DirtyCamPosViewFar = TRUE;
	m_DirtyCamPosViewNear = TRUE;
}

const gmtl::Vec4f& CBFXSharedParamData::GetCameraPosWithViewFar()
{
	if(m_DirtyCamPosViewFar)
	{
		gmtl::Vec4f* pCamPosViewFar = m_CameraPosWithViewFarStack.Get();
		(*pCamPosViewFar)[0] = m_CameraPos[0];
		(*pCamPosViewFar)[1] = m_CameraPos[1];
		(*pCamPosViewFar)[2] = m_CameraPos[2];
		(*pCamPosViewFar)[3] = m_Far;

		//// Derive the far plane
		//const gmtl::Matrix44f& proj = GetProjMatrix();
		//if(proj[3][3] >= 0.9f)
		//{
		//	// Assume orthogonal
		//	float nearPlane =  proj[2][3] / proj[2][2];
		//	float farPlane = nearPlane - (1.0f / proj[2][2]);
		//	_DEBUG_ASSERT(farPlane > 0.0f);
		//	(*pCamPosViewFar)[3] = -farPlane;
		//}
		//else
		//{
		//	// Perspective matrix
		//	float nearPlane =  proj[2][3] / proj[2][2];
		//	float top =  proj[2][3];
		//	float bottom = -1.0f - (proj[2][2]);
		//	float farPlane = top / bottom;
		//	(*pCamPosViewFar)[3] = farPlane;
		//		//proj[2][3] / (1.0f + proj[2][2]);
		//}

		m_DirtyCamPosViewFar = FALSE;
	}

	return *(m_CameraPosWithViewFarStack.GetLast());
}

const gmtl::Vec4f& CBFXSharedParamData::GetCameraPosWithViewNear()
{
	if(m_DirtyCamPosViewNear)
	{
		gmtl::Vec4f* pCamPosViewNear = m_CameraPosWithViewNearStack.Get();
		(*pCamPosViewNear)[0] = m_CameraPos[0];
		(*pCamPosViewNear)[1] = m_CameraPos[1];
		(*pCamPosViewNear)[2] = m_CameraPos[2];
		(*pCamPosViewNear)[3] = m_Near;

		//// Derive the near plane
		//const gmtl::Matrix44f& proj = GetProjMatrix();
		//(*pCamPosViewNear)[3] = proj[2][3] / (proj[2][2]);

		m_DirtyCamPosViewNear = FALSE;
	}

	return *(m_CameraPosWithViewNearStack.GetLast());
}

void CBFXSharedParamData::ResetParams()
{
	if(m_ViewProjStack.GetNumAllocated() > 1)
	{
		gmtl::Matrix44f* pTemp = (m_ViewProjStack.GetLast());
		m_ViewProjStack.Reset();
		*(m_ViewProjStack.Get()) = *pTemp;
	}

	if(m_CameraPosWithViewFarStack.GetNumAllocated() > 1)
	{
		gmtl::Vec4f* pTemp = m_CameraPosWithViewFarStack.GetLast();
		m_CameraPosWithViewFarStack.Reset();
		*(m_CameraPosWithViewFarStack.Get()) = *pTemp;
	}

	if(m_CameraPosWithViewNearStack.GetNumAllocated() > 1)
	{
		gmtl::Vec4f* pTemp = m_CameraPosWithViewNearStack.GetLast();
		m_CameraPosWithViewNearStack.Reset();
		*(m_CameraPosWithViewNearStack.Get()) = *pTemp;
	}
}

void CBFXSharedParamData::Finalize()
{
	GetCameraPosWithViewFar();
	GetCameraPosWithViewNear();
	GetViewProjMatrix();
}

_NAMESPACE_END