//================================================================================
//
//		PoissonDiskCubeBlur.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/16/2012
//
//================================================================================

#include "[shd]\BaseFXUtils\ParamSemanticDefs.fxh"

texture g_TargetCubeTex <int semantic = BFXU_RT1; int usage = 1;>;
sampler g_SampCubeTarget = sampler_state
{
    	Texture = <g_TargetCubeTex>;
    	MipFilter = LINEAR;
    	MinFilter = LINEAR;
    	MagFilter = LINEAR;
    	ADDRESSU  = WRAP;
    	ADDRESSV  = WRAP;
};

texture g_NoiseTex <int semantic = BFXU_RT2; int usage = 1;>;
sampler g_SampNoise = sampler_state
{
    	Texture = <g_NoiseTex>;
    	MipFilter = LINEAR;
    	MinFilter = LINEAR;
    	MagFilter = LINEAR;
    	ADDRESSU  = WRAP;
    	ADDRESSV  = WRAP;
};

float g_Scale <int semantic = BFXU_POISSON_DISK_SCALE; int usage = 1;>;
float2 g_NoiseScale <int semantic = BFXU_NOISE_SCALE; int usage = 1;>;

float3 g_XCubeAxis <int semantic = BFXU_X_CUBE_AXIS; int usage = 1;>;
float3 g_YCubeAxis <int semantic = BFXU_Y_CUBE_AXIS; int usage = 1;>;
float3 g_ZCubeAxis <int semantic = BFXU_Z_CUBE_AXIS; int usage = 1;>;

//=============================== Vertex Shaders ==========================================

struct VS_INCubeScreenSpaceVS
{
	float4 vsinput_0	: POSITION;
	float2 texUV		: TEXCOORD0;
};

struct VS_OUTCubeScreenSpaceVS
{
	float4 vsoutput_0	: POSITION0;

	float2 texUV		: TEXCOORD0;
	float3 xAxis		: TEXCOORD1;
	float3 yAxis		: TEXCOORD2;
	float3 zAxis		: TEXCOORD3;
};

VS_OUTCubeScreenSpaceVS CubeScreenSpaceVS(VS_INCubeScreenSpaceVS input)
{
	VS_OUTCubeScreenSpaceVS output;

	output.vsoutput_0 = input.vsinput_0;
	output.texUV = input.texUV;

	float2 rangeUV = input.texUV * 2.0f - 1.0f;

	float3 xCubeAxis = g_XCubeAxis;
	float3 yCubeAxis = g_YCubeAxis;
	float3 zCubeAxis = g_ZCubeAxis;

	// Flip the direction of z for D3D
	xCubeAxis.z = -xCubeAxis.z;
	yCubeAxis.z = -yCubeAxis.z;
	zCubeAxis.xy = -zCubeAxis.xy;
	
	float3 zAxis = xCubeAxis * rangeUV.x + yCubeAxis * -rangeUV.y + zCubeAxis;
	zAxis = normalize(zAxis);
	float3 xAxis = normalize( cross(yCubeAxis, zAxis) );
	float3 yAxis = normalize( cross(zAxis, xAxis) );

	output.xAxis = xAxis;
	output.yAxis = yAxis;
	output.zAxis = zAxis;

	return output;
}


//=============================== Pixel Shaders ==========================================

float4 TexCube(samplerCUBE samp, float3 normal)
{
	//normal.z = -normal.z;
	return texCUBE(samp, normal);
}

struct PS_IN
{
	float2 texUV		: TEXCOORD0;
	float3 xAxis		: TEXCOORD1;
	float3 yAxis		: TEXCOORD2;
	float3 zAxis		: TEXCOORD3;
};

struct PS_OUT
{
	float4 color : COLOR0;
};

