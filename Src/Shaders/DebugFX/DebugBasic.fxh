//==============================================================================
//
//		DebugBasic.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/30/2008
//
//==============================================================================

#ifndef DEBUGBASIC
#define DEBUGBASIC

struct VS_IN_DebugBasic
{
	float4 pos			: POSITION0;
	int4 indices		: BLENDINDICES;
};

struct VS_OUT_DebugBasic
{
	float4 pos			: POSITION0;
	float4 color		: TEXCOORD0;
};

struct PS_OUT_Color
{
	float4 color	: COLOR0;
};

VS_OUT_DebugBasic
DebugBasicVS(VS_IN_DebugBasic input)
{
	VS_OUT_DebugBasic output = (VS_OUT_DebugBasic) 0;
		
	// NOTE: Assumes either position or texUV has been offset appropriately
	float4 worldPos = mul(input.pos, g_matWorldBatch[input.indices.r]);
	output.pos = mul(worldPos, g_matViewProj);
	 
	output.color = g_ColorBatch[input.indices.r];
	
	return output;
}

PS_OUT_Color
DebugBasicPS(VS_OUT_DebugBasic input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
		
	output.color = input.color;
	
	return output;
}

#endif