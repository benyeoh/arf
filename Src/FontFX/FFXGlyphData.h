//==============================================================================
//
//		FFXGlyphData.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		12/12/2007
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

struct FFXGlyphData
{
	int kerningOffsetX;
	int kerningOffsetY;
	int advance;
	
	gmtl::Vec4f uv;
	gmtl::Vec2i	size;
};

_NAMESPACE_END
