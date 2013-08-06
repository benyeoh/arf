//==============================================================================
//
//		BFXUGaussianBlurParams.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/21/2008
//
//==============================================================================

#include "BaseFXUtils.h"

_NAMESPACE_BEGIN

float 
BFXUGaussianBlurParams::GetGaussianDistribution(float x, float rho)
{
	float g = 1.0f / (sqrtf( 2.0f * 3.14159265359f ) * rho);
	g *= expf( -(x*x)/(2*rho*rho) );
	
	return g;
}

void 
BFXUGaussianBlurParams::InitializeGaussianFilter(float standardDeviation,
										uint kernelSize,
										const gmtl::Vec2f* pSpreadDir,  
										const gmtl::Vec2f* pConstantOffset,
										float weight,
										float texWidth, 
										float texHeight,
										boolean isReduceable)
{
	m_GaussianSize = (kernelSize % 2) == 0 ? kernelSize+1 : kernelSize;	
	m_GaussianSize = m_GaussianSize < 5 ? 5 : m_GaussianSize;
	
	// Texture offsets
	gmtl::Vec2f texelSize;
	texelSize[0] = 1.0f / texWidth;
	texelSize[1] = 1.0f / texHeight;

    uint index = 0;
	float totalWeight = 0.0f;

    uint offsetsPerSide = m_GaussianSize / 2;

    if(!isReduceable)
    {
		// Set the first index as the center
        m_GaussianOffsets[index][0] = 0.0f;
        m_GaussianOffsets[index][1] = 0.0f;
		
		float curWeight = GetGaussianDistribution(0.0f, standardDeviation);
		m_GaussianWeights[index] = curWeight;
		totalWeight += curWeight;
		index++;

        for(uint i=0; i < offsetsPerSide; ++i)
        {
            uint curOffset = offsetsPerSide - index + 1;
            m_GaussianOffsets[index][0] = (*pSpreadDir)[0] * curOffset * texelSize[0] + ((*pConstantOffset)[0] * texelSize[0]);
            m_GaussianOffsets[index][1] = (*pSpreadDir)[1] * curOffset * texelSize[1] + ((*pConstantOffset)[1] * texelSize[1]);
			
			float curWeight = GetGaussianDistribution((float) curOffset, standardDeviation);
            m_GaussianWeights[index] = curWeight;
			totalWeight += curWeight;

            index++;
        }	
    }
    else
    {
        for(uint i=0; i < offsetsPerSide; ++i)
        {
            uint curOffset = offsetsPerSide - index;
            m_GaussianOffsets[index][0] = (*pSpreadDir)[0] * curOffset * texelSize[0] + ((*pConstantOffset)[0] * texelSize[0]);
            m_GaussianOffsets[index][1] = (*pSpreadDir)[1] * curOffset * texelSize[1] + ((*pConstantOffset)[1] * texelSize[1]);

			float curWeight = GetGaussianDistribution((float) curOffset, standardDeviation);
			m_GaussianWeights[index] = curWeight;
			totalWeight += curWeight;

			index++;
        }	

        m_GaussianOffsets[index][0] = 0.0f;
        m_GaussianOffsets[index][1] = 0.0f;
		
		float curWeight = GetGaussianDistribution(0.0f, standardDeviation);
        m_GaussianWeights[index] = curWeight;

		totalWeight += curWeight;
		index++;
    }

	for(uint i=0; i < offsetsPerSide; ++i)
	{
		uint curOffset = index - offsetsPerSide;
		m_GaussianOffsets[index][0] = -(*pSpreadDir)[0] * curOffset * texelSize[0] - ((*pConstantOffset)[0] * texelSize[0]); 
		m_GaussianOffsets[index][1] = -(*pSpreadDir)[1] * curOffset * texelSize[1] - ((*pConstantOffset)[1] * texelSize[1]);

		float curWeight = GetGaussianDistribution((float) curOffset, standardDeviation);
		m_GaussianWeights[index] = curWeight;
	
		totalWeight += curWeight;
		index++;
	}	
	
	_LOOPi(index)
	{
		m_GaussianWeights[i] /= totalWeight;
		m_GaussianWeights[i] *= weight;
	}

	if(isReduceable)
	{
		// Pack by taking advantage of mip mapping
		index = 0;
		for(uint i=0; i < m_GaussianSize-1; i+=2)
		{
			float w1 = m_GaussianWeights[i];
			float w2 = m_GaussianWeights[i+1];

			gmtl::Vec2f newOffset;
			newOffset[0] = m_GaussianOffsets[i][0] * (w1/(w1+w2)) + m_GaussianOffsets[i+1][0] * (w2/(w1+w2));
			newOffset[1] = m_GaussianOffsets[i][1] * (w1/(w1+w2)) + m_GaussianOffsets[i+1][1] * (w2/(w1+w2));

			m_GaussianOffsets[index] = newOffset;
			m_GaussianWeights[index] = w1+w2;			
			index++;
		}
		
		m_GaussianOffsets[index] = m_GaussianOffsets[m_GaussianSize-1];
		m_GaussianWeights[index] = m_GaussianWeights[m_GaussianSize-1];
		index++;
		_DEBUG_ASSERT(index == offsetsPerSide+1);
		
		m_GaussianSize = index;
	}
}


//gmtl::Vec2f* 
//BFXUGaussianBlurParams::GetGaussianOffsets()
//{
//	return m_GaussianOffsets;
//}

const gmtl::Vec2f* 
BFXUGaussianBlurParams::GetGaussianOffsets()
{
	return m_GaussianOffsets;
}

const float* 
BFXUGaussianBlurParams::GetGaussianWeights()
{
	return m_GaussianWeights;
}
	
uint 
BFXUGaussianBlurParams::GetGaussianSize()
{
	return m_GaussianSize;
}
	
_NAMESPACE_END