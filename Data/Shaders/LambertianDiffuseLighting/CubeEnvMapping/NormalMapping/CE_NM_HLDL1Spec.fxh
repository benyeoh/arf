//==============================================================================
//
//		CE_NM_HLDL1Spec.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		10/21/2007
//
//==============================================================================

#ifndef CE_NM_HLDL1SPEC
#define CE_NM_HLDL1SPEC
/*
struct VS_OUT_CE_NM_HLDL1Spec
{
	float4 pos			: POSITION0;
	float2 texUV		: TEXCOORD0;
	float3 posToCam		: TEXCOORD1;

	float3 T	: TEXCOORD2;	// Required to get the reflection vector in world space
	float3 B	: TEXCOORD3;	// We instead transform the normal map to world space
	float3 N	: TEXCOORD4;
};

VS_OUT_CE_NM_HLDL1Spec
CE_NM_HLDL1SpecVS(VS_IN_NM input)
{
	VS_OUT_CE_NM_HLDL1Spec output = (VS_OUT_CE_NM_HLDL1Spec) 0;
		
	float4 worldPos = mul(input.pos, g_matWorld);
	output.pos = mul(worldPos, g_matViewProj);

	float3 N = normalize( mul(input.normal, g_matWorld) );
	float3 T = normalize( mul(input.tangent, g_matWorld) );
	float3 B = normalize( mul(input.binormal, g_matWorld) );
	
	output.N = N;
	output.T = T;
	output.B = B;
	
	//float3x3 matTangentSpace;
	//matTangentSpace[0] = T;
	//matTangentSpace[1] = B;
	//matTangentSpace[2] = N;
	
	//output.skyDir = mul( matTangentSpace, float3(0.0f, 1.0f, 0.0f) );
	//output.lightDir = mul( matTangentSpace, -g_LightDir.dir );
	
	output.texUV = input.texUV;
	output.posToCam = g_CameraPos - worldPos.xyz;
	
	//output.posToCam = mul( matTangentSpace, g_CameraPos - worldPos.xyz );
	
	return output;
}

PS_OUT_Color
CE_NM_HLDL1SpecPS(VS_OUT_CE_NM_HLDL1Spec input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
	
	float4 diffuseColor = tex2D(g_SampDiffuse, input.texUV);
	float4 normalMap = SampleNormalMap(input.texUV);
	float gloss = SampleGlossMap(input.texUV);

	// Construct the transpose of the tangent space matrix - ie, it's inverse
	float3x3 matInvTangentSpace;
	matInvTangentSpace[0] = (input.T);
	matInvTangentSpace[1] = (input.B);
	matInvTangentSpace[2] = (input.N);
	
	float3 worldNormal = Normalize( mul( normalMap.xyz, matInvTangentSpace ) );	
	float3 posToCamNorm = Normalize(input.posToCam);
	float3 R = GetReflection( worldNormal, posToCamNorm );
	float4 envColor = SampleCubeEnvMap( R );

	float NDotL = saturate( dot( worldNormal, -g_LightDir.dir ) );
	float lightFresnel = ( GetReflectFresnel( NDotL ) ) * saturate( dot(matInvTangentSpace[2], -g_LightDir.dir) * 5.0f + 0.5f );
	float fresnel = GetReflectFresnel( worldNormal, R );
	
	float4 hemiColor = GetHemiAmbientColor(worldNormal.y);
	float4 dirColor = (NDotL) * g_LightDir.color;
	float specular = GetPhongSpecular(-g_LightDir.dir, R, g_LightDir.invSize * gloss,  gloss);
	
	float4 localColor = GetLocalColor(g_LightDir.color, dirColor, diffuseColor, lightFresnel, specular, gloss);
	float4 globalColor =  GetGlobalColor(envColor, hemiColor, diffuseColor, fresnel, gloss);
	
	output.color.rgb = input.T; //float4( localColor.rgb + globalColor.rgb, diffuseColor.a );//1.0f - exp( -float4( localColor + globalColor, diffuseColor.a ) * 1.0f);

	return output;
}
*/

