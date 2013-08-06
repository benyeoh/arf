//==============================================================================
//
//		ScreenSpaceShaderFns.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		10/2/2007
//
//==============================================================================

#ifndef SCREENSPACESHADERFNS
#define SCREENSPACESHADERFNS

struct VS_OUT_ScreenSpaceBasic
{
	float4 pos			: POSITION0;
	float2 texUV		: TEXCOORD0;
};

VS_OUT_ScreenSpaceBasic
ScreenSpaceBasicVS(VS_IN_2D input)
{
	VS_OUT_ScreenSpaceBasic output = (VS_OUT_ScreenSpaceBasic) 0;
		
	// NOTE: Assumes either position or texUV has been offset appropriately
	output.pos = float4(input.pos.xy, 0.0f, 1.0f);
	output.texUV = input.texUV;
	
	return output;
}

PS_OUT_Color
ScreenSpaceBasicPS(VS_OUT_ScreenSpaceBasic input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
		
	output.color = tex2D(g_SampRT1, input.texUV);
	
	return output;
}

PS_OUT_Color
ScreenSpaceAddBlendPS(VS_OUT_ScreenSpaceBasic input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
		
	output.color = tex2D(g_SampRT1, input.texUV);
	output.color.rgb += tex2D(g_SampRT2, input.texUV).rgb;
	
	return output;
}

#endif