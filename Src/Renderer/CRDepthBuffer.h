//==============================================================================
//
//		CRDepthBuffer.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/25/2007
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CRDepthBuffer : public CRResource< IRDepthBuffer >
{
protected: 
	uint m_Width;
	uint m_Height;
	eRDepthFormat m_Format;
	uint m_MultiSampleQuality;
	
public: 
	CRDepthBuffer()
	{
	}

	virtual ~CRDepthBuffer()
	{
	}

public: 
	uint GetWidth() const; 
	uint GetHeight() const;
	eRDepthFormat GetDepthFormat() const;
	uint GetMultiSampleQuality() const;
	
	void SetSize(uint width, uint height);
	void SetDepthFormat(eRDepthFormat format);
	void SetMultiSampleQuality(uint numSamples);
	
};

_NAMESPACE_END
