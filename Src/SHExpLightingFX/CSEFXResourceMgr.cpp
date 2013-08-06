//==============================================================================
//
//		CSEFXResourceMgr.cpp
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
CSEFXResourceMgr::SetSHExpLightingFX(ISEFXSHExpLightingFX* pSHExpLightingFX)
{ 
	m_pSHExpLightingFX = (CSEFXSHExpLightingFX*) pSHExpLightingFX; 
}

float	
CSEFXResourceMgr::ComputeDifferentialAngles(float* pToStore, uint numPixelsPerSide)
{
	float halfPixelOffset	= 1.0f / (numPixelsPerSide);
	float fullPixelOffset	= halfPixelOffset * 2.0f;

	gmtl::VecA3f initLookup;
	initLookup[0]	= -1.0f + halfPixelOffset;
	initLookup[1]	= -1.0f + halfPixelOffset;
	initLookup[2]	= 1.0f;

	gmtl::VecA3f lookup = initLookup;

	float weightSum = 0.0f;

	_LOOPi(numPixelsPerSide)
	{
		_LOOPj(numPixelsPerSide)
		{
			float radius;
			VecLength(&radius, &lookup);

			// Dot product of lookup vector and face normal
			float cosTheta = 1.0f / radius;	

			// dA = 1 unit pixel weighted by the cosine of the lookup/face normal
			// -> dOmega = dA / r^2
			// We will normalize this "unit pixel" by the total weights summed later
			float dOmega = cosTheta / (radius * radius);

			pToStore[i * numPixelsPerSide + j] = dOmega;
			weightSum += dOmega;

			lookup[0] = lookup[0] + fullPixelOffset;
		}

		lookup[0] = initLookup[0];
		lookup[1] = lookup[1] + fullPixelOffset;
	}

	return weightSum;
}

void
CSEFXResourceMgr::ComputeMonteCarloCubeSamplingOneFace(const gmtl::VecA3f& initLookup,
													   const gmtl::VecA3f& diffU,
													   const gmtl::VecA3f& diffV,
													   float* pDiffAngles,
													   uint numPixelsPerSide,
													   eRCubeFace cubeFace,
													   float* pOut,
													   AddCubeDataInDirEx addCubeDataFn,
													   void* pUserInfo)
{
	gmtl::VecA3f lookupVec;
	gmtl::VecA3f diffVTotal;
	diffVTotal[0] = 0.0f;
	diffVTotal[1] = 0.0f;
	diffVTotal[2] = 0.0f;

	lookupVec[0] = initLookup[0];
	lookupVec[1] = initLookup[1];
	lookupVec[2] = initLookup[2];

	_LOOPi(numPixelsPerSide)
	{
		_LOOPj(numPixelsPerSide)
		{
			// Project to SH
			gmtl::VecA3f	normalizedLookup;
			NormalizeVec(&normalizedLookup, &lookupVec);

			uint index	= i * numPixelsPerSide + j;
			addCubeDataFn(pOut, pDiffAngles[index], normalizedLookup, index, cubeFace, pUserInfo);

			// Increment lookup vector in U direction
			VecVecAdd(&_CAST_VECA4(lookupVec), &_CAST_VECA4(lookupVec), &_CAST_VECA4(diffU));;
		}

		// Increment lookup vector in V direction
		VecVecAdd(&_CAST_VECA4(diffVTotal), &_CAST_VECA4(diffVTotal), &_CAST_VECA4(diffV));
		VecVecAdd(&_CAST_VECA4(lookupVec), &_CAST_VECA4(initLookup), &_CAST_VECA4(diffVTotal));
	}
}

void	
CSEFXResourceMgr::ComputeMonteCarloCubeSampling(float* pDest,
												uint numChannels,
												uint numSamplesPerSide, 
												AddCubeDataInDirEx addCubeDataFn,
												void* pUserInfo)
{
	uint numPixelsPerSide	= numSamplesPerSide;
	float halfPixelOffset	= 1.0f / (numPixelsPerSide);
	float fullPixelOffset	= halfPixelOffset * 2.0f;

	// Clear the buffer
	memset(pDest, 0, sizeof(float) * numChannels);

	// Compute differential solid angles
	float* pDiffAngles = (float*) _MALLOC( sizeof(float) * numPixelsPerSide * numPixelsPerSide );
	float weightPerFace = ComputeDifferentialAngles(pDiffAngles, numPixelsPerSide);

	// +X
	gmtl::VecA3f initLookup;
	initLookup[0]	= 1.0f;
	initLookup[1]	= 1.0f - halfPixelOffset;
	initLookup[2]	= -1.0f + halfPixelOffset;
	gmtl::VecA3f diffU;
	diffU[0]		= 0.0f;
	diffU[1]		= 0.0f;
	diffU[2]		= fullPixelOffset;
	gmtl::VecA3f diffV;
	diffV[0]		= 0.0f;
	diffV[1]		= -fullPixelOffset;
	diffV[2]		= 0.0f;

	gmtl::Vec4i toWrite;
	toWrite[0] = toWrite[1] = toWrite[2] = toWrite[3] = 0;
	ComputeMonteCarloCubeSamplingOneFace(initLookup, diffU, diffV, 
		pDiffAngles, numPixelsPerSide, CF_POS_X, pDest, addCubeDataFn, pUserInfo);

	// -X
	initLookup[0]	= -1.0f;
	initLookup[1]	= 1.0f - halfPixelOffset;
	initLookup[2]	= 1.0f - halfPixelOffset;
	diffU[0]		= 0.0f;
	diffU[1]		= 0.0f;
	diffU[2]		= -fullPixelOffset;
	diffV[0]		= 0.0f;
	diffV[1]		= -fullPixelOffset;
	diffV[2]		= 0.0f;

	ComputeMonteCarloCubeSamplingOneFace(initLookup, diffU, diffV, 
		pDiffAngles, numPixelsPerSide, CF_NEG_X, pDest, addCubeDataFn, pUserInfo);

	// +Y
	initLookup[0]	= -1.0f + halfPixelOffset;;
	initLookup[1]	= 1.0f;
	initLookup[2]	= 1.0f - halfPixelOffset;
	diffU[0]		= fullPixelOffset;
	diffU[1]		= 0.0f;
	diffU[2]		= 0.0f;
	diffV[0]		= 0.0f;
	diffV[1]		= 0.0f;
	diffV[2]		= -fullPixelOffset;

	ComputeMonteCarloCubeSamplingOneFace(initLookup, diffU, diffV,
		pDiffAngles, numPixelsPerSide, CF_POS_Y, pDest, addCubeDataFn, pUserInfo);

	// -Y
	initLookup[0]	= -1.0f + halfPixelOffset;;
	initLookup[1]	= -1.0f;
	initLookup[2]	= -1.0f + halfPixelOffset;
	diffU[0]		= fullPixelOffset;
	diffU[1]		= 0.0f;
	diffU[2]		= 0.0f;
	diffV[0]		= 0.0f;
	diffV[1]		= 0.0f;
	diffV[2]		= fullPixelOffset;

	ComputeMonteCarloCubeSamplingOneFace(initLookup, diffU, diffV,
		pDiffAngles, numPixelsPerSide, CF_NEG_Y, pDest, addCubeDataFn, pUserInfo);

	// +Z
	initLookup[0]	= 1.0f - halfPixelOffset;;
	initLookup[1]	= 1.0f - halfPixelOffset;
	initLookup[2]	= 1.0f;
	diffU[0]		= -fullPixelOffset;
	diffU[1]		= 0.0f;
	diffU[2]		= 0.0f;
	diffV[0]		= 0.0f;
	diffV[1]		= -fullPixelOffset;
	diffV[2]		= 0.0f;

	ComputeMonteCarloCubeSamplingOneFace(initLookup, diffU, diffV,
		pDiffAngles, numPixelsPerSide, CF_POS_Z, pDest, addCubeDataFn, pUserInfo);

	// -Z
	initLookup[0]	= -1.0f + halfPixelOffset;;
	initLookup[1]	= 1.0f - halfPixelOffset;
	initLookup[2]	= -1.0f;
	diffU[0]		= fullPixelOffset;
	diffU[1]		= 0.0f;
	diffU[2]		= 0.0f;
	diffV[0]		= 0.0f;
	diffV[1]		= -fullPixelOffset;
	diffV[2]		= 0.0f;

	ComputeMonteCarloCubeSamplingOneFace(initLookup, diffU, diffV,
		pDiffAngles, numPixelsPerSide, CF_NEG_Z, pDest, addCubeDataFn, pUserInfo);

	// Weight constants
	float weight = (4.0f * gmtl::Math::PI) / (weightPerFace * 6);
	_LOOPi(numChannels)
	{
		pDest[i] *= weight;
	}

	_FREE(pDiffAngles);
}

