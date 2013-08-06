//==============================================================================
//
//		CAFXResourceMgr.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/7/2009
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CAFXResourceMgr : public IAFXResourceMgr
{
private:
	CAFXAmbientFX* m_pAmbientFX;

public:
	CAFXResourceMgr()
		: m_pAmbientFX(NULL)
	{
	}

	virtual ~CAFXResourceMgr()
	{
	}

private:
	float	ComputeDifferentialAngles(float* pToStore, uint numPixelsPerSide);

	float	GetAzimuthalCoeff(int l);
	void	CompressOneFace(const gmtl::VecA3f& initLookup,
							const gmtl::VecA3f& diffU,
							const gmtl::VecA3f& diffV,
							uint* pARGB,
							float* pDiffAngles,
							uint numPixelsPerSide, 
							int l, 
							int m,
							float toneMapContrast,
							gmtl::VecA3f& coeffOut);

	void	CompressOneFaceAllBands(int numSHBands, 
									const gmtl::VecA3f& initLookup,
									const gmtl::VecA3f& diffU,
									const gmtl::VecA3f& diffV,
									uint* pARGB,
									uint numPixelsPerSide,
									float* pDiffAngles,
									float toneMapContrast,
									gmtl::Vec3f* pRGBOut);

	void	CompressCubeMap(IRTextureCube* pTexCube, int numSHBands, float toneMapContrast, gmtl::Vec3f* pRGBOut);

public:
	void	SetAmbientFX(CAFXAmbientFX* pAmbientFX) { m_pAmbientFX = pAmbientFX; }

	IAFXParamPool*	CreateParamPool();
	void	CompressAmbientCubeMap(IRTextureCube* pTexCube, AFXAmbientCoeffs* pRGBOut, float freqContrast, float brightenFactor);
};

_NAMESPACE_END
