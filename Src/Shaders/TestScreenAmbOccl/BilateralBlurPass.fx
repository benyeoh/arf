//================================================================================
//
//		BilateralBlurPass.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/5/2012
//
//================================================================================

#include "[shd]\TestScreenAmbOccl\ParamSemanticDefs.fxh"

texture g_AOTex <int semantic = AO_TEX; int usage = 1;>;
sampler g_SampAO = sampler_state
{
    	Texture = <g_AOTex>;
    	MipFilter = LINEAR;
    	MinFilter = LINEAR;
    	MagFilter = LINEAR;
    	ADDRESSU  = CLAMP;
    	ADDRESSV  = CLAMP;
};

float2 g_DirectionUVStep <int semantic = DIRECTION_UV_STEP; int usage = 0;>;
float2 g_BlurParams <int semantic = BLUR_PARAMS; int usage = 0;>;

//=============================== Functions ==========================================

float4 Tex2D(sampler samp, float2 uv)
{
	return tex2D(samp, uv);
}

float Vec4X(float4 v4)
{
	return v4.x;
}

float3 CompressNormalizedFloat8BitRGB(float val)
{
/*
	float cur = val * 255.0f;
	float x = trunc(cur) * (1.0f / 255.0f);

	cur = frac(cur) * 256.0f;
	float y = trunc(cur) * (1.0f / 255.0f);

	cur = frac(cur) * 256.0f;
	float z = trunc(cur) * (1.0f / 255.0f);
*/

	float2 cur2 = val * float2(255.0f, 255.0f * 256.0f);

	float3 cur3 = float3(cur2.x, frac(cur2.xy) * 256.0f);
	
	float3 toReturn = trunc(cur3) * (1.0f / 255.0f);
	return toReturn;
}

float UncompressNormalizedFloat8BitRGB(float3 rgb)
{
	float3 val = rgb * float3(1.0f, 1.0f / 256.0f, 1.0f / (256.0f * 256.0f));
	return dot(val, float3(1.0f, 1.0f, 1.0f));
}

//=============================== Vertex Shaders ==========================================

struct VS_INSimpleScreenSpaceVS
{
	float4 vsinput_0 : POSITION;
	float2 vsinput_1 : TEXCOORD0;
};

struct VS_OUTSimpleScreenSpaceVS
{
	float4 vsoutput_0 : POSITION0;
	float2 vsoutput_1 : TEXCOORD0;
};

VS_OUTSimpleScreenSpaceVS SimpleScreenSpaceVS(VS_INSimpleScreenSpaceVS input)
{
	VS_OUTSimpleScreenSpaceVS output;

	output.vsoutput_0 = input.vsinput_0;// - float4(1.0f / 508.0f, -1.0f / 370.0f, 0.0f, 0.0f);
	output.vsoutput_1 = input.vsinput_1;
	return output;
}


//=============================== Pixel Shaders ==========================================

struct PS_IN
{
	float2 texUV : TEXCOORD0;
};

struct PS_OUT
{
	float4 color : COLOR0;
};

PS_OUT BilateralGaussian7PS(PS_IN input)
{
	PS_OUT output;

	// Gaussian coefficients
	const float s_Gaussian[4] = { 0.356642f, 0.239400f, 0.072410f, 0.009869f };
	//	{ 0.398943, 0.241971, 0.053991, 0.004432, 0.000134 };  // stddev = 1.0
	//	{ 0.153170, 0.144893, 0.122649, 0.092902, 0.062970 };  // stddev = 2.0
	//	{ 0.111220, 0.107798, 0.098151, 0.083953, 0.067458, 0.050920, 0.036108 }; // stddev = 3.0

	float4 texVal = tex2D( g_SampAO, input.texUV );
	float linearDepth = UncompressNormalizedFloat8BitRGB(texVal.yzw);

	float totalWeight = s_Gaussian[0];
	float curAO = texVal.x * s_Gaussian[0];

	for(int i=1; i < 4; i++)
	{
		float4 sampleVal = tex2D( g_SampAO, input.texUV + i * g_DirectionUVStep );
		float sampleLinearDepth = UncompressNormalizedFloat8BitRGB(sampleVal.yzw);

		float weight = g_BlurParams.x + s_Gaussian[i];
		float depthDiff = abs(linearDepth - sampleLinearDepth);

		weight *= max(0.0f, 1.0f - (2000.0f * g_BlurParams.y) * depthDiff);

		curAO += weight * sampleVal.x;
		totalWeight += weight;
	}

	curAO = curAO / totalWeight;

	output.color = float4(curAO, texVal.yzw);
	return output;
}

//=============================== Techniques ==========================================

technique t0
{
	pass p0
	{
		ZEnable = FALSE;

		VertexShader = compile vs_3_0 SimpleScreenSpaceVS();
		PixelShader = compile ps_3_0 BilateralGaussian7PS();
	}

};
