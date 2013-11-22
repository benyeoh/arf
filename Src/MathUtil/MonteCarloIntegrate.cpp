//==============================================================================
//
//		MonteCarloIntegrate.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		16/10/09
//
//==============================================================================

#include "MathUtil.h"

_NAMESPACE_BEGIN

//float	
//ComputeDifferentialAngles(float* pToStore, uint numPixelsPerSide)
//{
//	float halfPixelOffset	= 1.0f / (numPixelsPerSide);
//	float fullPixelOffset	= halfPixelOffset * 2.0f;
//
//	gmtl::VecA3f initLookup;
//	initLookup[0]	= -1.0f + halfPixelOffset;
//	initLookup[1]	= -1.0f + halfPixelOffset;
//	initLookup[2]	= 1.0f;
//
//	gmtl::VecA3f lookup = initLookup;
//
//	float weightSum = 0.0f;
//
//	_LOOPi(numPixelsPerSide)
//	{
//		_LOOPj(numPixelsPerSide)
//		{
//			float radius;
//			VecLength(&radius, &lookup);
//
//			// Dot product of lookup vector and face normal
//			float cosTheta = 1.0f / radius;	
//
//			// dA = 1 unit pixel weighted by the cosine of the lookup/face normal
//			// -> dOmega = dA / r^2
//			// We will normalize this "unit pixel" by the total weights summed later
//			float dOmega = cosTheta / (radius * radius);
//
//			pToStore[i * numPixelsPerSide + j] = dOmega;
//			weightSum += dOmega;
//
//			lookup[0] = lookup[0] + fullPixelOffset;
//		}
//
//		lookup[0] = initLookup[0];
//		lookup[1] = lookup[1] + fullPixelOffset;
//	}
//
//	return weightSum;
//}

float GetAreaElement(float x, float y)
{
    // Compute area of the region specified by a point (x, y) on the plane projected onto the sphere
    return atan2(x * y, sqrt(x * x + y * y + 1));
}

void ComputeDifferentialAngles(float* pToStore, uint numPixelsPerSide)
{
    float halfPixelOffset	= 1.0f / (numPixelsPerSide);
    float fullPixelOffset	= halfPixelOffset * 2.0f;

    float u = -1.0f + halfPixelOffset;
    float initU = u;

    float v = -1.0f + halfPixelOffset;

    _LOOPi(numPixelsPerSide)
    {
        _LOOPj(numPixelsPerSide)
        {
            float x0 = u - halfPixelOffset;
            float y0 = v - halfPixelOffset;
            float x1 = u + halfPixelOffset;
            float y1 = v + halfPixelOffset;

            // Compute area of the projected cube map texel onto the sphere
            // See ATI CubeMapGen
            float solidAngle = GetAreaElement(x0, y0) - GetAreaElement(x0, y1) - GetAreaElement(x1, y0) + GetAreaElement(x1, y1);           
            pToStore[i * numPixelsPerSide + j] = solidAngle;// / (4.0f * LM_PI);

            u += fullPixelOffset;
        }

        u = initU;
        v += fullPixelOffset;
    }
}


void	ComputeMonteCarloCubeSamplingOneFace(const gmtl::VecA3f& initLookup,
											 const gmtl::VecA3f& diffU,
											 const gmtl::VecA3f& diffV,
											 float* pDiffAngles,
											 uint numPixelsPerSide,
											 float* pOut,
											 AddCubeDataInDir addCubeDataFn,
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
			addCubeDataFn(pOut, pDiffAngles[index], normalizedLookup, index, pUserInfo);

			// Increment lookup vector in U direction
			VecVecAdd(&_CAST_VECA4(lookupVec), &_CAST_VECA4(lookupVec), &_CAST_VECA4(diffU));;
		}

		// Increment lookup vector in V direction
		VecVecAdd(&_CAST_VECA4(diffVTotal), &_CAST_VECA4(diffVTotal), &_CAST_VECA4(diffV));
		VecVecAdd(&_CAST_VECA4(lookupVec), &_CAST_VECA4(initLookup), &_CAST_VECA4(diffVTotal));
	}
}

void	
ComputeMonteCarloCubeSampling(float* pDest,
							uint numChannels,
							uint numSamplesPerSide, 
							AddCubeDataInDir addCubeDataFn,
							void* pUserInfo)
{
	uint numPixelsPerSide	= numSamplesPerSide;
	float halfPixelOffset	= 1.0f / (numPixelsPerSide);
	float fullPixelOffset	= halfPixelOffset * 2.0f;

	// Clear the buffer
	memset(pDest, 0, sizeof(float) * numChannels);

	// Compute differential solid angles
	float* pDiffAngles = (float*) _MALLOC( sizeof(float) * numPixelsPerSide * numPixelsPerSide );
	//float weightPerFace = ComputeDifferentialAngles(pDiffAngles, numPixelsPerSide);
    ComputeDifferentialAngles(pDiffAngles, numPixelsPerSide);

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
		pDiffAngles, numPixelsPerSide, pDest, addCubeDataFn, pUserInfo);

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
		pDiffAngles, numPixelsPerSide, pDest, addCubeDataFn, pUserInfo);

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
		pDiffAngles, numPixelsPerSide, pDest, addCubeDataFn, pUserInfo);

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
		pDiffAngles, numPixelsPerSide, pDest, addCubeDataFn, pUserInfo);

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
		pDiffAngles, numPixelsPerSide, pDest, addCubeDataFn, pUserInfo);

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
		pDiffAngles, numPixelsPerSide, pDest, addCubeDataFn, pUserInfo);

	//// Weight constants
	//float weight = (4.0f * gmtl::Math::PI) / (weightPerFace * 6);
	//_LOOPi(numChannels)
	//{
	//	pDest[i] *= weight;
	//}

	_FREE(pDiffAngles);
}

_NAMESPACE_END
