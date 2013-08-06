//==============================================================================
//
//		CopyToDepth.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		23/11/09
//
//==============================================================================

#include "[shd]\BaseFXUtils\ParamSemanticDefs.fxh"

texture g_ZPosAccumTex <int semantic = BFXU_RT1; int usage = 1;>;
sampler g_SampZPosAccum = sampler_state
{
    	Texture = <g_ZPosAccumTex>;
    	MipFilter = POINT;
    	MinFilter = POINT;
    	MagFilter = POINT;
    	ADDRESSU  = CLAMP;
    	ADDRESSV  = CLAMP;
};


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


	output.vsoutput_0 = input.vsinput_0;
	output.vsoutput_1 = input.vsinput_1;
	return output;
}


//=============================== Pixel Shaders ==========================================

struct PS_INCopyToDepthPS
{
	float2 psinput_0 : TEXCOORD0;
};

struct PS_OUTCopyToDepthPS
{
	float psoutput_0 : DEPTH;
	float4 psoutput_1 : COLOR0;
};

PS_OUTCopyToDepthPS CopyToDepthPS(PS_INCopyToDepthPS input)
{
	PS_OUTCopyToDepthPS output;

	float4 Tex2D_0 = Tex2D(g_SampZPosAccum, input.psinput_0);

	float Vec4X_1 = Vec4X(Tex2D_0);

	output.psoutput_0 = Vec4X_1;
	output.psoutput_1 = 0.0f;
	return output;
}


//=============================== Techniques ==========================================

technique t0
{
	pass p0
	{
		ZFunc = ALWAYS;
		ColorWriteEnable = 0;
		VertexShader = compile vs_2_0 SimpleScreenSpaceVS();
		PixelShader = compile ps_2_0 CopyToDepthPS();
	}

};

