//================================================================================
//
//		AOPass.fx
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

texture g_NormalsTex <int semantic = NORMALS_TEX; int usage = 1;>;
sampler g_SampNormals = sampler_state
{
    	Texture = <g_NormalsTex>;
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

int g_NumSamples <int semantic = NUM_SAMPLES; int usage = 0;>;
float4 g_AlchemyAOParams <int semantic = ALCHEMY_AO_PARAMS; int usage = 0;>;	// x = intensity, y = contrast, z = bias 
float4 g_DistributedSampleParams <int semantic = DISTRIBUTED_SAMPLE_PARAMS; int usage = 0;>; // x = world radius, y = num of turns, z = screen radius control

struct FarPlaneOffsets
{
	float4 offsets[3];
};

FarPlaneOffsets g_FarPlaneOffsets <int semantic = FAR_PLANE_OFFSETS; int usage = 0;>;
float4 g_CameraPosWithViewFar <int semantic = CAMERA_POS_WITH_VIEW_FAR; int usage = 0;>;
float4 g_ProjParams <int semantic = PROJ_PARAMS; int usage = 1;>; // xy = Proj matrix values, zw = screen width/height

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

float3 Uncompress8BitVec4ToVec3(float4 val8Bit)
{
	float3 hiUncompress = val8Bit.xyz * float3(255.0f * 8.0f, 255.0f * 8.0f, 255.0f * 4.0f);
	float curLo = val8Bit.w * (255.0f / 32.0f);
	float loX = trunc(curLo);
	curLo = frac(curLo) * 8.0f;
	float loY = trunc(curLo);
	float loZ = frac(curLo) * 4.0f;
	
	float3 loUncompress = float3(loX, loY, loZ);
	float3 uncompress = (hiUncompress + loUncompress) * float3(1.0f / 2047.0f, 1.0f / 2047.0f, 1.0f / 1023.0f);
	return uncompress;
}

float3 RangeMiddleZeroVec3(float3 v3)
{
	return ((v3 - 0.5f) * 2.0f);
}

float3 GetViewCenteredPos(float2 texUV, float linearDepth)
{
	float3 xOffset = texUV.x * g_FarPlaneOffsets.offsets[1];
	float3 yOffset = texUV.y * g_FarPlaneOffsets.offsets[2];
	float3 farOffset = g_FarPlaneOffsets.offsets[0] + xOffset + yOffset;

	float3 viewCenteredPos = farOffset * linearDepth;
	return viewCenteredPos;
}

float ComputeNoise(float2 texUV)
{
	const static float2 NOISE_SCALE = 12.0f;
	float randomPhi = (tex2D( g_SampNoise, texUV * NOISE_SCALE).r) * 2.0f * 3.1425f;	
	return randomPhi;
}

float BilateralQuadBlur(float val, float texUV, float linearDepth)
{
	const float DEPTH_DIFF_THRESHOLD = 0.02f;

	// Bilateral box-filter in a quad (see paper)
	float2 quadXYCmp = floor(texUV * g_ProjParams.zw) * 0.5f;
	//quadXYCmp = ((1.0f - (frac(quadXYCmp) * 2.0f)) - 0.5f) * 2.0f; //frac(quadXYCmp) * -2.0f + 0.5f;
	quadXYCmp = ( 1.0f - (frac(quadXYCmp) * 4.0f) );
	//quadXYCmp = ( 0.5f - (frac(quadXYCmp) * 2.0f) ) * 2.0f;
	
	if (abs(ddx(linearDepth)) < DEPTH_DIFF_THRESHOLD) 
	{
	//	val += (ddx(val) * quadXYCmp.x) * 0.5f;
	}

	if (abs(ddy(linearDepth)) < DEPTH_DIFF_THRESHOLD) 
	{
	//	val -= (ddy(val) * quadXYCmp.y) * 0.5f;
	}

	return val;
}

float4 ComputeDistributionUV(int sampleNumber, float randomPhi, float viewZ, float2 uv)
{
	float alpha = (1.0f / (float)g_NumSamples) * ((float) sampleNumber + 1.5f);
	float2 screenRadius = (g_DistributedSampleParams.xx * g_ProjParams.xy) / viewZ;
	float2 h = screenRadius * alpha;

	float theta = 2.0f * 3.14159265359f * alpha * g_DistributedSampleParams.y + randomPhi;
	float sinTheta = sin(theta);
	float cosTheta = cos(theta);
	//sincos(theta, sinTheta, cosTheta);
	float2 offset = float2(cosTheta, sinTheta);

	// Only compute for x direction (assume pixels are same sized)
	float mipLevel = min( log2((h.x * g_ProjParams.z) / g_DistributedSampleParams.z), (float) NUM_Z_MIPS );

	float2 sampleUV = (h * offset + uv);
	return float4(sampleUV, 0.0f, mipLevel);
}

float4 ComputePoissonUV(int sampleNumber, float2 uv)
{
	const float2 s_Offsets[16] = 
	{ 
		float2(0.09431711f, 0.5941997f),
		float2(0.4307326f, -0.0523821f),
		float2(0.5709904f, 0.4426674f),
		float2(-0.2226814f, 0.1452472f),
		float2(-0.4285987f, 0.8203481f),
		float2(0.4150211f, 0.9065326f),
		float2(-0.5673039f, 0.4133336f),
		float2(-0.9493649f, 0.254185f),
		float2(-0.7343475f, -0.3721381f),
		float2(-0.1060963f, -0.2668646f),
		float2(-0.4190173f, -0.6772105f),
		float2(-0.1290366f, -0.9784364f),
		float2(0.3123248f, -0.5879446f),
		float2(0.7560202f, -0.6177078f),
		float2(0.8244288f, -0.1976019f),
		float2(0.9751366f, 0.2143216f)
	};

	return float4(s_Offsets[sampleNumber] * 0.01f + uv, 0.0f, 0.0f);
}

float ComputeAlchemyFalloff(float3 center, float linearDepth, float3 samplePos, float3 normal)
{
	const static float EPSILON = 0.0001f;

	float3 v = samplePos - center;
	float top = max(0.0f, dot(v, normal) - g_AlchemyAOParams.z * linearDepth);
	float distFalloff = dot(v, v) + EPSILON;

	return top / distFalloff;
}

float ComputeAlchemyFinal(float totalFalloff)
{
	float accessibility = max(0.0f, 1.0f - (2.0f * g_AlchemyAOParams.x * (1.0f / (float)g_NumSamples) * totalFalloff));
	return pow(accessibility, g_AlchemyAOParams.y);
}

float ComputeUpdatedAlchemyFalloff(float3 center, float linearDepth, float3 samplePos, float3 normal)
{
	const static float EPSILON = 0.001f;

	float3 v = samplePos - center;
	float vDistSq = dot(v, v);
	float worldFalloff = pow( max(0.0f, (g_DistributedSampleParams.x * g_DistributedSampleParams.x) - vDistSq), 3.0f);
	float geometricFalloff = max( 0.0f, (dot(v, normal) - g_AlchemyAOParams.z) / (vDistSq + EPSILON) );
	return worldFalloff * geometricFalloff;
}

float ComputeUpdatedAlchemyFinal(float totalFalloff)
{ 
	float worldDistFactor = pow(g_DistributedSampleParams.x, 6.0f);
	float accessibility = max(0.0f, 1.0f - ((5.0f * g_AlchemyAOParams.x) / (worldDistFactor * (float)g_NumSamples)) * totalFalloff);
	return pow(accessibility, g_AlchemyAOParams.y);
}

PS_OUT AOPassPS(PS_IN input)
{
	PS_OUT output;

	float randomPhi = ComputeNoise(input.texUV);	
	float linearDepth = tex2D( g_SampZPos, input.texUV ).r;
	float4 compressedNormal = tex2D( g_SampNormals, input.texUV );
	float3 normal = normalize( RangeMiddleZeroVec3( Uncompress8BitVec4ToVec3(compressedNormal) ) );

	float3 viewCenteredPos = GetViewCenteredPos(input.texUV, linearDepth);
	float viewZ = linearDepth * g_CameraPosWithViewFar.w;

	float totalOcclusion = 0.0f;

	for(int i=0; i < g_NumSamples; i++)
	{
		// Compute distribution
		float4 sampleUV = ComputeDistributionUV(i, randomPhi, viewZ, input.texUV);

		// Reset mip level for now
		//sampleUV.w = 0.0f;
		float sampleLinearDepth = tex2Dlod(g_SampZPos, sampleUV);
		float3 sampleViewCenteredPos = GetViewCenteredPos(sampleUV.xy, sampleLinearDepth);

		// Compute AO
		float occlusion = ComputeAlchemyFalloff(viewCenteredPos, linearDepth, sampleViewCenteredPos, normal);
		totalOcclusion += occlusion;
	}

	float accessibility = ComputeAlchemyFinal(totalOcclusion);

	// Bilateral box-filter in a quad (see paper)
	accessibility = BilateralQuadBlur(accessibility, input.texUV, linearDepth);

	output.color.xyzw = accessibility;
	return output;
}

PS_OUT AOPassUpdatedAlchemyPS(PS_IN input)
{
	PS_OUT output;

	float randomPhi = ComputeNoise(input.texUV);	
	float linearDepth = tex2D( g_SampZPos, input.texUV ).r;
	float4 compressedNormal = tex2D( g_SampNormals, input.texUV );
	float3 normal = normalize( RangeMiddleZeroVec3( Uncompress8BitVec4ToVec3(compressedNormal) ) );

	float3 viewCenteredPos = GetViewCenteredPos(input.texUV, linearDepth);
	float viewZ = linearDepth * g_CameraPosWithViewFar.w;

	float totalOcclusion = 0.0f;

	for(int i=0; i < g_NumSamples; i++)
	{
		// Compute distribution
		float4 sampleUV = ComputeDistributionUV(i, randomPhi, viewZ, input.texUV);

		// Reset mip level for now
		//sampleUV.w = 0.0f;
		float sampleLinearDepth = tex2Dlod(g_SampZPos, sampleUV);
		float3 sampleViewCenteredPos = GetViewCenteredPos(sampleUV.xy, sampleLinearDepth);

		// Compute AO
		float occlusion = ComputeUpdatedAlchemyFalloff(viewCenteredPos, linearDepth, sampleViewCenteredPos, normal);
		totalOcclusion += occlusion;
	}
	
	float accessibility = ComputeUpdatedAlchemyFinal(totalOcclusion);

	// Bilateral box-filter in a quad (see paper)
	accessibility = BilateralQuadBlur(accessibility, input.texUV, linearDepth);

	output.color.xyzw = accessibility;
	return output;
}

//=============================== Techniques ==========================================

technique t0
{
	pass p0
	{
		ZEnable = FALSE;
		StencilEnable		= TRUE;
		StencilFunc = EQUAL;
		StencilRef = 0x1;
		StencilMask = 0x1;

		VertexShader = compile vs_3_0 SimpleScreenSpaceVS();
		PixelShader = compile ps_3_0 AOPassPS();
	}

};

technique t1
{
	pass p0
	{
		ZEnable = FALSE;
		StencilEnable		= TRUE;
		StencilFunc = EQUAL;
		StencilRef = 0x1;
		StencilMask = 0x1;

		VertexShader = compile vs_3_0 SimpleScreenSpaceVS();
		PixelShader = compile ps_3_0 AOPassUpdatedAlchemyPS();
	}

};