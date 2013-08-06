//==============================================================================
//
//		VectorFns.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		13/10/2009
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

// TODO: Move this to DeferredFXUtils
void
ComputeCameraFarPlaneCornerVecs(gmtl::VecA3f* pToStore, gmtl::VecA3f& eyePos, gmtl::MatrixA44f& invViewProj);

_NAMESPACE_END