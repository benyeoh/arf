//==============================================================================
//
//		VectorFns.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		13/10/2009
//
//==============================================================================

#include "BaseFXUtils.h"

_NAMESPACE_BEGIN

// TODO: Move this to DeferredFXUtils
void
ComputeCameraFarPlaneCornerVecs(gmtl::VecA3f* pToStore, gmtl::VecA3f& eyePos, gmtl::MatrixA44f& invViewProj)
{
	gmtl::VecA4f screenSpaceFarBounds[4];

	screenSpaceFarBounds[0][0] = -1.0f; screenSpaceFarBounds[0][1] = 1.0f; screenSpaceFarBounds[0][2] = 1.0f; screenSpaceFarBounds[0][3] = 1.0f; // far top left
	screenSpaceFarBounds[1][0] = 1.0f; screenSpaceFarBounds[1][1] = 1.0f; screenSpaceFarBounds[1][2] = 1.0f; screenSpaceFarBounds[1][3] = 1.0f; // far top right
	screenSpaceFarBounds[2][0] = -1.0f; screenSpaceFarBounds[2][1] = -1.0f; screenSpaceFarBounds[2][2] = 1.0f; screenSpaceFarBounds[2][3] = 1.0f; // far bottom left
	screenSpaceFarBounds[3][0] = 1.0f; screenSpaceFarBounds[3][1] = -1.0f; screenSpaceFarBounds[3][2] = 1.0f; screenSpaceFarBounds[3][3] = 1.0f; // far bottom right

	gmtl::VecA4f frustumPtsWorldSpace[4];
	BatchTransformAndProjectVecW1(&invViewProj, screenSpaceFarBounds, frustumPtsWorldSpace, 4);

	VecVecSub(&_CAST_VECA4(pToStore[0]), &frustumPtsWorldSpace[0], &_CAST_VECA4(eyePos));
	VecVecSub(&_CAST_VECA4(pToStore[1]), &frustumPtsWorldSpace[1], &_CAST_VECA4(eyePos));
	VecVecSub(&_CAST_VECA4(pToStore[2]), &frustumPtsWorldSpace[2], &_CAST_VECA4(eyePos));
	VecVecSub(&_CAST_VECA4(pToStore[3]), &frustumPtsWorldSpace[3], &_CAST_VECA4(eyePos));
}

_NAMESPACE_END