void
AddUnitSH(float* pDest, float weight, const gmtl::VecA3f& dir, uint sampleIndex, eRCubeFace cubeFace, void* pUserInfo)
{
	uint numBandsSq = (uint)(size_t) pUserInfo;

	_LOOPi(numBandsSq)
	{
		pDest[i] += ( EvaluateSHFastCartesian(i, _CAST_VEC3(dir)) * weight );
	}
}

void
AddCosineLobeSH(float* pDest, float weight, const gmtl::VecA3f& dir, uint sampleIndex, eRCubeFace cubeFace, void* pUserInfo)
{
	uint numBandsSq = (uint)(size_t) pUserInfo;
	gmtl::VecA3f up;
	up[0] = 0.0f;
	up[1] = 1.0f;
	up[2] = 0.0f;

	float nDotL;
	VecVecDot(&nDotL, &dir, &up);
	nDotL = nDotL < 0.0f ? 0.0f : (nDotL > 1.0f ? 1.0f : nDotL);
	weight *= nDotL;

	_LOOPi(numBandsSq)
	{
		pDest[i] += ( EvaluateSHFastCartesian(i, _CAST_VEC3(dir)) * weight);
	}
}

void
AddRadianceCosine(float* pDest, float weight, const gmtl::VecA3f& dir, uint sampleIndex, eRCubeFace cubeFace, void* pUserInfo)
{
	// This should be 1.0 / PI, but we brighten the diffuse term since we actually need HDR to be realistic
	const static float DIFFUSE_TERM_WEIGHT	= 1.0f / gmtl::Math::PI;
	const static float BRIGHTEN_TERM		= 1.8f;
	
	RadianceCosineFaceData* pCubeFaceData = (RadianceCosineFaceData*) pUserInfo;

	float nDotL;
	VecVecDot(&nDotL, &dir, &pCubeFaceData->normal);
	nDotL = nDotL < 0.0f ? 0.0f : (nDotL > 1.0f ? 1.0f : nDotL);
	weight *= nDotL;

	gmtl::VecA3f color;
	//color[0] = 1.0f - gmtl::Math::exp(-pCubeFaceData->cubeFaceData[cubeFace][sampleIndex*3]*0.7f);
	//color[1] = 1.0f - gmtl::Math::exp(-pCubeFaceData->cubeFaceData[cubeFace][sampleIndex*3+1]*0.7f);
	//color[2] = 1.0f - gmtl::Math::exp(-pCubeFaceData->cubeFaceData[cubeFace][sampleIndex*3+2]*0.7f);

	color[0] = (pCubeFaceData->cubeFaceData[cubeFace][sampleIndex*3] * DIFFUSE_TERM_WEIGHT * BRIGHTEN_TERM);
	color[1] = (pCubeFaceData->cubeFaceData[cubeFace][sampleIndex*3+1] * DIFFUSE_TERM_WEIGHT * BRIGHTEN_TERM);
	color[2] = (pCubeFaceData->cubeFaceData[cubeFace][sampleIndex*3+2] * DIFFUSE_TERM_WEIGHT * BRIGHTEN_TERM);

	_LOOPi(pCubeFaceData->numBandsSq)
	{
		float sh = ( EvaluateSHFastCartesian(i, _CAST_VEC3(dir)) * weight );

		gmtl::VecA3f colorSH;
		VecScalarMult(&_CAST_VECA4(colorSH), &_CAST_VECA4(color), sh);			
		pDest[i*3]		+= colorSH[0];
		pDest[i*3+1]	+= colorSH[1];
		pDest[i*3+2]	+= colorSH[2];
	}
}

void
AddSphereVisibility(float* pDest, float weight, const gmtl::VecA3f& dir, uint sampleIndex, eRCubeFace cubeFace, void* pUserInfo)
{
	VisData* pVisData = (VisData*) pUserInfo;
	gmtl::VecA3f up;
	up[0] = 0.0f;
	up[1] = 1.0f;
	up[2] = 0.0f;

	float vis		= 1.0f;

	float nDotL;
	VecVecDot(&nDotL, &dir, &up);
	if(nDotL > pVisData->cosTheta)
		vis = 0.0f;

	weight *= vis;

	_LOOPi(pVisData->numBands)
	{
		uint lm = i*(i+1);
		pDest[i] += ( EvaluateSHFastCartesian(lm, _CAST_VEC3(dir)) * weight );
	}
}

void	
CSEFXResourceMgr::CreateUnitSHCoeffs(float* pDest, uint numSamplesPerSide, uint numBands)
{
	ComputeMonteCarloCubeSampling(pDest, numBands * numBands, numSamplesPerSide, AddUnitSH, (void*) (size_t) (numBands * numBands));
}

void	
CSEFXResourceMgr::CreateCosineLobeCoeffs(float* pDest, uint numSamplesPerSide, uint numBands)
{
	ComputeMonteCarloCubeSampling(pDest, numBands * numBands, numSamplesPerSide, AddCosineLobeSH, (void*) (size_t) (numBands * numBands));	
}

void
CSEFXResourceMgr::ComputeYlmCubeOneFace(const gmtl::VecA3f& initLookup,
									   const gmtl::VecA3f& diffU,
									   const gmtl::VecA3f& diffV,
									   uint numPixelsPerSide,
									   uint channelStride,
									   uint numBandsSq,
									   float* pOut)
{
	gmtl::VecA3f lookupVec;
	gmtl::VecA3f diffVTotal;
	diffVTotal[0] = 0.0f;
	diffVTotal[1] = 0.0f;
	diffVTotal[2] = 0.0f;

	lookupVec[0] = initLookup[0];
	lookupVec[1] = initLookup[1];
	lookupVec[2] = initLookup[2];

	_LOOPi(numPixelsPerSide)
	{
		_LOOPj(numPixelsPerSide)
		{			
			// Project to SH
			gmtl::VecA3f	normalizedLookup;
			NormalizeVec(&normalizedLookup, &lookupVec);

			// Store the SH coefficients for each band
			uint index	= i * numPixelsPerSide + j;
			_LOOPk(numBandsSq)
			{
				pOut[index * channelStride + k]		= EvaluateSHFastCartesian(k, _CAST_VEC3(normalizedLookup));
			}

			// Increment lookup vector in U direction
			VecVecAdd(&_CAST_VECA4(lookupVec), &_CAST_VECA4(lookupVec), &_CAST_VECA4(diffU));;
		}

		// Increment lookup vector in V direction
		VecVecAdd(&_CAST_VECA4(diffVTotal), &_CAST_VECA4(diffVTotal), &_CAST_VECA4(diffV));
		VecVecAdd(&_CAST_VECA4(lookupVec), &_CAST_VECA4(initLookup), &_CAST_VECA4(diffVTotal));
	}
}

