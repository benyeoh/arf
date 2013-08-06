//================================================================================
//
//		ScreenTexturedQuad.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		6/12/2011
//
//================================================================================

#include "[shd]\TestRasterizer\ScreenTexturedQuad.fxh"

technique t0
{
	pass p1
	{
		ZEnable			= FALSE;
		ZWriteEnable	= FALSE;		
		VertexShader	=	compile vs_3_0 ScreenTexturedQuadVS();
		PixelShader		=	compile ps_3_0 ScreenTexturedQuadPS();
	}
};
