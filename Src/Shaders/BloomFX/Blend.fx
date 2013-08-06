//==============================================================================
//
//		Blend.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/22/2008
//
//==============================================================================

texture		g_TexRT1 < int semantic = 4250; 
					int usage = 1; 
					string name = "RT 1"; >;

sampler		g_SampRT1 = sampler_state
{
    Texture = (g_TexRT1);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = CLAMP;
    ADDRESSV  = CLAMP;
};

struct VS_IN_2D
{
	float4 pos		: POSITION;
	float2 texUV	: TEXCOORD0;
};

struct PS_OUT_Color
{
	float4 color : COLOR0;
};


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
ScreenSpaceBlendPS(VS_OUT_ScreenSpaceBasic input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
		
	output.color = tex2D(g_SampRT1, input.texUV);
	
	return output;
}

technique t0
{
	pass p0
	{
		AlphaBlendEnable = TRUE;
		SrcBlend = INVDESTCOLOR;
		DestBlend = ONE;
		VertexShader	=	compile vs_2_0 ScreenSpaceBasicVS();
		PixelShader		=	compile ps_2_0 ScreenSpaceBlendPS();
	}
};

technique t1
{
	pass p1
	{
		VertexShader	=	compile vs_2_0 ScreenSpaceBasicVS();
		PixelShader		=	compile ps_2_0 ScreenSpaceBlendPS();
	}
};