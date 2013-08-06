//==============================================================================
//
//		CSEFXParamPool.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		13/10/2009
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CSEFXParamPool : public ISEFXParamPool, public IBFXParamCallback
{
	struct VisibilitySphereGroup
	{
		Sphere* pSpheres;
		Sphere* pBoundingSphere;
		uint	numSpheres;
	};

private:
	ISEFXSHExpLightingFX*		m_pSHExpLightingFX;

	IRTextureCube*		m_pRadianceCoeff[SEFX_MAX_SH_BANDS * 3];
	float*				m_pUnitCoeff;
	uint				m_UnitCoeffSize;

	IRTexture2D*			m_pVisibilitySpheresCoeffTex;
	IRTextureCube*			m_pYlmTex[SEFX_MAX_SH_BANDS];
	IRTexture2D*			m_pABTableTex;

	IRTextureRT*			m_pOccl1;
	IRTextureRT*			m_pOccl2;
	IRTextureRT*			m_pOccl3;
	IRTextureRT*			m_pOccl4;

	IRTexture2DPtr			m_pDynamicSpheresTex;
	uint					m_NumSpheres;

public:
	CSEFXParamPool()
		: m_pSHExpLightingFX(NULL)
		, m_pUnitCoeff(NULL)
		, m_UnitCoeffSize(0)
		, m_pABTableTex(NULL)
		, m_pOccl1(NULL)
		, m_pOccl2(NULL)
		, m_pOccl3(NULL)
		, m_pOccl4(NULL)
	{
	}

	virtual ~CSEFXParamPool()
	{
	}

public:
	void Initialize();

	void SetSHExpLightingFX(ISEFXSHExpLightingFX* pSHExpLightingFX)		{ m_pSHExpLightingFX = pSHExpLightingFX; }

	void SetRadianceCoeff(IRTextureCube** ppCoeff, uint numCubeMapsPerColor);
	void SetUnitCoeff(float* pUnitCoeff, uint numBands);

	void SetVisibilitySpheresCoeffTex(IRTexture2D* pTex)	{ m_pVisibilitySpheresCoeffTex = pTex; }
	void SetABTableTex(IRTexture2D* pTex)					{ m_pABTableTex = pTex; }
	void SetYlmTextures(IRTextureCube** ppCoeff, uint numCubeMaps);

	IRTexture2D* GetDynamicSphereTex()						{ return m_pDynamicSpheresTex; }
	void SetNumSpheres(uint numSpheres)						{ m_NumSpheres = numSpheres; }

	void	GetParam(uint semantic, REffectParam* pToStore);
	boolean IsParamAvailable(uint semantic);
	boolean IsMorePassesRequired();
	void	FinishedResolvingEffect();
	void	ResetParams();
};

_NAMESPACE_END
