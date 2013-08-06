
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
	const static float3 s_Offset[] = 
	{	
		normalize(float3(-1,-1,-1)),
		normalize(float3(-1, 1,-1)),
		normalize(float3(-1, 1, 1)),
		normalize(float3(-1,-1, 1)),
		normalize(float3(1,-1,-1)),
		normalize(float3(1, 1,-1)),
		normalize(float3(1, 1, 1)),
		normalize(float3(1,-1, 1)),
	};	
	
	const static int	NUM_ITERS				= 2;
	const static int 	NUM_SAMPLES				= 8;			// Number of samples for SSAO
	const static float	NOISE_SCALE				= 30.0f;		// Noise repeat scale                           
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
	float3 sampNormal	= (normalData.xyz) * 2.0f - 1.0f;
	//( normalData.xyz * 2.0f - 1.0f );

	float totalOccl		= 0.0f;
	
	// Get a random noise vector to increase quality for low number of samples
	// TODO: Pre-normalize the noise texture
	float3 randomVec	= normalize( (tex2D( g_SampNoise, (uv) * NOISE_SCALE ).rgb - 0.5f) * 2.0f );
	
	const static float SUPER_SCALE = 0.00009f;
	
	float3 scale;
	scale.xy			= max( (float2(1.0f / g_ZPosTexSize) * 0.43f / (depth * 2.5f + 0.01f)), float2(20.0f * depth / g_ZPosTexSize) );
	scale.z				= SUPER_SCALE;
	float3 scaleInc		= scale;
	
	float weightFactor	= 1.0f / ((depth * 0.05f + 0.003f) );
	float weightBias	= 1.0f + SUPER_SCALE * weightFactor;
	
	const static float FALLOFF = 1.0f / (2.0f * SUPER_SCALE);
	
	for(int j=0; j < NUM_ITERS; j++)
	{	
		for(int i=0; i < NUM_SAMPLES; i++)
		{
			// Get sample offset vector based on noise vector
			float3 rotateOffset = reflect( s_Offset[i], randomVec );	
			
			// Clamp current UV and round to nearest pixel to avoid false occlusion
			float3 ray			= rotateOffset * scale;
			
			float2 rayXY = ray.xy;
			ray.xy = max(abs(ray.xy), float2(1.0f / g_ZPosTexSize));
			if(rayXY.x < 0.0f)
				ray.x = -ray.x;
			if(rayXY.y < 0.0f)
				ray.y = -ray.y;
			
			float2 curUV 		= saturate(uv + ray.xy);
			float2 oppUV		= saturate(uv - ray.xy);
				
			//curUV				= RoundToNearestUV(curUV, g_ZPosTexSize);
			//oppUV				= RoundToNearestUV(oppUV, g_ZPosTexSize);
			
			float curDepth 		= tex2D(g_SampZPosAccum, curUV).r;
			float curOppDepth 	= tex2D(g_SampZPosAccum, oppUV).r;
			float rayDepth		= depth + ray.z; 
			float oppDepth		= depth - ray.z;
			
			float depthDiff		= curDepth - rayDepth;
			float oppDepthDiff	= curOppDepth - oppDepth;
			
			float oppOcclFactor	= saturate(0.5f + oppDepthDiff * FALLOFF);
			float oppWeight		= saturate(weightBias + oppDepthDiff * weightFactor * 1.0f); 
			float weight		= saturate(weightBias + depthDiff * weightFactor * 1.0f); 
			float occlFactor	= saturate(0.5f + depthDiff * FALLOFF); 
			
			float guess = lerp(0.5f, 1.0f - oppOcclFactor, oppWeight);

			totalOccl += lerp(guess, occlFactor, weight);
		}
		
		scale += scaleInc;
	}

	// Find the final occlusion value using a contrast scale
	// Fade it out also with increasing depth
	float occl		= totalOccl / (NUM_SAMPLES * NUM_ITERS);
	//occl			= 1.0f - exp( occl * -1.75f );
	
	//float occl	= 1.0f - saturate( totalOccl / (NUM_SAMPLES * NUM_ITERS) );
	
	
	//saturate(1.0f - totalOccl * CONTRAST_SCALE / (depth * DEPTH_FADEOUT_SCALE + NUM_SAMPLES * NUM_ITERS));

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

