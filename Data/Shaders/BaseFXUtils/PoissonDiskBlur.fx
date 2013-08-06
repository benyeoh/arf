//================================================================================
//
//		PoissonDiskBlur.fx
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/5/2012
//
//================================================================================

#include "[shd]\BaseFXUtils\ParamSemanticDefs.fxh"

texture g_TargetTex <int semantic = BFXU_RT1; int usage = 1;>;
sampler g_SampTarget = sampler_state
{
    	Texture = <g_TargetTex>;
    	MipFilter = LINEAR;
    	MinFilter = LINEAR;
    	MagFilter = LINEAR;
    	ADDRESSU  = CLAMP;
    	ADDRESSV  = CLAMP;
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

//=============================== Functions ==========================================

float4 Tex2D(sampler samp, float2 uv)
{
	return tex2D(samp, uv);
}

float Vec4X(float4 v4)
{
	return v4.x;
}


//=============================== Vertex Shaders ==========================================

struct VS_INSimpleScreenSpaceVS
{
	float4 vsinput_0 : POSITION;
	float2 vsinput_1 : TEXCOORD0;
};

struct VS_OUTSimpleScreenSpaceVS
{
	float4 vsoutput_0 : POSITION0;
	float2 vsoutput_1 : TEXCOORD0;
};

VS_OUTSimpleScreenSpaceVS SimpleScreenSpaceVS(VS_INSimpleScreenSpaceVS input)
{
	VS_OUTSimpleScreenSpaceVS output;

	output.vsoutput_0 = input.vsinput_0;// - float4(1.0f / 508.0f, -1.0f / 370.0f, 0.0f, 0.0f);
	output.vsoutput_1 = input.vsinput_1;
	return output;
}


//=============================== Pixel Shaders ==========================================

struct PS_IN
{
	float2 texUV : TEXCOORD0;
};

struct PS_OUT
{
	float4 color : COLOR0;
};

PS_OUT PoissonDiskBlur4PS(PS_IN input)
{
	const float2 s_Offsets[4] = 
	{ 
		float2(0.8711244f, 0.1095603f),
		float2(0.1300417f, -0.7391417f),
		float2(-0.2700687f, 0.5261486f),
		float2(-0.6673112f, -0.2287632f),
	};

	PS_OUT output;

	float4 outColor = 0.0f;

	for(int i=0; i < 4; i++)
	{
		outColor += Tex2D(g_SampTarget, input.texUV + s_Offsets[i] * g_Scale);
	}

	output.color = outColor * 1.0f / 4.0f;
	return output;
}

PS_OUT PoissonDiskBlur8PS(PS_IN input)
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

	float4 outColor = 0.0f;

	for(int i=0; i < 8; i++)
	{
		outColor += Tex2D(g_SampTarget, input.texUV + s_Offsets[i] * g_Scale);
	}

	output.color = outColor * 1.0f / 8.0f;
	return output;
}

PS_OUT PoissonDiskBlur12PS(PS_IN input)
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

	float4 outColor = 0.0f;

	for(int i=0; i < 12; i++)
	{
		outColor += Tex2D(g_SampTarget, input.texUV + s_Offsets[i] * g_Scale);
	}

	output.color = outColor * 1.0f / 12.0f;
	return output;
}

PS_OUT PoissonDiskBlur16PS(PS_IN input)
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

	float4 outColor = 0.0f;

	for(int i=0; i < 16; i++)
	{
		outColor += Tex2D(g_SampTarget, input.texUV + s_Offsets[i] * g_Scale);
	}

	output.color = outColor * 1.0f / 16.0f;
	return output;
}

PS_OUT PoissonDiskBlur4WithNoisePS(PS_IN input)
{
	const float2 s_Offsets[4] = 
	{ 
		float2(0.8711244f, 0.1095603f),
		float2(0.1300417f, -0.7391417f),
		float2(-0.2700687f, 0.5261486f),
		float2(-0.6673112f, -0.2287632f),
	};

	PS_OUT output;

	float2 randomVec	= (tex2D( g_SampNoise, input.texUV * g_NoiseScale).ar - 0.5f) * 2.0f;	
	randomVec = normalize(randomVec);

	float4 outColor = 0.0f;
	
	for(int i=0; i < 4; i++)
	{
		float2 rotateOffset = reflect( s_Offsets[i], randomVec );
		outColor += Tex2D(g_SampTarget, input.texUV + rotateOffset * g_Scale);
	}

	output.color = outColor * 1.0f / 4.0f;
	return output;
}

