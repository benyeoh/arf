
#include "Common.cg"

cbuffer SAOParams
{
	float4 g_ViewFarPlaneUVOffset;
	float4 g_ProjParams;	// x = proj[0][0], y = proj[1][1], z = screenWidth, w = screenHeight

	float g_Intensity;		
	float g_Contrast;
	float g_Bias;
	float g_SampleOffset;

	float g_OcclusionMax;
	float g_FalloffRadius;
	float g_SpiralTurns;
	float g_ScreenMipRadiusFactor;		// Higher value forces higher LOD sampling

	float g_FarPlane;
    float g_NearFalloffThreshold;
    float g_MaxScreenRadius;
	int	g_NumSamples;
}

cbuffer LinearizeParams
{
	float g_ProjMat22;						// Q = far / (far - near)
	float g_ProjMat32RcpFar;				// Q * near / far
}

cbuffer DownsampleParams
{
	float2 g_RenderTargetSize;
}

Texture2D<float4>	g_NormalTex;
Texture2D<float>	g_DepthTex;
Texture2D<float4>	g_AOTex;

SamplerState g_SamplePoint
{
	Filter                      = MIN_MAG_MIP_POINT;
	AddressU                    = CLAMP;
	AddressV                    = CLAMP;
};

SamplerState g_SampleLinear
{
	Filter                      = MIN_MAG_MIP_LINEAR;
	AddressU                    = CLAMP;
	AddressV                    = CLAMP;
};

SamplerState g_SamplePointMipLinear;

struct VS_INPUT
{
	float3 position : POSITION; 
	float2 texCoord : TEXCOORD0; 
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION; 
	float2 texCoord : TEXCOORD0;
};

VS_OUTPUT MainVS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	Out.position.x = In.position.x;
	Out.position.y = In.position.y;
	Out.position.z = In.position.z;
	Out.position.w = 1.0f;
	
	Out.texCoord = In.texCoord;
	
	return Out;    
}

VS_OUTPUT MainFarPlaneVS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	Out.position.x = In.position.x;
	Out.position.y = In.position.y;
	Out.position.z = 1.0f;
	Out.position.w = 1.0f;
	
	Out.texCoord = In.texCoord;
	
	return Out;    
}

// Linearize depth
float GetLinearizeDepth(float projDepth)
{
	return -g_ProjMat32RcpFar / ( projDepth - g_ProjMat22 );
	//return -g_ProjMat32RcpFar / (g_ProjMat22 - projDepth);
}

//出力は32bit
#if defined(BUILD_LinearizeDepthPS) && defined(EBONY_ORBIS)
#pragma PSSL_target_output_format(target 0 FMT_32_R)
#endif
float LinearizeDepthPS(VS_OUTPUT In) : SV_TARGET0
{
	float linearDepth = GetLinearizeDepth( g_DepthTex.SampleLevel( g_SamplePoint, In.texCoord, 0 ) );
	return linearDepth; 
}

// Downsampling
// Rotated grid
float SampleRotatedGrid(float2 uv)
{
	int2 pixelPos = uv * g_RenderTargetSize;
	pixelPos = pixelPos * 2 + int2((pixelPos.y & 1) ^ 1, (pixelPos.x & 1) ^ 1);
	return g_DepthTex.Load( int3(pixelPos, 0) );
}

// Rotated grid
float4 SampleMax(float2 uv)
{
	float d1 = g_DepthTex.SampleLevel( g_SamplePoint, uv + float2(-0.25f, 0.25f) / g_RenderTargetSize, 0 ).r;
	float d2 = g_DepthTex.SampleLevel( g_SamplePoint, uv + float2(0.25f, 0.25f) / g_RenderTargetSize, 0 ).r;
	float d3 = g_DepthTex.SampleLevel( g_SamplePoint, uv + float2(-0.25f, -0.25f) / g_RenderTargetSize, 0 ).r;
	float d4 = g_DepthTex.SampleLevel( g_SamplePoint, uv + float2(0.25f, -0.25f) / g_RenderTargetSize, 0 ).r;

	return max( max( max(d1, d2), d3 ), d4 );
}

