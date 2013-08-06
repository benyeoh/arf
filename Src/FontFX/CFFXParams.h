//==============================================================================
//
//		CFFXParams.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		12/21/2007
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CFFXParams : public IFFXParams
{
private: 
	const IRTexture2D* m_pFontTexture;
	
	CFFXFontFX* m_pFontFX;
		
public: 
	CFFXParams()
		: m_pFontTexture(NULL)
	{
	}

	~CFFXParams()
	{
	}

public: 
	uint InitializeEffect(IREffect& effect, float detailLevel, REffectParam* params);

	void SetFontTexture(const IRTexture2D* pTex);
	const IRTexture2D* GetFontTexture();

	void GetParameter(REffectParam& param, uint semantic);
			
	void SetFontFX(CFFXFontFX* pFontFX) { m_pFontFX = pFontFX; }
};

_NAMESPACE_END
