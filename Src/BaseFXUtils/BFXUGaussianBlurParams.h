//==============================================================================
//
//		BFXUGaussianBlurParams.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/21/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class BFXUGaussianBlurParams
{
private: 
	gmtl::Vec2f m_GaussianOffsets[BFXU_MAX_FILTER_KERNEL_SIZE*2+1];
	float m_GaussianWeights[BFXU_MAX_FILTER_KERNEL_SIZE*2+1];
	uint m_GaussianSize;
	
public: 
	BFXUGaussianBlurParams()
		: m_GaussianSize(0)
	{
	}

	~BFXUGaussianBlurParams()
	{
	}

private:
	float GetGaussianDistribution(float x, float rho);
	uint DetermineGaussianSize(float& standardDeviation);
	
public: 
	void InitializeGaussianFilter(float standardDeviation, 
								uint kernelSize,
								const gmtl::Vec2f* pSpreadDir,
								const gmtl::Vec2f* pConstantOffset,
								float weight,
								float texWidth, 
								float texHeight,
								boolean isReduceable=FALSE);

	//gmtl::Vec2f* GetGaussianOffsets();
	const gmtl::Vec2f* GetGaussianOffsets();
	const float* GetGaussianWeights();
	uint GetGaussianSize(); 
};

_NAMESPACE_END
