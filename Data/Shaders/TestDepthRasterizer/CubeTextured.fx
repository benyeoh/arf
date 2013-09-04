//================================================================================
//
//		CubeTextured.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		8/12/2011
//
//================================================================================

#include "[shd]\TestRasterizer\CubeTextured.fxh"

technique t0
{
	pass p1
	{
		VertexShader	=	compile vs_3_0 CubeTexturedVS();
		PixelShader		=	compile ps_3_0 CubeTexturedPS();
	}
};
