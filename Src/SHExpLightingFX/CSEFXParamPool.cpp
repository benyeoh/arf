//==============================================================================
//
//		CSEFXParamPool.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		13/10/2009
//
//==============================================================================

#include "SHExpLightingFXFramework.h"

_NAMESPACE_BEGIN

void
CSEFXParamPool::Initialize()
{
	m_pDynamicSpheresTex = 
		((CSEFXSHExpLightingFX*)m_pSHExpLightingFX)->GetRenderer().GetRResourceMgr().CreateTexture2D(
			NULL, 128, 1, 1, TEXF_A32B32G32R32F, TEXU_DYNAMIC);
}

void 
CSEFXParamPool::SetRadianceCoeff(IRTextureCube** ppCoeff, uint numCubeMapsPerColor)	
{ 
	_LOOPi(numCubeMapsPerColor)
	{
		m_pRadianceCoeff[i]		= ppCoeff[i];
		m_pRadianceCoeff[i+4]	= ppCoeff[i+numCubeMapsPerColor];
		m_pRadianceCoeff[i+4*2]	= ppCoeff[i+numCubeMapsPerColor*2];
	}
}

void 
CSEFXParamPool::SetUnitCoeff(float* pUnitCoeff, uint numBands)
{ 
	m_pUnitCoeff		= pUnitCoeff; 
	m_UnitCoeffSize		= numBands * numBands;
}

void 
CSEFXParamPool::SetYlmTextures(IRTextureCube** ppCoeff, uint numCubeMaps)
{
	_LOOPi(numCubeMaps)
	{
		m_pYlmTex[i] = ppCoeff[i];
	}
}

void	
CSEFXParamPool::GetParam(uint semantic, REffectParam* pToStore)
{
	switch(semantic)
	{
	case SEFX_TEX_R1:
		{
			pToStore->SetTextureCube(m_pRadianceCoeff[0]);
			break;
		}

	case SEFX_TEX_R2:
		{
			pToStore->SetTextureCube(m_pRadianceCoeff[1]);
			break;
		}

	case SEFX_TEX_R3:
		{
			pToStore->SetTextureCube(m_pRadianceCoeff[2]);
			break;
		}

	case SEFX_TEX_R4:
		{
			pToStore->SetTextureCube(m_pRadianceCoeff[3]);
			break;
		}

	case SEFX_TEX_G1:
		{
			pToStore->SetTextureCube(m_pRadianceCoeff[4]);
			break;
		}

	case SEFX_TEX_G2:
		{
			pToStore->SetTextureCube(m_pRadianceCoeff[5]);
			break;
		}

	case SEFX_TEX_G3:
		{
			pToStore->SetTextureCube(m_pRadianceCoeff[6]);
			break;
		}

	case SEFX_TEX_G4:
		{
			pToStore->SetTextureCube(m_pRadianceCoeff[7]);
			break;
		}

	case SEFX_TEX_B1:
		{
			pToStore->SetTextureCube(m_pRadianceCoeff[8]);
			break;
		}

	case SEFX_TEX_B2:
		{
			pToStore->SetTextureCube(m_pRadianceCoeff[9]);
			break;
		}

	case SEFX_TEX_B3:
		{
			pToStore->SetTextureCube(m_pRadianceCoeff[10]);
			break;
		}

	case SEFX_TEX_B4:
		{
			pToStore->SetTextureCube(m_pRadianceCoeff[11]);
			break;
		}

	case SEFX_UNIT_COEFFS:
		{
			pToStore->SetValue(m_pUnitCoeff, sizeof(float) * m_UnitCoeffSize);
			break;
		}

	case SEFX_VIS_SPHERES_64:
		{
			//pToStore->SetValue(m_SphereGroups[m_CurGroupOffset].pSpheres, sizeof(Sphere) * 64);
			break;
		}

	case SEFX_VIS_SPHERES_32:
		{
			//pToStore->SetValue(m_SphereGroups[m_CurGroupOffset].pSpheres, sizeof(Sphere) * 32);
			break;
		}

	case SEFX_VIS_SPHERES_128:
		{
			//pToStore->SetValue(m_SphereGroups[m_CurGroupOffset].pSpheres, sizeof(Sphere) * 128);
			break;
		}
		
	case SEFX_TEX_VIS_SPHERES:
		{
			pToStore->SetTexture2D(m_pVisibilitySpheresCoeffTex);
			break;
		}
		
	case SEFX_TEX_YLM_1:
		{
			pToStore->SetTextureCube(m_pYlmTex[0]);
			break;
		}

	case SEFX_TEX_YLM_2:
		{
			pToStore->SetTextureCube(m_pYlmTex[1]);
			break;
		}

	case SEFX_TEX_YLM_3:
		{
			pToStore->SetTextureCube(m_pYlmTex[2]);
			break;
		}
		
	case SEFX_TEX_YLM_4:
		{
			pToStore->SetTextureCube(m_pYlmTex[3]);
			break;
		}

	case SEFX_TEX_AB_TABLE:
		{
			pToStore->SetTexture2D(m_pABTableTex);
			break;
		}

	case SEFX_TEX_VIS_SPHERES_ATTR:
		{
			pToStore->SetTexture2D(m_pDynamicSpheresTex);
			break;
		}

	case SEFX_NUM_VIS_SPHERES:
		{
			pToStore->SetInt(m_NumSpheres);
			break;
		}

	default:	_DEBUG_ASSERT(FALSE); break;
	}
}

boolean 
CSEFXParamPool::IsParamAvailable(uint semantic)
{
	return TRUE;
}

boolean 
CSEFXParamPool::IsMorePassesRequired()
{
	return FALSE;
}

void	
CSEFXParamPool::FinishedResolvingEffect()
{

}

void	
CSEFXParamPool::ResetParams()
{
}

_NAMESPACE_END