// Average
float4 SampleAverage(float2 uv)
{
	return g_DepthTex.SampleLevel( g_SampleLinear, uv, 0 );
}

#if defined(BUILD_DownsampleDepthRotatedGridPS) && defined(EBONY_ORBIS)
#pragma PSSL_target_output_format(target 0 FMT_32_R)
#endif
float DownsampleDepthRotatedGridPS(VS_OUTPUT In) : SV_TARGET0
{
	return SampleRotatedGrid(In.texCoord);
}

#if defined(BUILD_DownsampleDepthTopLeftPS) && defined(EBONY_ORBIS)
#pragma PSSL_target_output_format(target 0 FMT_32_R)
#endif
float DownsampleDepthTopLeftPS(VS_OUTPUT In) : SV_TARGET0
{
	int2 pixelPos = In.texCoord * g_RenderTargetSize; // round down
	pixelPos = pixelPos * 2;
	return g_DepthTex.Load( int3(pixelPos, 0) );
}

// AO
float3 ComputeViewPos(float2 uv, float linearDepth)
{
	return float3(g_ViewFarPlaneUVOffset.xy + uv * g_ViewFarPlaneUVOffset.zw, g_FarPlane) * linearDepth;
}

float3 ComputeNormalFromDDXY(float3 center, float2 texUV, float centerDepth)
{
	float4 linearDepth;
	linearDepth.x = g_DepthTex.SampleLevel( g_SamplePoint, texUV - float2(1.0f / g_ProjParams.z, 0.0f), 0 ).r;
	linearDepth.y = g_DepthTex.SampleLevel( g_SamplePoint, texUV + float2(1.0f / g_ProjParams.z, 0.0f), 0 ).r;
	linearDepth.z = g_DepthTex.SampleLevel( g_SamplePoint, texUV - float2(0.0f, 1.0f / g_ProjParams.w), 0 ).r;
	linearDepth.w = g_DepthTex.SampleLevel( g_SamplePoint, texUV + float2(0.0f, 1.0f / g_ProjParams.w), 0 ).r;

	float4 linearDepthDiffAbs = abs(linearDepth.xyzw - centerDepth);
	float4 diffXY;
	diffXY.xy = linearDepthDiffAbs.x < linearDepthDiffAbs.y ? float2(linearDepth.x, -1.0f) : float2(linearDepth.y, 1.0f);
	diffXY.zw = linearDepthDiffAbs.z < linearDepthDiffAbs.w ? float2(linearDepth.z, -1.0f) : float2(linearDepth.w, 1.0f);

	float3 posLR = ComputeViewPos(texUV + float2(1.0f / g_ProjParams.z, 0.0f) * diffXY.y, diffXY.x);
	float3 posTB = ComputeViewPos(texUV + float2(0.0f, 1.0f / g_ProjParams.w) * diffXY.w, diffXY.z);

	float3 diffLR = (posLR - center) * diffXY.y;
	float3 diffTB = (posTB - center) * diffXY.w;
	
	float3 normal = normalize( cross(diffLR, diffTB) );
	return normal;

/*
	float2 texUVddx = texUV;
	float2 texUVddy = texUV;

	texUVddx.x += 1.0f / g_ProjParams.z;
	texUVddy.y += 1.0f / g_ProjParams.w;

	float linearDepthddx = g_DepthTex.SampleLevel( g_SamplePoint, texUVddx, 0 ).r;
	float linearDepthddy = g_DepthTex.SampleLevel( g_SamplePoint, texUVddy, 0 ).r;

	float3 posddx = ComputeViewPos(texUVddx, linearDepthddx);
	float3 posddy = ComputeViewPos(texUVddy, linearDepthddy);

	float3 diffddx = posddx - center;
	float3 diffddy = posddy - center;

	float3 normal = normalize( cross(diffddx, diffddy) );
	return normal;
*/
}


