//==============================================================================
//
//		MonteCarloIntegrate.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		16/10/09
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

typedef void (*AddCubeDataInDir)(float* pDest, float weight, const gmtl::VecA3f& dir, uint sampleIndex, void* pUserInfo);

void	ComputeMonteCarloCubeSampling(float* pDest,
									  uint numChannels,
									  uint numSamplesPerSide, 
									  AddCubeDataInDir addCubeDataFn,
									  void* pUserInfo);

_NAMESPACE_END
