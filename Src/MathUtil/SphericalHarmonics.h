//==============================================================================
//
//		SphericalHarmonics.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		2/9/2008
//
//==============================================================================


#pragma once

_NAMESPACE_BEGIN

typedef gmtl::Matrix<float, 9, 9>	Matrix99f;
typedef gmtl::Matrix<float, 16, 16> Matrix1616f;
typedef gmtl::Vec<float, 9>			Vec9f;
typedef gmtl::Vec<float, 16>		Vec16f;

extern void CartesianToSpherical(gmtl::Vec2f& dest, const gmtl::Vec3f& source);

extern void SphericalToCartesian(gmtl::Vec3f& dest, const gmtl::Vec2f& source);

extern float EvaluateSHFull(int l, int m, const gmtl::Vec2f& polar);

extern float EvaluateSHFastCartesian(uint i, const gmtl::Vec3f& dir);

extern float EvaluateSHPartialCartesian(uint i);

extern void EvaluateSHTripleProductTable(float* pDest, uint numBands, IAppCallback* pCallback);

extern void EvaluateSHTripleProductTransfer4(Matrix1616f& dest, float* pTriProdTable, float* pSHCoeffs);

extern void EvaluateSHTripleProductTransfer3(Matrix99f& dest, float* pTriProdTable, float* pSHCoeffs);

extern void EvaluateSHTripleProduct(float* pDest, float* pTriProdTable, float* pF, float* pG, uint numBands);

extern void EvaluateOptimalLinearZH(const float* pSH, gmtl::Vec3f& optimalLinearDest);

extern void ExpandZHToSH(const float* pZH, float* pSH, uint numBands, const gmtl::Vec3f& lobeAxis);

extern void CompressSHToZH(const float* pSH, float* pZH, uint numBands, const gmtl::Vec3f& lobeAxis);

extern void RotateSH3(const float* pSH, float* pOut, const gmtl::Matrix33f& rotMat);

_NAMESPACE_END