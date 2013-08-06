//==============================================================================
//
//		CAFXResourceMgr.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/7/2009
//
//==============================================================================

#include "AmbientFXFramework.h"

_NAMESPACE_BEGIN

void
CAFXResourceMgr::CompressOneFace(const gmtl::VecA3f& initLookup,
								 const gmtl::VecA3f& diffU,
								 const gmtl::VecA3f& diffV,
								 uint* pARGB,
								 float* pDiffAngles,
								 uint numPixelsPerSide, 
								 int l, 
								 int m,
								 float toneMapContrast,
								 gmtl::VecA3f& coeffOut)
{
	gmtl::VecA3f lookupVec;
	gmtl::VecA3f diffVTotal;
	diffVTotal[0] = 0.0f;
	diffVTotal[1] = 0.0f;
	diffVTotal[2] = 0.0f;

	lookupVec[0] = initLookup[0];
	lookupVec[1] = initLookup[1];
	lookupVec[2] = initLookup[2];

	uint I = l*(l+1)+m;

	_LOOPi(numPixelsPerSide)
	{
		_LOOPj(numPixelsPerSide)
		{
			uint index	= i * numPixelsPerSide + j;

			gmtl::VecA3f color;
			uint argb	= pARGB[index];
			color[0]	= ((argb >> 16) & 0xFF) / 255.0f;	// R
			color[1]	= ((argb >> 8) & 0xFF) / 255.0f;	// G
			color[2]	= (argb & 0xFF) / 255.0f;			// B

			//color[0] *= 0.27f / ( 0.066 + 0.001f );	
			//color[1] *= 0.27f / ( 0.066 + 0.001f );	
			//color[2] *= 0.27f / ( 0.066 + 0.001f );

			//// Range [0..2 * g_WhiteCutoff] if color is [0..g_WhiteCutoff]
			//color[0] *= ( 1.0f + ( color[0] / ( 0.77f ) ) );
			//color[1] *= ( 1.0f + ( color[1] / ( 0.77f ) ) );
			//color[2] *= ( 1.0f + ( color[2] / ( 0.77f ) ) );

			//// Remove dark pixels
			//color[0] -= 5.0f;
			//color[1] -= 5.0f;
			//color[2] -= 5.0f;

			//color[0] = gmtl::Math::Max( color[0], 0.0f );
			//color[1] = gmtl::Math::Max( color[1], 0.0f );
			//color[2] = gmtl::Math::Max( color[2], 0.0f );

			//// Map from 0..1
			//color[0] /= ( 10.0f + color[0] );
			//color[1] /= ( 10.0f + color[1] );
			//color[2] /= ( 10.0f + color[2] );

			//float luminance = 0.3f * color[0] + 0.59f * color[1] + 0.11f * color[2];

			//color[0]	= color[0] * 2.0f;//gmtl::Math::exp(-luminance * 0.96f);// ((argb >> 16) & 0xFF) / 255.0f;	// R
			//color[1]	= color[1] * 2.0f;//gmtl::Math::exp(-luminance * 0.96f);//((argb >> 8) & 0xFF) / 255.0f;	// G
			//color[2]	= color[2] * 2.0f;//gmtl::Math::exp(-luminance * 0.96f);//(argb & 0xFF) / 255.0f;			// B

			//color[0]	= gmtl::Math::Max(0.0f, 1.0f - gmtl::Math::exp(-1.0f * color[0]));// ((argb >> 16) & 0xFF) / 255.0f;	// R
			//color[1]	= gmtl::Math::Max(0.0f, 1.0f - gmtl::Math::exp(-1.0f * color[1]));//((argb >> 8) & 0xFF) / 255.0f;	// G
			//color[2]	= gmtl::Math::Max(0.0f, 1.0f - gmtl::Math::exp(-1.0f * color[2]));//(argb & 0xFF) / 255.0f;			// B

			color[0]	= color[0] * gmtl::Math::pow(color[0], toneMapContrast);// ((argb >> 16) & 0xFF) / 255.0f;	// R
			color[1]	= color[1] * gmtl::Math::pow(color[1], toneMapContrast);//((argb >> 8) & 0xFF) / 255.0f;	// G
			color[2]	= color[2] * gmtl::Math::pow(color[2], toneMapContrast);//(argb & 0xFF) / 255.0f;			// B

			// Project to SH
			gmtl::VecA3f	normalizedLookup;
			NormalizeVec(&normalizedLookup, &lookupVec);

			//gmtl::VecA3f	up;
			//up[0] = 0.0f;
			//up[1] = 1.0f;
			//up[2] = 0.0f;

			//float nDotL;
			//VecVecDot(&nDotL, &up, &normalizedLookup);
			//if(nDotL < 0.0f)
			//	nDotL = 0.0f;
			//if(nDotL > 1.0f)
			//	nDotL = 1.0f;

			float sh	= EvaluateSHFastCartesian(I, _CAST_VEC3(normalizedLookup));
			sh			*= pDiffAngles[index];// * nDotL;

			VecScalarMult(&_CAST_VECA4(color), &_CAST_VECA4(color), sh);			
			VecVecAdd(&_CAST_VECA4(coeffOut), &_CAST_VECA4(coeffOut), &_CAST_VECA4(color));

			// Increment lookup vector in U direction
			VecVecAdd(&_CAST_VECA4(lookupVec), &_CAST_VECA4(lookupVec), &_CAST_VECA4(diffU));;
		}

		// Increment lookup vector in V direction
		VecVecAdd(&_CAST_VECA4(diffVTotal), &_CAST_VECA4(diffVTotal), &_CAST_VECA4(diffV));
		VecVecAdd(&_CAST_VECA4(lookupVec), &_CAST_VECA4(initLookup), &_CAST_VECA4(diffVTotal));
	}
}

