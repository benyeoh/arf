//==============================================================================
//
//		ShaderFunctions.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Wednesday, February 07, 2007
//
//==============================================================================

#ifndef _SHADERFUNCTIONS
#define _SHADERFUNCTIONS

// Common

float3 Normalize( float3 vec )
{
	// Todo: CubeMap implementation
	return normalize( vec );
}

float Power(float amount, float exp)
{
	return pow( amount, exp );
}

float2 Power(float2 amount, float exp)
{
	return pow( amount, exp );
}

float3 Power(float3 amount, float exp)
{
	return pow( amount, exp );
}

float4 Power(float4 amount, float exp)
{
	return pow( amount, exp );
}


// Lighting

float GetLightStrength(float3 lightDir, float3 normal)
{
	return saturate( dot( lightDir, normal ) );
}

float4 GetHemiAmbientColor(float nY)
{
	return lerp(g_LightHemi.groundColor, g_LightHemi.skyColor, nY * 0.5f + 0.5f );
}

// Point Light and Spot Light
float GetAttenuation(float3 posToLightOverRange)
{
	return 1.0f - saturate( dot(posToLightOverRange, posToLightOverRange) );
}

float4 GetPointLightColorAtPos(LightPoint pointLight, float3 pos, float3 normal)
{
	float3 posToLight = pointLight.pos - pos;
	float3 posToLightNorm = Normalize(posToLight);
	float3 posToLightOverRange = posToLight * g_LightPoint.invRange;
	float NDotL = GetLightStrength( posToLightNorm, normal );
	float attenuation = 1.0f - saturate( dot(posToLightOverRange, posToLightOverRange) );

	return pointLight.color * NDotL * attenuation;
}

float4 GetPointLightColor(LightPoint pointLight, float3 posToLight, float3 normal)
{
	float3 posToLightNorm = Normalize(posToLight);
	float3 posToLightOverRange = posToLight * g_LightPoint.invRange;
	float NDotL = GetLightStrength( posToLightNorm, normal );
	float attenuation = 1.0f - saturate( dot(posToLightOverRange, posToLightOverRange) );

	return pointLight.color * NDotL * attenuation;
}

float4 GetPointLightColor(LightPoint pointLight, float3 posToLightOverRange, float3 posToLight, float3 normal)
{
	float3 posToLightNorm = Normalize(posToLight);
	float NDotL = GetLightStrength( posToLightNorm, normal );
	float attenuation = 1.0f - saturate( dot(posToLightOverRange, posToLightOverRange) );

	return pointLight.color * NDotL * attenuation;
}

float4 GetPointLightColor(LightPoint pointLight, float3 posToLightOverRange, float NDotL)
{
	float attenuation = 1.0f - saturate( dot(posToLightOverRange, posToLightOverRange) );

	return pointLight.color * NDotL * attenuation;
}

float4 GetLocalColor(float4 lightColor, float4 modLightColor, float4 diffuseColor, float fresnel, float specular, float gloss)
{
	return specular * lightColor * (fresnel) +
		modLightColor * saturate(1.0f - fresnel * gloss) * diffuseColor;
} 

float4 GetGlobalColor(float4 envColor, float4 hemiColor, float4 diffuseColor, float fresnel, float gloss)
{
	return envColor * (hemiColor) * float4(g_EnvMapTint.rgb * 1.5f, 1.0f) * fresnel * gloss + 
						  hemiColor * saturate(1.0f - fresnel * gloss) * diffuseColor;
}

// Reflection
float3 GetReflection(float3 N, float3 I)
{
	return ( 2 * ( dot(N, I) ) * N - I );
}

float3 GetReflection(float3 N, float3 I, float NDotI)
{
	return ( 2 * ( NDotI ) * N - I );
}

// Specular
float GetPhongSpecular(float3 posToCamNorm, float3 reflectedLightNorm, float sharpnessMod, float strengthMod)
{	
	const static float SPECULAR_OVERBRIGHT = 2.80f;
	
	return Power( saturate( dot(reflectedLightNorm, posToCamNorm) ), 32.0f * sharpnessMod) * strengthMod * SPECULAR_OVERBRIGHT;
}

float GetPhongSpecularNDotL(float3 posToCamNorm, float3 posToLightNorm, float3 N, float NDotL, float sharpnessMod, float strengthMod)
{
	float3 R = GetReflection(N, posToLightNorm, NDotL);
	return GetPhongSpecular(posToCamNorm, R, sharpnessMod, strengthMod);
}

float GetPhongSpecular(float3 posToCamNorm, float3 N, float3 L, float sharpnessMod, float strengthMod)
{
	return GetPhongSpecularNDotL(posToCamNorm, L, N, dot(N, L), sharpnessMod, strengthMod);
}

// Fresnel
float GetReflectFresnel(float NDotI)
{	
	float base = 1.0f - ( NDotI );
	float fresnelMin = 1.0f;
	float fresnelExp = 0.0f;
	return (fresnelMin + Power( base, fresnelExp ) * (1.0f - fresnelMin));
}

float GetReflectFresnel(float3 N, float3 I)
{
	float NDotI = saturate( dot(N, I) );	
	return GetReflectFresnel(NDotI);
}

#endif