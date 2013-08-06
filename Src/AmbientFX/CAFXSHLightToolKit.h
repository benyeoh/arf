//==============================================================================
//
//		CAFXSHToolKit.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		07/10/2009
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

typedef void (*AddCubeDataInDirEx)(float* pDest, float weight, const gmtl::VecA3f& dir, uint sampleIndex, eRCubeFace cubeFace, void* pUserInfo);

struct RadianceCosineFaceData
{
	float* cubeFaceData[6];
	gmtl::VecA3f normal;
	uint numBandsSq;
	uint numSamplesPerSide;
};

class CAFXSHLightToolKit : public IAFXSHLightToolKit
{

private:
	CAFXAmbientFX* m_pAmbientFX;

public:
	CAFXSHLightToolKit()
		: m_pAmbientFX(NULL)
	{
	}

	virtual ~CAFXSHLightToolKit()
	{
	}

private:
	//void	AddRadianceCosine(float* pDest, 
	//						float weight, 
	//						const gmtl::VecA3f& dir, 
	//						uint sampleIndex, 
	//						eRCubeFace cubeFace, 
	//						void* pUserInfo);
	//void	AddUnitSH(float* pDest, 
	//				float weight, 
	//				const gmtl::VecA3f& dir, 
	//				uint sampleIndex, 
	//				eRCubeFace cubeFace, 
	//				void* pUserInfo);

	float	ComputeDifferentialAngles(float* pToStore, uint numPixelsPerSide);

	void	ComputeMonteCarloCubeSamplingOneFace(const gmtl::VecA3f& initLookup,
												const gmtl::VecA3f& diffU,
												const gmtl::VecA3f& diffV,
												float* pDiffAngles,
												uint numPixelsPerSide,
												eRCubeFace cubeFace,
												float* pOut,
												AddCubeDataInDirEx addCubeDataFn,
												void* pUserInfo);

	void	ComputeMonteCarloCubeSampling(float* pDest,
										uint numChannels,
										uint numSamplesPerSide, 
										AddCubeDataInDirEx addCubeDataFn,
										void* pUserInfo);

	void	ComputeRadianceCosineLobeCubeOneFace(const gmtl::VecA3f& initLookup,
												const gmtl::VecA3f& diffU,
												const gmtl::VecA3f& diffV,
												uint numPixelsPerSide,
												uint channelStride,
												float* pOut,
												RadianceCosineFaceData& radianceData);

public:
	void	SetAmbientFX(CAFXAmbientFX* pAmbientFX) { m_pAmbientFX = pAmbientFX; }

	void	CreateUnitSHCoeffs(float* pDest, uint numSamplesPerSide, uint numBands);
	void	CreateRadianceCosineLobeSHCoeffs(IRTextureCube** ppDest, IRTextureCube* pSource, uint numBands);
	void	CreateCosineLobeCoeffs(float* pDest, uint numSamplesPerSide, uint numBands);
	void	CreateHemiLobeCoeffs(float* pDest, uint numSamplesPerSide, uint numBands);

	void	ConvertToZH(float* pZH,  gmtl::Vec3f* pLobeAxis, const float* pSH, uint numBands, void* pD3DDevice);
};

_NAMESPACE_END
