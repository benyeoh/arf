
//=============================== Globals ==========================================

texture g_ZPosAccumTex <int semantic = 4265; int usage = 1;>;
sampler g_SampZPosAccum = sampler_state
{
	Texture = <g_ZPosAccumTex>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
 	ADDRESSU  = CLAMP;
    ADDRESSV  = CLAMP;
};

float2 g_ScreenPixelOffset <int semantic = 4270; int usage = 1; > = float2(0.0f, 0.0f);
float2 g_ZPosTexSize <int semantic = 4271; int usage = 1;>;

texture g_NoiseTex <int semantic = 4266; int usage = 1;>;
sampler g_SampNoise = sampler_state
{
	Texture = <g_NoiseTex>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
 	ADDRESSU  = WRAP;
    ADDRESSV  = WRAP;
};

texture g_NormalTex <int semantic = 4273; int usage = 1;>;
sampler g_SampNormal = sampler_state
{
	Texture = <g_NormalTex>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
 	ADDRESSU  = CLAMP;
    ADDRESSV  = CLAMP;
};

struct FarPlaneOffsets
{
	float4 offsets[3];
};

float4 g_CameraPosWithViewFar <int semantic = 15; int usage = 0;>;

FarPlaneOffsets g_FarPlaneOffsets <int semantic = 205; int usage = 0;>;


//=============================== Functions ==========================================

float3 GetFarPlanePosFromUV(float2 uv, FarPlaneOffsets planeOffsets)
{
	return planeOffsets.offsets[0].xyz + uv.x * planeOffsets.offsets[1].xyz + uv.y * planeOffsets.offsets[2].xyz;
}

float3 GetWorldPos(float2 uv, float depth)
{
	return depth * GetFarPlanePosFromUV(uv, g_FarPlaneOffsets);// + g_CameraPosWithViewFar.xyz;
}

float2 RoundToNearestUV(float2 uv, float2 texSize)
{
	float2 remainUV = frac(uv * texSize);
	float2 offset = remainUV - 0.5f;
	
	return uv - (offset) / texSize;
}

//=============================== Vertex Shaders ==========================================

struct VS_INScreenSpaceVS
{
	float4 vsinput_0	: POSITION;
	float2 vsinput_1	: TEXCOORD0;
};

struct VS_OUTScreenSpaceVS
{
	float4 vsoutput_0	: POSITION0;
	float2 texUV		: TEXCOORD0;
	float3 farPlanePos	: TEXCOORD1;
};

VS_OUTScreenSpaceVS ScreenSpaceVS(VS_INScreenSpaceVS input)
{
	VS_OUTScreenSpaceVS output;

	output.vsoutput_0	= input.vsinput_0;
	output.texUV		= input.vsinput_1;
	output.farPlanePos	= GetFarPlanePosFromUV(input.vsinput_1, g_FarPlaneOffsets);
	
	return output;
}


//=============================== Pixel Shaders ==========================================

struct PS_INSSAOPS
{
	float2 texUV			: TEXCOORD0;
	float3 farPlaneOffset	: TEXCOORD1;
};

struct PS_OUTSSAOPS
{
	float4 psoutput_0 : COLOR0;
};