void
CSEFXResourceMgr::ComputeRadianceCosineLobeCubeOneFace(const gmtl::VecA3f& initLookup,
														   const gmtl::VecA3f& diffU,
														   const gmtl::VecA3f& diffV,
														   uint numPixelsPerSide,
														   uint channelStride,
														   float* pOut,
														   RadianceCosineFaceData& radianceData)
{
	gmtl::VecA3f lookupVec;
	gmtl::VecA3f diffVTotal;
	diffVTotal[0] = 0.0f;
	diffVTotal[1] = 0.0f;
	diffVTotal[2] = 0.0f;

	lookupVec[0] = initLookup[0];
	lookupVec[1] = initLookup[1];
	lookupVec[2] = initLookup[2];

	_LOOPi(numPixelsPerSide)
	{
		_LOOPj(numPixelsPerSide)
		{			
			// Project to SH
			gmtl::VecA3f	normalizedLookup;
			NormalizeVec(&normalizedLookup, &lookupVec);

			radianceData.normal = normalizedLookup;

			float colorSH[SEFX_MAX_SH_BANDS * SEFX_MAX_SH_BANDS * 3];
			ComputeMonteCarloCubeSampling(colorSH, radianceData.numBandsSq * 3, 
				radianceData.numSamplesPerSide, AddRadianceCosine, &radianceData);

			// Store the SH coefficients for each band for the NDotL lobe
			uint index	= i * numPixelsPerSide + j;
			_LOOPk(radianceData.numBandsSq)
			{
				pOut[index * channelStride * 3 + k * 3]		= colorSH[k * 3];
				pOut[index * channelStride * 3 + k * 3 + 1]	= colorSH[k * 3 + 1];
				pOut[index * channelStride * 3 + k * 3 + 2]	= colorSH[k * 3 + 2];
			}

			// Increment lookup vector in U direction
			VecVecAdd(&_CAST_VECA4(lookupVec), &_CAST_VECA4(lookupVec), &_CAST_VECA4(diffU));;
		}

		// Increment lookup vector in V direction
		VecVecAdd(&_CAST_VECA4(diffVTotal), &_CAST_VECA4(diffVTotal), &_CAST_VECA4(diffV));
		VecVecAdd(&_CAST_VECA4(lookupVec), &_CAST_VECA4(initLookup), &_CAST_VECA4(diffVTotal));

		m_pSHExpLightingFX->GetAppCallback().Log(ToStringAuto(_W("row: %d\n"), i));
	}
}

