//==============================================================================
//
//		CRDepthBuffer.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/25/2007
//
//==============================================================================

#include "RendererFramework.h"

_NAMESPACE_BEGIN

uint 
CRDepthBuffer::GetWidth() const
{
	return m_Width;
}
 
uint 
CRDepthBuffer::GetHeight() const
{
	return m_Height;
}

eRDepthFormat 
CRDepthBuffer::GetDepthFormat() const
{
	return m_Format;
}

uint 
CRDepthBuffer::GetMultiSampleQuality() const
{
	return m_MultiSampleQuality;
}

void 
CRDepthBuffer::SetSize(uint width, uint height)
{
	m_Width = width;
	m_Height = height;
}

void 
CRDepthBuffer::SetDepthFormat(eRDepthFormat format)
{
	m_Format = format;
}

void 
CRDepthBuffer::SetMultiSampleQuality(uint numSamples)
{
	m_MultiSampleQuality = numSamples;
}

_NAMESPACE_END