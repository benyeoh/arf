//==============================================================================
//
//		CAFXParamPool.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		08/09/2009
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CAFXParamPool : public CRefCounted<IAFXParamPool>
{
private:
	CAFXAmbientFX*		m_pAmbientFX;

	AFXAmbientCoeffs*	m_pAmbientSHCoeff;
	IRTextureCube*		m_pRadianceCoeff[AFX_MAX_SH_BANDS * 3];
	float*				m_pUnitCoeff;
	uint				m_UnitCoeffSize;

	float*				m_pLightZHCoeffs;
	uint				m_LightZHSize;

	AFXTransferMatrix*	m_pTransferMatR;
	AFXTransferMatrix*	m_pTransferMatG;
	AFXTransferMatrix*	m_pTransferMatB;
	
public:
	CAFXParamPool()
		: m_pAmbientFX(NULL)
		, m_pAmbientSHCoeff(NULL)
		, m_pUnitCoeff(NULL)
		, m_UnitCoeffSize(0)
		, m_pTransferMatR(NULL)
		, m_pTransferMatG(NULL)
		, m_pTransferMatB(NULL)
	{
	}

	virtual ~CAFXParamPool()
	{
	}

public:
	void SetAmbientFX(CAFXAmbientFX* pAmbientFX)		{ m_pAmbientFX = pAmbientFX; }

	void SetAmbientSHCoeff(AFXAmbientCoeffs* pSHCoeff)	{ m_pAmbientSHCoeff = pSHCoeff; }
	void SetRadianceCoeff(IRTextureCube** ppCoeff, uint numCubeMapsPerColor);
	void SetUnitCoeff(float* pUnitCoeff, uint numBands);
	void SetTransferMatrixRGB(AFXTransferMatrix* pR, AFXTransferMatrix* pG, AFXTransferMatrix* pB);

	void	GetParam(uint semantic, REffectParam* pToStore);
	boolean IsParamAvailable(uint semantic);
	boolean IsMorePassesRequired();
	void	FinishedResolvingEffect();
	void	ResetParams();
};

_NAMESPACE_END
