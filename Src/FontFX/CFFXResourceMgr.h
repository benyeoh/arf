//==============================================================================
//
//		CFFXResourceMgr.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/8/2007
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CFFXResourceMgr : public IFFXResourceMgr
{
private: 
	CFFXFontFX* m_pFontFX;
	
public: 
	CFFXResourceMgr()
		: m_pFontFX(NULL)
	{
	}

	virtual ~CFFXResourceMgr()
	{
	}

public: 
	IFFXFontFace* CreateFontFace(IByteBuffer* pFontData, uint pointSize, uint faceIndex);
	
	void SetFontFX(CFFXFontFX* pFontFX) { m_pFontFX = pFontFX; }
};

_NAMESPACE_END
