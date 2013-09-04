//================================================================================
//
//		ScreenTriangle.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		2/12/2011
//
//================================================================================

#ifndef SCREENTRIANGLE
#define SCREENTRIANGLE

struct VS_IN_ScreenTriangle
{
	float4 pos			: POSITION0;
};

struct VS_OUT_ScreenTriangle
{
	float4 pos			: POSITION0;
};

struct PS_OUT_Color
{
	float4 color	: COLOR0;
};

VS_OUT_ScreenTriangle
ScreenTriangleVS(VS_IN_ScreenTriangle input)
{
	VS_OUT_ScreenTriangle output = (VS_OUT_ScreenTriangle) 0;
		
	output.pos = input.pos;
	output.pos.w = 1.0f;
	
	return output;
}

PS_OUT_Color
ScreenTrianglePS(VS_OUT_ScreenTriangle input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
		
	output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
	
	return output;
}

#endif