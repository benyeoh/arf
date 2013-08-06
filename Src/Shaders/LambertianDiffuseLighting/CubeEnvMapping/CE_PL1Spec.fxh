//==============================================================================
//
//		CE_PL1Spec.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		10/16/2007
//
//==============================================================================

#ifndef CE_PL1SPEC
#define CE_PL1SPEC

struct VS_OUT_CE_PL1Spec
{
	float4 pos					: POSITION;
	float2 texUV				: TEXCOORD0;
	float3 posToLight			: TEXCOORD1;
	float3 normal				: TEXCOORD2;
	float3 posToCam				: TEXCOORD3;
	float3 posToLightOverRange	: TEXCOORD4;
};

VS_OUT_CE_PL1Spec
CE_PL1SpecVS( VS_IN_Basic input )
{
	VS_OUT_CE_PL1Spec output = (VS_OUT_CE_PL1Spec) 0;
	
	float4 worldPos = mul(input.pos, g_matWorld);
	output.pos = mul(worldPos, g_matViewProj);
	output.texUV = input.texUV;
	
	output.normal = ( mul(input.normal, g_matWorld) );
	
	output.posToLight = g_LightPoint.pos - worldPos.xyz;
	output.posToCam = g_CameraPos - worldPos.xyz;
	output.posToLightOverRange = output.posToLight * g_LightPoint.invRange;
	
	return output;
}

PS_OUT_Color
CE_PL1SpecPS( VS_OUT_CE_PL1Spec input )
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
	
	float4 diffuseColor = tex2D(g_SampDiffuse, input.texUV);
	float gloss = SampleGlossMap(input.texUV);
	
	DoAlphaTest(diffuseColor.a);
	
	float3 N = Normalize(input.normal);
	float pointLightDist = length(input.posToLight);
	
	float3 posToLightNorm = input.posToLight / pointLightDist;
	float3 posToCamNorm = Normalize(input.posToCam);
	float NDotL = ( dot( N, posToLightNorm ) );

	float lightFresnel = GetReflectFresnel( N, posToLightNorm );	
	float attenuation = GetAttenuation(input.posToLightOverRange);

	float specular = GetPhongSpecularNDotL(posToCamNorm, posToLightNorm, N, NDotL, 
		pointLightDist * g_LightPoint.invSize * gloss, gloss * attenuation);
	float4 pointLightColor =  g_LightPoint.color * saturate(NDotL) * attenuation;
	
	float4 localColor = GetLocalColor(g_LightPoint.color, pointLightColor, diffuseColor, lightFresnel, specular, gloss);
	 
	output.color = float4(localColor.rgb, diffuseColor.a);
	
	return output;
}

#endif