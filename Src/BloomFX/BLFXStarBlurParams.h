//==============================================================================
//
//		BLFXStarBlurParams.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		8/20/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class BLFXStarBlurParams
{
private: 
	gmtl::Vec2f	m_LineOffsets[BLFX_MAX_STAR_LINE_SIZE];
	float		m_Weights[BLFX_MAX_STAR_LINE_SIZE];
	
public: 
	BLFXStarBlurParams()
	{
	}

	virtual ~BLFXStarBlurParams()
	{
	}

public: 
	void InitializeBlurParams(uint lineSize, 
							uint offsetScale, 
							const gmtl::Vec2f& lineDir, 
							float attnScale,
							float brightnessMult,										
							uint texWidth,
							uint texHeight);
	
	gmtl::Vec2f* GetLineOffsets() { return m_LineOffsets; }
	float* GetWeights() { return m_Weights; }
};

_NAMESPACE_END
