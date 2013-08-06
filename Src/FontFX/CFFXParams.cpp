//==============================================================================
//
//		CFFXParams.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		12/21/2007
//
//==============================================================================

#include "FontFXFramework.h"

_NAMESPACE_BEGIN

void
CFFXParams::SetFontTexture(const IRTexture2D* pTex)
{
	m_pFontTexture = pTex;
}

const IRTexture2D* 
CFFXParams::GetFontTexture()
{
	return m_pFontTexture;
}

void 
CFFXParams::GetParameter(REffectParam& param, uint semantic)
{
	switch(semantic)
	{
	case FFX_FONT_TEXTURE:
		{
			param.SetTexture2D(GetFontTexture());
			break;
		}

	default:
		{	
			_DEBUG_ASSERT(FALSE);
//			m_pFontFX->GetRenderFX().GetRFXCommon().GetParameter(param, semantic);
			break;
		}
	}	
}

uint 
CFFXParams::InitializeEffect(IREffect& effect, float detailLevel, REffectParam* params)
{
	uint tech = (uint)((effect.GetNumOfTechs()-1) * detailLevel + 0.5f);
	uint numDynamicParams = effect.GetTemplate()->GetNumOfDynamicParams(tech);

	_LOOPi(numDynamicParams)
	{
		uint semantic = effect.GetTemplate()->GetDynamicParamSemantic(tech, i);
		GetParameter(params[i], semantic);
	}

	return tech;
}

_NAMESPACE_END