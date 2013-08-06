//==============================================================================
//
//		HLDL1.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Tuesday, February 06, 2007
//
//==============================================================================

#ifndef HLDL1
#define HLDL1

struct VS_OUT_HLDL1
{
	float4 pos		: POSITION0;
	float4 color	: COLOR0;
	float2 texUV	: TEXCOORD0;
};

VS_OUT_HLDL1
HLDL1VS(VS_IN_Basic input)
{
	VS_OUT_HLDL1 output = (VS_OUT_HLDL1) 0;
		
    float4 worldPos = mul(input.pos, g_matWorld);
	output.pos = mul(worldPos, g_matViewProj);
	
  //  output.pos = mul(output.pos, g_matViewProj);
	  
	float3 N = normalize( mul(input.normal, g_matWorld) );
	float4 hemiColor = GetHemiAmbientColor(N.y);
	
	float NDotL = saturate( dot(N, -g_LightDir.dir) );
	float4 dirColor = NDotL * g_LightDir.color;
	
	output.color = hemiColor + dirColor;
	output.texUV = input.texUV;
	
	return output;
}

PS_OUT_Color
HLDL1PS(VS_OUT_HLDL1 input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
	
	float4 diffuseColor = tex2D(g_SampDiffuse, input.texUV);
	output.color = float4(diffuseColor.rgb * input.color.rgb, diffuseColor.a);
	
	ConditionalAlphaTest(diffuseColor.a);
	
	return output;
}

#endif
