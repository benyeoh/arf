//==============================================================================
//
//		ConstTextures.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/28/2007
//
//==============================================================================

#ifndef _CONSTTEXTURES
#define _CONSTTEXTURES

texture		g_TexDiffuse < int semantic = RFX_DIFFUSE_TEX; 
						   int usage = 1;
						   string name = "Diffuse Texture"; >;
						   
sampler		g_SampDiffuse = sampler_state
{
    Texture = (g_TexDiffuse);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = WRAP;
    ADDRESSV  = WRAP;
};

texture		g_TexNormalMap < int semantic = RFX_NORMAL_MAP_TEX; 
							 int usage = 1; 
							 string name = "Normal Map"; >;
							 
sampler		g_SampNormalMap = sampler_state
{
    Texture = (g_TexNormalMap);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = WRAP;
    ADDRESSV  = WRAP;
};

texture		g_TexGlossMap < int semantic = RFX_GLOSS_MAP_TEX;
							int usage = 1;
							string name = "Gloss Map"; >;
							
sampler		g_SampGlossMap = sampler_state
{
    Texture = (g_TexGlossMap);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = WRAP;
    ADDRESSV  = WRAP;
};
							
textureCUBE		g_TexCubeEnvMap < int semantic = RFX_CUBE_ENV_MAP_TEX;
								int usage = 1;
								string name = "Cube Environment Map"; >;
							
samplerCUBE		g_SampCubeEnvMap = sampler_state
{
    Texture = (g_TexCubeEnvMap);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = WRAP;
    ADDRESSV  = WRAP;
	ADDRESSW  = WRAP;
};													
							
float4 SampleNormalMap(float2 texUV)
{
	float4 normalMap = tex2D(g_SampNormalMap, texUV);
	normalMap.xyz = normalize((normalMap.xyz - 0.5f) * 2.0f);
	return normalMap;
}

float SampleGlossMap(float2 texUV)
{
	return tex2D(g_SampGlossMap, texUV).r;
}

float4 SampleCubeEnvMap(float3 normal)
{
	return texCUBE(g_SampCubeEnvMap, normal);
}

float4 SampleCubeEnvMapLOD(float3 normal, float lod)
{
	return texCUBElod(g_SampCubeEnvMap, float4(normal, lod));
}

#endif