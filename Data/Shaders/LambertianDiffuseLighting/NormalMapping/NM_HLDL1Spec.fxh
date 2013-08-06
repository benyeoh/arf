//==============================================================================
//
//		NM_HLDL1Spec.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/28/2007
//
//==============================================================================

#ifndef NM_HLDL1SPEC
#define NM_HLDL1SPEC

struct VS_OUT_NM_HLDL1Spec
{
	float4 pos			: POSITION0;
	float2 texUV		: TEXCOORD0;
	float3 skyDir		: TEXCOORD1;
	float3 lightDir		: TEXCOORD2;
	float3 posToCam		: TEXCOORD3;
};

VS_OUT_NM_HLDL1Spec
NM_HLDL1SpecVS(VS_IN_NM input)
{
	VS_OUT_NM_HLDL1Spec output = (VS_OUT_NM_HLDL1Spec) 0;
		
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
	output.posToCam = mul( matTangentSpace, g_CameraPos - worldPos.xyz );
	
	return output;
}

PS_OUT_Color
NM_HLDL1SpecPS(VS_OUT_NM_HLDL1Spec input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
	
	float4 diffuseColor = tex2D(g_SampDiffuse, input.texUV);
	float4 normalMap = SampleNormalMap(input.texUV);
	float gloss = SampleGlossMap(input.texUV);

	ConditionalAlphaTest(diffuseColor.a);
	
	float3 posToCamNorm = Normalize(input.posToCam);
	float3 lightDir =  Normalize(input.lightDir);
	
	float dirLightNDotL = dot(normalMap.xyz, lightDir);
	float hemiN = dot(normalMap.xyz, input.skyDir);
	
	float specular = GetPhongSpecularNDotL(posToCamNorm, lightDir, normalMap.xyz, dirLightNDotL, g_LightDir.invSize * gloss, gloss);
	
	float3 dirColor = saturate(dirLightNDotL) * g_LightDir.color.rgb;
	float3 totalLightColor = dirColor + GetHemiAmbientColor(hemiN);
	
	output.color = float4(diffuseColor.rgb * totalLightColor
						+ specular * dirColor, diffuseColor.a);
	
	return output;
}

#endif