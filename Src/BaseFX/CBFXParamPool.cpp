//==============================================================================
//
//		CBFXParamPool.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/28/2008
//
//==============================================================================

#include "BaseFXFramework.h"

_NAMESPACE_BEGIN

void CBFXParamPool::SetCurrentContext(uint id)
{
	m_CurContext = id;
}

uint CBFXParamPool::GetCurrentContext()
{
	return m_CurContext;
}

void
CBFXParamPool::SetCameraProperties(const gmtl::Vec3f& cameraPos, float nearPlane, float farPlane)
{
	m_pSharedParam->SetCameraProperties(cameraPos, nearPlane, farPlane);
}

const gmtl::Vec4f&
CBFXParamPool::GetCameraPosWithViewFar()
{
	return m_pSharedParam->GetCameraPosWithViewFar();
}

const gmtl::Vec4f& CBFXParamPool::GetCameraPosWithViewNear()
{
	return m_pSharedParam->GetCameraPosWithViewNear();
}

void 
CBFXParamPool::SetViewMatrix(const gmtl::Matrix44f& view)
{
	m_pSharedParam->SetViewMatrix(view);

	m_DirtyWorldViewProjMat = TRUE;
	m_DirtyWorldViewMat = TRUE;
}

const gmtl::Matrix44f& 
CBFXParamPool::GetViewMatrix()
{
	return m_pSharedParam->GetViewMatrix();
}

void 
CBFXParamPool::SetProjMatrix(const gmtl::Matrix44f& proj)
{
	m_pSharedParam->SetProjMatrix(proj);

	m_DirtyWorldViewProjMat = TRUE;
}

const gmtl::Matrix44f& 
CBFXParamPool::GetProjMatrix()
{
	return m_pSharedParam->GetProjMatrix();
}

void 
CBFXParamPool::SetWorldMatrix(const gmtl::Matrix44f& world)
{
	m_pWorld = &world;
	m_DirtyWorldViewProjMat = TRUE;
	m_DirtyWorldViewMat = TRUE;
}

const gmtl::Matrix44f& 
CBFXParamPool::GetWorldMatrix()
{
	return *m_pWorld;
}

void 
CBFXParamPool::SetZDistanceFromCamera(float dist)
{
	m_ZDistance = dist;
}

float 
CBFXParamPool::GetZDistanceFromCamera()
{
	return m_ZDistance;
}

//void 
//CBFXParamPool::SetViewZFar(float zFar)
//{
//	m_ViewZFar = zFar;
//}
//	

const gmtl::Matrix44f& 
CBFXParamPool::GetWorldViewProjMatrix()
{	
	if(m_DirtyWorldViewProjMat)
	{
		// TODO: Deprecated for now
		_DEBUG_ASSERT(FALSE);

		const gmtl::Matrix44f& viewProjMat = m_pSharedParam->GetViewProjMatrix();
		gmtl::Matrix44f* pMat = m_WorldViewProjStack.Get();
		MatMatMult(pMat, &viewProjMat, m_pWorld);
		m_DirtyWorldViewProjMat = FALSE;
	}

	return *(m_WorldViewProjStack.GetLast());
}

const gmtl::Matrix44f& 
CBFXParamPool::GetViewProjMatrix()
{
	return m_pSharedParam->GetViewProjMatrix();
}

const gmtl::Matrix44f& CBFXParamPool::GetWorldViewMatrix()
{
	if(m_DirtyWorldViewMat)
	{
		gmtl::Matrix44f* pMat = m_WorldViewStack.Get();
		MatMatMult(pMat, &m_pSharedParam->GetViewMatrix(), m_pWorld);
		m_DirtyWorldViewMat = FALSE;
	}

	return *(m_WorldViewStack.GetLast());
}

void 
CBFXParamPool::SetHemiLight(const BFXHemiLight* pHemiLight)
{
	m_pHemiLight = pHemiLight;
}

const BFXHemiLight*
CBFXParamPool::UseHemiLight()
{
	if(m_pHemiLight && !m_IsHemiLightUsed)
	{
		m_IsHemiLightUsed = TRUE;
		return m_pHemiLight;
	}

	return NULL;
}

void 
CBFXParamPool::SetDirLight(const BFXDirLight* pDirLight)
{
	m_pDirLight = pDirLight;
}

const BFXDirLight*
CBFXParamPool::UseDirLight()
{
	if(m_pDirLight && !m_IsDirLightUsed)
	{
		m_IsDirLightUsed = TRUE;
		return m_pDirLight;
	}
	
	return NULL;
}

void 
CBFXParamPool::SetSpotLights(const BFXSpotLight** ppSpotLight, uint numSpotLights)
{
	_LOOPi(numSpotLights)
	{
		m_pSpotLights[i] = ppSpotLight[i];
	}

	m_TotalNumSpotLights = numSpotLights;
}

const BFXSpotLight*
CBFXParamPool::UseNextSpotLight()
{
	if(m_CurSpotLightStart < m_TotalNumSpotLights)
	{
		m_CurSpotLightStart++;
		return m_pSpotLights[m_CurSpotLightStart-1];
	}
	
	return NULL;
}

void 
CBFXParamPool::SetPointLights(const BFXPointLight** ppPointLight, uint numPointLights)
{
	_LOOPi(numPointLights)
	{
		m_pPointLights[i] = ppPointLight[i];
	}

	m_TotalNumPointLights = numPointLights;
}

