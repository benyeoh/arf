//==============================================================================
//
//		ScreenSpaceFont.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		12/16/2007
//
//==============================================================================

#ifndef SCREENSPACEFONT
#define SCREENSPACEFONT

struct VS_IN_ScreenSpaceFont
{
	float4 pos			: POSITION0;
	float4 color		: COLOR0;
	float2 texUV		: TEXCOORD0;
};

struct VS_OUT_ScreenSpaceFont
{
	float4 pos			: POSITION0;
	float4 color		: TEXCOORD0;
	float2 texUV		: TEXCOORD1;
};

VS_OUT_ScreenSpaceFont
ScreenSpaceFontVS(VS_IN_ScreenSpaceFont input)
{
	VS_OUT_ScreenSpaceFont output = (VS_OUT_ScreenSpaceFont) 0;
		
	// NOTE: Assumes either position or texUV has been offset appropriately
	output.pos = float4(input.pos.xy, 0.0f, 1.0f);
	output.texUV = input.texUV;
	output.color = input.color;
	
	return output;
}

PS_OUT_Color
ScreenSpaceFontPS(VS_OUT_ScreenSpaceFont input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
		
	output.color = float4(1.0f, 1.0f, 1.0f, tex2D(g_SampFont, input.texUV).r) * input.color;
	
	return output;
}

#endif