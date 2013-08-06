//==============================================================================
//
//		CSHFXParamPool.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		8/10/2008
//
//==============================================================================

#include "ShadowFXFramework.h"

_NAMESPACE_BEGIN

void 
//CSHFXParamPool::SetCascadedCasterIndex(uint cascadedIndex)
CSHFXParamPool::SetCascadedCasterFlag(uint cascadedFlag)
{
	//m_CascadedCasterIndex = cascadedIndex;
	m_CascadedCasterFlag = cascadedFlag;
	m_CurrentCasterFlag = m_CascadedCasterFlag;
}
	
uint
CSHFXParamPool::GetCurrentCascadedCasterIndex()
{
	return m_CurrentCasterIndex;
}

void 
CSHFXParamPool::SetCascadedDirLight(ISHFXCascadedDirLight* pDirLight)
{
	m_pCascadedDirLight = (CSHFXCascadedDirLight*) pDirLight;
}

void 
CSHFXParamPool::GetParam(uint semantic, REffectParam* pToStore)
{
	switch(semantic)
	{
		case SHFX_DIR_LIGHT_CASCADED_PROJS:
		{
			const SHFXCascadedProj* proj = UseCascadedProjWithTexOffset();
			pToStore->SetValue((void*) proj->cascadedViewProj, sizeof(gmtl::MatrixA44f) * 4);
			break;
		}
			
		case SHFX_DIR_LIGHT_Z_SPLITS_NEAR:
		{
			pToStore->SetValue((void*) m_pCascadedDirLight->GetSplitZ().splitNear, sizeof(float) * 4);
			break;
		}
		
		case SHFX_DIR_LIGHT_Z_SPLITS_FAR:
		{
			pToStore->SetValue((void*) m_pCascadedDirLight->GetSplitZ().splitFar, sizeof(float) * 4);
			break;
		}
						
		case SHFX_DIR_LIGHT_CASTER_PROJ:
		{
			// Use the correct proj for the caster
			while(!(m_CurrentCasterFlag & 0x1))
			{
				m_CurrentCasterFlag >>= 1;
				m_CurrentCasterIndex++;
				
				_DEBUG_ASSERT(m_CurrentCasterIndex <= 4);
			}
						
			gmtl::Matrix44f* pProj = 
				(gmtl::Matrix44f*) &m_pCascadedDirLight->GetProj().cascadedViewProj[m_CurrentCasterIndex];//m_CascadedCasterIndex];
			pToStore->SetMatrix44(pProj);

			// Increment to the next caster proj		
			m_CurrentCasterFlag >>= 1;
			m_CurrentCasterIndex++;
			break;
		}
				
		case SHFX_DIR_LIGHT_CASCADED_RT:
		{
			pToStore->SetTexture2D(m_pCascadedDirLight->GetShadowMap());
			break;
		}


		case SHFX_POINT_LIGHT_CAST_PARAMS:
		{
			SHFXShadowPLParams* pParams = UseNextCasterPointLightParam();
			pToStore->SetValue(pParams, sizeof(SHFXShadowPLParams));
			break;
		}

		case SHFX_POINT_LIGHT_SM_1:
		case SHFX_POINT_LIGHT_SM_2:
		case SHFX_POINT_LIGHT_SM_3:
		case SHFX_POINT_LIGHT_SM_4:
		{
			CSHFXPointLight* pPointLight = UseNextRecPointLight();
			pToStore->SetTexture2D(pPointLight->GetShadowMap());
			break;
		}
		
		case SHFX_CUBE_POINT_LIGHT_CAST_PARAMS:
		{
			SHFXCubeShadowPLParams* pParams = m_pCasterCubePL->GetParam();
			pToStore->SetValue(pParams, sizeof(SHFXCubeShadowPLParams));
			break;
		}

		case SHFX_CUBE_POINT_LIGHT_CAST_VIEW_PROJ:
		{
			gmtl::MatrixA44f* pViewProj = m_pCasterCubePL->GetViewProj(m_CasterCubeFace);
			pToStore->SetMatrix44((gmtl::Matrix44f*) pViewProj);
			break;
		}

		case SHFX_CUBE_POINT_LIGHT_SM_1:
		case SHFX_CUBE_POINT_LIGHT_SM_2:
		case SHFX_CUBE_POINT_LIGHT_SM_3:
		case SHFX_CUBE_POINT_LIGHT_SM_4:
		{
			CSHFXCubePointLight* pPointLight = UseNextRecCubePointLight();
			pToStore->SetTextureCube(pPointLight->GetShadowMap());
			break;
		}

		case SHFX_DIR_LIGHT_Z_SPLITS_VIEW_NEAR:
		{
			pToStore->SetValue((void*) m_pCascadedDirLight->GetSplitZView().splitNear, sizeof(float) * 4);
			break;
		}

		case SHFX_DIR_LIGHT_Z_SPLITS_VIEW_FAR:
		{
			pToStore->SetValue((void*) m_pCascadedDirLight->GetSplitZView().splitFar, sizeof(float) * 4);
			break;
		}

		case SHFX_DIR_LIGHT_TEX_OFFSETS:
		{
			pToStore->SetVector2(m_pCascadedDirLight->GetTexOffsets());
			break;
		}

		default:	_DEBUG_ASSERT(FALSE); break;
	}
}