const BFXPointLight*
CBFXParamPool::UseNextPointLight()
{
	if(m_CurPointLightStart < m_TotalNumPointLights)
	{
		m_CurPointLightStart++;
		return m_pPointLights[m_CurPointLightStart-1];
	}

	return NULL;
}

void 
CBFXParamPool::ResetLightsCount()
{
	m_IsHemiLightUsed = FALSE;
	m_IsDirLightUsed = FALSE;

	m_CurPointLightStart = 0;
	m_CurSpotLightStart = 0;
}

boolean 
CBFXParamPool::CheckHemiLightAvailable()
{
	return !m_IsHemiLightUsed;
}

boolean 
CBFXParamPool::CheckDirLightAvailable()
{
	return m_pDirLight && !m_IsDirLightUsed;
}

boolean 
CBFXParamPool::CheckPointLightsAvailable(uint numPointLights)
{
	int pointLightIndex = m_CurPointLightStart + numPointLights;
	if(pointLightIndex <= m_TotalNumPointLights)
		return TRUE;

	return FALSE;
}

boolean 
CBFXParamPool::CheckSpotLightsAvailable(uint numSpotLights)
{
	int spotLightIndex = m_CurSpotLightStart + numSpotLights;
	if(spotLightIndex <= m_TotalNumSpotLights)
		return TRUE;

	return FALSE;
}

void 
CBFXParamPool::GetParam(uint semantic, REffectParam* pToStore)
{
	switch(semantic)
	{
		case BFX_WORLD:
		{	
			pToStore->SetMatrix44(&(GetWorldMatrix()));
			break;
		}

		case BFX_VIEW:
		{
			pToStore->SetMatrix44(&(GetViewMatrix()));
			break;
		}

		case BFX_PROJ:
		{
			pToStore->SetMatrix44(&(GetProjMatrix()));
			break;
		}

		case BFX_VIEW_PROJ:
		{
			pToStore->SetMatrix44(&(GetViewProjMatrix()));
			break;
		}

		case BFX_WORLD_VIEW_PROJ:
		{
			pToStore->SetMatrix44(&(GetWorldViewProjMatrix()));
			break;
		}

		case BFX_DIR_LIGHT:
		{
			const BFXDirLight* pDirLight = UseDirLight();
			pToStore->SetValue((void*)pDirLight, sizeof(BFXDirLight));
			break;
		}

		case BFX_HEMI_LIGHT:
		{
			const BFXHemiLight* pHemiLight = UseHemiLight();
			pToStore->SetValue((void*)pHemiLight, sizeof(BFXHemiLight));
			break;
		}

		case BFX_POINT_LIGHT1:
		case BFX_POINT_LIGHT2:
		case BFX_POINT_LIGHT3:
		case BFX_POINT_LIGHT4:
		{
			const BFXPointLight* pLight = UseNextPointLight();
			_DEBUG_ASSERT(pLight);
			pToStore->SetValue((void*)pLight, sizeof(BFXPointLight));
			break;
		}

		case BFX_CAMERA_POS_WITH_VIEW_FAR:
		{
			pToStore->SetVector4( &(GetCameraPosWithViewFar()) );
			break;
		}		

		case BFX_WORLD_VIEW:
		{
			pToStore->SetMatrix44(&(GetWorldViewMatrix()));
			break;
		}

		case BFX_CAMERA_POS_WITH_VIEW_NEAR:
		{
			pToStore->SetVector4( &(GetCameraPosWithViewNear()) );
			break;
		}

		default:
		{
			_DEBUG_ASSERT(FALSE);
			break;
		}
	}	
}

boolean 
CBFXParamPool::IsParamAvailable(uint semantic)
{
	switch(semantic)
	{
		case BFX_DIR_LIGHT:
		{
			return CheckDirLightAvailable();
		}

		case BFX_HEMI_LIGHT:
		{
			return CheckHemiLightAvailable();
		}

		case BFX_POINT_LIGHT1:
		{
			return CheckPointLightsAvailable(1);
		}
		
		case BFX_POINT_LIGHT2:
		{
			return CheckPointLightsAvailable(2);
		}
		
		case BFX_POINT_LIGHT3:
		{
			return CheckPointLightsAvailable(3);
		}
		
		case BFX_POINT_LIGHT4:
		{
			return CheckPointLightsAvailable(4);
		}
	}	
	
	return TRUE;
}

boolean 
CBFXParamPool::IsMorePassesRequired()
{
	if(m_CurPointLightStart > 0 && m_CurPointLightStart < m_TotalNumPointLights)
		return TRUE;
	//if(m_CurSpotLightStart > 0 && m_CurSpotLightStart < m_TotalNumSpotLights)
	//	return TRUE;
		
	return FALSE;
}

void 
CBFXParamPool::FinishedResolvingEffect()
{
	ResetLightsCount();
}

void 
CBFXParamPool::ResetParams()
{
	if(m_WorldViewProjStack.GetNumAllocated() > 1)
	{
		gmtl::Matrix44f* pTemp = (m_WorldViewProjStack.GetLast());
		m_WorldViewProjStack.Reset();
		*(m_WorldViewProjStack.Get()) = *pTemp;
	}

	if(m_WorldViewStack.GetNumAllocated() > 1)
	{
		gmtl::Matrix44f* pTemp = m_WorldViewStack.GetLast();
		m_WorldViewStack.Reset();
		*(m_WorldViewStack.Get()) = *pTemp;
	}

	m_pSharedParam->ResetParams();
}

void CBFXParamPool::FinalizeSharedParams()
{
	m_pSharedParam->Finalize();
}

_NAMESPACE_END