PS_OUT PoissonDiskBlur8WithNoisePS(PS_IN input)
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

	float2 randomVec	= (tex2D( g_SampNoise, input.texUV * g_NoiseScale).ar - 0.5f) * 2.0f;	
	randomVec = normalize(randomVec);
	float4 outColor = 0.0f;

	for(int i=0; i < 8; i++)
	{
		float2 rotateOffset = reflect( s_Offsets[i], randomVec );
		outColor += Tex2D(g_SampTarget, input.texUV + rotateOffset * g_Scale);
	}

	output.color = outColor * 1.0f / 8.0f;
	return output;
}

PS_OUT PoissonDiskBlur12WithNoisePS(PS_IN input)
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

	float2 randomVec	= (tex2D( g_SampNoise, input.texUV * g_NoiseScale).ar - 0.5f) * 2.0f;	
	randomVec = normalize(randomVec);
	float4 outColor = 0.0f;

	for(int i=0; i < 12; i++)
	{
		float2 rotateOffset = reflect( s_Offsets[i], randomVec );
		outColor += Tex2D(g_SampTarget, input.texUV + rotateOffset * g_Scale);
	}

	output.color = outColor * 1.0f / 12.0f;
	return output;
}

PS_OUT PoissonDiskBlur16WithNoisePS(PS_IN input)
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

	float2 randomVec	= (tex2D( g_SampNoise, input.texUV * g_NoiseScale).ar - 0.5f) * 2.0f;	
	randomVec = normalize(randomVec);
	float4 outColor = 0.0f;

	for(int i=0; i < 16; i++)
	{
		float2 rotateOffset = reflect( s_Offsets[i], randomVec );
		outColor += Tex2D(g_SampTarget, input.texUV + rotateOffset * g_Scale);
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
		VertexShader = compile vs_3_0 SimpleScreenSpaceVS();
		PixelShader = compile ps_3_0 PoissonDiskBlur4PS();
	}

};

technique t1
{
	pass p0
	{
		ZEnable = FALSE;
		VertexShader = compile vs_3_0 SimpleScreenSpaceVS();
		PixelShader = compile ps_3_0 PoissonDiskBlur8PS();
	}

};

technique t2
{
	pass p0
	{
		ZEnable = FALSE;
		VertexShader = compile vs_3_0 SimpleScreenSpaceVS();
		PixelShader = compile ps_3_0 PoissonDiskBlur12PS();
	}

};

technique t3
{
	pass p0
	{
		ZEnable = FALSE;
		VertexShader = compile vs_3_0 SimpleScreenSpaceVS();
		PixelShader = compile ps_3_0 PoissonDiskBlur16PS();
	}

};

technique t4
{
	pass p0
	{
		ZEnable = FALSE;
		VertexShader = compile vs_3_0 SimpleScreenSpaceVS();
		PixelShader = compile ps_3_0 PoissonDiskBlur4WithNoisePS();
	}

};

technique t8
{
	pass p0
	{
		ZEnable = FALSE;
		VertexShader = compile vs_3_0 SimpleScreenSpaceVS();
		PixelShader = compile ps_3_0 PoissonDiskBlur8WithNoisePS();
	}

};

technique t9
{
	pass p0
	{
		ZEnable = FALSE;
		VertexShader = compile vs_3_0 SimpleScreenSpaceVS();
		PixelShader = compile ps_3_0 PoissonDiskBlur12WithNoisePS();
	}

};

technique t10
{
	pass p0
	{
		ZEnable = FALSE;
		VertexShader = compile vs_3_0 SimpleScreenSpaceVS();
		PixelShader = compile ps_3_0 PoissonDiskBlur16WithNoisePS();
	}

};