PS_OUT PoissonDiskCubeBlur4PS(PS_IN input)
{
	const float2 s_Offsets[4] = 
	{ 
		float2(0.8711244f, 0.1095603f),
		float2(0.1300417f, -0.7391417f),
		float2(-0.2700687f, 0.5261486f),
		float2(-0.6673112f, -0.2287632f),
	};

	PS_OUT output;

	float3 xAxis = normalize(input.xAxis);
	float3 yAxis = normalize(input.yAxis);
	float3 zAxis = normalize(input.zAxis);

	float4 outColor = 0.0f;

	for(int i=0; i < 4; i++)
	{
		float3 normal = g_Scale * ((xAxis * s_Offsets[i].x) + (yAxis * s_Offsets[i].y)) + zAxis;
		outColor += TexCube(g_SampCubeTarget, normal);
	}

	output.color = outColor * 1.0f / 4.0f;
	return output;
}

PS_OUT PoissonDiskCubeBlur8PS(PS_IN input)
{
	const float2 s_Offsets[8] = 
	{ 
		float2(0.1695201f, -0.05953508f),
		float2(0.4917141f, 0.8534482f),
		float2(-0.8644773f, 0.0836703f),
		float2(-0.2943875f, 0.8032797f),
		float2(0.751675f, -0.5007603f),
		float2(0.8232338f, 0.2550838f),
		float2(0.03609824f, -0.8680585f),
		float2(-0.7443366f, -0.6284065f)
	};

	PS_OUT output;

	float3 xAxis = normalize(input.xAxis);
	float3 yAxis = normalize(input.yAxis);
	float3 zAxis = normalize(input.zAxis);
	
	float4 outColor = 0.0f;

	for(int i=0; i < 8; i++)
	{
		float3 normal = g_Scale * ((xAxis * s_Offsets[i].x) + (yAxis * s_Offsets[i].y)) + zAxis;
		outColor += TexCube(g_SampCubeTarget, normal);
	}

	output.color = outColor * 1.0f / 8.0f;
	return output;
}

PS_OUT PoissonDiskCubeBlur12PS(PS_IN input)
{
	const float2 s_Offsets[12] = 
	{ 
		float2(0.3518657f, -0.7296223f),
		float2(0.8669271f, -0.1982861f),
		float2(0.1751744f, -0.1415784f),
		float2(-0.5058741f, -0.5981063f),
		float2(-0.06567506f, -0.9714326f),
		float2(0.7206139f, 0.3936176f),
		float2(-0.4052159f, 0.3813289f),
		float2(-0.3775871f, -0.1442793f),
		float2(0.3404936f, 0.6875984f),
		float2(-0.1862736f, 0.8969402f),
		float2(-0.8769998f, 0.1781189f),
		float2(-0.9405025f, -0.2986519f),
	};

	PS_OUT output;
	
	float3 xAxis = normalize(input.xAxis);
	float3 yAxis = normalize(input.yAxis);
	float3 zAxis = normalize(input.zAxis);

	float4 outColor = 0.0f;

	for(int i=0; i < 12; i++)
	{
		float3 normal = g_Scale * ((xAxis * s_Offsets[i].x) + (yAxis * s_Offsets[i].y)) + zAxis;
		outColor += TexCube(g_SampCubeTarget, normal);
	}

	output.color = outColor * 1.0f / 12.0f;
	return output;
}

PS_OUT PoissonDiskCubeBlur16PS(PS_IN input)
{
	const float2 s_Offsets[16] = 
	{ 
		float2(0.09431711f, 0.5941997f),
		float2(0.4307326f, -0.0523821f),
		float2(0.5709904f, 0.4426674f),
		float2(-0.2226814f, 0.1452472f),
		float2(-0.4285987f, 0.8203481f),
		float2(0.4150211f, 0.9065326f),
		float2(-0.5673039f, 0.4133336f),
		float2(-0.9493649f, 0.254185f),
		float2(-0.7343475f, -0.3721381f),
		float2(-0.1060963f, -0.2668646f),
		float2(-0.4190173f, -0.6772105f),
		float2(-0.1290366f, -0.9784364f),
		float2(0.3123248f, -0.5879446f),
		float2(0.7560202f, -0.6177078f),
		float2(0.8244288f, -0.1976019f),
		float2(0.9751366f, 0.2143216f)
	};

	PS_OUT output;
	
	float3 xAxis = normalize(input.xAxis);
	float3 yAxis = normalize(input.yAxis);
	float3 zAxis = normalize(input.zAxis);

	float4 outColor = 0.0f;

	for(int i=0; i < 16; i++)
	{
		float3 normal = g_Scale * ((xAxis * s_Offsets[i].x) + (yAxis * s_Offsets[i].y)) + zAxis;
		outColor += TexCube(g_SampCubeTarget, normal);
	}

	output.color = outColor * 1.0f / 16.0f;
	return output;
}