float
CAFXResourceMgr::GetAzimuthalCoeff(int l)
{
	_DEBUG_ASSERT(l < 7);

	// TODO: See analytical forumula in Ranamoorthy's paper

	const static float AZIMUTH_COEFF[7] =
	{
		3.141593f,// * 0.4f,
		2.094395f,// * 0.52f,
		0.785398f,// * 0.89f,
		0.0f,
		-0.130900f,
		0.0f,
		0.049087f
	};

	return AZIMUTH_COEFF[l];	
}

void
CAFXResourceMgr::CompressOneFaceAllBands(int numSHBands, 
										 const gmtl::VecA3f& initLookup,
										 const gmtl::VecA3f& diffU,
										 const gmtl::VecA3f& diffV,
										 uint* pARGB,
										 uint numPixelsPerSide, 
										 float* pDiffAngles,
										 float toneMapContrast,
										 gmtl::Vec3f* pRGBOut)
{
	_LOOP(l, numSHBands)
	{
		for(int m=-l; m <= l; m++)
		{
			gmtl::VecA3f coeffOut(0.0f, 0.0f, 0.0f);
			CompressOneFace(initLookup, diffU, diffV, pARGB, pDiffAngles, numPixelsPerSide, l, m, toneMapContrast, coeffOut);
			gmtl::Vec3f& toStore = pRGBOut[l*(l+1)+m];
			toStore[0] += coeffOut[0];
			toStore[1] += coeffOut[1];
			toStore[2] += coeffOut[2];
		}
	}
}