float3 ComputeNormalFromBuffer(float2 uv)
{
	float4 texColor = g_NormalTex.SampleLevel( g_SampleLinear, uv, 0 );

	float3 normal;
	normal.xyz = texColor.xyz;
	normal.z = -normal.z;
	//normal.z = -sqrt( saturate(1.0f - dot(normal.xy, normal.xy)) );
	
	return normal;
}

float ComputeNoise(float2 uv)
{
	// Hash function used in the HPG12 AlchemyAO paper
	// For DX11
	int2 pixelPos = uv * g_ProjParams.zw;		
	float randomPhi = (3 * pixelPos.x ^ pixelPos.y + pixelPos.x * pixelPos.y) * 10.0f;

	//const static float2 NOISE_SCALE = 13.0f;
	//float randomPhi = (tex2D( g_SampNoise, texUV * NOISE_SCALE).r) * 2.0f * 3.1425f;	
	return randomPhi;//3.1425f;
}

float ComputeScreenRadius(float viewZ)
{
	return min(g_FalloffRadius.x * g_ProjParams.x / viewZ, g_MaxScreenRadius);
}

float ScreenToWorldRadius(float screenRadius, float viewZ)
{
    return screenRadius * viewZ / g_ProjParams.x;
}

float3 ComputeDistributionUV(float sampleNumber, float totalNumSamplesRcp, float randomPhi, float screenRadius, float2 uv)
{
	float alpha = totalNumSamplesRcp * (sampleNumber + g_SampleOffset);
	float h = screenRadius * alpha;

	float theta = alpha * g_SpiralTurns + randomPhi;
	float sinTheta;// = sin(theta);
	float cosTheta;// = cos(theta);
	sincos(theta, sinTheta, cosTheta);
	float2 offset = float2(cosTheta, sinTheta);

	// Only compute for x direction (assume pixels are same sized)
	int mipLevel = ( log2(h * g_ScreenMipRadiusFactor) );

	float2 sampleUV = (h * offset + uv);
	return float3(sampleUV, mipLevel);
}

