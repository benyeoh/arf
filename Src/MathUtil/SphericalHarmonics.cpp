//==============================================================================
//
//		SphericalHarmonics.cpp
//
//			See Robin Green's paper
//
//		Author:		Ben Yeoh
//		Date:		2/9/2008
//
//==============================================================================

#include "MathUtil.h"

_NAMESPACE_BEGIN

void CartesianToSpherical(gmtl::Vec2f& dest, const gmtl::Vec3f& source)
{
	// Elevation = theta = dest[0] -> [0, PI]
	// Azimuth = phi = dest[1] - > [0, 2PI]

	// Elevation
	dest[0] = gmtl::Math::aCos(source[1]);

	// Azimuth
	dest[1] = gmtl::Math::aTan2(-source[2], source[0]);
	if(dest[1] < 0.0f)
		dest[1] += gmtl::Math::TWO_PI;

	//if( dest[0] < gmtl::GMTL_EPSILON || 
	//	dest[0] > (gmtl::Math::PI - gmtl::GMTL_EPSILON) )
	//{
	//	dest[1] = 0.0f;
	//}
	//else
	//{
	//	float sinTheta = gmtl::Math::sin(dest[0]);
	//	dest[1] = gmtl::Math::aCos(source[0] / sinTheta);
	//	if(source[2] > 0)
	//	{
	//		dest[1] = gmtl::Math::TWO_PI - dest[1];
	//	}
	//}
}

void SphericalToCartesian(gmtl::Vec3f& dest, const gmtl::Vec2f& source)
{
	// Elevation = theta = source[0] -> [0, PI]
	// Azimuth = phi = source[1] - > [0, 2PI]

	float sinElevation	= gmtl::Math::sin(source[0]);
	float sinAzimuth = gmtl::Math::sin(source[1]);
	float cosElevation = gmtl::Math::cos(source[0]);
	float cosAzimuth = gmtl::Math::cos(source[1]);

	dest[0] = sinElevation * cosAzimuth;
	dest[1] = cosElevation;
	dest[2] = -(sinElevation * sinAzimuth);
}

float EvaluateLegendrePoly(int l, int m, float x)
{
	float pmm = 1.0f;
	if(m > 0)
	{
		float somx2 = gmtl::Math::sqrt((1.0f - x) * (1.0f + x));
		float fact	= 1.0f;
		for(int i=1; i <= m; i++)
		{
			pmm *= (-fact) * somx2;
			fact += 2.0f;
		}
	}

	if(l == m)
		return pmm;

	float pmmp1 = x * (2.0f * m + 1.0f) * pmm;
	if(l == (m+1))
		return pmmp1;

	float pll = 0.0f;
	for(int ll=m+2; ll <= l; ++ll)
	{
		pll = ( (2.0f * ll - 1.0f) * x * pmmp1 - (ll + m - 1.0f) * pmm ) / (ll - m);
		pmm = pmmp1;
		pmmp1 = pll;
	}

	return pll;
}

float EvaluateReNorm(int l, int m)
{
	float top		= ((2.0f * l + 1.0f) * FastFactorial(l-m));
	float bottom	= (4.0f * gmtl::Math::PI * FastFactorial(l+m));

	return gmtl::Math::sqrt( top / bottom );
}

float EvaluateSHFull(int l, int m, const gmtl::Vec2f& polar)
{
	const static float SQRT_2 = 1.41421f;
	if(m == 0)
		return EvaluateReNorm(l, 0) * EvaluateLegendrePoly(l, m, gmtl::Math::cos(polar[0]));
	else if(m > 0)
		return SQRT_2 * EvaluateReNorm(l, m) * gmtl::Math::cos(m * polar[1]) * EvaluateLegendrePoly(l, m, gmtl::Math::cos(polar[0]));

	return SQRT_2 * EvaluateReNorm(l, -m) * gmtl::Math::sin(-m * polar[1]) * EvaluateLegendrePoly(l, m, gmtl::Math::cos(polar[0]));
}