float
CAFXResourceMgr::ComputeDifferentialAngles(float* pToStore, uint numPixelsPerSide)
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
CAFXResourceMgr::CompressCubeMap(IRTextureCube* pTexCube, int numSHBands, float toneMapContrast, gmtl::Vec3f* pRGBOut)
{
	// TODO: Assumes cube map is in ARGB format
	_DEBUG_ASSERT(pTexCube->GetFormat() == TEXF_A8R8G8B8);

	uint numPixelsPerSide	= pTexCube->GetSize(0);
	float halfPixelOffset	= 1.0f / (numPixelsPerSide);
	float fullPixelOffset	= halfPixelOffset * 2.0f;

	_LOOPi(numSHBands*numSHBands)
	{
		pRGBOut[i][0] = 0.0f;
		pRGBOut[i][1] = 0.0f;
		pRGBOut[i][2] = 0.0f;
	}

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
	uint pitch	= 0;
	uint* pARGB = (uint*) pTexCube->Lock(0, CF_POS_X, pitch, &toWrite);
	_DEBUG_ASSERT(pitch == (sizeof(uint) * numPixelsPerSide));
	CompressOneFaceAllBands(numSHBands ,initLookup, diffU, diffV, pARGB, numPixelsPerSide, pDiffAngles, toneMapContrast, pRGBOut);
	pTexCube->Unlock(0, CF_POS_X);

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

	pARGB = (uint*) pTexCube->Lock(0, CF_NEG_X, pitch, &toWrite);
	_DEBUG_ASSERT(pitch == (sizeof(uint) * numPixelsPerSide));
	CompressOneFaceAllBands(numSHBands ,initLookup, diffU, diffV, pARGB, numPixelsPerSide, pDiffAngles, toneMapContrast, pRGBOut);
	pTexCube->Unlock(0, CF_NEG_X);

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

	pARGB = (uint*) pTexCube->Lock(0, CF_POS_Y, pitch, &toWrite);
	_DEBUG_ASSERT(pitch == (sizeof(uint) * numPixelsPerSide));
	CompressOneFaceAllBands(numSHBands ,initLookup, diffU, diffV, pARGB, numPixelsPerSide, pDiffAngles, toneMapContrast, pRGBOut);
	pTexCube->Unlock(0, CF_POS_Y);

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

	pARGB = (uint*) pTexCube->Lock(0, CF_NEG_Y, pitch, &toWrite);
	_DEBUG_ASSERT(pitch == (sizeof(uint) * numPixelsPerSide));
	CompressOneFaceAllBands(numSHBands ,initLookup, diffU, diffV, pARGB, numPixelsPerSide, pDiffAngles, toneMapContrast, pRGBOut);
	pTexCube->Unlock(0, CF_NEG_Y);

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

	pARGB = (uint*) pTexCube->Lock(0, CF_POS_Z, pitch, &toWrite);
	_DEBUG_ASSERT(pitch == (sizeof(uint) * numPixelsPerSide));
	CompressOneFaceAllBands(numSHBands ,initLookup, diffU, diffV, pARGB, numPixelsPerSide, pDiffAngles, toneMapContrast, pRGBOut);
	pTexCube->Unlock(0, CF_POS_Z);

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

	pARGB = (uint*) pTexCube->Lock(0, CF_NEG_Z, pitch, &toWrite);
	_DEBUG_ASSERT(pitch == (sizeof(uint) * numPixelsPerSide));
	CompressOneFaceAllBands(numSHBands ,initLookup, diffU, diffV, pARGB, numPixelsPerSide, pDiffAngles, toneMapContrast, pRGBOut);
	pTexCube->Unlock(0, CF_NEG_Z);

	// Weight luminance constants
	float weight = (4.0f * gmtl::Math::PI) / (weightPerFace * 6);
	_LOOPi(numSHBands*numSHBands)
	{
		gmtl::Vec3f& toStore = pRGBOut[i];
		toStore[0] *= weight;
		toStore[1] *= weight;
		toStore[2] *= weight;
	}

	_FREE(pDiffAngles);
}

