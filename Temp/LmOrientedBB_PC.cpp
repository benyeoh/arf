/**
 *  @file       LmOrientedBB_PC.cpp
 *  @brief
 *
 *	@date       2013/7/30
 *	@author     Ben Yeoh
 *
 */

#include "LmPrecompile.h"
#include "Math/LmVector3.h"
#include "Math/LmAABB.h"
#include "Math/LmOrientedBB.h"
#include "Math/LmSphere.h"
#include "Math/LmMatrix.h"
#include "Math/LmMatrix4.h"

LM_NS_BEGIN_MATH

inline void LmOrientedBB::TransformToPoints(LmMatrix4Arg mat, LmVector4 toStore[8]) const
{
    __m128 corner   = _mm_sub_ps(Center().Vec128(), XHalfExtent().Vec128());
    corner          = _mm_sub_ps(corner , YHalfExtent().Vec128());
    corner          = _mm_sub_ps(corner , ZHalfExtent().Vec128());

    __m128 cornerX = _mm_shuffle_ps(corner, corner, _MM_SHUFFLE(0, 0, 0, 0));
    __m128 cornerZ = _mm_shuffle_ps(corner, corner, _MM_SHUFFLE(2, 2, 2, 2));
    __m128 cornerY = _mm_shuffle_ps(corner, corner, _MM_SHUFFLE(1, 1, 1, 1));

    // Transform the corner
    cornerX = _mm_mul_ps(cornerX, mat.GetXAxis().Vec128());
    cornerY = _mm_mul_ps(cornerY, mat.GetYAxis().Vec128());
    cornerZ = _mm_mul_ps(cornerZ, mat.GetZAxis().Vec128());
    cornerX = _mm_add_ps(cornerX, cornerY);
    cornerZ = _mm_add_ps(cornerZ, mat.GetTranslation().Vec128());
    __m128 transformedCorner = _mm_add_ps(cornerX, cornerZ);

    __m128 fullExtentX = _mm_add_ps(XHalfExtent().Vec128(), XHalfExtent().Vec128());
    __m128 fullExtentY = _mm_add_ps(YHalfExtent().Vec128(), YHalfExtent().Vec128());
    __m128 fullExtentZ = _mm_add_ps(ZHalfExtent().Vec128(), ZHalfExtent().Vec128());

    __m128 extentXx = _mm_shuffle_ps(fullExtentX, fullExtentX, _MM_SHUFFLE(0, 0, 0, 0));
    __m128 extentYx = _mm_shuffle_ps(fullExtentY, fullExtentY, _MM_SHUFFLE(0, 0, 0, 0));
    __m128 extentZx = _mm_shuffle_ps(fullExtentZ, fullExtentZ, _MM_SHUFFLE(0, 0, 0, 0));
    __m128 extentXy = _mm_shuffle_ps(fullExtentX, fullExtentX, _MM_SHUFFLE(1, 1, 1, 1));
    __m128 extentYy = _mm_shuffle_ps(fullExtentY, fullExtentY, _MM_SHUFFLE(1, 1, 1, 1));
    __m128 extentZy = _mm_shuffle_ps(fullExtentZ, fullExtentZ, _MM_SHUFFLE(1, 1, 1, 1));
    __m128 extentXz = _mm_shuffle_ps(fullExtentX, fullExtentX, _MM_SHUFFLE(2, 2, 2, 2));
    __m128 extentYz = _mm_shuffle_ps(fullExtentY, fullExtentY, _MM_SHUFFLE(2, 2, 2, 2));
    __m128 extentZz = _mm_shuffle_ps(fullExtentZ, fullExtentZ, _MM_SHUFFLE(2, 2, 2, 2));

    extentXx = _mm_mul_ps(extentXx, mat.GetXAxis().Vec128());
    extentYx = _mm_mul_ps(extentYx, mat.GetXAxis().Vec128());
    extentZx = _mm_mul_ps(extentZx, mat.GetXAxis().Vec128());
    extentXy = _mm_mul_ps(extentXy, mat.GetYAxis().Vec128());
    extentYy = _mm_mul_ps(extentYy, mat.GetYAxis().Vec128());
    extentZy = _mm_mul_ps(extentZy, mat.GetYAxis().Vec128());
    extentXz = _mm_mul_ps(extentXz, mat.GetZAxis().Vec128());
    extentYz = _mm_mul_ps(extentYz, mat.GetZAxis().Vec128());
    extentZz = _mm_mul_ps(extentZz, mat.GetZAxis().Vec128());

    __m128 transformedX = _mm_add_ps(extentXx, extentXy);
    transformedX = _mm_add_ps(transformedX, extentXz);
    __m128 transformedY = _mm_add_ps(extentYx, extentYy);
    transformedY = _mm_add_ps(transformedY, extentYz);
    __m128 transformedZ = _mm_add_ps(extentZx, extentZy);
    transformedZ = _mm_add_ps(transformedZ, extentZz);

    // Now we get the eight points of the OBB
    __m128 dx = _mm_add_ps(transformedCorner, transformedX);	// dx
    __m128 dy = _mm_add_ps(transformedCorner, transformedY);	// dy
    __m128 dxdy = _mm_add_ps(dx, transformedY);	// dx + dy

    __m128 dz = _mm_add_ps(transformedCorner, transformedZ);	// dz
    __m128 dxdz = _mm_add_ps(dx, transformedZ);	// dx + dz
    __m128 dydz = _mm_add_ps(dy, transformedZ);	// dy + dz
    __m128 dxdydz = _mm_add_ps(dxdy, transformedZ);	// dx + dy + dz

    toStore[0].Vec128() = transformedCorner;
    toStore[1].Vec128() = dx;
    toStore[2].Vec128() = dy;
    toStore[3].Vec128() = dxdy;
    toStore[4].Vec128() = dz;
    toStore[5].Vec128() = dxdz;
    toStore[6].Vec128() = dydz;
    toStore[7].Vec128() = dxdydz;
}


LM_NS_END_MATH