PS_OUT PoissonDiskCubeBlur4WithNoisePS(PS_IN input)
{
	const float2 s_Offsets[4] = 
	{ 
		float2(0.8711244f, 0.1095603f),
		float2(0.1300417f, -0.7391417f),
		float2(-0.2700687f, 0.5261486f),
		float2(-0.6673112f, -0.2287632f),
	};

	PS_OUT output;
	
	float3 xAxis = normalize(input.xAxis);
	float3 yAxis = normalize(input.yAxis);
	float3 zAxis = normalize(input.zAxis);

	float2 randomVec	= (tex2D( g_SampNoise, input.texUV * g_NoiseScale).ar - 0.5f) * 2.0f;	
	randomVec = normalize(randomVec);

	float4 outColor = 0.0f;
	
	for(int i=0; i < 4; i++)
	{
		float2 rotateOffset = reflect( s_Offsets[i], randomVec );

		float3 normal = g_Scale * ((xAxis * rotateOffset.x) + (yAxis * rotateOffset.y)) + zAxis;
		outColor += TexCube(g_SampCubeTarget, normal);
	}

	output.color = outColor * 1.0f / 4.0f;
	return output;
}

PS_OUT PoissonDiskCubeBlur8WithNoisePS(PS_IN input)
{
	const float2 s_Offsets[8] = 
	{ 
		float2(0.1695201f, -0.05953508f),
		float2(0.4917141f, 0.8534482f),
		float2(-0.8644773f, 0.0836703f),
		float2(-0.2943875f, 0.8032797f),
		float2(0.751675f, -0.5007603f),
		float2(0.8232338f, 0.2550838f),
		float2(0.03609824f, -0.8680585f),
		float2(-0.7443366f, -0.6284065f)
	};

	PS_OUT output;
	
	float3 xAxis = normalize(input.xAxis);
	float3 yAxis = normalize(input.yAxis);
	float3 zAxis = normalize(input.zAxis);

	float2 randomVec	= (tex2D( g_SampNoise, input.texUV * g_NoiseScale).ar - 0.5f) * 2.0f;	
	randomVec = normalize(randomVec);
	float4 outColor = 0.0f;

	for(int i=0; i < 8; i++)
	{
		float2 rotateOffset = reflect( s_Offsets[i], randomVec );
		
		float3 normal = g_Scale * ((xAxis * rotateOffset.x) + (yAxis * rotateOffset.y)) + zAxis;
		outColor += TexCube(g_SampCubeTarget, normal);
	}

	output.color = outColor * 1.0f / 8.0f;
	return output;
}

PS_OUT PoissonDiskCubeBlur12WithNoisePS(PS_IN input)
{
	const float2 s_Offsets[12] = 
	{ 
		float2(0.3518657f, -0.7296223f),
		float2(0.8669271f, -0.1982861f),
		float2(0.1751744f, -0.1415784f),
		float2(-0.5058741f, -0.5981063f),
		float2(-0.06567506f, -0.9714326f),
		float2(0.7206139f, 0.3936176f),
		float2(-0.4052159f, 0.3813289f),
		float2(-0.3775871f, -0.1442793f),
		float2(0.3404936f, 0.6875984f),
		float2(-0.1862736f, 0.8969402f),
		float2(-0.8769998f, 0.1781189f),
		float2(-0.9405025f, -0.2986519f),
	};

	PS_OUT output;
	
	float3 xAxis = normalize(input.xAxis);
	float3 yAxis = normalize(input.yAxis);
	float3 zAxis = normalize(input.zAxis);

	float2 randomVec	= (tex2D( g_SampNoise, input.texUV * g_NoiseScale).ar - 0.5f) * 2.0f;	
	randomVec = normalize(randomVec);
	float4 outColor = 0.0f;

	for(int i=0; i < 12; i++)
	{
		float2 rotateOffset = reflect( s_Offsets[i], randomVec );
				
		float3 normal = g_Scale * ((xAxis * rotateOffset.x) + (yAxis * rotateOffset.y)) + zAxis;
		outColor += TexCube(g_SampCubeTarget, normal);
	}

	output.color = outColor * 1.0f / 12.0f;
	return output;
}

