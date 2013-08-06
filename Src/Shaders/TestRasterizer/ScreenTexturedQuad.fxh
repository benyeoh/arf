//================================================================================
//
//		ScreenTexturedQuad.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		6/12/2011
//
//================================================================================

#ifndef SCREENTEXTUREDQUAD
#define SCREENTEXTUREDQUAD

texture		g_TexQuad								< int semantic = 55; int usage = 1; >;
sampler		g_SampQuad = sampler_state
{
    Texture = (g_TexQuad);
    MipFilter = POINT;
    MinFilter = POINT;
    MagFilter = POINT;
    ADDRESSU  = CLAMP;
    ADDRESSV  = CLAMP;
};

struct VS_IN_ScreenTexturedQuad
{
	float4 pos			: POSITION0;
	float2 texUV		: TEXCOORD0;
};

struct VS_OUT_ScreenTexturedQuad
{
	float4 pos			: POSITION0;
	float2 texUV		: TEXCOORD0;
};

struct PS_OUT_Color
{
	float4 color	: COLOR0;
};

VS_OUT_ScreenTexturedQuad
ScreenTexturedQuadVS(VS_IN_ScreenTexturedQuad input)
{
	VS_OUT_ScreenTexturedQuad output = (VS_OUT_ScreenTexturedQuad) 0;
		
	output.pos = input.pos;	
	output.texUV = input.texUV;
	
	return output;
}

PS_OUT_Color
ScreenTexturedQuadPS(VS_OUT_ScreenTexturedQuad input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
		
	output.color = tex2D(g_SampQuad, input.texUV).rrrr;
	
	return output;
}

#endif