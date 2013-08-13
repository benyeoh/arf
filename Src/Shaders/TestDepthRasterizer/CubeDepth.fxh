//================================================================================
//
//		CubeDepth.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		8/12/2011
//
//================================================================================

#ifndef CUBEDEPTH
#define CUBEDEPTH

float4x4 g_WorldViewProj <int semantic = 0; int usage = 0;>;
float g_Far <int semantic = 1; int usage = 1;>;

struct VS_IN_CubeDepth
{
	float4 pos			: POSITION0;
	float2 texUV		: TEXCOORD0;
};

struct VS_OUT_CubeDepth
{
	float4 pos			: POSITION0;
	float depth			: TEXCOORD0;
	float2 texUV		: TEXCOORD1;
};

struct PS_OUT_Color
{
	float4 color	: COLOR0;
};

VS_OUT_CubeDepth
CubeDepthVS(VS_IN_CubeDepth input)
{
	VS_OUT_CubeDepth output = (VS_OUT_CubeDepth) 0;
		
	output.pos = mul(input.pos, g_WorldViewProj);	
	output.depth = output.pos.z / g_Far;
	output.texUV = input.texUV;
		
	return output;
}

PS_OUT_Color
CubeDepthPS(VS_OUT_CubeDepth input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
		
	float2 colorMod = input.texUV * 4.0f;
	
	colorMod = frac(colorMod);
	colorMod = colorMod > 0.5f;
	
	// Exclusive OR
	output.color = (colorMod.x + colorMod.y) - 2.0f * (colorMod.x * colorMod.y);
	
	//output.color = output.color * input.depth;
	output.color = input.depth;
	
	return output;
}

#endif