void	
CSEFXResourceMgr::CreateRadianceCosineLobeSHCoeffs(IRTextureCube** ppDest, IRTextureCube* pSource, uint numBands)
{
	uint numBandsSq			= numBands * numBands;
	uint numCubeMaps		= FastCeiling((numBandsSq - gmtl::GMTL_EPSILON)/ 4.0f);

	uint numPixelsSource	= pSource->GetSize(0);
	uint numPixelsDest		= ppDest[0]->GetSize(0);

	float halfPixelOffsetDest		= 1.0f / (numPixelsDest);
	float fullPixelOffsetDest		= halfPixelOffsetDest * 2.0f;

	RadianceCosineFaceData radianceData;
	radianceData.numSamplesPerSide	= numPixelsSource;
	radianceData.numBandsSq = numBandsSq;

	_LOOPi(6)
	{
		radianceData.cubeFaceData[i] = (float*) _MALLOC( sizeof(float) * 3 * numPixelsSource * numPixelsSource );
		uint pitch;
		gmtl::Vec4i toWrite;
		toWrite[0] = toWrite[1] = toWrite[2] = toWrite[3] = 0;
		byte* pSrcData = pSource->Lock(0, (eRCubeFace) i, pitch, &toWrite);
		_DEBUG_ASSERT(pitch == numPixelsSource * sizeof(byte) * 4);

		_LOOPj(numPixelsSource)
		{
			_LOOPk(numPixelsSource)
			{
				uint srcIndex	= j * pitch + k * sizeof(byte) * 4;
				uint destIndex	= j * numPixelsSource * 3 + k * 3; 

				uint argb	= *((uint*) &(pSrcData[srcIndex]));
				float red	= ((argb >> 16) & 0xFF) / 255.0f;	// R
				float green	= ((argb >> 8) & 0xFF) / 255.0f;	// G
				float blue	= (argb & 0xFF) / 255.0f;			// B

				radianceData.cubeFaceData[i][destIndex]		= red;
				radianceData.cubeFaceData[i][destIndex + 1] = green;
				radianceData.cubeFaceData[i][destIndex + 2] = blue;
			}
		}

		pSource->Unlock(0, (eRCubeFace) i);
	}

	float* pCubeFaceDest[6];
	uint destTexelSize = sizeof(float) * numCubeMaps * 4 * 3;
	_LOOPi(6)
	{
		pCubeFaceDest[i]	= (float*) _MALLOC( destTexelSize * numPixelsDest * numPixelsDest );
		memset(pCubeFaceDest[i], 0, destTexelSize * numPixelsDest * numPixelsDest);	
	}

	// +X
	m_pSHExpLightingFX->GetAppCallback().Log(_W("Compressing +X...\n"));
	gmtl::VecA3f initLookup;
	initLookup[0]	= 1.0f;
	initLookup[1]	= 1.0f - halfPixelOffsetDest;
	initLookup[2]	= -1.0f + halfPixelOffsetDest;
	gmtl::VecA3f diffU;
	diffU[0]		= 0.0f;
	diffU[1]		= 0.0f;
	diffU[2]		= fullPixelOffsetDest;
	gmtl::VecA3f diffV;
	diffV[0]		= 0.0f;
	diffV[1]		= -fullPixelOffsetDest;
	diffV[2]		= 0.0f;

	gmtl::Vec4i toWrite;
	toWrite[0] = toWrite[1] = toWrite[2] = toWrite[3] = 0;
	ComputeRadianceCosineLobeCubeOneFace(initLookup, diffU, diffV, 
		numPixelsDest, numCubeMaps * 4, pCubeFaceDest[CF_POS_X], radianceData);

	// -X
	m_pSHExpLightingFX->GetAppCallback().Log(_W("Compressing -X...\n"));
	initLookup[0]	= -1.0f;
	initLookup[1]	= 1.0f - halfPixelOffsetDest;
	initLookup[2]	= 1.0f - halfPixelOffsetDest;
	diffU[0]		= 0.0f;
	diffU[1]		= 0.0f;
	diffU[2]		= -fullPixelOffsetDest;
	diffV[0]		= 0.0f;
	diffV[1]		= -fullPixelOffsetDest;
	diffV[2]		= 0.0f;

	ComputeRadianceCosineLobeCubeOneFace(initLookup, diffU, diffV, 
		numPixelsDest, numCubeMaps * 4, pCubeFaceDest[CF_NEG_X], radianceData);

	// +Y
	m_pSHExpLightingFX->GetAppCallback().Log(_W("Compressing +Y...\n"));
	initLookup[0]	= -1.0f + halfPixelOffsetDest;;
	initLookup[1]	= 1.0f;
	initLookup[2]	= 1.0f - halfPixelOffsetDest;
	diffU[0]		= fullPixelOffsetDest;
	diffU[1]		= 0.0f;
	diffU[2]		= 0.0f;
	diffV[0]		= 0.0f;
	diffV[1]		= 0.0f;
	diffV[2]		= -fullPixelOffsetDest;

	ComputeRadianceCosineLobeCubeOneFace(initLookup, diffU, diffV, 
		numPixelsDest, numCubeMaps * 4, pCubeFaceDest[CF_POS_Y], radianceData);

	// -Y
	m_pSHExpLightingFX->GetAppCallback().Log(_W("Compressing -Y...\n"));
	initLookup[0]	= -1.0f + halfPixelOffsetDest;
	initLookup[1]	= -1.0f;
	initLookup[2]	= -1.0f + halfPixelOffsetDest;
	diffU[0]		= fullPixelOffsetDest;
	diffU[1]		= 0.0f;
	diffU[2]		= 0.0f;
	diffV[0]		= 0.0f;
	diffV[1]		= 0.0f;
	diffV[2]		= fullPixelOffsetDest;

	ComputeRadianceCosineLobeCubeOneFace(initLookup, diffU, diffV, 
		numPixelsDest, numCubeMaps * 4, pCubeFaceDest[CF_NEG_Y], radianceData);

	// +Z
	m_pSHExpLightingFX->GetAppCallback().Log(_W("Compressing +Z...\n"));
	initLookup[0]	= 1.0f - halfPixelOffsetDest;;
	initLookup[1]	= 1.0f - halfPixelOffsetDest;
	initLookup[2]	= 1.0f;
	diffU[0]		= -fullPixelOffsetDest;
	diffU[1]		= 0.0f;
	diffU[2]		= 0.0f;
	diffV[0]		= 0.0f;
	diffV[1]		= -fullPixelOffsetDest;
	diffV[2]		= 0.0f;

	ComputeRadianceCosineLobeCubeOneFace(initLookup, diffU, diffV, 
		numPixelsDest, numCubeMaps * 4, pCubeFaceDest[CF_POS_Z], radianceData);

	// -Z
	m_pSHExpLightingFX->GetAppCallback().Log(_W("Compressing -Z...\n"));
	initLookup[0]	= -1.0f + halfPixelOffsetDest;;
	initLookup[1]	= 1.0f - halfPixelOffsetDest;
	initLookup[2]	= -1.0f;
	diffU[0]		= fullPixelOffsetDest;
	diffU[1]		= 0.0f;
	diffU[2]		= 0.0f;
	diffV[0]		= 0.0f;
	diffV[1]		= -fullPixelOffsetDest;
	diffV[2]		= 0.0f;

	ComputeRadianceCosineLobeCubeOneFace(initLookup, diffU, diffV, 
		numPixelsDest, numCubeMaps * 4, pCubeFaceDest[CF_NEG_Z], radianceData);

	// Copy results to the cube map textures
	// TODO: We need half-floats instead...
	const static uint MAX_CUBE_MAPS_NEEDED = ((SEFX_MAX_SH_BANDS * SEFX_MAX_SH_BANDS) / 4) + 1;
	byte* pCubeMapDest[MAX_CUBE_MAPS_NEEDED * 3][6];

	uint bandStride		= 3;  // Per color
	uint channelStride	= numCubeMaps * 4 * bandStride;	

	// Red
	m_pSHExpLightingFX->GetAppCallback().Log(_W("Copying red channel...\n"));
	_LOOPi(numCubeMaps)
	{
		uint pitch = 0;
		_LOOPj(6)
		{
			// For each cube map, copy data for each SH coeff to the appropriate cube map face
			pCubeMapDest[i][j] = ppDest[i]->Lock(0, (eRCubeFace) j, pitch, NULL);
			for(uint h=0; h < numPixelsDest; h++)
			{
				for(uint w=0; w < numPixelsDest; w++)
				{
					uint destIndex = h * pitch + w * sizeof(float) * 4;
					uint srcIndex = h * numPixelsDest * channelStride + w * channelStride + i * bandStride * 4;

					*((float*) &(pCubeMapDest[i][j][destIndex]))						= pCubeFaceDest[j][srcIndex];
					*((float*) &(pCubeMapDest[i][j][destIndex + sizeof(float)]))		= pCubeFaceDest[j][srcIndex + bandStride];
					*((float*) &(pCubeMapDest[i][j][destIndex + sizeof(float) * 2]))	= pCubeFaceDest[j][srcIndex + bandStride * 2];
					*((float*) &(pCubeMapDest[i][j][destIndex + sizeof(float) * 3]))	= pCubeFaceDest[j][srcIndex + bandStride * 3];
				}
			}

			ppDest[i]->Unlock(0, (eRCubeFace) j);
		}
	}

	// Green
	m_pSHExpLightingFX->GetAppCallback().Log(_W("Copying green channel...\n"));
	uint greenOffset = numCubeMaps;
	_LOOPi(numCubeMaps)
	{
		uint pitch = 0;
		_LOOPj(6)
		{
			// For each cube map, copy data for each SH coeff to the appropriate cube map face
			pCubeMapDest[greenOffset + i][j] = ppDest[greenOffset + i]->Lock(0, (eRCubeFace) j, pitch, NULL);
			for(uint h=0; h < numPixelsDest; h++)
			{
				for(uint w=0; w < numPixelsDest; w++)
				{
					uint destIndex = h * pitch + w * sizeof(float) * 4;
					uint srcIndex = h * numPixelsDest * channelStride + w * channelStride + i * bandStride * 4 + 1;

					*((float*) &(pCubeMapDest[greenOffset + i][j][destIndex]))						= pCubeFaceDest[j][srcIndex];
					*((float*) &(pCubeMapDest[greenOffset + i][j][destIndex + sizeof(float)]))		= pCubeFaceDest[j][srcIndex + bandStride];
					*((float*) &(pCubeMapDest[greenOffset + i][j][destIndex + sizeof(float) * 2]))	= pCubeFaceDest[j][srcIndex + bandStride * 2];
					*((float*) &(pCubeMapDest[greenOffset + i][j][destIndex + sizeof(float) * 3]))	= pCubeFaceDest[j][srcIndex + bandStride * 3];
				}
			}

			ppDest[greenOffset + i]->Unlock(0, (eRCubeFace) j);
		}
	}

	// Blue
	m_pSHExpLightingFX->GetAppCallback().Log(_W("Copying blue channel...\n"));
	uint blueOffset = greenOffset + numCubeMaps;
	_LOOPi(numCubeMaps)
	{
		uint pitch = 0;
		_LOOPj(6)
		{
			// For each cube map, copy data for each SH coeff to the appropriate cube map face
			pCubeMapDest[blueOffset + i][j] = ppDest[blueOffset + i]->Lock(0, (eRCubeFace) j, pitch, NULL);
			for(uint h=0; h < numPixelsDest; h++)
			{
				for(uint w=0; w < numPixelsDest; w++)
				{
					uint destIndex = h * pitch + w * sizeof(float) * 4;
					uint srcIndex = h * numPixelsDest * channelStride + w * channelStride + i * bandStride * 4 + 2;

					*((float*) &(pCubeMapDest[blueOffset + i][j][destIndex]))						= pCubeFaceDest[j][srcIndex];
					*((float*) &(pCubeMapDest[blueOffset + i][j][destIndex + sizeof(float)]))		= pCubeFaceDest[j][srcIndex + bandStride];
					*((float*) &(pCubeMapDest[blueOffset + i][j][destIndex + sizeof(float) * 2]))	= pCubeFaceDest[j][srcIndex + bandStride * 2];
					*((float*) &(pCubeMapDest[blueOffset + i][j][destIndex + sizeof(float) * 3]))	= pCubeFaceDest[j][srcIndex + bandStride * 3];
				}
			}

			ppDest[blueOffset + i]->Unlock(0, (eRCubeFace) j);
		}
	}

	_LOOPi(6)
	{
		_FREE( radianceData.cubeFaceData[i] );
		_FREE( pCubeFaceDest[i] );
	}
}

