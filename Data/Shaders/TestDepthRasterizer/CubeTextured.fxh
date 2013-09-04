//================================================================================
//
//		CubeTextured.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		8/12/2011
//
//================================================================================

#ifndef CUBETEXTURED
#define CUBETEXTURED

float4x4 g_WorldViewProj <int semantic = 0; int usage = 0;>;

struct VS_IN_CubeTextured
{
	float4 pos			: POSITION0;
	float2 texUV		: TEXCOORD0;
};

struct VS_OUT_CubeTextured
{
	float4 pos			: POSITION0;
	float2 texUV		: TEXCOORD0;
};

struct PS_OUT_Color
{
	float4 color	: COLOR0;
};

VS_OUT_CubeTextured
CubeTexturedVS(VS_IN_CubeTextured input)
{
	VS_OUT_CubeTextured output = (VS_OUT_CubeTextured) 0;
		
	output.pos = mul(input.pos, g_WorldViewProj);	
	output.texUV = input.texUV;
	
	return output;
}

PS_OUT_Color
CubeTexturedPS(VS_OUT_CubeTextured input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
		
	float2 colorMod = input.texUV * 4.0f;
	
	colorMod = frac(colorMod);
	colorMod = colorMod > 0.5f;
	
	// Exclusive OR
	output.color = (colorMod.x + colorMod.y) - 2.0f * (colorMod.x * colorMod.y);
	
	return output;
}

#endif