float3 ComputePoisson32DistributionUV(int sampleNumber, float2 randomSinCos, float screenRadius, float2 uv)
{
    const float2 POISSON_DISK[32] =
    {
        float2( -0.8177347f, 0.5533974f ),
        float2( -0.6688132f, 0.2254228f ),
        float2( -0.5193512f, 0.6943126f ),
        float2( -0.290081f, 0.4116245f ),
        float2( 0.0132946f, 0.7425355f ),
        float2( -0.9834577f, 0.03774452f ),
        float2( -0.6324883f, -0.05413958f ),
        float2( -0.9021741f, -0.2461275f ),
        float2( -0.2996171f, -0.07363617f ),
        float2( -0.007870576f, 0.1384686f ),
        float2( -0.2881964f, 0.9086592f ),
        float2( -0.5371919f, -0.4440133f ),
        float2( -0.811841f, -0.5194715f ),
        float2( 0.4237712f, 0.4639156f ),
        float2( 0.3885317f, 0.1238827f ),
        float2( 0.1290494f, -0.209084f ),
        float2( -0.126803f, -0.3286369f ),
        float2( 0.02186069f, 0.4338363f ),
        float2( 0.9011614f, 0.0127611f ),
        float2( 0.6640795f, 0.2264349f ),
        float2( 0.7466206f, 0.5088775f ),
        float2( 0.4422842f, -0.3218993f ),
        float2( 0.2903487f, 0.8791544f ),
        float2( 0.5559115f, 0.7576901f ),
        float2( -0.1217322f, -0.7082906f ),
        float2( -0.480333f, -0.7835478f ),
        float2( 0.8019803f, -0.3733774f ),
        float2( 0.9413655f, 0.2936744f ),
        float2( 0.5905918f, -0.07309945f ),
        float2( 0.5413913f, -0.5890846f ),
        float2( 0.2322987f, -0.6306686f ),
        float2( 0.1941509f, -0.912946f )
    };

    const float POISSON_DISK_LENGTH[32] =
    {
        length( POISSON_DISK[0] ),
        length( POISSON_DISK[1] ),
        length( POISSON_DISK[2] ),
        length( POISSON_DISK[3] ),
        length( POISSON_DISK[4] ),
        length( POISSON_DISK[5] ),
        length( POISSON_DISK[6] ),
        length( POISSON_DISK[7] ),
        length( POISSON_DISK[8] ),
        length( POISSON_DISK[9] ),
        length( POISSON_DISK[10] ),
        length( POISSON_DISK[11] ),
        length( POISSON_DISK[12] ),
        length( POISSON_DISK[13] ),
        length( POISSON_DISK[14] ),
        length( POISSON_DISK[15] ),
        length( POISSON_DISK[16] ),
        length( POISSON_DISK[17] ),
        length( POISSON_DISK[18] ),
        length( POISSON_DISK[19] ),
        length( POISSON_DISK[20] ),
        length( POISSON_DISK[21] ),
        length( POISSON_DISK[22] ),
        length( POISSON_DISK[23] ),
        length( POISSON_DISK[24] ),
        length( POISSON_DISK[25] ),
        length( POISSON_DISK[26] ),
        length( POISSON_DISK[27] ),
        length( POISSON_DISK[28] ),
        length( POISSON_DISK[29] ),
        length( POISSON_DISK[30] ),
        length( POISSON_DISK[31] ),
    };

    float2 s = POISSON_DISK[sampleNumber];
    float2 sinCosX = randomSinCos * s.xx;
    float2 sinCosY = randomSinCos * s.yy;
    float2 rotSample;
    rotSample.x = sinCosX.y - sinCosY.x;
    rotSample.y = sinCosX.x + sinCosY.y;

	float h = screenRadius * POISSON_DISK_LENGTH[sampleNumber];

	// Only compute for x direction (assume pixels are same sized)
	int mipLevel = ( log2(h * g_ScreenMipRadiusFactor) );

	float2 sampleUV = rotSample * screenRadius + uv; //(h * offset + uv);
	return float3(sampleUV, mipLevel);
}


float ComputeSphereFalloff(float3 center, float linearDepth, float3 samplePos, float3 normal, float bias)
{
	const float EPSILON = 0.0001f;
	
	float3 v = samplePos - center;
	float lengthSq = dot(v, v);
	float3 dirV = v * rsqrt(lengthSq);
	float cosAlpha = saturate( dot(normal, dirV ) );
	
	return clamp(cosAlpha * 1.0f / ((lengthSq) + EPSILON + bias * bias), 0.0f, g_OcclusionMax);  
}

float ComputeLinearFalloff(float falloffRadius, float3 center, float3 samplePos, float3 normal, float bias, float occlusionMax, float occlusionScale)
{	
	const float EPSILON = 0.01;
  
	float3 posToSample = (samplePos - center) / falloffRadius;
    float posToSampleDistSq = dot(posToSample, posToSample);
	float distFalloff = posToSampleDistSq + EPSILON;

	float closeOcclMax = max(occlusionMax - posToSampleDistSq * occlusionScale, 1.0);

    float resNear = clamp( max(0.0, dot(posToSample, normal) - bias) / (posToSampleDistSq + EPSILON), 0.0, closeOcclMax );
	return resNear;
}

float ComputeUpdatedAlchemyFalloff(float falloffRadius, float3 center, float linearDepth, float3 samplePos, float3 normal)
{
	const float EPSILON = 0.0001f;

	float3 v = samplePos - center;
	float vDistSq = dot(v, v);
	float worldFalloff = pow( max( 0.0f, (falloffRadius * falloffRadius) - vDistSq ), 3.0f);
	float geometricFalloff = max( 0.0f, (dot(v, normal) ) / (vDistSq + EPSILON) );
	return worldFalloff * geometricFalloff;
}

