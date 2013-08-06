//==============================================================================
//
//		CAFXParamPool.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/8/2009
//
//==============================================================================

#include "AmbientFXFramework.h"

_NAMESPACE_BEGIN

void 
CAFXParamPool::SetRadianceCoeff(IRTextureCube** ppCoeff, uint numCubeMapsPerColor)	
{ 
	_LOOPi(numCubeMapsPerColor)
	{
		m_pRadianceCoeff[i]		= ppCoeff[i];
		m_pRadianceCoeff[i+4]	= ppCoeff[i+numCubeMapsPerColor];
		m_pRadianceCoeff[i+4*2]	= ppCoeff[i+numCubeMapsPerColor*2];
	}
}

void 
CAFXParamPool::SetUnitCoeff(float* pUnitCoeff, uint numBands)
{ 
	m_pUnitCoeff		= pUnitCoeff; 
	m_UnitCoeffSize		= numBands * numBands;
}

void CAFXParamPool::SetTransferMatrixRGB(AFXTransferMatrix* pR, AFXTransferMatrix* pG, AFXTransferMatrix* pB)
{
	m_pTransferMatR = pR;
	m_pTransferMatG = pG;
	m_pTransferMatB = pB;
}


//void 
//CAFXParamPool::SetLightZHCoeff(float* pLightZH, uint numBands)
//{
//	m_pLightZHCoeffs = pLightZH;
//	m_LightZHSize = numBands * numBands;
//}

void	
CAFXParamPool::GetParam(uint semantic, REffectParam* pToStore)
{
	switch(semantic)
	{
		case AFX_AMBIENT_SH_COEFFS:
		{
			pToStore->SetValue((void*) m_pAmbientSHCoeff, 27 * sizeof(float));
			break;
		}

		case AFX_TEX_R1:
		{
			pToStore->SetTextureCube(m_pRadianceCoeff[0]);
			break;
		}

		case AFX_TEX_R2:
		{
			pToStore->SetTextureCube(m_pRadianceCoeff[1]);
			break;
		}

		case AFX_TEX_R3:
		{
			pToStore->SetTextureCube(m_pRadianceCoeff[2]);
			break;
		}

		case AFX_TEX_R4:
		{
			pToStore->SetTextureCube(m_pRadianceCoeff[3]);
			break;
		}

		case AFX_TEX_G1:
		{
			pToStore->SetTextureCube(m_pRadianceCoeff[4]);
			break;
		}
	
		case AFX_TEX_G2:
		{
			pToStore->SetTextureCube(m_pRadianceCoeff[5]);
			break;
		}

		case AFX_TEX_G3:
		{
			pToStore->SetTextureCube(m_pRadianceCoeff[6]);
			break;
		}

		case AFX_TEX_G4:
		{
			pToStore->SetTextureCube(m_pRadianceCoeff[7]);
			break;
		}

		case AFX_TEX_B1:
		{
			pToStore->SetTextureCube(m_pRadianceCoeff[8]);
			break;
		}

		case AFX_TEX_B2:
		{
			pToStore->SetTextureCube(m_pRadianceCoeff[9]);
			break;
		}
		
		case AFX_TEX_B3:
		{
			pToStore->SetTextureCube(m_pRadianceCoeff[10]);
			break;
		}

		case AFX_TEX_B4:
		{
			pToStore->SetTextureCube(m_pRadianceCoeff[11]);
			break;
		}

		case AFX_UNIT_COEFFS:
		{
			pToStore->SetValue(m_pUnitCoeff, sizeof(float) * m_UnitCoeffSize);
			break;
		}

		case AFX_TRANSFER_MATRIX_R:
		{
			pToStore->SetValue(m_pTransferMatR, sizeof(float) * 9 * 9);
			break;
		}

		case AFX_TRANSFER_MATRIX_G:
		{
			pToStore->SetValue(m_pTransferMatG, sizeof(float) * 9 * 9);
			break;
		}

		case AFX_TRANSFER_MATRIX_B:
		{
			pToStore->SetValue(m_pTransferMatB, sizeof(float) * 9 * 9);
			break;
		}

		default:	_DEBUG_ASSERT(FALSE); break;
	}
}

boolean 
CAFXParamPool::IsParamAvailable(uint semantic)
{
	return TRUE;
}

boolean 
CAFXParamPool::IsMorePassesRequired()
{
	return FALSE;
}

void	
CAFXParamPool::FinishedResolvingEffect()
{

}

void	
CAFXParamPool::ResetParams()
{

}

_NAMESPACE_END
