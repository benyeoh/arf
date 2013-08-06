//================================================================================
//
//		AccumulateOccl.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/2/2012
//
//================================================================================

#ifndef ACCUMULATEOCCL
#define ACCUMULATEOCCL

struct VS_IN_AccumulateOccl
{
	float4 pos			: POSITION0;
	float2 texUV		: TEXCOORD0;
};

struct VS_OUT_AccumulateOccl
{
	float4 pos			: POSITION0;
	float2 texUV		: TEXCOORD0;
};

struct PS_OUT_Color
{
	float4 colorBand1		: COLOR0;
	float4 colorBand2		: COLOR1;
	float4 colorBandLast	: COLOR2;
};

VS_OUT_AccumulateOccl AccumulateOcclVS(VS_IN_AccumulateOccl input)
{
	VS_OUT_AccumulateOccl output = (VS_OUT_AccumulateOccl) 0;
	
	output.pos = input.pos;
	output.texUV = input.texUV;
	
	return output;
}

PS_OUT_Color AccumulateOcclPS(VS_OUT_AccumulateOccl input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
	
	float4 srcTLBand1 = tex2D(g_SampSrc1, input.texUV + float2(-0.5f * g_TexSizeRcp, -0.5f * g_TexSizeRcp));
	float4 srcTRBand1 = tex2D(g_SampSrc1, input.texUV + float2(0.5f * g_TexSizeRcp, -0.5f * g_TexSizeRcp));
	float4 srcBLBand1 = tex2D(g_SampSrc1, input.texUV + float2(-0.5f * g_TexSizeRcp, 0.5f * g_TexSizeRcp));
	float4 srcBRBand1 = tex2D(g_SampSrc1, input.texUV + float2(0.5f * g_TexSizeRcp, 0.5f * g_TexSizeRcp));

	float4 srcTLBand2 = tex2D(g_SampSrc2, input.texUV + float2(-0.5f * g_TexSizeRcp, -0.5f * g_TexSizeRcp));
	float4 srcTRBand2 = tex2D(g_SampSrc2, input.texUV + float2(0.5f * g_TexSizeRcp, -0.5f * g_TexSizeRcp));
	float4 srcBLBand2 = tex2D(g_SampSrc2, input.texUV + float2(-0.5f * g_TexSizeRcp, 0.5f * g_TexSizeRcp));
	float4 srcBRBand2 = tex2D(g_SampSrc2, input.texUV + float2(0.5f * g_TexSizeRcp, 0.5f * g_TexSizeRcp));

	float4 srcTLBandLast = tex2D(g_SampSrc3, input.texUV + float2(-0.5f * g_TexSizeRcp, -0.5f * g_TexSizeRcp));
	float4 srcTRBandLast = tex2D(g_SampSrc3, input.texUV + float2(0.5f * g_TexSizeRcp, -0.5f * g_TexSizeRcp));
	float4 srcBLBandLast = tex2D(g_SampSrc3, input.texUV + float2(-0.5f * g_TexSizeRcp, 0.5f * g_TexSizeRcp));
	float4 srcBRBandLast = tex2D(g_SampSrc3, input.texUV + float2(0.5f * g_TexSizeRcp, 0.5f * g_TexSizeRcp));

	output.colorBand1 = srcTLBand1 + srcTRBand1 + srcBLBand1 + srcBRBand1;
	output.colorBand2 = srcTLBand2 + srcTRBand2 + srcBLBand2 + srcBRBand2;
	output.colorBandLast = srcTLBandLast + srcTRBandLast + srcBLBandLast + srcBRBandLast;
	
	return output;
}

float4 GetBand1YLM(float3 lobeDir)
{
	const static float4 y1 = float4(0.282095f, 0.488602f, 0.488602f, 0.488602f);

	float3 dir = lobeDir;
	dir.yz = float2(-dir.z, dir.y);
	float4 factor = float4(1.0f, dir.yzx);

	return y1 * factor;
}

