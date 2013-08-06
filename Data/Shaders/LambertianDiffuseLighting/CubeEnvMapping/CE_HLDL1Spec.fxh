//==============================================================================
//
//		CE_HLDL1Spec.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		10/16/2007
//
//==============================================================================

#ifndef CE_HLDL1SPEC
#define CE_HLDL1SPEC

struct VS_OUT_CE_HLDL1Spec
{
	float4 pos			: POSITION0;
	float4 hemiColor	: COLOR0;
	float2 texUV		: TEXCOORD0;
	float3 normal		: TEXCOORD1;
	float3 posToCam		: TEXCOORD2;
};

VS_OUT_CE_HLDL1Spec
CE_HLDL1SpecVS(VS_IN_Basic input)
{
	VS_OUT_CE_HLDL1Spec output = (VS_OUT_CE_HLDL1Spec) 0;
		
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
CE_HLDL1SpecPS(VS_OUT_CE_HLDL1Spec input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
	
	float4 diffuseColor = tex2D(g_SampDiffuse, input.texUV);
	float gloss = SampleGlossMap(input.texUV);
	
	ConditionalAlphaTest(diffuseColor.a);
	
	float3 N = Normalize(input.normal);
	float3 posToCamNorm = Normalize(input.posToCam);
	float3 R = GetReflection( N, posToCamNorm );
	float4 envColor = SampleCubeEnvMap( R );
	
	float lightFresnel = ( GetReflectFresnel( N, -g_LightDir.dir ));
	float fresnel = GetReflectFresnel( N, R );
	float NDotL = saturate( dot( N, -g_LightDir.dir) );
	
	float4 dirColor = (NDotL) * g_LightDir.color;
	float specular = GetPhongSpecular(-g_LightDir.dir, R, g_LightDir.invSize * gloss, gloss);
	
	float4 localColor = GetLocalColor(g_LightDir.color, dirColor, diffuseColor, lightFresnel, specular, gloss);
	float4 globalColor =  GetGlobalColor(envColor, input.hemiColor, diffuseColor, fresnel, gloss);
	
	output.color = float4( localColor.rgb + globalColor.rgb, diffuseColor.a );//1.0f - exp( -float4( localColor + globalColor, diffuseColor.a ) * 1.0f);

	return output;
}

#endif