void	
CSEFXResourceMgr::CreateYlmCoeffs(IRTextureCube** ppDest, uint numBands)
{
	uint numBandsSq			= numBands * numBands;
	uint numCubeMaps		= FastCeiling((numBandsSq - gmtl::GMTL_EPSILON)/ 4.0f);

	uint numPixelsDest		= ppDest[0]->GetSize(0);

	float halfPixelOffsetDest		= 1.0f / (numPixelsDest);
	float fullPixelOffsetDest		= halfPixelOffsetDest * 2.0f;

	float* pCubeFaceDest[6];
	uint destTexelSize = sizeof(float) * numCubeMaps * 4;
	_LOOPi(6)
	{
		pCubeFaceDest[i]	= (float*) _MALLOC( destTexelSize * numPixelsDest * numPixelsDest );
		memset(pCubeFaceDest[i], 0, destTexelSize * numPixelsDest * numPixelsDest);	
	}

	// +X
	gmtl::VecA3f initLookup;
	initLookup[0]	= 1.0f;
	initLookup[1]	= 1.0f - halfPixelOffsetDest;
	initLookup[2]	= -1.0f + halfPixelOffsetDest;
	gmtl::VecA3f diffU;
	diffU[0]		= 0.0f;
	diffU[1]		= 0.0f;
	diffU[2]		= fullPixelOffsetDest;
	gmtl::VecA3f diffV;
	diffV[0]		= 0.0f;
	diffV[1]		= -fullPixelOffsetDest;
	diffV[2]		= 0.0f;

	ComputeYlmCubeOneFace(initLookup, diffU, diffV, 
		numPixelsDest, numCubeMaps * 4, numBandsSq, pCubeFaceDest[CF_POS_X]);

	// -X
	initLookup[0]	= -1.0f;
	initLookup[1]	= 1.0f - halfPixelOffsetDest;
	initLookup[2]	= 1.0f - halfPixelOffsetDest;
	diffU[0]		= 0.0f;
	diffU[1]		= 0.0f;
	diffU[2]		= -fullPixelOffsetDest;
	diffV[0]		= 0.0f;
	diffV[1]		= -fullPixelOffsetDest;
	diffV[2]		= 0.0f;

	ComputeYlmCubeOneFace(initLookup, diffU, diffV, 
		numPixelsDest, numCubeMaps * 4, numBandsSq, pCubeFaceDest[CF_NEG_X]);

	// +Y
	initLookup[0]	= -1.0f + halfPixelOffsetDest;;
	initLookup[1]	= 1.0f;
	initLookup[2]	= 1.0f - halfPixelOffsetDest;
	diffU[0]		= fullPixelOffsetDest;
	diffU[1]		= 0.0f;
	diffU[2]		= 0.0f;
	diffV[0]		= 0.0f;
	diffV[1]		= 0.0f;
	diffV[2]		= -fullPixelOffsetDest;

	ComputeYlmCubeOneFace(initLookup, diffU, diffV, 
		numPixelsDest, numCubeMaps * 4, numBandsSq, pCubeFaceDest[CF_POS_Y]);

	// -Y
	initLookup[0]	= -1.0f + halfPixelOffsetDest;
	initLookup[1]	= -1.0f;
	initLookup[2]	= -1.0f + halfPixelOffsetDest;
	diffU[0]		= fullPixelOffsetDest;
	diffU[1]		= 0.0f;
	diffU[2]		= 0.0f;
	diffV[0]		= 0.0f;
	diffV[1]		= 0.0f;
	diffV[2]		= fullPixelOffsetDest;

	ComputeYlmCubeOneFace(initLookup, diffU, diffV, 
		numPixelsDest, numCubeMaps * 4, numBandsSq, pCubeFaceDest[CF_NEG_Y]);

	// +Z
	initLookup[0]	= 1.0f - halfPixelOffsetDest;;
	initLookup[1]	= 1.0f - halfPixelOffsetDest;
	initLookup[2]	= 1.0f;
	diffU[0]		= -fullPixelOffsetDest;
	diffU[1]		= 0.0f;
	diffU[2]		= 0.0f;
	diffV[0]		= 0.0f;
	diffV[1]		= -fullPixelOffsetDest;
	diffV[2]		= 0.0f;

	ComputeYlmCubeOneFace(initLookup, diffU, diffV, 
		numPixelsDest, numCubeMaps * 4, numBandsSq, pCubeFaceDest[CF_POS_Z]);

	// -Z
	initLookup[0]	= -1.0f + halfPixelOffsetDest;;
	initLookup[1]	= 1.0f - halfPixelOffsetDest;
	initLookup[2]	= -1.0f;
	diffU[0]		= fullPixelOffsetDest;
	diffU[1]		= 0.0f;
	diffU[2]		= 0.0f;
	diffV[0]		= 0.0f;
	diffV[1]		= -fullPixelOffsetDest;
	diffV[2]		= 0.0f;

	ComputeYlmCubeOneFace(initLookup, diffU, diffV, 
		numPixelsDest, numCubeMaps * 4, numBandsSq, pCubeFaceDest[CF_NEG_Z]);

	// Copy results to the cube map textures
	// TODO: We need half-floats instead...
	const static uint MAX_CUBE_MAPS_NEEDED = ((SEFX_MAX_SH_BANDS * SEFX_MAX_SH_BANDS) / 4) + 1;
	byte* pCubeMapDest[MAX_CUBE_MAPS_NEEDED * 3][6];

	uint channelStride	= numCubeMaps * 4;	

	_LOOPi(numCubeMaps)
	{
		uint pitch = 0;
		_LOOPj(6)
		{
			// For each cube map, copy data for each SH coeff to the appropriate cube map face
			pCubeMapDest[i][j] = ppDest[i]->Lock(0, (eRCubeFace) j, pitch, NULL);
			for(uint h=0; h < numPixelsDest; h++)
			{
				for(uint w=0; w < numPixelsDest; w++)
				{
					uint destIndex = h * pitch + w * sizeof(float) * 4;
					uint srcIndex = h * numPixelsDest * channelStride + w * channelStride + i * 4;

					*((float*) &(pCubeMapDest[i][j][destIndex]))						= pCubeFaceDest[j][srcIndex];
					*((float*) &(pCubeMapDest[i][j][destIndex + sizeof(float)]))		= pCubeFaceDest[j][srcIndex + 1];
					*((float*) &(pCubeMapDest[i][j][destIndex + sizeof(float) * 2]))	= pCubeFaceDest[j][srcIndex + 2];
					*((float*) &(pCubeMapDest[i][j][destIndex + sizeof(float) * 3]))	= pCubeFaceDest[j][srcIndex + 3];
				}
			}

			ppDest[i]->Unlock(0, (eRCubeFace) j);
		}
	}

	_LOOPi(6)
	{
		_FREE( pCubeFaceDest[i] );
	}
}

