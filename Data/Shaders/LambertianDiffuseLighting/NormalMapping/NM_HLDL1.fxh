//==============================================================================
//
//		NM_HLDL1.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/22/2007
//
//==============================================================================

#ifndef NM_HLDL1
#define NM_HLDL1

struct VS_OUT_NM_HLDL1
{
	float4 pos			: POSITION0;
	float2 texUV		: TEXCOORD0;
	float3 skyDir		: TEXCOORD1;
	float3 lightDir		: TEXCOORD2;
};

VS_OUT_NM_HLDL1
NM_HLDL1VS(VS_IN_NM input)
{
	VS_OUT_NM_HLDL1 output = (VS_OUT_NM_HLDL1) 0;
		
	float4 worldPos = mul(input.pos, g_matWorld);
	output.pos = mul(worldPos, g_matViewProj);

	float3 N = normalize( mul(input.normal, g_matWorld) );
	float3 T = normalize( mul(input.tangent, g_matWorld) );
	float3 B = normalize( mul(input.binormal, g_matWorld) );
	
	float3x3 matTangentSpace;
	matTangentSpace[0] = T;
	matTangentSpace[1] = B;
	matTangentSpace[2] = N;
	
	output.skyDir = mul( matTangentSpace, float3(0.0f, 1.0f, 0.0f) );
	output.lightDir = mul( matTangentSpace, -g_LightDir.dir );
	
	output.texUV = input.texUV;
	
	return output;
}

PS_OUT_Color
NM_HLDL1PS(VS_OUT_NM_HLDL1 input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
	
	float4 diffuseColor = tex2D(g_SampDiffuse, input.texUV);
	float4 normalMap = SampleNormalMap(input.texUV);
	
	ConditionalAlphaTest(diffuseColor.a);
	
	float dirLightNDotL = GetLightStrength(normalMap.xyz, Normalize(input.lightDir));
	float hemiN = dot(normalMap.xyz, input.skyDir);
	
	float3 totalLightColor = dirLightNDotL * g_LightDir.color.rgb + GetHemiAmbientColor(hemiN);
	
	output.color = float4(diffuseColor.rgb * totalLightColor, diffuseColor.a);
	
	return output;
}

#endif

