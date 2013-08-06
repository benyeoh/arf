//==============================================================================
//
//		VarParams.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/28/2007
//
//==============================================================================

#ifndef _VARPARAMS
#define _VARPARAMS

// Transforms
float4x4 g_matViewProj		< int semantic = RFX_VIEW_PROJ; >;
float4x4 g_matWorldViewProj	< int semantic = RFX_WORLD_VIEW_PROJ; >;
float4x4 g_matWorld			< int semantic = RFX_WORLD; >;
float4x4 g_matProj			< int semantic = RFX_PROJ; >;
float4x4 g_matView			< int semantic = RFX_VIEW; >;


// Lighting
struct LightDir
{
	float3 dir;
	float4 color;
	float  invSize;
};

struct LightHemi
{
	float4 skyColor;
	float4 groundColor;
};

struct LightPoint
{
	float3 pos;
	float4 color;
	float invRange;
	float invSize;
};

LightDir	g_LightDir < int semantic = RFX_DIR_LIGHT; >;
LightHemi	g_LightHemi	< int semantic = RFX_HEMI_LIGHT; >;
LightPoint g_LightPoint < int semantic = RFX_POINT_LIGHT; >;


// Camera
float3 g_CameraPos < int semantic = RFX_CAMERA_POS; >;

// Post Process
// float2 g_PostProcessRTSize < int semantic = RFX_POST_PROCESS_RT_SIZE; >;

// Bloom and HDR
float g_AverageLuminance	< int semantic = RFX_AVERAGE_LUMINANCE; >;

#endif



