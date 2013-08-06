//==============================================================================
//
//		HLDL1Spec.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/28/2007
//
//==============================================================================

#ifndef HLDL1SPEC
#define HLDL1SPEC

struct VS_OUT_HLDL1Spec
{
	float4 pos			: POSITION0;
	float4 hemiColor	: COLOR0;
	float2 texUV		: TEXCOORD0;
	float3 normal		: TEXCOORD1;
	float3 posToCam		: TEXCOORD2;
};

VS_OUT_HLDL1Spec
HLDL1SpecVS(VS_IN_Basic input)
{
	VS_OUT_HLDL1Spec output = (VS_OUT_HLDL1Spec) 0;
		
    float4 worldPos = mul(input.pos, g_matWorld);
	output.pos = mul(worldPos, g_matViewProj);
		  
	float3 N = normalize( mul(input.normal, g_matWorld) );
	float4 hemiColor = GetHemiAmbientColor(N.y);
	
	output.normal = N;
	output.hemiColor = hemiColor;
	output.texUV = input.texUV;	
	output.posToCam = g_CameraPos - worldPos.xyz;
	
	return output;
}

PS_OUT_Color
HLDL1SpecPS(VS_OUT_HLDL1Spec input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
	
	float4 diffuseColor = tex2D(g_SampDiffuse, input.texUV);
	float gloss = SampleGlossMap(input.texUV);
	
	ConditionalAlphaTest(diffuseColor.a);
	
	float3 N = Normalize(input.normal);
	float3 posToCamNorm = Normalize(input.posToCam);
	
	float NDotL = ( dot( N, -g_LightDir.dir) );
	
	float4 dirColor = saturate(NDotL) * g_LightDir.color;
	float specular = GetPhongSpecularNDotL(posToCamNorm, -g_LightDir.dir, N, NDotL, g_LightDir.invSize * gloss, gloss);
	
	output.color = float4(diffuseColor.rgb * (input.hemiColor.rgb + dirColor.rgb) 
						+ dirColor.rgb * specular, diffuseColor.a);
	
	return output;
}

#endif