PS_OUT PoissonDiskCubeBlur16WithNoisePS(PS_IN input)
{
	const float2 s_Offsets[16] = 
	{ 
		float2(0.09431711f, 0.5941997f),
		float2(0.4307326f, -0.0523821f),
		float2(0.5709904f, 0.4426674f),
		float2(-0.2226814f, 0.1452472f),
		float2(-0.4285987f, 0.8203481f),
		float2(0.4150211f, 0.9065326f),
		float2(-0.5673039f, 0.4133336f),
		float2(-0.9493649f, 0.254185f),
		float2(-0.7343475f, -0.3721381f),
		float2(-0.1060963f, -0.2668646f),
		float2(-0.4190173f, -0.6772105f),
		float2(-0.1290366f, -0.9784364f),
		float2(0.3123248f, -0.5879446f),
		float2(0.7560202f, -0.6177078f),
		float2(0.8244288f, -0.1976019f),
		float2(0.9751366f, 0.2143216f)
	};

	PS_OUT output;
		
	float3 xAxis = normalize(input.xAxis);
	float3 yAxis = normalize(input.yAxis);
	float3 zAxis = normalize(input.zAxis);

	float2 randomVec	= (tex2D( g_SampNoise, input.texUV * g_NoiseScale).ar - 0.5f) * 2.0f;	
	randomVec = normalize(randomVec);
	float4 outColor = 0.0f;

	for(int i=0; i < 16; i++)
	{
		float2 rotateOffset = reflect( s_Offsets[i], randomVec );
				
		float3 normal = g_Scale * ((xAxis * rotateOffset.x) + (yAxis * rotateOffset.y)) + zAxis;
		outColor += TexCube(g_SampCubeTarget, normal);
	}

	output.color = outColor * 1.0f / 16.0f;
	return output;
}



//=============================== Techniques ==========================================

technique t0
{
	pass p0
	{
		ZEnable = FALSE;
		VertexShader = compile vs_3_0 CubeScreenSpaceVS();
		PixelShader = compile ps_3_0 PoissonDiskCubeBlur4PS();
	}

};

technique t1
{
	pass p0
	{
		ZEnable = FALSE;
		VertexShader = compile vs_3_0 CubeScreenSpaceVS();
		PixelShader = compile ps_3_0 PoissonDiskCubeBlur8PS();
	}

};

technique t2
{
	pass p0
	{
		ZEnable = FALSE;
		VertexShader = compile vs_3_0 CubeScreenSpaceVS();
		PixelShader = compile ps_3_0 PoissonDiskCubeBlur12PS();
	}

};

technique t3
{
	pass p0
	{
		ZEnable = FALSE;
		VertexShader = compile vs_3_0 CubeScreenSpaceVS();
		PixelShader = compile ps_3_0 PoissonDiskCubeBlur16PS();
	}

};

technique t4
{
	pass p0
	{
		ZEnable = FALSE;
		VertexShader = compile vs_3_0 CubeScreenSpaceVS();
		PixelShader = compile ps_3_0 PoissonDiskCubeBlur4WithNoisePS();
	}

};

technique t8
{
	pass p0
	{
		ZEnable = FALSE;
		VertexShader = compile vs_3_0 CubeScreenSpaceVS();
		PixelShader = compile ps_3_0 PoissonDiskCubeBlur8WithNoisePS();
	}

};

technique t9
{
	pass p0
	{
		ZEnable = FALSE;
		VertexShader = compile vs_3_0 CubeScreenSpaceVS();
		PixelShader = compile ps_3_0 PoissonDiskCubeBlur12WithNoisePS();
	}

};

technique t10
{
	pass p0
	{
		ZEnable = FALSE;
		VertexShader = compile vs_3_0 CubeScreenSpaceVS();
		PixelShader = compile ps_3_0 PoissonDiskCubeBlur16WithNoisePS();
	}

};