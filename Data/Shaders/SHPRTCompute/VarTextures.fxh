//================================================================================
//
//		VarTextures.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/2/2012
//
//================================================================================

#ifndef _SHPRTCOMPUTE_VARTEXTURES
#define _SHPRTCOMPUTE_VARTEXTURES

texture		g_TexWeights < int semantic = SHPRTCOMPUTE_WEIGHT_TEXTURE; >;
sampler		g_SampWeight = sampler_state
{
    Texture = (g_TexWeights);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = CLAMP;
    ADDRESSV  = CLAMP;
};

texture		g_TexSrc1 < int semantic = SHPRTCOMPUTE_SRC_TEXTURE_1; >;
sampler		g_SampSrc1 = sampler_state
{
    Texture = (g_TexSrc1);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = CLAMP;
    ADDRESSV  = CLAMP;
};

texture		g_TexSrc2 < int semantic = SHPRTCOMPUTE_SRC_TEXTURE_2; >;
sampler		g_SampSrc2 = sampler_state
{
    Texture = (g_TexSrc2);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = CLAMP;
    ADDRESSV  = CLAMP;
};

texture		g_TexSrc3 < int semantic = SHPRTCOMPUTE_SRC_TEXTURE_3; >;
sampler		g_SampSrc3 = sampler_state
{
    Texture = (g_TexSrc3);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = CLAMP;
    ADDRESSV  = CLAMP;
};
				 
#endif			