float ComputeUpdatedAlchemyFinal(float falloffRadius, float totalFalloff, float totalNumSamplesRcp)
{ 
	float worldDistFactor = pow(falloffRadius, 6.0f);
	float accessibility = max(0.0f, 1.0f - ((5.0f * g_Intensity * totalNumSamplesRcp) / (worldDistFactor)) * totalFalloff);
	return pow(accessibility, g_Contrast);
}

float ComputeFinal(float totalFalloff, float totalNumSamplesRcp)
{
	float accessibility = max(0.0f, 1.0f - (g_Intensity * totalNumSamplesRcp * totalFalloff));
	return pow(accessibility, g_Contrast);
}

float4 ComputeAOAndCompressLinearZ(float2 uv, int numSamples)
{
	float randomPhi = ComputeNoise(uv);
	
	float linearDepth = g_DepthTex.SampleLevel( g_SamplePoint, uv, 0 ).r;
	float3 viewCenteredPos = ComputeViewPos(uv, linearDepth);

	// Use normal from screen derivatives
	// But switch to normal buffer once translucent objects are not rendered in depth
	//float3 normal = ComputeNormalFromDDXY(viewCenteredPos, uv, linearDepth);
	float3 normal = ComputeNormalFromBuffer(uv);

	float viewZ = viewCenteredPos.z;

	// Get clamped screen radius from linear z distance
	float screenRadius = ComputeScreenRadius(viewZ);

	// Convert screen radius to world radius
    float falloffRadius = ScreenToWorldRadius(screenRadius, viewZ);

	// Ray distance falloff scale
	const float FALLOFF_UNIT_SCALE = 2.0;

	// Attenuate ray based on clamped falloff radius
	float scaledFalloff		= falloffRadius * (1.0 / FALLOFF_UNIT_SCALE);

	// Max occlusion from 'near' occluders in world space
	const float NEAR_FALLOFF_SCALE = 0.8;
	float occlusionMax		= g_OcclusionMax * (1.0 - g_NearFalloffThreshold * NEAR_FALLOFF_SCALE / falloffRadius);
	float occlusionScale	= g_OcclusionMax * scaledFalloff * scaledFalloff / (g_NearFalloffThreshold * g_NearFalloffThreshold);

	float totalOcclusion = 0.0f;
	float bias = g_Bias * linearDepth;
	float numSamplesRcp = 1.0f / numSamples;

	//[unroll]
	for(int i=0; i < numSamples; i++)
	{
		// Compute distribution
		float3 sampleUV = ComputeDistributionUV(i, numSamplesRcp, randomPhi, screenRadius, uv);

		// Reset mip level for now
		//sampleUV.w = 0.0f;
		float sampleLinearDepth = g_DepthTex.SampleLevel( g_SamplePointMipLinear, sampleUV.xy, sampleUV.z).r;
		float3 sampleViewCenteredPos = ComputeViewPos(sampleUV.xy, sampleLinearDepth);

		// Compute AO
		float occlusion = ComputeLinearFalloff(scaledFalloff, viewCenteredPos, sampleViewCenteredPos, normal, bias, occlusionMax, occlusionScale);
		//ComputeUpdatedAlchemyFalloff(viewCenteredPos, linearDepth, sampleViewCenteredPos, normal);
		totalOcclusion += occlusion;
	}

	float accessibility = ComputeFinal(totalOcclusion, numSamplesRcp);
	//ComputeUpdatedAlchemyFinal(totalOcclusion, numSamplesRcp);

	// If we are using normal buffer, then we can output/compress linear depth instead
	// for faster blur pass
	// Alternatively, store color
	float4 colorOut = accessibility;
	colorOut.yzw = normal * 0.5f + 0.5f;
	//colorOut.yzw = CompressNormalizedFloat8BitRGB(linearDepth);
	//colorOut = float4(normal * 0.5f + 0.5f, 1.0f);
	return colorOut;
}

