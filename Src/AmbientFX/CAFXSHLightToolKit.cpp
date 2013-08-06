//==============================================================================
//
//		CAFXSHLightToolKit.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		07/10/2009
//
//==============================================================================

#include "AmbientFXFramework.h"

_NAMESPACE_BEGIN

float	
CAFXSHLightToolKit::ComputeDifferentialAngles(float* pToStore, uint numPixelsPerSide)
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
CAFXSHLightToolKit::ComputeMonteCarloCubeSamplingOneFace(const gmtl::VecA3f& initLookup,
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
CAFXSHLightToolKit::ComputeMonteCarloCubeSampling(float* pDest,
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
AddHemiLobeSH(float* pDest, float weight, const gmtl::VecA3f& dir, uint sampleIndex, eRCubeFace cubeFace, void* pUserInfo)
{
	uint numBandsSq = (uint)(size_t) pUserInfo;

	if(dir[1] < 0.0f)
		weight = 0.0f;

	_LOOPi(numBandsSq)
	{
		pDest[i] += ( EvaluateSHFastCartesian(i, _CAST_VEC3(dir)) * weight);
	}
}

void
AddRadianceCosine(float* pDest, float weight, const gmtl::VecA3f& dir, uint sampleIndex, eRCubeFace cubeFace, void* pUserInfo)
{
	RadianceCosineFaceData* pCubeFaceData = (RadianceCosineFaceData*) pUserInfo;

	float nDotL;
	VecVecDot(&nDotL, &dir, &pCubeFaceData->normal);
	nDotL = nDotL < 0.0f ? 0.0f : (nDotL > 1.0f ? 1.0f : nDotL);
	weight *= nDotL;

	gmtl::VecA3f color;
	color[0] = 1.0f - gmtl::Math::exp(-pCubeFaceData->cubeFaceData[cubeFace][sampleIndex*3]*1.0f);
	color[1] = 1.0f - gmtl::Math::exp(-pCubeFaceData->cubeFaceData[cubeFace][sampleIndex*3+1]*1.0f);
	color[2] = 1.0f - gmtl::Math::exp(-pCubeFaceData->cubeFaceData[cubeFace][sampleIndex*3+2]*1.0f);

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
CAFXSHLightToolKit::CreateUnitSHCoeffs(float* pDest, uint numSamplesPerSide, uint numBands)
{
	ComputeMonteCarloCubeSampling(pDest, numBands * numBands, numSamplesPerSide, AddUnitSH, (void*) (size_t) (numBands * numBands));
}

void	
CAFXSHLightToolKit::CreateCosineLobeCoeffs(float* pDest, uint numSamplesPerSide, uint numBands)
{
	ComputeMonteCarloCubeSampling(pDest, numBands * numBands, numSamplesPerSide, AddCosineLobeSH, (void*) (size_t) (numBands * numBands));	
}

void CAFXSHLightToolKit::CreateHemiLobeCoeffs(float* pDest, uint numSamplesPerSide, uint numBands)
{
	ComputeMonteCarloCubeSampling(pDest, numBands * numBands, numSamplesPerSide, AddHemiLobeSH, (void*) (size_t) (numBands * numBands));	
}

void
CAFXSHLightToolKit::ComputeRadianceCosineLobeCubeOneFace(const gmtl::VecA3f& initLookup,
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

			float colorSH[AFX_MAX_SH_BANDS * AFX_MAX_SH_BANDS * 3];
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

		m_pAmbientFX->GetAppCallback().Log(ToStringAuto(_W("row: %d\n"), i));
	}
}

void	
CAFXSHLightToolKit::CreateRadianceCosineLobeSHCoeffs(IRTextureCube** ppDest, IRTextureCube* pSource, uint numBands)
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
	m_pAmbientFX->GetAppCallback().Log(_W("Compressing +X...\n"));
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
	m_pAmbientFX->GetAppCallback().Log(_W("Compressing -X...\n"));
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
	m_pAmbientFX->GetAppCallback().Log(_W("Compressing +Y...\n"));
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
	m_pAmbientFX->GetAppCallback().Log(_W("Compressing -Y...\n"));
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
	m_pAmbientFX->GetAppCallback().Log(_W("Compressing +Z...\n"));
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
	m_pAmbientFX->GetAppCallback().Log(_W("Compressing -Z...\n"));
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
	const static uint MAX_CUBE_MAPS_NEEDED = ((AFX_MAX_SH_BANDS * AFX_MAX_SH_BANDS) / 4) + 1;
	byte* pCubeMapDest[MAX_CUBE_MAPS_NEEDED * 3][6];

	uint bandStride		= 3;  // Per color
	uint channelStride	= numCubeMaps * 4 * bandStride;	

	// Red
	m_pAmbientFX->GetAppCallback().Log(_W("Copying red channel...\n"));
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
	m_pAmbientFX->GetAppCallback().Log(_W("Copying green channel...\n"));
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
	m_pAmbientFX->GetAppCallback().Log(_W("Copying blue channel...\n"));
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

//void	
//CAFXSHLightToolKit::ConvertToZH(float* pZH, gmtl::Vec3f* pLobeAxis, const float* pSH, uint numBands, void* pD3DDevice)
//{
//	// Requires D3D Win32 for now
//
//	// Create the placeholder mesh because D3D needs it to initialize
//	IDirect3DDevice9* pDevice = (IDirect3DDevice9*) pD3DDevice;
//	ID3DXMesh* pMesh = NULL;
//	D3DXCreatePolygon(pDevice, 1.0f, 3, &pMesh, NULL);
//	DWORD* pOrigAdj = (DWORD*) _MALLOC(sizeof(DWORD) * pMesh->GetNumFaces() * 3);
//	DWORD* pAdj		= (DWORD*) _MALLOC(sizeof(DWORD) * pMesh->GetNumFaces() * 3);
//	pMesh->GenerateAdjacency(1e-6f, pOrigAdj);
//	HRESULT hr = pMesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT, pOrigAdj, pAdj, NULL, NULL);
//	_DEBUG_ASSERT(SUCCEEDED(hr));
//
//	ID3DXPRTEngine* pPRTEngine = NULL;
//	hr = D3DXCreatePRTEngine(pMesh, pAdj, FALSE, NULL, &pPRTEngine);
//	_DEBUG_ASSERT(SUCCEEDED(hr));
//	pPRTEngine->SetSamplingInfo(1024, FALSE, TRUE, FALSE, 0.0f);
//
//	// Fill the source SH buffer to convert
//	// NOTE: The ComputeLDPRT function requires 3 color channels for some reason...
//	uint numVerts = pPRTEngine->GetNumVerts();
//	ID3DXPRTBuffer* pSHBuffer = NULL;
//	ID3DXPRTBuffer* pZHBuffer = NULL;
//	hr = D3DXCreatePRTBuffer(numVerts, numBands * numBands, 3, &pSHBuffer);
//	_DEBUG_ASSERT(SUCCEEDED(hr));
//	hr = D3DXCreatePRTBuffer(numVerts, numBands, 3, &pZHBuffer);
//	_DEBUG_ASSERT(SUCCEEDED(hr));
//
//	float* pSHSrc = NULL;
//	_LOOPi(numVerts)
//	{
//		hr = pSHBuffer->LockBuffer(i, 1, &pSHSrc);
//		_DEBUG_ASSERT(SUCCEEDED(hr));
//		memcpy(pSHSrc, pSH, sizeof(float) * numBands * numBands);
//		memcpy(pSHSrc + numBands * numBands, pSH, sizeof(float) * numBands * numBands);
//		memcpy(pSHSrc + numBands * numBands * 2, pSH, sizeof(float) * numBands * numBands);
//		hr = pSHBuffer->UnlockBuffer();
//		_DEBUG_ASSERT(SUCCEEDED(hr));
//	}
//
//	// Compute ZH for N=1
//	// Should supply a normal
//	D3DXVECTOR3 normals[1024];
//	hr = pPRTEngine->ComputeLDPRTCoeffs(pSHBuffer, numBands, normals, pZHBuffer);
//	_DEBUG_ASSERT(SUCCEEDED(hr));
//
//	// Copy results (from the first sample is enough)
//	float* pZHSrc = NULL;
//	pZHBuffer->LockBuffer(0, 1, &pZHSrc);
//	memcpy(pZH, pZHSrc, sizeof(float) * numBands);
//	pZHBuffer->UnlockBuffer();
//
//	// Copy results from normal buffer
//	// Change coordinate axis
//	(*pLobeAxis)[0] = normals[0].x;
//	(*pLobeAxis)[1] = normals[0].z;
//	(*pLobeAxis)[2] = -normals[0].y;
//
//	_RELEASE(pSHBuffer);
//	_RELEASE(pZHBuffer);
//	_RELEASE(pMesh);
//	_RELEASE(pPRTEngine);
//	_FREE(pAdj);
//	_FREE(pOrigAdj);
//}

_NAMESPACE_END
