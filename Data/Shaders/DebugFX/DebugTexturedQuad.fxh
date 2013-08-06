//==============================================================================
//
//		DebugTexturedQuad.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/22/2008
//
//==============================================================================

#ifndef DEBUGTEXTUREDQUAD
#define DEBUGTEXTUREDQUAD

struct VS_IN_DebugTexturedQuad
{
	float4 pos			: POSITION0;
	float2 texUV		: TEXCOORD0;
	int4 indices		: BLENDINDICES;
};

struct VS_OUT_DebugTexturedQuad
{
	float4 pos			: POSITION0;
	float2 texUV		: TEXCOORD0;
};

struct PS_OUT_Color
{
	float4 color	: COLOR0;
};

VS_OUT_DebugTexturedQuad
DebugTexturedQuadVS(VS_IN_DebugTexturedQuad input)
{
	VS_OUT_DebugTexturedQuad output = (VS_OUT_DebugTexturedQuad) 0;
		
	float4 worldPos = float4(g_PosBatch[input.indices.r], 1.0f);
	float4 viewPos = mul(worldPos, g_matView);
	
	viewPos.xy = viewPos.xy + input.pos.xy * g_QuadSizeBatch[input.indices.r].xy * 0.5f;
	
	output.pos = mul(viewPos, g_matProj);	
	output.texUV = g_QuadTexPosBatch[input.indices.r] + input.texUV * g_QuadTexSizeBatch[input.indices.r];
	
	return output;
}

PS_OUT_Color
DebugTexturedQuadPS(VS_OUT_DebugTexturedQuad input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
		
	output.color = tex2D(g_SampQuad, input.texUV);
	
	return output;
}

#endif