PS_OUTSSAOPS SSAOPS(PS_INSSAOPS input)
{
	const static float2 s_Offset[] = 
	{
		{ 0.0f,	1.0f },
		{ 1.0f, 0.0f },
		{ 0.0f, -1.0f },
		{ -1.0f, 0.0f },
		{ -0.5f, 0.5f },
		{ 0.5f, 0.5f },
		{ -0.5f, -0.5f },
		{ 0.5f, -0.5f }
		
		/*
		{ -0.997497, 0.127171 },
		{ 0.170019, -0.0402539 },
		{ -0.39201, -0.970031 },
		{ 0.911801, 0.851436 },
		{ 0.680288, -0.952513 },
		{ -0.0851161, 0.996033 },
		{ -0.926389, 0.959655 },
		{ 0.955382, 0.0366527 }
		*/
	};	
	
	const static int	NUM_ITERS				= 3;
	const static int 	NUM_SAMPLES				= 8;			// Number of samples for SSAO
	const static float	NOISE_SCALE				= 50.0f;		// Noise repeat scale                           
	const static float	UV_OFFSET_DEPTH_EPSILON = 0.0115f;		// Sample UV offset control to avoid div by 0   
	const static float	UV_OFFSET_DEPTH_SCALE	= 8.75f;		// Sample UV offset scaled with distance

	const static float	ELEMENT_SIZE				= 0.155f * NUM_ITERS;
	const static float	ELEMENT_SIZE_DEPTH_OFFSET	= 0.00f;
	const static float	NORMAL_BOOST				= 1.2f;
	
	const static float	CONTRAST_SCALE			= 2.60f;		// Occlusion constrast scale
	const static float	DEPTH_FADEOUT_SCALE		= 0.0f;		    // Fade out SSAO based on depth (to avoid aliasing artifacts)

	PS_OUTSSAOPS output;
	
	// Clamp UV
	float2 uv = input.texUV;
	//uv = RoundToNearestUV(uv, g_ZPosTexSize);
	
	float depth 		= tex2D(g_SampZPosAccum, uv).r;
	float3 worldPos		= depth * input.farPlaneOffset;	//GetWorldPos(uv, depth);

	// Derive the world normal
	float4 normalData	= tex2D(g_SampNormal, uv);	
	float3 sampNormal	= ((normalData).xyz * 2.0f - 1.0f);
	//( normalData.xyz * 2.0f - 1.0f );

	float totalOccl		= 0.0f;
	
	// Get a random noise vector to increase quality for low number of samples
	// TODO: Pre-normalize the noise texture
	float2 randomVec	= normalize( (tex2D( g_SampNoise, (uv) * NOISE_SCALE ).gb - 0.5f) * 2.0f );	
	float2 scale		= //2.0f * g_ScreenPixelOffset.xy;//max(float2(0.0019f, 0.0029f) / (depth * UV_OFFSET_DEPTH_SCALE + UV_OFFSET_DEPTH_EPSILON), 2.0f * g_ScreenPixelOffset.xy);
							max(float2(0.0019f, 0.0029f) / (depth * UV_OFFSET_DEPTH_SCALE + UV_OFFSET_DEPTH_EPSILON), 2.5f * (1.0f / g_ZPosTexSize.xy));
	float2 scaleInc		= scale;
	
	for(int j=0; j < NUM_ITERS; j++)
	{	
		for(int i=0; i < NUM_SAMPLES; i++)
		{
			// Get sample offset vector based on noise vector
			float2 rotateOffset = reflect( s_Offset[i], randomVec );

			// Clamp current UV and round to nearest pixel to avoid false occlusion
			float2 curUV 		= saturate(uv + (rotateOffset * scale));
			curUV				= RoundToNearestUV(curUV, g_ZPosTexSize);
			
			// Find the sample-to-surface vector						
			float curDepth 		= tex2D(g_SampZPosAccum, curUV).r;
			float3 curPos		= GetWorldPos(curUV, curDepth);
			float3 sampleToCur	= curPos - worldPos;
		
			//float3 curNormal	= ( tex2D(g_SampNormal, curUV).xyz * 2.0f - 1.0f );	
			
			// Attenuate based on surface similarity
			float distSq		= dot(sampleToCur, sampleToCur) + 0.00001f;
			float rcpDist       = rsqrt(distSq);	
			float3 sampleToCurNorm  = (sampleToCur) * rcpDist;
		    
		    float elementArea = (ELEMENT_SIZE + (curDepth * ELEMENT_SIZE_DEPTH_OFFSET));
			
			totalOccl += (1.0f - rsqrt(elementArea/(distSq) + 1.0f)) *
						(saturate( NORMAL_BOOST * dot(sampNormal, sampleToCurNorm) ));
				
						//(1.0f - saturate( dot(curNormal, -sampleToCurNorm) )) *						
		}
		
		scale += scaleInc;
	}

	// Find the final occlusion value using a contrast scale
	// Fade it out also with increasing depth
	float occl	= saturate(1.0f - totalOccl * CONTRAST_SCALE / (depth * DEPTH_FADEOUT_SCALE + NUM_SAMPLES * NUM_ITERS));

	// Compress z-depth for later use in blur pass
	float depth1 = depth * 255.0f;
	float depth2 = frac(depth1);
	depth1 = (depth1 - depth2) / 255.0f;
	depth2 = depth2 * 255.0f;
	float depth3 = frac(depth2);
	depth2 = (depth2 - depth3) / 255.0f;   

	output.psoutput_0 = float4(occl, depth1, depth2, depth3);

//float4(sampNormal.xyz * 0.5f + 0.5f, 1.0f);

	return output;
}


//=============================== Techniques ==========================================

technique t0
{
	pass p0
	{
		VertexShader = compile vs_3_0 ScreenSpaceVS();
		PixelShader = compile ps_3_0 SSAOPS();
	}

};

