//==============================================================================
//
//		ScreenSpaceBasic.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		10/11/2007
//
//==============================================================================

#include "[shd]\Globals\AllExceptPerm.fxh"
#include "[shd]\PostProcess\ScreenSpaceShaderFns.fxh"

_DECL_EFFECT_TYPE = RFX_TYPE_POST_PROCESS;						  
_DECL_EFFECT_DESC = "Screen Space Basic";

technique t0
{
	pass p0
	{
		VertexShader	=	compile vs_2_0 ScreenSpaceBasicVS();
		PixelShader		=	compile ps_2_0 ScreenSpaceBasicPS();
	}
};

	