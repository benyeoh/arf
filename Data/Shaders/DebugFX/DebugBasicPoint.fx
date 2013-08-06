//==============================================================================
//
//		DebugBasicPoint.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		2/3/2008
//
//==============================================================================

#include "[shd]\DebugFX\AllDebugFX.fxh"
#include "[shd]\DebugFX\DebugBasicPoint.fxh"	

technique t0
{
	pass p1
	{
		VertexShader	=	compile vs_2_0 DebugBasicPointVS();
		PixelShader		=	compile ps_2_0 DebugBasicPointPS();
	}
};