float EvaluateSHFastCartesian(uint i, const gmtl::Vec3f& dir)
{
	_DEBUG_ASSERT(i < 16);

	float sh = EvaluateSHPartialCartesian(i);
	
	float x = dir[0];
	float y = -dir[2];
	float z = dir[1];

	switch(i)
	{
		case 1:		sh *= y; break;
		case 2:		sh *= z; break;
		case 3:		sh *= x; break;

		case 4:		sh *= (y * x); break;
		case 5:		sh *= (y * z); break;
		case 6:		sh *= (3.0f * z * z - 1.0f); break;
		case 7:		sh *= (x * z); break;
		case 8:		sh *= (x * x - y * y); break;

		case 9:		sh *= (y * (3.0f * x * x - y * y)); break;
		case 10:	sh *= (y * x * z); break;
		case 11:	sh *= (y * (-1.0f + 5.0f * z * z)); break;
		case 12:	sh *= (z * (5.0f * z * z - 3.0f)); break;
		case 13:	sh *= (x * (-1.0f + 5.0f * z * z)); break;
		case 14:	sh *= (z * (x * x - y * y)); break;
		case 15:	sh *= (x * (x * x - 3.0f * y * y)); break;

		//case 1:		sh *= -y; break;
		//case 2:		sh *= z; break;
		//case 3:		sh *= -x; break;
		//
		//case 4:		sh *= (y * x); break;
		//case 5:		sh *= -(y * z); break;
		//case 6:		sh *= (3.0f * z * z - 1.0f); break;
		//case 7:		sh *= -(x * z); break;
		//case 8:		sh *= (x * x - y * y); break;
		//case 9:		sh *= -(y * (3.0f * x * x - y * y)); break;

		//case 10:	sh *= (y * x * z); break;
		//case 11:	sh *= -(y * (-1.0f + 5.0f * z * z)); break;
		//case 12:	sh *= (z * (5.0f * z * z - 3.0f)); break;
		//case 13:	sh *= -(x * (-1.0f + 5.0f * z * z)); break;
		//case 14:	sh *= (z * (x * x - y * y)); break;
		//case 15:	sh *= -(x * (x * x - 3.0f * y * y)); break;

		//case 16:    sh *= (y * x * (x * x - y * y)); break;
		//case 17:    sh *= -(y * (3.0f * x * x - y * y) * z); break;
		//case 18:    sh *= (y * x * (-1.0f + 7.0f * z * z)); break;
		//case 19:    sh *= -(y * z * (-3.0f + 7.0f * z * z)); break;
		//case 20:    sh *= (35.0f * z * z * z * z - 30.0f * z * z + 3.0f); break;
		//case 21:    sh *= -(x * z * (-3.0f + 7.0f * z * z)); break;
		//case 22:    sh *= ((x * x - y * y) * (-1.0f + 7.0f * z * z)); break;
		//case 23:    sh *= -(x * (x * x - 3.0f * y * y) * z); break;
		//case 24:    sh *= (x * x * x * x - 6.0f * y * y * x * x + y * y * y * y); break;

		//case 25:    sh *= -(y * (5.0f * x * x * x * x - 10.0f * y * y * x * x + y * y * y * y) ); break;
		//case 26:    sh *= (y * x * (x * x - y * y) * z); break;
		//case 27:    sh *= -(y * (3.0f * x * x - y * y) * (-1.0f + 9.0f * z * z)); break;
		//case 28:    sh *= (y * x * z * (-1.0f + 3.0f * z * z)); break;
		//case 29:    sh *= -(y * (-14.0f * z * z + 21.0f * z * z * z * z + 1.0f)); break;
		//case 30:    sh *= (z * (63.0f * z * z * z * z - 70.0f * z * z + 15.0f)); break;
		//case 31:    sh *= -(x * (-14.0f * z * z + 21.0f * z * z * z * z + 1.0f)); break;
		//case 32:    sh *= ((x * x - y * y) * z * (-1.0f + 3.0f * z * z)); break;
		//case 33:    sh *= -(x * (x * x - 3.0f * y * y) * (-1.0f + 9.0f * z * z)); break;
		//case 34:    sh *= ((x * x * x * x - 6.0f * y * y * x * x + y * y * y * y) * z); break;
		//case 35:    sh *= -(x * (x * x * x * x - 10.0f * y * y * x * x + 5.0f * y * y * y * y)); break;
	}

	return sh;
}

