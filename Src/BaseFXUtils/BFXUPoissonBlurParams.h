//================================================================================
//
//		BFXUPoissonBlurParams.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/5/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

class BFXUPoissonBlurParams
{
private: 
	float m_Scale;
	eBFXUPoissonBlurSamples m_PoissonDiskSamples;

	gmtl::Vec2f m_NoiseScale;
	IRTexture2DPtr m_pNoiseTex;

public: 
	BFXUPoissonBlurParams()
		: m_Scale(1.0f)
		, m_PoissonDiskSamples(POISSON_4_SAMPLE)
	{
	}

	~BFXUPoissonBlurParams()
	{
	}

private:
	IRTexture2D* CreateNoiseTexture(IRRenderer* pRenderer, uint width, uint height);

public: 
	void InitializePoissonFilter(eBFXUPoissonBlurSamples numSamples, IRRenderer* pRenderer, float spreadScale, float texWidth, float texHeight, boolean isUseNoise);

	eBFXUPoissonBlurSamples GetPoissonSamples();
	float GetScale();
	IRTexture2D* GetNoiseTex();
	const gmtl::Vec2f* GetNoiseScale();
};

_NAMESPACE_END