struct VS_OUT_CE_NM_HLDL1Spec
{
	float4 pos			: POSITION0;
	float2 texUV		: TEXCOORD0;
	float3 posToCam		: TEXCOORD1;
	float3 skyDir		: TEXCOORD2;
	float3 lightDir		: TEXCOORD3;
	
	float3 T	: TEXCOORD4;	// Required to get the reflection vector in world space
	float3 B	: TEXCOORD5;	// We instead transform the normal map to world space
	float3 N	: TEXCOORD6;
};

VS_OUT_CE_NM_HLDL1Spec
CE_NM_HLDL1SpecVS(VS_IN_NM input)
{
	VS_OUT_CE_NM_HLDL1Spec output = (VS_OUT_CE_NM_HLDL1Spec) 0;
		
	float4 worldPos = mul(input.pos, g_matWorld);
	output.pos = mul(worldPos, g_matViewProj);

	float3 N = normalize( mul(input.normal, g_matWorld) );
	float3 T = normalize( mul(input.tangent, g_matWorld) );
	float3 B = normalize( mul(input.binormal, g_matWorld) );
	
	output.N = N;
	output.T = T;
	output.B = B;
	
	float3x3 matTangentSpace;
	matTangentSpace[0] = T;
	matTangentSpace[1] = B;
	matTangentSpace[2] = N;
	
	output.skyDir = mul( matTangentSpace, float3(0.0f, 1.0f, 0.0f) );
	output.lightDir = mul( matTangentSpace, -g_LightDir.dir );
	output.texUV = input.texUV;	
	output.posToCam = mul( matTangentSpace, g_CameraPos - worldPos.xyz );
	//output.posToCam = g_CameraPos - worldPos.xyz;
	
	return output;
}

PS_OUT_Color
CE_NM_HLDL1SpecPS(VS_OUT_CE_NM_HLDL1Spec input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
	
	float4 diffuseColor = tex2D(g_SampDiffuse, input.texUV);
	float4 normalMap = SampleNormalMap(input.texUV);
	float gloss = SampleGlossMap(input.texUV);

	ConditionalAlphaTest(diffuseColor.a);
	
	// Construct the transpose of the tangent space matrix - ie, it's inverse
	float3x3 matInvTangentSpace;
	matInvTangentSpace[0] = (input.T);
	matInvTangentSpace[1] = (input.B);
	matInvTangentSpace[2] = (input.N);

	float3 posToCamNorm = Normalize(input.posToCam);
	float3 lightDir =  Normalize(input.lightDir);
	float3 R = GetReflection( normalMap.xyz, posToCamNorm );
	float3 worldR = mul( R, matInvTangentSpace );
	float4 envColor = SampleCubeEnvMap( worldR );

	float NDotL = saturate( dot( normalMap.xyz, lightDir ) );
	float lightFresnel = ( GetReflectFresnel( NDotL ) ) * saturate( lightDir.z * 5.0f + 0.5f );
	float fresnel = GetReflectFresnel( normalMap.xyz, R );
	
	float4 hemiColor = GetHemiAmbientColor( dot(normalMap.xyz, input.skyDir) );
	float4 dirColor = (NDotL) * g_LightDir.color;
	float specular = GetPhongSpecular(lightDir, R, g_LightDir.invSize * gloss, gloss);
	
	float4 localColor = GetLocalColor(g_LightDir.color, dirColor, diffuseColor, lightFresnel, specular, gloss);
	float4 globalColor =  GetGlobalColor(envColor, hemiColor, diffuseColor, fresnel, gloss);
	
	output.color = float4( localColor.rgb + globalColor.rgb, diffuseColor.a );//1.0f - exp( -float4( localColor + globalColor, diffuseColor.a ) * 1.0f);

	return output;
}


#endif