float EvaluateSHPartialCartesian(uint i)
{
	// TODO: Only first 6 bands
	//const static ubiFloat SH_COLLAPSED[36] =
	//{
	//	0.282095f,

	//	0.488602f,
	//	0.488602f,
	//	0.488602f,

	//	1.092548f,
	//	1.092548f,
	//	0.315392f,
	//	1.092548f,
	//	0.546274f,

	//	0.590044f,
	//	2.890611f,
	//	0.457046f,
	//	0.373176f,
	//	0.457046f,
	//	1.445306f,
	//	0.590044f,

	//	2.503342942f,
	//	1.77013077f,
	//	0.946174696f,
	//	0.669046544f,
	//	0.105785547f,
	//	0.669046544f,
	//	0.473087348f,
	//	1.77013077f,
	//	0.625835735f,

	//	0.656382057f,
	//	8.30264926f,
	//	0.489238299f,
	//	4.793536785f,
	//	0.452946651f,
	//	0.116950322f,
	//	0.452946651f,
	//	2.396768392f,
	//	0.489238299f,
	//	2.075662315f,
	//	0.656382057f,
	//};

	// TODO: Only first 4 bands
	const static float SH_COLLAPSED[16] =
	{
		0.282095f,

		0.488602f,
		0.488602f,
		0.488602f,
		
		1.092548f,
		1.092548f,
		0.315392f,
		1.092548f,
		0.546274f,
		
		0.590044f,
		2.890611f,
		0.457046f,
		0.373176f,
		0.457046f,
		1.445306f,
		0.590044f,
	};

	_DEBUG_ASSERT(i < 16);

	return SH_COLLAPSED[i];
}

void AddTripleProduct(float* pDest, float weight, const gmtl::VecA3f& dir, uint sampleIndex, void* pUserInfo)
{
	uint ijk = (uint) (size_t) pUserInfo;
	uint i = (ijk >> 16) & 0xFF;
	uint j = (ijk >> 8) & 0xFF;
	uint k = ijk & 0xFF;

	*pDest += ((EvaluateSHFastCartesian(i, _CAST_VEC3(dir)) *
			EvaluateSHFastCartesian(j, _CAST_VEC3(dir)) *
			EvaluateSHFastCartesian(k, _CAST_VEC3(dir))) * weight);
}

void EvaluateSHTripleProductTable(float* pDest, uint numBands, IAppCallback* pCallback)
{
	// Naive way of doing it, but just to be safe
	// Should take advantage of symmetry
	uint numBandsSq = numBands * numBands;
	uint index = 0;
	uint ijk = 0;
	_LOOPi(numBandsSq)
	{
		if(pCallback)
			pCallback->Log(ToStringAuto(_W("i: %d\n"), i));

		_LOOPj(numBandsSq)
		{
			_LOOPk(numBandsSq)
			{
				ijk = i << 16 | j << 8 | k;
				ComputeMonteCarloCubeSampling(pDest + index, 1, 32, AddTripleProduct, (void*)(size_t) ijk);
				index++;
			}
		}
	}
}

void EvaluateSHTripleProductTransfer4(Matrix1616f& dest, float* pTriProdTable, float* pSHCoeffs)
{
	_LOOPi(16)
	{
		_LOOPj(16)
		{
			dest[i][j] = 0.0f;

			_LOOPk(16)
			{
				dest[i][j] += (pTriProdTable[i*16*16 + j*16 + k] * pSHCoeffs[k]);
			}
		}
	}
}

void EvaluateSHTripleProductTransfer3(Matrix99f& dest, float* pTriProdTable, float* pSHCoeffs)
{
	_LOOPi(9)
	{
		_LOOPj(9)
		{
			dest[i][j] = 0.0f;

			_LOOPk(9)
			{
				dest[i][j] += (pTriProdTable[i*9*9 + j*9 + k] * pSHCoeffs[k]);
			}
		}
	}
}

void EvaluateSHTripleProduct(float* pDest, float* pTriProdTable, float* pF, float* pG, uint numBands)
{
	uint numBandsSq = numBands * numBands;
	_LOOPi(numBandsSq)
	{
		float coeff = 0.0f;
		_LOOPj(numBandsSq)
		{
			_LOOPk(numBandsSq)
			{
				coeff += (pTriProdTable[i*numBandsSq*numBandsSq + j*numBandsSq + k] * pF[j] * pG[k]);
			}
		}

		pDest[i] = coeff;
	}
}


