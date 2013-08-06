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

#ifndef GAUSSIANBLUR1D
#define GAUSSIANBLUR1D

struct VS_OUT_GaussianBlur1D
{
	float4 pos			: POSITION0;
	float2 texUVTap0	: TEXCOORD0;
	float2 texUVTap1	: TEXCOORD1;
	float2 texUVTap2	: TEXCOORD2;
	float2 texUVTap3	: TEXCOORD3;
	float2 texUVTap4	: TEXCOORD4;
	float2 texUVTap5	: TEXCOORD5;
	float2 texUVTap6	: TEXCOORD6;
	float2 texUV		: TEXCOORD7;
	
};

VS_OUT_GaussianBlur1D
GaussianBlur1DVS(VS_IN_2D input)
{
	VS_OUT_GaussianBlur1D output = (VS_OUT_GaussianBlur1D) 0;
		
	// NOTE: Assumes either position or texUV has been offset appropriately
	output.pos = float4(input.pos.xy, 0.0f, 1.0f);
	output.texUV = input.texUV;
	output.texUVTap0 = input.texUV + g_FilterTextureOffsets[0];
	output.texUVTap1 = input.texUV + g_FilterTextureOffsets[1];
	output.texUVTap2 = input.texUV + g_FilterTextureOffsets[2];
	output.texUVTap3 = input.texUV + g_FilterTextureOffsets[3];
	output.texUVTap4 = input.texUV + g_FilterTextureOffsets[4];
	output.texUVTap5 = input.texUV + g_FilterTextureOffsets[5];
	output.texUVTap6 = input.texUV + g_FilterTextureOffsets[6];
	
	return output;
}

PS_OUT_Color
GaussianBlur1D2SamplesPS(VS_OUT_GaussianBlur1D input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
	float4 tap0 = tex2D(g_SampRT1, input.texUVTap0) * g_FilterWeights[0];
	float4 tap1 = tex2D(g_SampRT1, input.texUVTap1) * g_FilterWeights[1];
	
	output.color = tap0 + tap1;

	return output;
}

PS_OUT_Color
GaussianBlur1D3SamplesPS(VS_OUT_GaussianBlur1D input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
	float4 tap0 = tex2D(g_SampRT1, input.texUVTap0) * g_FilterWeights[0];
	float4 tap1 = tex2D(g_SampRT1, input.texUVTap1) * g_FilterWeights[1];
	float4 tap2 = tex2D(g_SampRT1, input.texUVTap2) * g_FilterWeights[2];
	
	output.color = tap0 + tap1 + tap2;

	return output;
}

PS_OUT_Color
GaussianBlur1D4SamplesPS(VS_OUT_GaussianBlur1D input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
	float4 tap0 = tex2D(g_SampRT1, input.texUVTap0) * g_FilterWeights[0];
	float4 tap1 = tex2D(g_SampRT1, input.texUVTap1) * g_FilterWeights[1];
	float4 tap2 = tex2D(g_SampRT1, input.texUVTap2) * g_FilterWeights[2];
	float4 tap3 = tex2D(g_SampRT1, input.texUVTap3) * g_FilterWeights[3];
	
	output.color = tap0 + tap1 + tap2 + tap3;

	return output;
}

PS_OUT_Color
GaussianBlur1D5SamplesPS(VS_OUT_GaussianBlur1D input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
	float4 tap0 = tex2D(g_SampRT1, input.texUVTap0) * g_FilterWeights[0];
	float4 tap1 = tex2D(g_SampRT1, input.texUVTap1) * g_FilterWeights[1];
	float4 tap2 = tex2D(g_SampRT1, input.texUVTap2) * g_FilterWeights[2];
	float4 tap3 = tex2D(g_SampRT1, input.texUVTap3) * g_FilterWeights[3];
	float4 tap4 = tex2D(g_SampRT1, input.texUVTap4) * g_FilterWeights[4];
	
	output.color = tap0 + tap1 + tap2 + tap3 + tap4;
	
	return output;
}

