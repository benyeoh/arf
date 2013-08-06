//==============================================================================
//
//		GaussianBlur1D.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		10/1/2007
//
//==============================================================================


float2 g_FilterTextureOffsets[8]	< int semantic = 4267; 
									int usage = 1; 
									string name = "Filter Texture Offsets"; >;
															
float g_FilterWeights[8]			< int semantic = 4268; 
									int usage = 1; 
									string name = "Filter Weights"; >;

texture		g_TexRT1 < int semantic = 4265; 
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

struct VS_OUT_StarBlur
{
	float4 pos			: POSITION0;
	float2 texUV		: TEXCOORD0;
	
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

VS_OUT_StarBlur
StarBlurVS(VS_IN_2D input)
{
	VS_OUT_StarBlur output = (VS_OUT_StarBlur) 0;
		
	// NOTE: Assumes either position or texUV has been offset appropriately
	output.pos = float4(input.pos.xy, 0.0f, 1.0f);
	output.texUV = input.texUV;
	
	return output;
}

#define __DEFINE_STAR_BLUR_PS(x) \
 \
PS_OUT_Color \
StarBlur##x##SamplesPS(VS_OUT_StarBlur input) \
{ \
	PS_OUT_Color output = (PS_OUT_Color) 0; \
 \
	for(int i=0; i < x; ++i) \
	{ \
		float2 texUV = input.texUV + g_FilterTextureOffsets[i]; \
		output.color += (tex2D(g_SampRT1, texUV) * g_FilterWeights[i]); \
	} \
 \
	for(int i=0; i < x; ++i) \
	{ \
		float2 texUV = input.texUV - g_FilterTextureOffsets[i]; \
		output.color += (tex2D(g_SampRT1, texUV) * g_FilterWeights[i]); \
	} \
 \
	return output; \
}

__DEFINE_STAR_BLUR_PS(4)
__DEFINE_STAR_BLUR_PS(5)
__DEFINE_STAR_BLUR_PS(6)
__DEFINE_STAR_BLUR_PS(7)
__DEFINE_STAR_BLUR_PS(8)

technique t0
{
	pass p0
	{
		VertexShader	=	compile vs_2_0 StarBlurVS();
		PixelShader		=	compile ps_2_0 StarBlur4SamplesPS();
	}
};

technique t1
{
	pass p0
	{
		VertexShader	=	compile vs_2_0 StarBlurVS();
		PixelShader		=	compile ps_2_0 StarBlur5SamplesPS();
	}
};


technique t2
{
	pass p0
	{
		VertexShader	=	compile vs_2_0 StarBlurVS();
		PixelShader		=	compile ps_2_0 StarBlur6SamplesPS();
	}
};

technique t3
{
	pass p0
	{
		VertexShader	=	compile vs_2_0 StarBlurVS();
		PixelShader		=	compile ps_2_0 StarBlur7SamplesPS();
	}
};

technique t4
{
	pass p0
	{
		VertexShader	=	compile vs_2_0 StarBlurVS();
		PixelShader		=	compile ps_2_0 StarBlur8SamplesPS();
	}
};