float4 GetBand2YLM(float3 lobeDir)
{
	const static float4 y2 = float4(1.092548f, 1.092548f, 0.315392f, 1.092548f);

	float3 dir = lobeDir;
	dir.yz = float2(-dir.z, dir.y);
	float4 factor = dir.yyzx * dir.xzzz;
	factor.z = 3.0f * factor.z - 1.0f;

	return y2 * factor;
}

float GetBandLastYLM(float3 lobeDir)
{
	const static float yLast = 0.546274f;

	float3 dir = lobeDir;
	dir.yz = float2(-dir.z, dir.y);
	return yLast * (dir.x * dir.x - dir.y * dir.y);
}

float3 GetDirFromUV(float2 uv, float3x3 paraView)
{
	float2 uvRange = uv;
	uvRange.y = 1.0f - uvRange.y;
	uvRange = uvRange * 2.0f - 1.0f;
			
	float3 dir;
	dir.x = (2.0f * uvRange.x) / (uvRange.y * uvRange.y + uvRange.x * uvRange.x + 1.0f);
	dir.y = -(uvRange.y * uvRange.y + uvRange.x * uvRange.x - 1.0f) / (uvRange.y * uvRange.y + uvRange.x * uvRange.x + 1.0f);
	dir.z = (2.0f * uvRange.y) / (uvRange.y * uvRange.y + uvRange.x * uvRange.x + 1.0f);

	dir = normalize(dir);
	dir = mul(paraView, dir);

	return dir;
}

PS_OUT_Color
InitializeOcclRedChannelPS(VS_OUT_AccumulateOccl input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
	
	float2 topLeftUV = input.texUV + float2(-0.5f * g_TexSizeRcp, -0.5f * g_TexSizeRcp);
	float2 topRightUV = input.texUV + float2(0.5f * g_TexSizeRcp, -0.5f * g_TexSizeRcp);
	float2 bottomLeftUV = input.texUV + float2(-0.5f * g_TexSizeRcp, 0.5f * g_TexSizeRcp);
	float2 bottomRightUV = input.texUV + float2(0.5f * g_TexSizeRcp, 0.5f * g_TexSizeRcp);

	float srcTL = tex2D(g_SampSrc1, topLeftUV).r;
	float srcTR = tex2D(g_SampSrc1, topRightUV).r;
	float srcBL = tex2D(g_SampSrc1, bottomLeftUV).r;
	float srcBR = tex2D(g_SampSrc1, bottomRightUV).r;

	float weightTL = tex2D(g_SampWeight, topLeftUV).r;
	float weightTR = tex2D(g_SampWeight, topRightUV).r;
	float weightBL = tex2D(g_SampWeight, bottomLeftUV).r;
	float weightBR = tex2D(g_SampWeight, bottomRightUV).r;

	float4 ylmBand1TL = GetBand1YLM( GetDirFromUV(topLeftUV, g_ParaViewMat) );
	float4 ylmBand1TR = GetBand1YLM( GetDirFromUV(topRightUV, g_ParaViewMat) );
	float4 ylmBand1BL = GetBand1YLM( GetDirFromUV(bottomLeftUV, g_ParaViewMat) );
	float4 ylmBand1BR = GetBand1YLM( GetDirFromUV(bottomRightUV, g_ParaViewMat) );

	float4 ylmBand2TL = GetBand2YLM( GetDirFromUV(topLeftUV, g_ParaViewMat) );
	float4 ylmBand2TR = GetBand2YLM( GetDirFromUV(topRightUV, g_ParaViewMat) );
	float4 ylmBand2BL = GetBand2YLM( GetDirFromUV(bottomLeftUV, g_ParaViewMat) );
	float4 ylmBand2BR = GetBand2YLM( GetDirFromUV(bottomRightUV, g_ParaViewMat) );
	
	float ylmBandLastTL = GetBandLastYLM( GetDirFromUV(topLeftUV, g_ParaViewMat) );
	float ylmBandLastTR = GetBandLastYLM( GetDirFromUV(topRightUV, g_ParaViewMat) );
	float ylmBandLastBL = GetBandLastYLM( GetDirFromUV(bottomLeftUV, g_ParaViewMat) );
	float ylmBandLastBR = GetBandLastYLM( GetDirFromUV(bottomRightUV, g_ParaViewMat) );

	output.colorBand1			= weightTL * srcTL * ylmBand1TL + weightTR * srcTR * ylmBand1TR + weightBL * srcBL * ylmBand1BL + weightBR * srcBR * ylmBand1BR;
	output.colorBand2			= weightTL * srcTL * ylmBand2TL + weightTR * srcTR * ylmBand2TR + weightBL * srcBL * ylmBand2BL + weightBR * srcBR * ylmBand2BR;
	output.colorBandLast.xyzw	= weightTL * srcTL * ylmBandLastTL + weightTR * srcTR * ylmBandLastTR + weightBL * srcBL * ylmBandLastBL + weightBR * srcBR * ylmBandLastBR;
	
	return output;
}

