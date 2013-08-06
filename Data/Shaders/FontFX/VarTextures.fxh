//==============================================================================
//
//		VarTextures.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		12/16/2007
//
//==============================================================================

#ifndef _FONTFX_VARTEXTURES
#define _FONTFX_VARTEXTURES

texture		g_TexFont < int semantic = FFX_FONT_TEXTURE; >;

sampler		g_SampFont = sampler_state
{
    Texture = (g_TexFont);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = CLAMP;
    ADDRESSV  = CLAMP;
};
				 
#endif			