//==============================================================================
//
//		CSEFXResourceMgr.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		13/10/2009
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

struct VisData
{
	float cosTheta;
	uint numBands;
};

class CSEFXResourceMgr : public ISEFXResourceMgr
{
private:
	CSEFXSHExpLightingFX* m_pSHExpLightingFX;

public:
	CSEFXResourceMgr()
		: m_pSHExpLightingFX(NULL)
	{
	}

	virtual ~CSEFXResourceMgr()
	{
	}

private:
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

	void	ComputeYlmCubeOneFace(const gmtl::VecA3f& initLookup,
		const gmtl::VecA3f& diffU,
		const gmtl::VecA3f& diffV,
		uint numPixelsPerSide,
		uint channelStride,
		uint numBandsSq,
		float* pOut);

	void	EigenDecomposeTransferMat3(Matrix99f& src, Matrix99f& rot, Matrix99f& diag);
	void	EigenDecomposeTransferMat4(Matrix1616f& src, Matrix1616f& rot, Matrix1616f& diag);
	void	InvertExpEigenValues3(Matrix99f& res, Matrix99f& diag);
	void	InvertExpEigenValues4(Matrix1616f& res, Matrix1616f& diag);

	void	InvertExpSHCoeff3(float* pTripleProduct, float* pZHSrc, float* pZHDest);
	void	InvertExpSHCoeff4(float* pTripleProduct, float* pZHSrc, float* pZHDest);
	
	float	ComputeOptLinearApprox(float& a, float& b, float* pZH, float* pZHLog, uint numBands);

public:
	void	SetSHExpLightingFX(ISEFXSHExpLightingFX* pSHExpLightingFX);

	void	CreateUnitSHCoeffs(float* pDest, uint numSamplesPerSide, uint numBands);
	void	CreateRadianceCosineLobeSHCoeffs(IRTextureCube** ppDest, IRTextureCube* pSource, uint numBands);
	void	CreateCosineLobeCoeffs(float* pDest, uint numSamplesPerSide, uint numBands);
	void	CreateYlmCoeffs(IRTextureCube** ppDest, uint numBands);
	void	CreateSphereVisibilityCoeffs(IRTexture2D* pDest, IRTexture2D* pABTableTex, uint numBands);

	void	ConvertToZH(float* pZH,  gmtl::Vec3f* pLobeAxis, const float* pSH, uint numBands, void* pD3DDevice);

};

_NAMESPACE_END