float4 ComputeAOAndCompressLinearZPoisson32(float2 uv)
{
	float randomPhi = ComputeNoise(uv);
    float sinTheta;// = sin(theta);
	float cosTheta;// = cos(theta);
	sincos(randomPhi, sinTheta, cosTheta);
	
	float linearDepth = g_DepthTex.SampleLevel( g_SamplePoint, uv, 0 ).r;
	float3 viewCenteredPos = ComputeViewPos(uv, linearDepth);

	// Use normal from screen derivatives
	// But switch to normal buffer once translucent objects are not rendered in depth
	//float3 normal = ComputeNormalFromDDXY(viewCenteredPos, uv, linearDepth);
	float3 normal = ComputeNormalFromBuffer(uv);

	float viewZ = viewCenteredPos.z;
	float screenRadius = ComputeScreenRadius(viewZ);
    float falloffRadius = ScreenToWorldRadius(screenRadius, viewZ);

	float totalOcclusion = 0.0f;
	float bias = g_Bias * linearDepth;
	float numSamplesRcp = 1.0f / 32;

	
	// Ray distance falloff scale
	const float FALLOFF_UNIT_SCALE = 2.0;

	// Attenuate ray based on clamped falloff radius
	float scaledFalloff		= falloffRadius * (1.0 / FALLOFF_UNIT_SCALE);

	// Max occlusion from 'near' occluders in world space
	const float NEAR_FALLOFF_SCALE = 0.8;
	float occlusionMax		= g_OcclusionMax;// * (1.0 - g_NearFalloffThreshold * NEAR_FALLOFF_SCALE / falloffRadius);
	float occlusionScale	= g_OcclusionMax * scaledFalloff * scaledFalloff / (g_NearFalloffThreshold * g_NearFalloffThreshold);

	//[unroll]
	for(int i=0; i < 32; i++)
	{
		// Compute distribution
		float3 sampleUV = ComputePoisson32DistributionUV(i, float2(sinTheta, cosTheta), screenRadius, uv);

		// Reset mip level for now
		//sampleUV.w = 0.0f;
		float sampleLinearDepth = g_DepthTex.SampleLevel( g_SamplePointMipLinear, sampleUV.xy, sampleUV.z).r;
		float3 sampleViewCenteredPos = ComputeViewPos(sampleUV.xy, sampleLinearDepth);

		// Compute AO
		float occlusion = ComputeLinearFalloff(scaledFalloff, viewCenteredPos, sampleViewCenteredPos, normal, bias, occlusionMax, occlusionScale);
		//ComputeUpdatedAlchemyFalloff(viewCenteredPos, linearDepth, sampleViewCenteredPos, normal);
		totalOcclusion += occlusion;
	}

	float accessibility = ComputeFinal(totalOcclusion, numSamplesRcp);
	//ComputeUpdatedAlchemyFinal(totalOcclusion, numSamplesRcp);

	// If we are using normal buffer, then we can output/compress linear depth instead
	// for faster blur pass
	// Alternatively, store color
	float4 colorOut = accessibility;
	colorOut.yzw = normal * 0.5f + 0.5f;
	//colorOut.yzw = CompressNormalizedFloat8BitRGB(linearDepth);
	//colorOut = float4(normal * 0.5f + 0.5f, 1.0f);
	return colorOut;
}


float4 ComputeAOPS(VS_OUTPUT In) : SV_TARGET0
{
   // if(g_NumSamples != 32)
	    return ComputeAOAndCompressLinearZ(In.texCoord, g_NumSamples);
   // else
	//    return ComputeAOAndCompressLinearZPoisson32(In.texCoord);
}

// Compose results
float4 ComposeResultPS(VS_OUTPUT In) : SV_TARGET0
{
	return float4(g_AOTex.SampleLevel( g_SampleLinear, In.texCoord, 0 ).wwww );
}
