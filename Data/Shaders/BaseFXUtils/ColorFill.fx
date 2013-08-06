//==============================================================================
//
//		ColorFill.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/11/2008
//
//==============================================================================

#include "[shd]\BaseFXUtils\ParamSemanticDefs.fxh"

float4		g_ColorFill< int semantic = BFXU_COLOR_FILL; 
					int usage = 1; 
					string name = "Color Fill"; >;
					
struct VS_IN_ColorFill
{
	float4 pos			: POSITION0;
};

struct VS_OUT_ColorFill
{
	float4 pos			: POSITION0;
};

struct PS_OUT_Color
{
	float4 color	: COLOR0;
};

VS_OUT_ColorFill
ColorFillVS(VS_IN_ColorFill input)
{
	VS_OUT_ColorFill output = (VS_OUT_ColorFill) 0;
	
	output.pos = input.pos;
		
	return output;
}

PS_OUT_Color
ColorFillPS(VS_OUT_ColorFill input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
	
	output.color.argb = g_ColorFill;
	
	return output;
}

technique t0
{
	pass p0
	{
		ZWriteEnable = FALSE;
		ZEnable = FALSE;
		
		VertexShader	=	compile vs_2_0 ColorFillVS();
		PixelShader		=	compile ps_2_0 ColorFillPS();
	}
};