//==============================================================================
//
//		BLFXStarBlurParams.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		8/20/2008
//
//==============================================================================

#include "BloomFXFramework.h"

_NAMESPACE_BEGIN

void 
BLFXStarBlurParams::InitializeBlurParams(uint lineSize,
										uint offsetScale, 
										const gmtl::Vec2f& lineDir, 
										float attnScale,
										float brightnessMult,										
										uint texWidth,
										uint texHeight)
{
	_DEBUG_ASSERT(lineSize <= BLFX_MAX_STAR_LINE_SIZE);
	
	gmtl::Vec2f texOffset((lineDir[0] * 0.5f) / texWidth, (lineDir[1] * 0.5f) / texHeight);
	gmtl::Vec2f lineDirOffset(lineDir[0] / texWidth, lineDir[1] / texHeight);
	
	_LOOPi(lineSize)
	{
		m_LineOffsets[i] = ((float)i * offsetScale) * lineDirOffset + texOffset;
		m_Weights[i] = gmtl::Math::pow(attnScale, (float)(i * offsetScale)) * brightnessMult;
	}
}
	
	
_NAMESPACE_END