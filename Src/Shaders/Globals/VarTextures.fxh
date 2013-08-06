//==============================================================================
//
//		VarTextures.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/28/2007
//
//==============================================================================

#ifndef _VARTEXTURES
#define _VARTEXTURES

texture		g_TexRT1 < int semantic = RFX_TEX_RT1; >;

sampler		g_SampRT1 = sampler_state
{
    Texture = (g_TexRT1);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = CLAMP;
    ADDRESSV  = CLAMP;
};

texture		g_TexRT2 < int semantic = RFX_TEX_RT2; >;

sampler		g_SampRT2 = sampler_state
{
    Texture = (g_TexRT2);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = CLAMP;
    ADDRESSV  = CLAMP;
};
				 
#endif	 