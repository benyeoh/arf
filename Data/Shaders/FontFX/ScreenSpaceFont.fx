//==============================================================================
//
//		ScreenSpaceFont.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		12/17/2007
//
//==============================================================================

#include "[shd]\Globals\AllExceptPerm.fxh"
#include "[shd]\FontFX\AllFontFX.fxh"
#include "[shd]\FontFX\ScreenSpaceFont.fxh"

_DECL_EFFECT_TYPE = FFX_TYPE_FONT;						  
_DECL_EFFECT_DESC = "Screen Space Font";				

technique t0
{
	pass p1
	{
		AlphaBlendEnable = TRUE;
		AlphaTestEnable = TRUE;
		ZEnable = FALSE;
		ZWriteEnable = FALSE;
		SrcBlend = SRCALPHA;
		DestBlend = INVSRCALPHA;
		
		VertexShader	=	compile vs_2_0 ScreenSpaceFontVS();
		PixelShader		=	compile ps_2_0 ScreenSpaceFontPS();
	}
};
