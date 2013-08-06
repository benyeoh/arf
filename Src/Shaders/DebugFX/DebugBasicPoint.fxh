//==============================================================================
//
//		DebugBasicPoint.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/30/2008
//
//==============================================================================

#ifndef DEBUGBASICPOINT
#define DEBUGBASICPOINT

struct VS_IN_DebugBasicPoint
{
	float4 pos			: POSITION0;
	int4 indices		: BLENDINDICES;
};

struct VS_OUT_DebugBasicPoint
{
	float4 pos			: POSITION0;
	float4 color		: TEXCOORD0;
};

struct PS_OUT_Color
{
	float4 color	: COLOR0;
};

VS_OUT_DebugBasicPoint
DebugBasicPointVS(VS_IN_DebugBasicPoint input)
{
	VS_OUT_DebugBasicPoint output = (VS_OUT_DebugBasicPoint) 0;
		
	// NOTE: Assumes either position or texUV has been offset appropriately
	float4 worldPos = float4(input.pos.xyz + g_PosBatch[input.indices.r], 1.0f);
	output.pos = mul(worldPos, g_matViewProj);
	 
	output.color = g_ColorBatch[input.indices.r];
	
	return output;
}

PS_OUT_Color
DebugBasicPointPS(VS_OUT_DebugBasicPoint input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
		
	output.color = input.color;
	
	return output;
}

#endif