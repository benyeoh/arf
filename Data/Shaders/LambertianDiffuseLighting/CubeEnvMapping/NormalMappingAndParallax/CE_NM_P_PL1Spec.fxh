//==============================================================================
//
//		CE_NM_P_PL1Spec.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		10/25/2007
//
//==============================================================================

#ifndef CE_NM_P_PL1SPEC
#define CE_NM_P_PL1SPEC

struct VS_OUT_CE_NM_P_PL1Spec
{
	float4 pos					: POSITION;
	float2 texUV				: TEXCOORD0;
	float3 posToLight			: TEXCOORD1;
	float3 posToLightOverRange	: TEXCOORD2;
	float3 posToCam				: TEXCOORD3;
};

VS_OUT_CE_NM_P_PL1Spec
CE_NM_P_PL1SpecVS( VS_IN_NM input )
{
	VS_OUT_CE_NM_P_PL1Spec output = (VS_OUT_CE_NM_P_PL1Spec) 0;
	
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
CE_NM_P_PL1SpecPS( VS_OUT_CE_NM_P_PL1Spec input )
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
	
	float height = tex2D(g_SampNormalMap, input.texUV).a;
	
	float3 posToCamNorm = Normalize(input.posToCam);
	float2 offsetUV = input.texUV + ((height - 0.5f) * g_ParallaxThickness * posToCamNorm.xy);
	
	float4 diffuseColor = tex2D(g_SampDiffuse, offsetUV);
	float4 normalMap = SampleNormalMap(offsetUV);
	float gloss = SampleGlossMap(offsetUV);
	
	DoAlphaTest(diffuseColor.a);
	
	float pointLightDist = length(input.posToLight);
	float3 posToLightNorm = (input.posToLight) / pointLightDist;
	
	float NDotL = ( dot( normalMap.xyz, posToLightNorm ) );
	
	float lightFresnel = GetReflectFresnel( normalMap.xyz, posToLightNorm ) * saturate( posToLightNorm.z * 5.0f - 0.5f );	
	float attenuation = GetAttenuation(input.posToLightOverRange);
	float specular = GetPhongSpecularNDotL(posToCamNorm, posToLightNorm, normalMap.xyz, NDotL, 
										pointLightDist * g_LightPoint.invSize * gloss, gloss * attenuation);
	float4 pointLightColor =  g_LightPoint.color * saturate(NDotL) * attenuation;
	
	float4 localColor = GetLocalColor(g_LightPoint.color, pointLightColor, diffuseColor, lightFresnel, specular, gloss);
	
	output.color = float4(localColor.rgb, diffuseColor.a);
	
	return output;
}

#endif