void
CSEFXResourceMgr::InvertExpSHCoeff4(float* pTripleProduct, float* pZHSrc, float* pZHDest)
{
	float		shCoeffs[4 * 4];
	//ExpandZHToSH(pZHSrc, shCoeffs, 4);
	memset(shCoeffs, 0, sizeof(float) * 4 * 4);

	uint lastIndex = 0;
	_LOOPi(4)
	{
		uint curIndex = lastIndex + i * 2;
		shCoeffs[curIndex] = pZHSrc[i];
		lastIndex = curIndex;
	}

	Matrix1616f transferMat;
	EvaluateSHTripleProductTransfer4(transferMat, pTripleProduct, shCoeffs);

	Matrix1616f rot;
	Matrix1616f diag;
	EigenDecomposeTransferMat4(transferMat, rot, diag);

	Matrix1616f invDiag;
	gmtl::identity(invDiag);
	InvertExpEigenValues4(invDiag, diag);

	Matrix1616f invRot;
	gmtl::transpose(invRot, rot);

	Matrix1616f intMat;
	Matrix1616f finalMat;
	gmtl::mult(intMat, rot, invDiag);
	gmtl::mult(finalMat, intMat, invRot);

	Vec16f diff;
	_LOOPi(16)
	{
		diff[i] = shCoeffs[i];
	}

	diff[0] -= gmtl::Math::sqrt(4.0f * gmtl::Math::PI); 

	Vec16f dest;
	_LOOPi(16)
	{
		dest[i] = 0.0f;
	}
	gmtl::xform(dest, finalMat, diff);

	pZHDest[0] = dest[0];
	pZHDest[1] = dest[2];
	pZHDest[2] = dest[6];
	pZHDest[3] = dest[12];
}

void
CSEFXResourceMgr::InvertExpSHCoeff3(float* pTripleProduct, float* pZHSrc, float* pZHDest)
{
	float		shCoeffs[3 * 3];
	memset(shCoeffs, 0, sizeof(float) * 3 * 3);

	uint lastIndex = 0;
	_LOOPi(3)
	{
		uint curIndex = lastIndex + i * 2;
		shCoeffs[curIndex] = pZHSrc[i];
		lastIndex = curIndex;
	}

	//ExpandZHToSH(pZHSrc, shCoeffs, 3);

	Matrix99f transferMat;
	EvaluateSHTripleProductTransfer3(transferMat, pTripleProduct, shCoeffs);

	Matrix99f rot;
	Matrix99f diag;
	EigenDecomposeTransferMat3(transferMat, rot, diag);

	Matrix99f invDiag;
	gmtl::identity(invDiag);
	InvertExpEigenValues3(invDiag, diag);

	Matrix99f invRot;
	gmtl::transpose(invRot, rot);

	Matrix99f intMat;
	Matrix99f finalMat;
	gmtl::mult(intMat, rot, invDiag);
	gmtl::mult(finalMat, intMat, invRot);

	Vec9f diff;
	_LOOPi(9)
	{
		diff[i] = shCoeffs[i];
	}

	diff[0] -= gmtl::Math::sqrt(4.0f * gmtl::Math::PI); 

	Vec9f dest;
	_LOOPi(9)
	{
		dest[i] = 0.0f;
	}
	gmtl::xform(dest, finalMat, diff);

	pZHDest[0] = dest[0];
	pZHDest[1] = dest[2];
	pZHDest[2] = dest[6];
}

void
CSEFXResourceMgr::InvertExpEigenValues3(Matrix99f& res, Matrix99f& diag)
{
	const static float CLIP_EPSILON = 0.02f;

	float clipMax = CLIP_EPSILON * diag[0][0];

	_LOOPi(9)
	{
		float eigenValue = diag[i][i];
		eigenValue = gmtl::Math::Max(eigenValue, clipMax);

		float denom = eigenValue - 1.0f;
		_DEBUG_ASSERT(abs(denom) >= gmtl::GMTL_EPSILON);
		//if(abs(denom) <= gmtl::GMTL_EPSILON)
		//{
		//	denom = gmtl::GMTL_EPSILON;
		//}

		res[i][i] = gmtl::Math::log(eigenValue) / denom;
	}
}

void
CSEFXResourceMgr::InvertExpEigenValues4(Matrix1616f& res, Matrix1616f& diag)
{
	const static float CLIP_EPSILON = 0.02f;

	float clipMax = CLIP_EPSILON * diag[0][0];

	_LOOPi(16)
	{
		float eigenValue = diag[i][i];
		eigenValue = gmtl::Math::Max(eigenValue, clipMax);

		float denom = eigenValue - 1.0f;
		_DEBUG_ASSERT(abs(denom) >= gmtl::GMTL_EPSILON);
		//if(abs(denom) <= gmtl::GMTL_EPSILON)
		//{
		//	denom = gmtl::GMTL_EPSILON;
		//}

		res[i][i] = gmtl::Math::log(eigenValue) / denom;
	}
}

void
CSEFXResourceMgr::EigenDecomposeTransferMat3(Matrix99f& src, Matrix99f& rot, Matrix99f& diag)
{
	gmtl::Eigen eigen(9);
	_LOOPi(9)
	{
		_LOOPj(9)
		{
			eigen.Matrix(i, j) = src[i][j];
		}
	}

	eigen.DecrSortEigenStuffN();

	_LOOPi(9)
	{
		_LOOPj(9)
		{
			diag[i][j]	= 0.0f;
			rot[i][j]	= eigen.GetEigenvector(i, j);
		}

		diag[i][i] = eigen.GetEigenvalue(i);
	}


	//_LOOPi(9)
	//{
	//	// For verification
	//	Vec9f sh, sh2;
	//	Vec9f eigenVector;
	//	_LOOPj(9)
	//	{
	//		eigenVector[j] = rot[j][i] * 7.0f;
	//	}

	//	gmtl::xform(sh, src, eigenVector);

	//	Matrix99f intMat;
	//	Matrix99f resMat;
	//	Matrix99f invRot;
	//	gmtl::transpose(invRot, rot);
	//	gmtl::mult(intMat, invRot, rot);
	//	gmtl::mult(intMat, rot, diag);
	//	gmtl::mult(resMat, intMat, invRot);

	//	gmtl::xform(sh2, resMat, eigenVector);

	//	float meanSqError = 0.0f;
	//	_LOOPj(9)
	//	{
	//		meanSqError += (sh[j] - sh2[j]) * (sh[j] - sh2[j]);
	//	}
	//	meanSqError /= 9.0f;
	//	m_pSHExpLightingFX->GetAppCallback().Log(ToStringAuto(_W("error: %f\n"), meanSqError));
	//	meanSqError = 0.0f;
	//	_LOOPj(9)
	//	{
	//		_LOOPk(9)
	//		{
	//			meanSqError += (resMat[j][k] - src[j][k]) * (resMat[j][k] - src[j][k]);				
	//		}
	//	}
	//	meanSqError /= 9.0f;
	//	m_pSHExpLightingFX->GetAppCallback().Log(ToStringAuto(_W("errorMat: %f\n"), meanSqError));
	//}
}


