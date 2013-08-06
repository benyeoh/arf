//================================================================================
//
//		BFXUPoissonBlurParams.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/5/2012
//
//================================================================================

#include "BaseFXUtils.h"

_NAMESPACE_BEGIN
	
IRTexture2D* BFXUPoissonBlurParams::CreateNoiseTexture(IRRenderer* pRenderer, uint width, uint height)
{
	wchar dataBuffer[256];
	dataBuffer[0] = (wchar) width;
	dataBuffer[1] = (wchar) height;

	wstring dataName = _W("___PoissonBlurParams::Noise::");
	_DEBUG_ASSERT(dataName.length() < 256 - 2);
	memcpy(&(dataBuffer[2]), (void*) dataName.c_str(), dataName.length() * sizeof(wchar));

	CRCDataKey key;
	key.Set(dataBuffer, sizeof(wchar) * (dataName.size() + 2));
	IRTexture2D* pNoise = pRenderer->GetRResourceMgr().LoadTexture2D(&key, NULL);
	if(!pNoise)
	{
		pNoise = pRenderer->GetRResourceMgr().CreateTexture2D(&key, width, height, 1, TEXF_A8R8, TEXU_DEFAULT);

		uint pitch;
		byte* pDest = pNoise->Lock(0, pitch, NULL);

		_LOOPi(pNoise->GetHeight(0))
		{
			_LOOPj(pNoise->GetWidth(0))
			{
				float x = float(::rand())/float(RAND_MAX) * 2.0f - 1.0f;
				float y = float(::rand())/float(RAND_MAX) * 2.0f - 1.0f;
				float length = sqrt(x * x + y * y);
				x = x / length;
				y = y / length;

				byte xVal = (byte) (x * 255.0f);
				byte yVal = (byte) (y * 255.0f);
				pDest[j*2]		= xVal;
				pDest[j*2+1]	= yVal; 
			}

			pDest += pitch;
		}

		pNoise->Unlock(0);
	}

	return pNoise;
}

void BFXUPoissonBlurParams::InitializePoissonFilter(eBFXUPoissonBlurSamples numSamples, IRRenderer* pRenderer, float spreadScale, float texWidth, float texHeight, boolean isUseNoise)
{
	const static uint NOISE_TEX_SIZE = 32;

	if(isUseNoise)
	{
		m_pNoiseTex = CreateNoiseTexture(pRenderer, NOISE_TEX_SIZE, NOISE_TEX_SIZE);
		
		m_NoiseScale[0] = texWidth / NOISE_TEX_SIZE;
		m_NoiseScale[1] = texHeight / NOISE_TEX_SIZE;
	}

	m_Scale = spreadScale;
	m_PoissonDiskSamples = numSamples;
}

eBFXUPoissonBlurSamples BFXUPoissonBlurParams::GetPoissonSamples()
{
	return m_PoissonDiskSamples;
}

float BFXUPoissonBlurParams::GetScale()
{
	return m_Scale;
}

IRTexture2D* BFXUPoissonBlurParams::GetNoiseTex()
{
	return m_pNoiseTex;
}

const gmtl::Vec2f* BFXUPoissonBlurParams::GetNoiseScale()
{
	return &m_NoiseScale;
}

_NAMESPACE_END