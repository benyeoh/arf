//==============================================================================
//
//		ShaderIO.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Tuesday, June 06, 2006
//
//==============================================================================

#ifndef _SHADERIO
#define _SHADERIO

struct VS_IN_2D
{
	float4 pos		: POSITION;
	float2 texUV	: TEXCOORD0;
};

struct VS_IN_Basic
{
	float4 pos		: POSITION;
	float3 normal	: NORMAL;
	float2 texUV	: TEXCOORD0;
};

struct VS_IN_NM
{
	float4 pos		: POSITION;
	float3 normal	: NORMAL;
	float2 texUV	: TEXCOORD0;
	float3 tangent	: TANGENT;
	float3 binormal	: BINORMAL;	
};

struct PS_OUT_Color
{
	float4 color : COLOR0;
};

#endif