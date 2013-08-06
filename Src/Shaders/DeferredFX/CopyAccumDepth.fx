//==============================================================================
//
//		CopyAccumDepth.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		23/11/09
//
//==============================================================================


texture g_TargetTex <int semantic = 4265; int usage = 1;>;
sampler g_SampTarget = sampler_state
{
    	Texture = <g_TargetTex>;
    	MipFilter = POINT;
    	MinFilter = POINT;
    	MagFilter = POINT;
    	ADDRESSU  = CLAMP;
    	ADDRESSV  = CLAMP;
};

//float2 g_ScreenPixelOffset <int semantic = 4270; int usage = 1;>;


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
	//output.vsoutput_0.xy -= float2(g_ScreenPixelOffset.x, -g_ScreenPixelOffset.y); 
	output.vsoutput_1 = input.vsinput_1;
	return output;
}


//=============================== Pixel Shaders ==========================================

struct PS_INCopyAccumDepthPS
{
	float2 psinput_0 : TEXCOORD0;
};

struct PS_OUTCopyAccumDepthPS
{
	float4 color : COLOR0;
};

PS_OUTCopyAccumDepthPS CopyAccumDepthPS(PS_INCopyAccumDepthPS input)
{
	PS_OUTCopyAccumDepthPS output;

	output.color = Tex2D(g_SampTarget, input.psinput_0);
	return output;
}


//=============================== Techniques ==========================================

technique t0
{
	pass p0
	{
		ZEnable = FALSE;
		VertexShader = compile vs_3_0 SimpleScreenSpaceVS();
		PixelShader = compile ps_3_0 CopyAccumDepthPS();
	}

};