PS_OUT_Color
GaussianBlur1D6SamplesPS(VS_OUT_GaussianBlur1D input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
	float4 tap0 = tex2D(g_SampRT1, input.texUVTap0) * g_FilterWeights[0];
	float4 tap1 = tex2D(g_SampRT1, input.texUVTap1) * g_FilterWeights[1];
	float4 tap2 = tex2D(g_SampRT1, input.texUVTap2) * g_FilterWeights[2];
	float4 tap3 = tex2D(g_SampRT1, input.texUVTap3) * g_FilterWeights[3];
	float4 tap4 = tex2D(g_SampRT1, input.texUVTap4) * g_FilterWeights[4];
	float4 tap5 = tex2D(g_SampRT1, input.texUVTap5) * g_FilterWeights[5];
	
	output.color = tap0 + tap1 + tap2 + tap3 + tap4 + tap5;
	
	return output;
}

PS_OUT_Color
GaussianBlur1D7SamplesPS(VS_OUT_GaussianBlur1D input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
	float4 tap0 = tex2D(g_SampRT1, input.texUVTap0) * g_FilterWeights[0];
	float4 tap1 = tex2D(g_SampRT1, input.texUVTap1) * g_FilterWeights[1];
	float4 tap2 = tex2D(g_SampRT1, input.texUVTap2) * g_FilterWeights[2];
	float4 tap3 = tex2D(g_SampRT1, input.texUVTap3) * g_FilterWeights[3];
	float4 tap4 = tex2D(g_SampRT1, input.texUVTap4) * g_FilterWeights[4];
	float4 tap5 = tex2D(g_SampRT1, input.texUVTap5) * g_FilterWeights[5];
	float4 tap6 = tex2D(g_SampRT1, input.texUVTap6) * g_FilterWeights[6];
	
	output.color = tap0 + tap1 + tap2 + tap3 + tap4 + tap5 + tap6;
	
	return output;
}

#define __DEFINE_GAUSSIAN_PS(x) \
 \
PS_OUT_Color \
GaussianBlur1D##x##SamplesPS(VS_OUT_GaussianBlur1D input) \
{ \
	PS_OUT_Color output = (PS_OUT_Color) 0; \
 \
	float4 tap0 = tex2D(g_SampRT1, input.texUVTap0) * g_FilterWeights[0]; \
	float4 tap1 = tex2D(g_SampRT1, input.texUVTap1) * g_FilterWeights[1]; \
	float4 tap2 = tex2D(g_SampRT1, input.texUVTap2) * g_FilterWeights[2]; \
	float4 tap3 = tex2D(g_SampRT1, input.texUVTap3) * g_FilterWeights[3]; \
	float4 tap4 = tex2D(g_SampRT1, input.texUVTap4) * g_FilterWeights[4]; \
	float4 tap5 = tex2D(g_SampRT1, input.texUVTap5) * g_FilterWeights[5]; \
	float4 tap6 = tex2D(g_SampRT1, input.texUVTap6) * g_FilterWeights[6]; \
\
	for(int i=7; i < x; ++i) \
	{ \
		float2 texUV = input.texUV + g_FilterTextureOffsets[i]; \
		output.color += tex2D(g_SampRT1, texUV) * g_FilterWeights[i]; \
	} \
 \
	output.color += tap0 + tap1 + tap2 + tap3 + tap4 + tap5 + tap6; \
	return output; \
}

__DEFINE_GAUSSIAN_PS(8)
__DEFINE_GAUSSIAN_PS(9)
__DEFINE_GAUSSIAN_PS(10)
__DEFINE_GAUSSIAN_PS(11)
__DEFINE_GAUSSIAN_PS(12)
__DEFINE_GAUSSIAN_PS(13)
__DEFINE_GAUSSIAN_PS(14)
__DEFINE_GAUSSIAN_PS(15)

#endif
