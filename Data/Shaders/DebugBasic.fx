//==============================================================================
//
//		DebugBasic.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/30/2008
//
//==============================================================================

#include "[shd]\DebugFX\AllDebugFX.fxh"
#include "[shd]\DebugFX\DebugBasic.fxh"	

technique t0
{
	pass p1
	{
		VertexShader	=	compile vs_2_0 DebugBasicVS();
		PixelShader		=	compile ps_2_0 DebugBasicPS();
	}
};
