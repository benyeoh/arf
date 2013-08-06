//==============================================================================
//
//		DebugTexturedQuad.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/22/2008
//
//==============================================================================

#include "[shd]\DebugFX\AllDebugFX.fxh"
#include "[shd]\DebugFX\DebugTexturedQuad.fxh"	

technique t0
{
	pass p1
	{
		VertexShader	=	compile vs_2_0 DebugTexturedQuadVS();
		PixelShader		=	compile ps_2_0 DebugTexturedQuadPS();
	}
};