//IRTexture2DRT*
//CSHFXParamPool::UseCascadedDirLightRT()
//{
//	if(!m_IsCascadedDirLightUsed)
//	{
//		m_IsCascadedDirLightUsed = TRUE;
//		return m_pCascadedDirLight->GetShadowMap();
//	}
//	
//	return NULL;
//}

const SHFXCascadedProj*
CSHFXParamPool::UseCascadedProjWithTexOffset()
{
	if(!m_IsCascadedDirLightUsed)
	{
		m_IsCascadedDirLightUsed = TRUE;
		return &(m_pCascadedDirLight->GetProjWithTexOffset());
	}
	
	_DEBUG_ASSERT(FALSE);
	return NULL;
}

void	
CSHFXParamPool::SetPointLights(ISHFXPointLight** ppPointLight, uint* pCasterFlag, uint numPointLights)
{
	_LOOPi(numPointLights)
	{
		m_pPointLights[i] = (CSHFXPointLight*) ppPointLight[i];
		m_PLCasterFlag[i] = pCasterFlag[i];
	}

	m_NumPointLights = numPointLights;
}

SHFXShadowPLParams*
CSHFXParamPool::UseNextCasterPointLightParam()
{
	m_CurCasterPLIndex++;
	
	uint curCasterIndex = m_CurCasterPLIndex - 1; 
	uint plIndex = curCasterIndex >> 1;
	uint faceIndex = curCasterIndex & 0x1;
	uint casterFlag = m_PLCasterFlag[plIndex] >> faceIndex;
	while(!(casterFlag & 0x1))
	{
		m_CurCasterPLIndex++;

		curCasterIndex = m_CurCasterPLIndex - 1;
		plIndex = curCasterIndex >> 1;
		faceIndex = curCasterIndex & 0x1;
		casterFlag = m_PLCasterFlag[plIndex] >> faceIndex;
	}

	SHFXShadowPLParams* pToReturn = m_pPointLights[plIndex]->GetFaceParam(faceIndex);

	return pToReturn;
}

CSHFXPointLight* 
CSHFXParamPool::UseNextRecPointLight()
{
	if(m_CurRecPLIndex < m_NumPointLights)
	{
		m_CurRecPLIndex++;
		return m_pPointLights[m_CurRecPLIndex-1];
	}

	_DEBUG_ASSERT(FALSE);
	return NULL;
}

uint	
CSHFXParamPool::GetCurrentPLCasterFace()
{
	return (m_CurCasterPLIndex - 1) & 0x1;
}

CSHFXPointLight*
CSHFXParamPool::GetCurrentPLCaster()
{
	return m_pPointLights[((m_CurCasterPLIndex-1) >> 1)];	
}

