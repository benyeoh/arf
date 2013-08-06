//==============================================================================
//
//		NM_P_PL1.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/23/2007
//
//==============================================================================

#ifndef NM_P_PL1
#define NM_P_PL1

struct VS_OUT_NM_P_PL1
{
	float4 pos					: POSITION;
	float2 texUV				: TEXCOORD0;
	float3 posToLight			: TEXCOORD1;
	float3 posToLightOverRange	: TEXCOORD2;
	float3 posToCam				: TEXCOORD3;
};

VS_OUT_NM_P_PL1
NM_P_PL1VS( VS_IN_NM input )
{
	VS_OUT_NM_P_PL1 output = (VS_OUT_NM_P_PL1) 0;
	
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
	output.posToCam = mul( matTangentSpace, g_CameraPos - worldPos.xyz );

	output.posToLightOverRange = posToLight * g_LightPoint.invRange;
	
	return output;
}

PS_OUT_Color
NM_P_PL1PS( VS_OUT_NM_P_PL1 input )
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
	
	float height = tex2D(g_SampNormalMap, input.texUV).a;
	float3 posToCamNorm = Normalize(input.posToCam);
	float2 offsetUV = input.texUV + ((height - 0.5f) * g_ParallaxThickness * posToCamNorm.xy);
	
	float4 diffuseColor = tex2D(g_SampDiffuse, offsetUV);
	float4 normalMap = SampleNormalMap(offsetUV);
		
	DoAlphaTest(diffuseColor.a);
	
	float4 pointLightColor = GetPointLightColor(g_LightPoint, input.posToLightOverRange, input.posToLight, normalMap.xyz);
	
	output.color = float4(pointLightColor.rgb * diffuseColor.rgb, diffuseColor.a);
	
	return output;
}

#endif