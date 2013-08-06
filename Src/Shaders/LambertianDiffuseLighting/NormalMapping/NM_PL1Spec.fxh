//==============================================================================
//
//		NM_PL1Spec.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/28/2007
//
//==============================================================================

#ifndef NM_PL1SPEC
#define NM_PL1SPEC

struct VS_OUT_NM_PL1Spec
{
	float4 pos					: POSITION;
	float2 texUV				: TEXCOORD0;
	float3 posToLight			: TEXCOORD1;
	float3 posToLightOverRange	: TEXCOORD2;
	float3 posToCam				: TEXCOORD3;
};

VS_OUT_NM_PL1Spec
NM_PL1SpecVS( VS_IN_NM input )
{
	VS_OUT_NM_PL1Spec output = (VS_OUT_NM_PL1Spec) 0;
	
	float4 worldPos = mul(input.pos, g_matWorld);
	output.pos = mul(worldPos, g_matViewProj);
	output.texUV = input.texUV;
	
	float3 N = normalize( mul(input.normal, g_matWorld) );
	float3 T = normalize( mul(input.tangent, g_matWorld) );
	float3 B = normalize( mul(input.binormal, g_matWorld) );
	
	float3x3 matTangentSpace;
	matTangentSpace[0] = T;
	matTangentSpace[1] = B;
	matTangentSpace[2] = N;
	
	//output.normal = ( mul(input.normal, g_matWorld) );
	
	float3 posToLight = g_LightPoint.pos - worldPos.xyz;
	output.posToLight = mul( matTangentSpace, posToLight );

	output.posToLightOverRange = posToLight * g_LightPoint.invRange;
	output.posToCam = mul( matTangentSpace, g_CameraPos - worldPos.xyz );
	
	return output;
}

PS_OUT_Color
NM_PL1SpecPS( VS_OUT_NM_PL1Spec input )
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
	
	float4 diffuseColor = tex2D(g_SampDiffuse, input.texUV);
	float4 normalMap = SampleNormalMap(input.texUV);
	float gloss = SampleGlossMap(input.texUV);
	
	DoAlphaTest(diffuseColor.a);
	
	float pointLightDist = length(input.posToLight);
	float3 posToLightNorm = (input.posToLight) / pointLightDist;
	float3 posToCamNorm = Normalize(input.posToCam);
	float attenuation = GetAttenuation(input.posToLightOverRange);

	float NDotL = ( dot( normalMap.xyz, posToLightNorm ) );
	float specular = GetPhongSpecularNDotL(posToCamNorm, posToLightNorm, normalMap.xyz, NDotL, 
										pointLightDist * g_LightPoint.invSize * gloss, gloss * attenuation);
	
	float4 pointLightColor = g_LightPoint.color * saturate(NDotL) * attenuation;
	
	output.color = float4(pointLightColor.rgb * diffuseColor.rgb
						+ pointLightColor * specular, diffuseColor.a);
	
	return output;
}

#endif