ISHFXCascadedDirLight* 
CSHFXParamPool::GetCascadedDirLight()
{
	return m_pCascadedDirLight;
}

	
boolean 
CSHFXParamPool::CheckCascadedDirLightAvail()
{
	return m_pCascadedDirLight && !m_IsCascadedDirLightUsed;
}

void CSHFXParamPool::SetCasterCubePointLight(ISHFXCubePointLight* pCaster, eRCubeFace face)
{
	m_pCasterCubePL = (CSHFXCubePointLight*) pCaster;
	m_CasterCubeFace = face;
}

void CSHFXParamPool::SetCubePointLights(ISHFXCubePointLight** ppPointLight, uint numPointLights)
{
	_LOOPi(numPointLights)
	{
		m_pCubePointLights[i] = (CSHFXCubePointLight*) ppPointLight[i];
	}

	m_NumCubePointLights = numPointLights;
}

CSHFXCubePointLight* CSHFXParamPool::UseNextRecCubePointLight()
{
	if(m_CurRecCubePLIndex < m_NumCubePointLights)
	{
		m_CurRecCubePLIndex ++;
		return m_pCubePointLights[m_CurRecCubePLIndex-1];
	}

	_DEBUG_ASSERT(FALSE);
	return NULL;
}

CSHFXCubePointLight* CSHFXParamPool::GetCubePLCaster()
{
	return m_pCasterCubePL;
}

eRCubeFace  CSHFXParamPool::GetCubePLCasterFace()
{
	return m_CasterCubeFace;
}


boolean 
CSHFXParamPool::IsParamAvailable(uint semantic)
{
	switch(semantic)
	{
		case SHFX_DIR_LIGHT_CASCADED_PROJS:
		{
			return CheckCascadedDirLightAvail();
		}

		case SHFX_POINT_LIGHT_SM_1:
		case SHFX_POINT_LIGHT_SM_2:		
		case SHFX_POINT_LIGHT_SM_3:
		case SHFX_POINT_LIGHT_SM_4:
		{
			return (m_CurRecPLIndex + (semantic - SHFX_POINT_LIGHT_SM_1)) < m_NumPointLights;
		}

		case SHFX_CUBE_POINT_LIGHT_SM_1:
		case SHFX_CUBE_POINT_LIGHT_SM_2:		
		case SHFX_CUBE_POINT_LIGHT_SM_3:
		case SHFX_CUBE_POINT_LIGHT_SM_4:
		{
			return (m_CurRecCubePLIndex + (semantic - SHFX_CUBE_POINT_LIGHT_SM_1)) < m_NumCubePointLights;
		}
	}	
	
	return TRUE;
}

boolean 
CSHFXParamPool::IsMorePassesRequired()
{
	boolean isCascadedCasterDirPassRequired = (m_CurrentCasterIndex > 0) && (m_CurrentCasterFlag > 0);
	uint plIndex = (m_CurCasterPLIndex >> 1);
	boolean isCasterPLPassRequired = (m_CurCasterPLIndex > 0) && 
									(plIndex < m_NumPointLights) &&
									((m_PLCasterFlag[plIndex] >> (m_CurCasterPLIndex & 0x1)) > 0);

	return isCascadedCasterDirPassRequired || isCasterPLPassRequired;
	
	//if(m_CurPointLightStart > 0 && m_CurPointLightStart < m_TotalNumPointLights)
	//	return TRUE;
	//if(m_CurSpotLightStart > 0 && m_CurSpotLightStart < m_TotalNumSpotLights)
	//	return TRUE;
		
	//return FALSE;
}

void 
CSHFXParamPool::FinishedResolvingEffect()
{
	//ResetLightsCount();
	m_IsCascadedDirLightUsed = FALSE;
	m_CurrentCasterFlag = m_CascadedCasterFlag;
	m_CurrentCasterIndex = 0;

	m_CurCasterPLIndex = 0;
	m_CurRecPLIndex = 0;

	m_CurRecCubePLIndex = 0;
}

void 
CSHFXParamPool::ResetParams()
{
	//m_pViewProjStack.clear();
	//m_pWorldViewProjStack.clear();
	//ClearMat44f();
	//m_CascadedCasterFlag = 0;
}							
_NAMESPACE_END