//==============================================================================
//
//		Upsample.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		23/11/09
//
//==============================================================================

texture g_ResultTex <int semantic = 4265; int usage = 1;>;
sampler g_SampResult = sampler_state
{
    	Texture = <g_ResultTex>;
    	MipFilter = LINEAR;
    	MinFilter = LINEAR;
    	MagFilter = LINEAR;
    	ADDRESSU  = CLAMP;
    	ADDRESSV  = CLAMP;
};

float2 g_ResultTexSize <int semantic = 4271; int usage = 1; > = float2(0.0f, 0.0f);

texture g_DepthTex <int semantic = 4266; int usage = 1;>;
sampler g_SampDepth = sampler_state
{
    	Texture = <g_DepthTex>;
    	MipFilter = LINEAR;
    	MinFilter = LINEAR;
    	MagFilter = LINEAR;
    	ADDRESSU  = CLAMP;
    	ADDRESSV  = CLAMP;
};

texture g_AlbedoTex <int semantic = 4273; int usage = 1;>;
sampler g_SampAlbedo = sampler_state
{
    	Texture = <g_AlbedoTex>;
    	MipFilter = LINEAR;
    	MinFilter = LINEAR;
    	MagFilter = LINEAR;
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

float2 ComputeBilateralUV(sampler sampDepth, float curDepth, float2 uv, float2 texSize)
{
	// Find the originating coarse texel coordinate center
	
	const static float PIXEL_CENTER_OFFSET = 0.5f;
	
	float2 texelUV = ( (uv * texSize) - PIXEL_CENTER_OFFSET );
	float2 remainUV = frac( texelUV );
	float2 centerUV = texelUV - remainUV;
	
	// Sample the depths/normals for each coarse sample position
	float2 	uv1 = (centerUV + float2(PIXEL_CENTER_OFFSET, PIXEL_CENTER_OFFSET)) 	/ texSize.xy;
	float2 	uv2 = (centerUV + float2(1.0f + PIXEL_CENTER_OFFSET, PIXEL_CENTER_OFFSET)) / texSize.xy;
	float2 	uv3 = (centerUV + float2(PIXEL_CENTER_OFFSET, 1.0f + PIXEL_CENTER_OFFSET)) / texSize.xy;
	float2 	uv4 = (centerUV + float2(1.0f + PIXEL_CENTER_OFFSET, 1.0f + PIXEL_CENTER_OFFSET)) / texSize.xy;
	
	float4 	sampleDepths;
	sampleDepths.x = tex2D(sampDepth, uv1).r;
	sampleDepths.y = tex2D(sampDepth, uv2).r;
	sampleDepths.z = tex2D(sampDepth, uv3).r;
	sampleDepths.w = tex2D(sampDepth, uv4).r;
	
	/*
	float4	dotAmount;
	dotAmount.x = dot( (tex2D(sampNormal, uv1).rgb - 0.5f) * 2.0f, curNormal );
	dotAmount.y = dot( (tex2D(sampNormal, uv2).rgb - 0.5f) * 2.0f, curNormal );
	dotAmount.z = dot( (tex2D(sampNormal, uv3).rgb - 0.5f) * 2.0f, curNormal );
	dotAmount.w = dot( (tex2D(sampNormal, uv4).rgb - 0.5f) * 2.0f, curNormal );
	dotAmount = pow(saturate(dotAmount), 32.0f);
	*/
	
	// Compare weights 
	float4 weights = ( 1.0f / (0.00000001f + abs(sampleDepths - curDepth)) ); //* dotAmount;
	
	// Modulate weights by bilinear percentage
	float4 weights1 = float4( float2(1.0f - remainUV), float2(remainUV.x, 1.0f - remainUV.y) ) * weights.xxyy;
	float4 weights2 = float4( float2(1.0f - remainUV.x, remainUV.y), float2(remainUV) ) * weights.zzww;
	float2 totalWeightsUV = weights1.xy + weights1.zw + weights2.xy + weights2.zw;
	
	// Find the new bilinear amount for the course sample
	float2 adjRemainUV = float2(weights1.z + weights2.z, weights2.y + weights2.w) / totalWeightsUV.xy;
	float2 newUV = (centerUV + adjRemainUV + float2(PIXEL_CENTER_OFFSET, PIXEL_CENTER_OFFSET)) / texSize.xy;
	
	return newUV;
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

struct PS_INUpsamplePS
{
	float2 psinput_0 : TEXCOORD0;
};

struct PS_OUTUpsamplePS
{
	float4 color : COLOR0;
};

PS_OUTUpsamplePS UpsamplePS(PS_INUpsamplePS input)
{
	PS_OUTUpsamplePS output;

	float curDepth		= Tex2D(g_SampDepth, input.psinput_0);
	
	// Ordinarily, we should offset by another 1/4 texel (in addition to the D3D texel offset for the frame buffer),
	// because the result texture size is 1/2 the size of the frame buffer
	// However, we also need to offset by -1/2 texel because of the blur pass normal sampling to mitigate light bleed
	// We only offset by 3/8 texel for best results (to minimize 1 pixel aliasing)
	float2 uv			= input.psinput_0.xy + float2(0.25f / g_ResultTexSize.x, 0.25f / g_ResultTexSize.y);
	float2 bilateralUV	= uv;//ComputeBilateralUV(g_SampDepth, curDepth, uv, g_ResultTexSize);
	
	float4 albedo		= Tex2D(g_SampAlbedo, input.psinput_0);
	float4 occl			= pow(Tex2D(g_SampResult, bilateralUV).xxxx, 2.695f);

	occl				= 1.0f - exp( occl * -8.75f );
	
	output.color		= occl;// * albedo;
	
	
	return output;
}


//=============================== Techniques ==========================================

technique t0
{
	pass p0
	{
		VertexShader = compile vs_3_0 SimpleScreenSpaceVS();
		PixelShader = compile ps_3_0 UpsamplePS();
	}

};