PS_OUT_Color
InitializeOcclGreenChannelPS(VS_OUT_AccumulateOccl input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
	
	float2 topLeftUV = input.texUV + float2(-0.5f * g_TexSizeRcp, -0.5f * g_TexSizeRcp);
	float2 topRightUV = input.texUV + float2(0.5f * g_TexSizeRcp, -0.5f * g_TexSizeRcp);
	float2 bottomLeftUV = input.texUV + float2(-0.5f * g_TexSizeRcp, 0.5f * g_TexSizeRcp);
	float2 bottomRightUV = input.texUV + float2(0.5f * g_TexSizeRcp, 0.5f * g_TexSizeRcp);

	float srcTL = tex2D(g_SampSrc1, topLeftUV).g;
	float srcTR = tex2D(g_SampSrc1, topRightUV).g;
	float srcBL = tex2D(g_SampSrc1, bottomLeftUV).g;
	float srcBR = tex2D(g_SampSrc1, bottomRightUV).g;

	float weightTL = tex2D(g_SampWeight, topLeftUV).r;
	float weightTR = tex2D(g_SampWeight, topRightUV).r;
	float weightBL = tex2D(g_SampWeight, bottomLeftUV).r;
	float weightBR = tex2D(g_SampWeight, bottomRightUV).r;

	float4 ylmBand1TL = GetBand1YLM( GetDirFromUV(topLeftUV, g_ParaViewMat) );
	float4 ylmBand1TR = GetBand1YLM( GetDirFromUV(topRightUV, g_ParaViewMat) );
	float4 ylmBand1BL = GetBand1YLM( GetDirFromUV(bottomLeftUV, g_ParaViewMat) );
	float4 ylmBand1BR = GetBand1YLM( GetDirFromUV(bottomRightUV, g_ParaViewMat) );

	float4 ylmBand2TL = GetBand2YLM( GetDirFromUV(topLeftUV, g_ParaViewMat) );
	float4 ylmBand2TR = GetBand2YLM( GetDirFromUV(topRightUV, g_ParaViewMat) );
	float4 ylmBand2BL = GetBand2YLM( GetDirFromUV(bottomLeftUV, g_ParaViewMat) );
	float4 ylmBand2BR = GetBand2YLM( GetDirFromUV(bottomRightUV, g_ParaViewMat) );
	
	float ylmBandLastTL = GetBandLastYLM( GetDirFromUV(topLeftUV, g_ParaViewMat) );
	float ylmBandLastTR = GetBandLastYLM( GetDirFromUV(topRightUV, g_ParaViewMat) );
	float ylmBandLastBL = GetBandLastYLM( GetDirFromUV(bottomLeftUV, g_ParaViewMat) );
	float ylmBandLastBR = GetBandLastYLM( GetDirFromUV(bottomRightUV, g_ParaViewMat) );

	output.colorBand1			= weightTL * srcTL * ylmBand1TL + weightTR * srcTR * ylmBand1TR + weightBL * srcBL * ylmBand1BL + weightBR * srcBR * ylmBand1BR;
	output.colorBand2			= weightTL * srcTL * ylmBand2TL + weightTR * srcTR * ylmBand2TR + weightBL * srcBL * ylmBand2BL + weightBR * srcBR * ylmBand2BR;
	output.colorBandLast.xyzw	= weightTL * srcTL * ylmBandLastTL + weightTR * srcTR * ylmBandLastTR + weightBL * srcBL * ylmBandLastBL + weightBR * srcBR * ylmBandLastBR;
	
	return output;
}