void
CSEFXResourceMgr::EigenDecomposeTransferMat4(Matrix1616f& src, Matrix1616f& rot, Matrix1616f& diag)
{
	gmtl::Eigen eigen(16);
	_LOOPi(16)
	{
		_LOOPj(16)
		{
			eigen.Matrix(i, j) = src[i][j];
		}
	}

	eigen.DecrSortEigenStuffN();

	_LOOPi(16)
	{
		_LOOPj(16)
		{
			diag[i][j]	= 0.0f;
			rot[i][j]	= eigen.GetEigenvector(i, j);
		}

		diag[i][i] = eigen.GetEigenvalue(i);
	}


	//_LOOPi(16)
	//{
	//	// For verification
	//	Vec16f sh, sh2;
	//	Vec16f eigenVector;
	//	_LOOPj(16)
	//	{
	//		eigenVector[j] = rot[j][i] * 7.0f;
	//	}

	//	gmtl::xform(sh, src, eigenVector);

	//	Matrix1616f intMat;
	//	Matrix1616f resMat;
	//	Matrix1616f invRot;
	//	gmtl::transpose(invRot, rot);
	//	gmtl::mult(intMat, invRot, rot);
	//	gmtl::mult(intMat, rot, diag);
	//	gmtl::mult(resMat, intMat, invRot);

	//	gmtl::xform(sh2, resMat, eigenVector);

	//	float meanSqError = 0.0f;
	//	_LOOPj(16)
	//	{
	//		meanSqError += (sh[j] - sh2[j]) * (sh[j] - sh2[j]);
	//	}
	//	meanSqError /= 16.0f;
	//	m_pSHExpLightingFX->GetAppCallback().Log(ToStringAuto(_W("error: %f\n"), meanSqError));
	//	meanSqError = 0.0f;
	//	_LOOPj(16)
	//	{
	//		_LOOPk(16)
	//		{
	//			meanSqError += (resMat[j][k] - src[j][k]) * (resMat[j][k] - src[j][k]);				
	//		}
	//	}
	//	meanSqError /= 16.0f;
	//	m_pSHExpLightingFX->GetAppCallback().Log(ToStringAuto(_W("errorMat: %f\n"), meanSqError));
	//}
}

float
CSEFXResourceMgr::ComputeOptLinearApprox(float& a, float& b, float* pZH, float* pZHLog, uint numBands)
{
	// Account for DC isolation
	float zh[SEFX_MAX_SH_BANDS];
	float zhLog[SEFX_MAX_SH_BANDS];
	
	float zhScale = gmtl::Math::exp(-pZHLog[0] / 3.5449078f);
	_LOOPi(numBands)
	{
		zh[i]		= pZH[i] * zhScale;
		zhLog[i]	= pZHLog[i];
	}

	//zhLog[0] = 0.0f;

	a = (zh[0] / 3.5449078f);

	float topRes	= 0.0f;
	float bottomRes = 0.0f;
	
	_LOOPi(numBands-1)
	{
		topRes		+= (zh[i+1] * zhLog[i+1]);
		bottomRes	+= (zhLog[i+1] * zhLog[i+1]);
	}

	b = (topRes / bottomRes);

	return gmtl::Math::sqrt(bottomRes);
}

void	
CSEFXResourceMgr::CreateSphereVisibilityCoeffs(IRTexture2D* pDest, IRTexture2D* pABTableTex, uint numBands)
{
	uint numBandsSq = numBands * numBands;
	float* pTripleProduct = (float*) _MALLOC(sizeof(float) * numBandsSq * numBandsSq * numBandsSq);
	EvaluateSHTripleProductTable(pTripleProduct, numBands, &m_pSHExpLightingFX->GetAppCallback());

	uint texWidth			= pDest->GetWidth(0);
	float widthInc			= 1.0f / (texWidth - 1);

	float* pABTableTexDest = (float*) _MALLOC( sizeof(float) * 2 * texWidth );
	float* pSphereVisDest = (float*) _MALLOC( sizeof(float) * numBands * texWidth );
	float* pLength	= (float*) _MALLOC( sizeof(float) * texWidth );
	VisData visData;
	visData.numBands = numBands;

	// Compute the scales
	float scales[SEFX_MAX_SH_BANDS];
	_LOOPi(numBands)
	{
		scales[i] = gmtl::Math::sqrt( (4.0f * gmtl::Math::PI) / (2.0f * i + 1.0f) );
	}

	float lastA = 0.0f;
	float lastB = 0.0f;

	_LOOPi(texWidth)
	{
		m_pSHExpLightingFX->GetAppCallback().Log(ToStringAuto(_W("width: %d\n"), i));

		// Compute g(theta)
		uint index = i * numBands;
		float distSq = (widthInc * i);

		float theta = gmtl::Math::abs( gmtl::Math::aSin( gmtl::Math::sqrt(distSq) ) );
		if( (theta * 180.0f / gmtl::Math::PI) > 80.463082f ) //theta > 0.802664f)
			theta = (80.463082f * gmtl::Math::PI) / 180.0f;

		float cosTheta = gmtl::Math::cos(theta);

		visData.cosTheta = cosTheta;
		ComputeMonteCarloCubeSampling(pSphereVisDest + index, numBands, 128, AddSphereVisibility, &visData);

		_DEBUG_ASSERT(numBands >= 3 && numBands <= SEFX_MAX_SH_BANDS);

		float zh2[SEFX_MAX_SH_BANDS];

		_LOOPj(numBands)
		{
			zh2[j] = pSphereVisDest[index+j];//shSphere2[0];
		}
	
		float zhLog[SEFX_MAX_SH_BANDS];
		if(numBands == 3)
		{
			InvertExpSHCoeff3(pTripleProduct, zh2, zhLog);
		}
		else if(numBands == 4)
		{
			InvertExpSHCoeff4(pTripleProduct, zh2, zhLog);

			// Increase the range to simulate k spherical occluders
			//zhLog[0] *= 1.65f;
			//zhLog[1] *= 1.65f;
			//zhLog[2] *= 1.65f;
			//zhLog[3] *= 1.65f;
		}
		
		_LOOPj(numBands)
		{
			// Compute scaled g(theta)
			pSphereVisDest[index+j]	= //pSphereVisDest[index+i] * scales[i];
				zhLog[j] * scales[j];
		}

		_LOOPj(numBands)
		{
			// Increase the range to simulate k spherical occluders
			zhLog[j] *= 2.35f;
		}

		float a, b;
		float length = ComputeOptLinearApprox(a, b, zh2, zhLog, numBands);
		m_pSHExpLightingFX->GetAppCallback().Log(ToStringAuto(_W("length: %f\n"), length));
		pLength[i] = length;

		// Remove the length
		//if(length < 5.080370f)
		{
			lastA = a;
			lastB = b;
		}

		m_pSHExpLightingFX->GetAppCallback().Log(ToStringAuto(_W("theta: %f\n"), theta * 180.0f / gmtl::Math::PI));
		m_pSHExpLightingFX->GetAppCallback().Log(ToStringAuto(_W("zh: %f, %f, %f, %f\n"), zh2[0], zh2[1], zh2[2], zh2[3]));
		m_pSHExpLightingFX->GetAppCallback().Log(ToStringAuto(_W("zhLog: %f, %f, %f, %f\n"), zhLog[0], zhLog[1], zhLog[2], zhLog[3]));
		m_pSHExpLightingFX->GetAppCallback().Log(ToStringAuto(_W("a: %f, b: %f\n"), lastA, lastB));

		pABTableTexDest[i*2]	= lastA;
		pABTableTexDest[i*2+1]	= lastB;
	}

	// Only copy 1 row
	uint pitch = 0;
	float* pDestData = (float*) pDest->Lock(0, pitch, NULL);
	_DEBUG_ASSERT(pitch == (texWidth * sizeof(float) * 4));
	_LOOPi(texWidth)
	{
		_LOOPj(numBands)
		{
			pDestData[i * 4 + j] = pSphereVisDest[i * numBands + j];
		}
	}
	pDest->Unlock(0);

	// Copy a,b table
	float* pABTableDestData = (float*) pABTableTex->Lock(0, pitch, NULL);
	_DEBUG_ASSERT(pitch == (texWidth * sizeof(float) * 2));

	// Linearize the table as a function of length
	// Because the AB texture from the optimal linear approximation when used in the shader
	// is indexed by length. Unfortunately, the lengths are not stored linearly from 0 - 1, but when we access it
	// we assume it is..

	//uint maxIndex = 0;
	//float lastMaxLength = 0.0f;
	//_LOOPi(pLength[i])
	//{
	//	if(pLength[i] > lastMaxLength)
	//	{
	//		lastMaxLength = pLength[i];
	//		maxIndex = i;
	//	}
	//}

	float lengthInc = (pLength[texWidth-1] - pLength[0]) / (texWidth - 1); 
	float curLength = pLength[0];
	uint curMin = 0;
	uint curMax = 0;
	_LOOPi(texWidth)
	{
		while(curLength > pLength[curMax])
		{
			curMin = curMax;
			curMax++;
			if(curMax >= texWidth)
			{
				curMax = texWidth - 1;
				break;
			}
		}

		float lerp = (curLength - pLength[curMin]) / (pLength[curMax] - pLength[curMin]);
		if(curMin == curMax)
			lerp = 0.0f;
		float a = pABTableTexDest[curMin*2] + lerp * (pABTableTexDest[curMax*2] - pABTableTexDest[curMin*2]);
		float b = pABTableTexDest[curMin*2+1] + lerp * (pABTableTexDest[curMax*2+1] - pABTableTexDest[curMin*2+1]);

		pABTableDestData[i*2]	= a;
		pABTableDestData[i*2+1] = b;
		m_pSHExpLightingFX->GetAppCallback().Log(ToStringAuto(_W("length: %f, a: %f, b: %f\n"), curLength, a, b));

		curLength += lengthInc;
	}
	pABTableTex->Unlock(0);

	_FREE(pLength);
	_FREE(pABTableTexDest);
	_FREE(pSphereVisDest);
}

