//==============================================================================
//
//		LambertianDiffuse.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Tuesday, June 06, 2006
//
//==============================================================================

#include "Globals\ShaderDefs.fxh"
#include "Globals\ShaderIO.fxh"
#include "Globals\VarTransform.fxh"
#include "Globals\VarLighting.fxh"
#include "Globals\ConstDiffuseTextures.fxh"

const int EffectType = RFX_TYPE_DIFFUSE;

struct VS_OUT_Diffuse
{
	float4 pos		: POSITION0;
	float4 color	: COLOR0;
	float2 texUV	: TEXCOORD0;
};

VS_OUT_Diffuse
LambertianDiffuseVS(VS_IN_Basic input)
{
	VS_OUT_Diffuse output = (VS_OUT_Diffuse) 0;
	
	//float4 pos = mul(input.pos, g_matView);
	
    output.pos = mul(input.pos, g_matWorld);
    output.pos = mul(output.pos, g_matViewProj);
	//output.pos.z = 0.5;
//	output.pos.w = 1.0f;
	  
	float3 N = normalize( mul(input.normal, g_matWorld) );
	float4 hemiColor = lerp(g_LightHemi.groundColor, g_LightHemi.skyColor, N.y * 0.5f + 0.5f );
	
	float NDotL = saturate( dot(N, -g_LightDir.dir) );
	float4 dirColor = NDotL * g_LightDir.color;
	
	output.color = hemiColor + dirColor;
	output.texUV = input.texUV;
	
	return output;
}

PS_OUT_Color
LambertianDiffusePS(VS_OUT_Diffuse input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
	
	float4 diffuseTexColor = tex2D(g_SampDiffuse, input.texUV);
	output.color = float4(diffuseTexColor.rgb * input.color.rgb, diffuseTexColor.a);
	
	return output;
}

technique t0
{
	pass p0
	{
		//CullMode = NONE;
		VertexShader	=	compile vs_2_0 LambertianDiffuseVS();
		PixelShader		=	compile ps_2_0 LambertianDiffusePS();
	}
};

