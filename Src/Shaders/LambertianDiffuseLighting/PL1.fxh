//==============================================================================
//
//		PL1.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Monday, February 05, 2007
//
//==============================================================================

#ifndef PL1
#define PL1

struct VS_OUT_PL1
{
	float4 pos			: POSITION;
	float2 texUV		: TEXCOORD0;
	float3 posToLight	: TEXCOORD1;
	float3 normal		: TEXCOORD2;
};

VS_OUT_PL1
PL1VS( VS_IN_Basic input )
{
	VS_OUT_PL1 output = (VS_OUT_PL1) 0;
	
	float4 worldPos = mul(input.pos, g_matWorld);
	output.pos = mul(worldPos, g_matViewProj);
	output.texUV = input.texUV;
	
	output.normal = ( mul(input.normal, g_matWorld) );
	
	output.posToLight.xyz = g_LightPoint.pos - worldPos.xyz;

	return output;
}

PS_OUT_Color
PL1PS( VS_OUT_PL1 input )
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
	
	float4 diffuseColor = tex2D(g_SampDiffuse, input.texUV);
	float3 normal = Normalize(input.normal);
	
	DoAlphaTest(diffuseColor.a);
	
	float4 pointLightColor = GetPointLightColor(g_LightPoint, input.posToLight, normal);
	
	output.color = float4(pointLightColor * diffuseColor.rgb, diffuseColor.a);
	
	return output;
}

#endif