void	
CSEFXResourceMgr::ConvertToZH(float* pZH, gmtl::Vec3f* pLobeAxis, const float* pSH, uint numBands, void* pD3DDevice)
{
	// Requires D3D Win32 for now

	// Create the placeholder mesh because D3D needs it to initialize
	//IDirect3DDevice9* pDevice = (IDirect3DDevice9*) pD3DDevice;
	//ID3DXMesh* pMesh = NULL;
	//D3DXCreatePolygon(pDevice, 1.0f, 3, &pMesh, NULL);
	//DWORD* pOrigAdj = (DWORD*) _MALLOC(sizeof(DWORD) * pMesh->GetNumFaces() * 3);
	//DWORD* pAdj		= (DWORD*) _MALLOC(sizeof(DWORD) * pMesh->GetNumFaces() * 3);
	//pMesh->GenerateAdjacency(1e-6f, pOrigAdj);
	//HRESULT hr = pMesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT, pOrigAdj, pAdj, NULL, NULL);
	//_DEBUG_ASSERT(SUCCEEDED(hr));

	//ID3DXPRTEngine* pPRTEngine = NULL;
	//hr = D3DXCreatePRTEngine(pMesh, pAdj, FALSE, NULL, &pPRTEngine);
	//_DEBUG_ASSERT(SUCCEEDED(hr));
	//pPRTEngine->SetSamplingInfo(1024, FALSE, TRUE, FALSE, 0.0f);

	//// Fill the source SH buffer to convert
	//// NOTE: The ComputeLDPRT function requires 3 color channels for some reason...
	//uint numVerts = pPRTEngine->GetNumVerts();
	//ID3DXPRTBuffer* pSHBuffer = NULL;
	//ID3DXPRTBuffer* pZHBuffer = NULL;
	//hr = D3DXCreatePRTBuffer(numVerts, numBands * numBands, 3, &pSHBuffer);
	//_DEBUG_ASSERT(SUCCEEDED(hr));
	//hr = D3DXCreatePRTBuffer(numVerts, numBands, 3, &pZHBuffer);
	//_DEBUG_ASSERT(SUCCEEDED(hr));

	//float* pSHSrc = NULL;
	//_LOOPi(numVerts)
	//{
	//	hr = pSHBuffer->LockBuffer(i, 1, &pSHSrc);
	//	_DEBUG_ASSERT(SUCCEEDED(hr));
	//	memcpy(pSHSrc, pSH, sizeof(float) * numBands * numBands);
	//	memcpy(pSHSrc + numBands * numBands, pSH, sizeof(float) * numBands * numBands);
	//	memcpy(pSHSrc + numBands * numBands * 2, pSH, sizeof(float) * numBands * numBands);
	//	hr = pSHBuffer->UnlockBuffer();
	//	_DEBUG_ASSERT(SUCCEEDED(hr));
	//}

	//// Compute ZH for N=1
	//D3DXVECTOR3 normals[1024];
	//hr = pPRTEngine->ComputeLDPRTCoeffs(pSHBuffer, numBands, normals, pZHBuffer);
	//_DEBUG_ASSERT(SUCCEEDED(hr));

	//// Copy results (from the first sample is enough)
	//float* pZHSrc = NULL;
	//pZHBuffer->LockBuffer(0, 1, &pZHSrc);
	//memcpy(pZH, pZHSrc, sizeof(float) * numBands);
	//pZHBuffer->UnlockBuffer();

	//// Copy results from normal buffer
	//// Change coordinate axis
	//(*pLobeAxis)[0] = normals[0].x;
	//(*pLobeAxis)[1] = normals[0].z;
	//(*pLobeAxis)[2] = -normals[0].y;

	//_RELEASE(pSHBuffer);
	//_RELEASE(pZHBuffer);
	//_RELEASE(pMesh);
	//_RELEASE(pPRTEngine);
	//_FREE(pAdj);
	//_FREE(pOrigAdj);
}

_NAMESPACE_END
