//================================================================================
//
//		ScreenTriangle.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		2/12/2011
//
//================================================================================

#include "[shd]\TestRasterizer\ScreenTriangle.fxh"

technique t0
{
	pass p1
	{
		ZEnable			= FALSE;
		ZWriteEnable	= FALSE;		
		VertexShader	=	compile vs_3_0 ScreenTriangleVS();
		PixelShader		=	compile ps_3_0 ScreenTrianglePS();
	}
};
