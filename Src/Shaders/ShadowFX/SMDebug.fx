//================================================================================
//
//		SMDebug.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		8/24/2012
//
//================================================================================

float4		g_Offset	< int semantic = 273; >;													
texture		g_TexRT		< int semantic = 274; >;
//float		g_ExpAmount < int semantic = 272; >;

sampler		g_SampRT = sampler_state
{
    Texture = (g_TexRT);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = CLAMP;
    ADDRESSV  = CLAMP;
};

struct VS_IN_SMDebug
{
	float4 pos			: POSITION0;
	float2 texUV		: TEXCOORD0;
};

struct VS_OUT_SMDebug
{
	float4 pos			: POSITION0;
	float2 texUV		: TEXCOORD0;
};

struct PS_OUT_Color
{
	float4 color	: COLOR0;
};

float NormalizeExp(float expVal, float expK)
{
	return log(expVal) / expK;
}

/*
float2 UncompressRGBAToRG(float4 val)
{
	float2 hi = val.xy * 255.0f;
	float2 lo = val.zw;
	
	float2 toReturn = (hi + lo) * (256.0f / 65535.0f);
	return toReturn;
}

*/

VS_OUT_SMDebug
SMDebugVS(VS_IN_SMDebug input)
{
	VS_OUT_SMDebug output = (VS_OUT_SMDebug) 0;
	
	float2 size = input.pos * (g_Offset.zw - g_Offset.xy);
	output.pos = float4(g_Offset.xy + size, 0.0f, 1.0f);
	output.texUV = input.texUV;
	
	return output;
}

PS_OUT_Color
SMDebugPS(VS_OUT_SMDebug input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
		
	output.color = tex2D(g_SampRT, input.texUV).rrrr;
	//output.color.bg = 0.0f;
	
	return output;
}

/*

PS_OUT_Color
ExpCSMDebugSplit2PS(VS_OUT_ExpCSMDebug input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
		
	output.color = pow( NormalizeExp( tex2D(g_SampRT, input.texUV).g, g_ExpAmount ), 1.0f );
	output.color.rb = 0.0f;
	
	return output;
}

PS_OUT_Color
ExpCSMDebugSplit3PS(VS_OUT_ExpCSMDebug input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
		
	output.color = pow( NormalizeExp( tex2D(g_SampRT, input.texUV).b, g_ExpAmount ), 1.0f );
	output.color.rg = 0.0f;
	
	return output;
}

PS_OUT_Color
ExpCSMDebugSplit4PS(VS_OUT_ExpCSMDebug input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
		
	output.color = pow( NormalizeExp( tex2D(g_SampRT, input.texUV).a, g_ExpAmount ), 1.0f );
	output.color.r = 0.0f;
	
	return output;
}

*/

technique t0
{
	pass p1
	{
		VertexShader	=	compile vs_2_0 SMDebugVS();
		PixelShader		=	compile ps_2_0 SMDebugPS();
	}
};

/*

technique t1
{
	pass p1
	{
		VertexShader	=	compile vs_2_0 ExpCSMDebugVS();
		PixelShader		=	compile ps_2_0 ExpCSMDebugSplit2PS();
	}
};

technique t2
{
	pass p1
	{
		VertexShader	=	compile vs_2_0 ExpCSMDebugVS();
		PixelShader		=	compile ps_2_0 ExpCSMDebugSplit3PS();
	}
};

technique t3
{
	pass p1
	{
		VertexShader	=	compile vs_2_0 ExpCSMDebugVS();
		PixelShader		=	compile ps_2_0 ExpCSMDebugSplit4PS();
	}
};

*/