PS_OUT_Color
InitializeOcclBlueChannelPS(VS_OUT_AccumulateOccl input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
	
	float2 topLeftUV = input.texUV + float2(-0.5f * g_TexSizeRcp, -0.5f * g_TexSizeRcp);
	float2 topRightUV = input.texUV + float2(0.5f * g_TexSizeRcp, -0.5f * g_TexSizeRcp);
	float2 bottomLeftUV = input.texUV + float2(-0.5f * g_TexSizeRcp, 0.5f * g_TexSizeRcp);
	float2 bottomRightUV = input.texUV + float2(0.5f * g_TexSizeRcp, 0.5f * g_TexSizeRcp);

	float srcTL = tex2D(g_SampSrc1, topLeftUV).b;
	float srcTR = tex2D(g_SampSrc1, topRightUV).b;
	float srcBL = tex2D(g_SampSrc1, bottomLeftUV).b;
	float srcBR = tex2D(g_SampSrc1, bottomRightUV).b;

	float weightTL = tex2D(g_SampWeight, topLeftUV).r;
	float weightTR = tex2D(g_SampWeight, topRightUV).r;
	float weightBL = tex2D(g_SampWeight, bottomLeftUV).r;
	float weightBR = tex2D(g_SampWeight, bottomRightUV).r;

	float4 ylmBand1TL = GetBand1YLM( GetDirFromUV(topLeftUV, g_ParaViewMat) );
	float4 ylmBand1TR = GetBand1YLM( GetDirFromUV(topRightUV, g_ParaViewMat) );
	float4 ylmBand1BL = GetBand1YLM( GetDirFromUV(bottomLeftUV, g_ParaViewMat) );
	float4 ylmBand1BR = GetBand1YLM( GetDirFromUV(bottomRightUV, g_ParaViewMat) );

	float4 ylmBand2TL = GetBand2YLM( GetDirFromUV(topLeftUV, g_ParaViewMat) );
	float4 ylmBand2TR = GetBand2YLM( GetDirFromUV(topRightUV, g_ParaViewMat) );
	float4 ylmBand2BL = GetBand2YLM( GetDirFromUV(bottomLeftUV, g_ParaViewMat) );
	float4 ylmBand2BR = GetBand2YLM( GetDirFromUV(bottomRightUV, g_ParaViewMat) );
	
	float ylmBandLastTL = GetBandLastYLM( GetDirFromUV(topLeftUV, g_ParaViewMat) );
	float ylmBandLastTR = GetBandLastYLM( GetDirFromUV(topRightUV, g_ParaViewMat) );
	float ylmBandLastBL = GetBandLastYLM( GetDirFromUV(bottomLeftUV, g_ParaViewMat) );
	float ylmBandLastBR = GetBandLastYLM( GetDirFromUV(bottomRightUV, g_ParaViewMat) );

	output.colorBand1			= weightTL * srcTL * ylmBand1TL + weightTR * srcTR * ylmBand1TR + weightBL * srcBL * ylmBand1BL + weightBR * srcBR * ylmBand1BR;
	output.colorBand2			= weightTL * srcTL * ylmBand2TL + weightTR * srcTR * ylmBand2TR + weightBL * srcBL * ylmBand2BL + weightBR * srcBR * ylmBand2BR;
	output.colorBandLast.xyzw	= weightTL * srcTL * ylmBandLastTL + weightTR * srcTR * ylmBandLastTR + weightBL * srcBL * ylmBandLastBL + weightBR * srcBR * ylmBandLastBR;
	
	return output;
}

#endif