//==============================================================================
//
//		VarParams.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/30/2008
//
//==============================================================================

#ifndef _DEBUGFX_VARPARAMS
#define _DEBUGFX_VARPARAMS

float4x4 g_matView									< int semantic = DFX_VIEW; >;
float4x4 g_matProj									< int semantic = DFX_PROJ; >;
float4x4 g_matViewProj								< int semantic = DFX_VIEW_PROJ; >;

float4x4 g_matWorldBatch[DFX_MAX_NUM_OF_BATCHES]	< int semantic = DFX_WORLD_BATCH; >;
float4 g_ColorBatch[DFX_MAX_NUM_OF_BATCHES]			< int semantic = DFX_COLOR_BATCH; >;
float3 g_PosBatch[DFX_MAX_NUM_OF_BATCHES]			< int semantic = DFX_POS_BATCH; >;
float2 g_QuadSizeBatch[DFX_MAX_NUM_OF_BATCHES]		< int semantic = DFX_QUAD_SIZE_BATCH; >;
float2 g_QuadTexPosBatch[DFX_MAX_NUM_OF_BATCHES]	< int semantic = DFX_QUAD_TEX_POS_BATCH; >;
float2 g_QuadTexSizeBatch[DFX_MAX_NUM_OF_BATCHES]	< int semantic = DFX_QUAD_TEX_SIZE_BATCH; >;

texture		g_TexQuad								< int semantic = DFX_QUAD_TEXTURE; >;
sampler		g_SampQuad = sampler_state
{
    Texture = (g_TexQuad);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = WRAP;
    ADDRESSV  = WRAP;
};

#endif