void ExpandZHToSH(const float* pZH, float* pSH, uint numBands, const gmtl::Vec3f& lobeAxis)
{
	// Note: Expands a folded ZH
	//const float FOUR_PI = 4.0f * gmtl::Math::PI;
	const float FOUR_PI_RCP = 1.0f / (4.0f * gmtl::Math::PI);

	uint prevNumInBand = 0;
	_LOOPi(numBands)
	{
		uint numInBand = i * 2 + 1;

		float zhMod = /*gmtl::Math::sqrt(FOUR_PI / (2.0f * i + 1.0f)) **/ pZH[i] / ((2.0f * i + 1.0f) * FOUR_PI_RCP);
		_LOOPj(numInBand)
		{
			pSH[prevNumInBand+j] = EvaluateSHFastCartesian(prevNumInBand+j, lobeAxis) * zhMod;
		}

		prevNumInBand += numInBand;
	}

	//memset(pSH, 0, sizeof(float) * numBands * numBands);

	//uint lastIndex = 0;
	//_LOOPi(numBands)
	//{
	//	uint curIndex = lastIndex + i * 2;
	//	pSH[curIndex] = pZH[i];
	//	lastIndex = curIndex;
	//}
}

void EvaluateOptimalLinearZH(const float* pSH, gmtl::Vec3f& optimalLinearDest)
{
	// Find optimal linear axis
	// Note: Flip x,y values if using the SH constants from Sloan et al
	gmtl::Vec3f lobeAxis;
	lobeAxis[0] = pSH[3];
	lobeAxis[1] = pSH[1];
	lobeAxis[2] = pSH[2];
	gmtl::normalize(lobeAxis);

	optimalLinearDest[0] = lobeAxis[0];
	optimalLinearDest[1] = lobeAxis[2];
	optimalLinearDest[2] = -lobeAxis[1];
}

void CompressSHToZH(const float* pSH, float* pZH, uint numBands, const gmtl::Vec3f& lobeAxis)
{
	// Returns a folded ZH

	// Compress using optimal linear axis
	_DEBUG_ASSERT(numBands > 1);

	//const float FOUR_PI_RCP = 1.0f / (4.0f * gmtl::Math::PI);

	uint prevNumInBand = 0;
	_LOOPi(numBands)
	{
		uint numInBand = i * 2 + 1;
		float proj = 0;
		_LOOPj(numInBand)
		{
			proj += ( EvaluateSHFastCartesian(prevNumInBand+j, lobeAxis) * pSH[prevNumInBand+j] ); 
		}

		pZH[i] = proj; // / ((2.0f * i + 1.0f) * FOUR_PI_RCP);

		prevNumInBand += numInBand;
	}
}

void RotateSH3(const float* pSH, float* pOut, const gmtl::Matrix33f& transformMat)
{
	// Decompose rotation matrix to YZY euler angles
	// Find the line of nodes
	gmtl::Vec3f yAxis;
	yAxis.set(transformMat[1][0], transformMat[1][1], transformMat[1][2]);

	gmtl::Vec3f zAxis;
	zAxis.set(transformMat[2][0], transformMat[2][1], transformMat[2][2]);

	gmtl::Vec3f lineOfNodes;
	if(yAxis[1] >= 0.99f)
	{
		// Rotated y-axis is parallel to the reference y-axis
		// Therefore the line of nodes is simply the rotated z-axis
		lineOfNodes = zAxis;
	}
	else
	{
		gmtl::cross( lineOfNodes, gmtl::Vec3f(0.0f, 1.0f, 0.0f), yAxis );
	}

	// Rotation about ref Y
	float alpha = atan2(lineOfNodes[0], lineOfNodes[2]);

	// Rotation about rotated Z / lineOfNodes
	gmtl::Vec3f lineOfNodesCrossUp;
	gmtl::cross( lineOfNodesCrossUp, lineOfNodes, gmtl::Vec3f(0.0f, 1.0f, 0.0f) );
	float beta = atan2( gmtl::dot( lineOfNodesCrossUp, yAxis ), yAxis[1] );

	// Rotation about rotated Y
	gmtl::Vec3f yAxisCrossLineOfNodes;
	gmtl::cross( yAxisCrossLineOfNodes, yAxis, lineOfNodes );
	float gamma = atan2( gmtl::dot( yAxisCrossLineOfNodes, zAxis), gmtl::dot( lineOfNodes, zAxis ) );

	// TODO ...
}

_NAMESPACE_END