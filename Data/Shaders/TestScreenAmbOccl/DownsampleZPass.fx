//================================================================================
//
//		DownsampleZPass.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/5/2012
//
//================================================================================

#include "[shd]\TestScreenAmbOccl\ParamSemanticDefs.fxh"

texture g_ZPosTex <int semantic = Z_POS_TEX; int usage = 1;>;
sampler g_SampZPos = sampler_state
{
    	Texture = <g_ZPosTex>;
    	MipFilter = LINEAR;
    	MinFilter = LINEAR;
    	MagFilter = LINEAR;
    	ADDRESSU  = CLAMP;
    	ADDRESSV  = CLAMP;
};

texture g_NoiseTex <int semantic = NOISE_TEX; int usage = 1;>;
sampler g_SampNoise = sampler_state
{
    	Texture = <g_NoiseTex>;
    	MipFilter = LINEAR;
    	MinFilter = LINEAR;
    	MagFilter = LINEAR;
    	ADDRESSU  = WRAP;
    	ADDRESSV  = WRAP;
};

int g_SampleLOD <int semantic = SAMPLE_LOD; int usage = 1;>;
float2 g_HalfPixelOffset <int semantic = HALF_PIXEL_OFFSET; int usage = 1;>;

//=============================== Functions ==========================================

float4 Tex2D(sampler samp, float2 uv)
{
	return tex2D(samp, uv);
}

float Vec4X(float4 v4)
{
	return v4.x;
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

PS_OUT DownsampleZPassRotatedGridPS(PS_IN input)
{
	PS_OUT output;

	float2 s_RotatedGridDir[4] =
	{
		float2(-1.0f, -1.0f),
		float2(1.0f, -1.0f),
		float2(1.0f, 1.0f),
		float2(-1.0f, 1.0f),
	};

	// Simulate rotated grid with SM 3.0
	const static float2 NOISE_SCALE = 12.0f;

	float rand = tex2D( g_SampNoise, input.texUV * NOISE_SCALE ).r;
	float4 randLess = rand < float4(0.25f, 0.5f, 0.75f, 1.1f);
	float4 randGreater = rand >= float4(0.0f, 0.25f, 0.5f, 0.75f);
	float4 randCmp = randLess * randGreater;

	float2 dir = s_RotatedGridDir[0] * randCmp.x + s_RotatedGridDir[1] * randCmp.y + s_RotatedGridDir[2] * randCmp.z + s_RotatedGridDir[3] * randCmp.w;
	dir *= g_HalfPixelOffset;

	float depth = tex2Dlod( g_SampZPos, float4(input.texUV + dir, 0.0f, (float) g_SampleLOD) ).r;
	
	output.color = depth;
	return output;
}

PS_OUT DownsampleZPassAveragePS(PS_IN input)
{
	PS_OUT output;

	float depth = tex2Dlod( g_SampZPos, float4(input.texUV, 0.0f, (float) g_SampleLOD) ).r;
	
	output.color = depth;
	return output;
}

//=============================== Techniques ==========================================

technique t0
{
	pass p0
	{
		ZEnable = FALSE;

		VertexShader = compile vs_3_0 SimpleScreenSpaceVS();
		PixelShader = compile ps_3_0 DownsampleZPassRotatedGridPS();
	}

};

technique t1
{
	pass p0
	{
		ZEnable = FALSE;

		VertexShader = compile vs_3_0 SimpleScreenSpaceVS();
		PixelShader = compile ps_3_0 DownsampleZPassAveragePS();
	}

};