void	
CAFXResourceMgr::CompressAmbientCubeMap(IRTextureCube* pTexCube, AFXAmbientCoeffs* pRGBOut, float freqContrast, float brightenFactor)
{
	// Compress cube map using the defined number of bands
	gmtl::Vec3f rgb[NUM_SH_BANDS*NUM_SH_BANDS];

	// TODO: 
	_DEBUG_ASSERT(NUM_SH_BANDS <= 3);

	CompressCubeMap(pTexCube, NUM_SH_BANDS, 1.0f, rgb);

	//rgb[0][0] = 0.79f;
	//rgb[0][1] = 0.44f;
	//rgb[0][2] = 0.54f;
	//rgb[1][0] = 0.39f;
	//rgb[1][1] = 0.35f;
	//rgb[1][2] = 0.60f;
	//rgb[2][0] = -0.34f;
	//rgb[2][1] = -0.18f;
	//rgb[2][2] = -0.27f;
	//rgb[3][0] = -0.29f;
	//rgb[3][1] = -0.06f;
	//rgb[3][2] = -0.01f;
	//rgb[4][0] = -0.11f;
	//rgb[4][1] = -0.06f;
	//rgb[4][2] = 0.01f;
	//rgb[5][0] = -0.26f;
	//rgb[5][1] = -0.22f;
	//rgb[5][2] = -0.47f;
	//rgb[6][0] = -0.16f;
	//rgb[6][1] = -0.09f;
	//rgb[6][2] = -0.15f;
	//rgb[7][0] = 0.56f;
	//rgb[7][1] = 0.21f;
	//rgb[7][2] = 0.14f;
	//rgb[8][0] = 0.21f;
	//rgb[8][1] = -0.05f;
	//rgb[8][2] = 0.3f;

	// Scale by azimuthal term SH coefficients and basis functions
	// In effect, creating a more "contrasty" frequency
	_LOOP(l, NUM_SH_BANDS)
	{
		//float azimuthCoeff = GetAzimuthalCoeff(l);

		for(int m=-l; m <= l; m++)
		{
			uint i = l*(l+1)+m;
			float shCollapsed = EvaluateSHPartialCartesian(i);
			float contrastAmount = shCollapsed * freqContrast + (1.0f - freqContrast);
			gmtl::Vec3f& toStore = rgb[i];
			//toStore[0] *= azimuthCoeff;
			toStore[0] *= contrastAmount;
			//toStore[1] *= azimuthCoeff;
			toStore[1] *= contrastAmount;
			//toStore[2] *= azimuthCoeff;
			toStore[2] *= contrastAmount;
		}

		for(int m=-l; m <= l; m++)
		{
			uint i = l*(l+1)+m;
			gmtl::Vec3f& toStore = rgb[i];
			toStore[0] *= brightenFactor;
			toStore[1] *= brightenFactor;
			toStore[2] *= brightenFactor;
		}
	}

	//memset(rgb, 0, sizeof(rgb));
	//rgb[8][0] = 1.0f * EvaluateSHPartialCartesian(8);
	//rgb[8][1] = -1.0f * EvaluateSHPartialCartesian(8);
	//rgb[8][2] = 0.0f * EvaluateSHPartialCartesian(8);

	//_LOOPi(NUM_SH_BANDS * NUM_SH_BANDS)
	//{
	//	pRGBOut->color[i] = rgb[i];
	//}

	// Swizzle output to be GPU/SIMD friendly
	pRGBOut->rTerm1[0]	= rgb[0][0];
	pRGBOut->rTerm1[1]	= rgb[1][0];
	pRGBOut->rTerm1[2]	= rgb[2][0];
	pRGBOut->rTerm1[3]	= rgb[3][0];
	pRGBOut->rTerm2[0]	= rgb[4][0];
	pRGBOut->rTerm2[1]	= rgb[5][0];
	pRGBOut->rTerm2[2]	= rgb[6][0];
	pRGBOut->rTerm2[3]	= rgb[7][0];
	
	pRGBOut->gTerm1[0]	= rgb[0][1];
	pRGBOut->gTerm1[1]	= rgb[1][1];
	pRGBOut->gTerm1[2]	= rgb[2][1];
	pRGBOut->gTerm1[3]	= rgb[3][1];
	pRGBOut->gTerm2[0]	= rgb[4][1];
	pRGBOut->gTerm2[1]	= rgb[5][1];
	pRGBOut->gTerm2[2]	= rgb[6][1];
	pRGBOut->gTerm2[3]	= rgb[7][1];

	pRGBOut->bTerm1[0]	= rgb[0][2];
	pRGBOut->bTerm1[1]	= rgb[1][2];
	pRGBOut->bTerm1[2]	= rgb[2][2];
	pRGBOut->bTerm1[3]	= rgb[3][2];
	pRGBOut->bTerm2[0]	= rgb[4][2];
	pRGBOut->bTerm2[1]	= rgb[5][2];
	pRGBOut->bTerm2[2]	= rgb[6][2];
	pRGBOut->bTerm2[3]	= rgb[7][2];
	
	pRGBOut->rgbTerm[0] = rgb[8][0];
	pRGBOut->rgbTerm[1] = rgb[8][1];
	pRGBOut->rgbTerm[2] = rgb[8][2];
}

IAFXParamPool* CAFXResourceMgr::CreateParamPool()
{
	return _NEW CAFXParamPool();
}


_NAMESPACE_END
