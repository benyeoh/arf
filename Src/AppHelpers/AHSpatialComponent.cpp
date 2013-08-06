//==============================================================================
//
//		AHSpatialComponent.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/24/2009
//
//==============================================================================

#include "AppHelpers.h"

_NAMESPACE_BEGIN

#if defined(_IS_USE_APPHELPER_LIB_BASIC)

void AHSpatialComponent::SetWorldMatrix(const gmtl::MatrixA44f& mat)
{ 
	m_WorldMatrix = mat; 

	UpdateWorldBoundsTranslate();

	_LOOPi(m_pListeners.size())
	{
		m_pListeners[i]->OnComponentEvent(EVENT_COMP_SPATIAL_CHANGED, this);
	}
}

void 
AHSpatialComponent::SetWorldPos(const gmtl::VecA3f& vec)
{
	m_WorldMatrix[0][3] = vec[0];
	m_WorldMatrix[1][3] = vec[1];
	m_WorldMatrix[2][3] = vec[2];
	
	UpdateWorldBoundsTranslate();

	_LOOPi(m_pListeners.size())
	{
		m_pListeners[i]->OnComponentEvent(EVENT_COMP_SPATIAL_CHANGED, this);
	}
}

void AHSpatialComponent::GetWorldPos(gmtl::VecA3f& pos)
{
	pos[0] = m_WorldMatrix[0][3];
	pos[1] = m_WorldMatrix[1][3];
	pos[2] = m_WorldMatrix[2][3];
}

void 
AHSpatialComponent::SetWorldFacing(const gmtl::VecA3f& dir, const gmtl::VecA3f& up)
{
	gmtl::VecA3f right;
	gmtl::VecA3f yAxis;
	gmtl::VecA3f zAxis = dir;
	VecCrossNormalize(&right, &up, &zAxis);
	VecCrossNormalize(&yAxis, &zAxis, &right);
	
	// TEMP: Invert the axes to get the rotation
	m_WorldMatrix[0][0] = right[0];	m_WorldMatrix[0][1] = yAxis[0];	m_WorldMatrix[0][2] = zAxis[0];	//rotMat[0][3] = 0.0f; 
	m_WorldMatrix[1][0] = right[1];	m_WorldMatrix[1][1] = yAxis[1];	m_WorldMatrix[1][2] = zAxis[1];	//	rotMat[1][3] = 0.0f; 
	m_WorldMatrix[2][0] = right[2];	m_WorldMatrix[2][1] = yAxis[2];	m_WorldMatrix[2][2] = zAxis[2];	//rotMat[2][3] = 0.0f; 
	//rotMat[3][0] = 0.0f;		rotMat[3][1] = 0.0f;		rotMat[3][2] = 0.0f;		rotMat[3][3] = 1.0f;

	UpdateWorldBoundsTranslate();

	_LOOPi(m_pListeners.size())
	{
		m_pListeners[i]->OnComponentEvent(EVENT_COMP_SPATIAL_CHANGED, this);
	}
}

void AHSpatialComponent::SetWorldPosAndFacing(const gmtl::VecA3f& pos, const gmtl::VecA3f& dir, const gmtl::VecA3f& up)
{
	m_WorldMatrix[0][3] = pos[0];
	m_WorldMatrix[1][3] = pos[1];
	m_WorldMatrix[2][3] = pos[2];

	gmtl::VecA3f right;
	gmtl::VecA3f yAxis;
	gmtl::VecA3f zAxis = dir;
	VecCrossNormalize(&right, &up, &zAxis);
	VecCrossNormalize(&yAxis, &zAxis, &right);

	// TEMP: Invert the axes to get the rotation
	m_WorldMatrix[0][0] = right[0];	m_WorldMatrix[0][1] = yAxis[0];	m_WorldMatrix[0][2] = zAxis[0];	//rotMat[0][3] = 0.0f; 
	m_WorldMatrix[1][0] = right[1];	m_WorldMatrix[1][1] = yAxis[1];	m_WorldMatrix[1][2] = zAxis[1];	//	rotMat[1][3] = 0.0f; 
	m_WorldMatrix[2][0] = right[2];	m_WorldMatrix[2][1] = yAxis[2];	m_WorldMatrix[2][2] = zAxis[2];	//rotMat[2][3] = 0.0f; 

	UpdateWorldBoundsTranslate();

	_LOOPi(m_pListeners.size())
	{
		m_pListeners[i]->OnComponentEvent(EVENT_COMP_SPATIAL_CHANGED, this);
	}
}

void 
AHSpatialComponent::UpdateWorldBoundsTranslate()
{
	m_WorldAABox.min[0] = m_LocalAABox.min[0] + m_WorldMatrix[0][3];
	m_WorldAABox.min[1] = m_LocalAABox.min[1] + m_WorldMatrix[1][3];
	m_WorldAABox.min[2] = m_LocalAABox.min[2] + m_WorldMatrix[2][3];

	m_WorldAABox.max[0] = m_LocalAABox.max[0] + m_WorldMatrix[0][3];
	m_WorldAABox.max[1] = m_LocalAABox.max[1] + m_WorldMatrix[1][3];
	m_WorldAABox.max[2] = m_LocalAABox.max[2] + m_WorldMatrix[2][3];
}

void AHSpatialComponent::AddEventListener(AHComponent* pComp)
{
	_LOOPi(m_pListeners.size())
	{
		if(m_pListeners[i] == pComp)
			return;
	}

	m_pListeners.push_back(pComp);
}

void AHSpatialComponent::RemoveEventListener(AHComponent* pComp)
{
	_LOOPi(m_pListeners.size())
	{
		if(m_pListeners[i] == pComp)
		{
			m_pListeners.erase(m_pListeners.begin() + i);
			break;
		}
	}
}

void AHSpatialComponent::OnComponentEvent(int eventType, AHComponent* pSrc)
{
	if(eventType == EVENT_COMP_REMOVED)
		RemoveEventListener(pSrc);
}

#endif

_NAMESPACE_END