//==============================================================================
//
//		BrightPass.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		10/2/2007
//
//==============================================================================

float g_Exposure < int semantic = 51; >;															
float g_BloomThreshold < int semantic = 52; >;
// float g_BloomOffset	< int semantic = 53; >;

texture		g_TexRT1 < int semantic = 4250; 
					int usage = 1; 
					string name = "RT 1"; >;

sampler		g_SampRT1 = sampler_state
{
    Texture = (g_TexRT1);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = CLAMP;
    ADDRESSV  = CLAMP;
};
															
struct VS_OUT_GaussianBlur1D
{
	float4 pos			: POSITION0;
	float2 texUVTap0	: TEXCOORD0;
	float2 texUVTap1	: TEXCOORD1;
	float2 texUVTap2	: TEXCOORD2;
	float2 texUVTap3	: TEXCOORD3;
	float2 texUVTap4	: TEXCOORD4;
	float2 texUVTap5	: TEXCOORD5;
	float2 texUVTap6	: TEXCOORD6;
	float2 texUV		: TEXCOORD7;
	
};

struct VS_IN_2D
{
	float4 pos		: POSITION;
	float2 texUV	: TEXCOORD0;
};

struct PS_OUT_Color
{
	float4 color : COLOR0;
};

struct VS_OUT_ScreenSpaceBasic
{
	float4 pos			: POSITION0;
	float2 texUV		: TEXCOORD0;
};

VS_OUT_ScreenSpaceBasic
ScreenSpaceBasicVS(VS_IN_2D input)
{
	VS_OUT_ScreenSpaceBasic output = (VS_OUT_ScreenSpaceBasic) 0;
		
	// NOTE: Assumes either position or texUV has been offset appropriately
	output.pos = float4(input.pos.xy, 0.0f, 1.0f);
	output.texUV = input.texUV;
	
	return output;
}

PS_OUT_Color
BrightPassPS(VS_OUT_ScreenSpaceBasic input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
	
    float4 color = tex2D(g_SampRT1, input.texUV);

	// Perform exposure mapping
	float4 exposedColor = saturate(1.0f - exp( -g_Exposure * color )); 
	exposedColor = saturate(exposedColor - g_BloomThreshold);
	
	output.color = exposedColor / (1.0f - g_BloomThreshold);
	
	return output;
	
	/*
	// Perform tone mapping based on the middle gray color and average scene luminance
	// to find out the pixel color if tone mapping is performed
    color.rgb *= 0.18f / ( 0.066 + 0.001f );	
    
    // Range [0..2 * g_WhiteCutoff] if color is [0..g_WhiteCutoff]
    color.rgb *= ( 1.0f + ( color.rgb / ( 0.87f * 0.87f ) ) );
    
    // Remove dark pixels
    color.rgb -= 5.0f;

    color.rgb = max( color.rgb, 0.0f );

	// Map from 0..1
    color.rgb /= ( 10.0f + color.rgb );

    output.color = color;
    
	return output;
	*/
}

technique t0
{
	pass p0
	{
		VertexShader	=	compile vs_2_0 ScreenSpaceBasicVS();
		PixelShader		=	compile ps_2_0 BrightPassPS();
	}
};
