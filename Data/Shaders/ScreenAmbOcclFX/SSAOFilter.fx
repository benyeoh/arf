
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

struct FarPlaneCorners
{
	float4 corners[4];
};

FarPlaneCorners g_FarPlaneCornerVecs <int semantic = 202; int usage = 0;>;
float4 g_CameraPosWithViewFar <int semantic = 15; int usage = 0;>;

//=============================== Functions ==========================================

float2 Vec4XY(float4 v4)
{
	return v4.xy;
}

float4 MergeVec2ToVec4XY(float2 v1, float4 v2)
{
	return float4(v1.xy, v2.zw);
}

float3 LerpFarPlaneCornerVecFromPos(float2 screenPos, FarPlaneCorners farCorners, float2 pixelOffset)
{
	float xLerp = ( ( screenPos.x + 1.0f + pixelOffset.x ) * 0.5f );
	float yLerp = ( ( screenPos.y + 1.0f - pixelOffset.y ) * 0.5f );
	
	float3 topLR = lerp(farCorners.corners[0], farCorners.corners[1], xLerp);
	float3 bottomLR = lerp(farCorners.corners[2], farCorners.corners[3], xLerp);
	float3 final = lerp(bottomLR, topLR, yLerp);
	
	return final;
}

float2 FixScreenSpacePos(float2 screenPos, float2 pixOffset)
{
	return screenPos + float2(-pixOffset.x, pixOffset.y);
}

float2 RoundToNearestUV(float2 uv, float2 texSize)
{
	float2 remainUV = frac(uv * texSize);
	float2 offset = remainUV - 0.5f;
	
	return uv - (offset) / texSize;
	//return uv;
}

float4 ComputeSSAO(float2 uv, float3 cornerVec, sampler sampDepth)
{
	const static int 		NUM_SAMPLES = 8;
	const static float2 	s_Offset[] = {
	
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
		
	// Clamp UV
	//uv = RoundToNearestUV(uv, g_ZPosTexSize);
		
	float depth 		= tex2D(sampDepth, uv).r;
	float3 worldPos		= depth * cornerVec + g_CameraPosWithViewFar.xyz;
	
	const static float PIXEL_STUFF = 1.0f;
	
	float depthX 	= tex2D(sampDepth, uv + float2(g_ScreenPixelOffset.x * PIXEL_STUFF, 0.0f)).r - depth;//tex2D(sampDepth, uv + float2(-g_ScreenPixelOffset.x * PIXEL_STUFF, 0.0f)).r;
	float depthY 	= tex2D(sampDepth, uv + float2(0.0f, g_ScreenPixelOffset.y * PIXEL_STUFF)).r - depth;//tex2D(sampDepth, uv + float2(0.0f, -g_ScreenPixelOffset.y * PIXEL_STUFF)).r;
	//float depthX 	= tex2D(sampDepth, uv + float2(g_ScreenPixelOffset.x * PIXEL_STUFF, 0.0f)).r - tex2D(sampDepth, uv + float2(-g_ScreenPixelOffset.x * PIXEL_STUFF, 0.0f)).r;
	//float depthY 	= tex2D(sampDepth, uv + float2(0.0f, g_ScreenPixelOffset.y * PIXEL_STUFF)).r - tex2D(sampDepth, uv + float2(0.0f, -g_ScreenPixelOffset.y * PIXEL_STUFF)).r;
	float3 uvDepth	= float3(uv.x, uv.y, depth);
	
	//float3 ddxUVDepth = ddx(uvDepth);
	//float3 ddyUVDepth = ddy(uvDepth);
	float3 ddxUVDepth = float3(g_ScreenPixelOffset.x * PIXEL_STUFF, 0.0f, depthX); //ddx(uvDepth) ;
	float3 ddyUVDepth = float3(0.0f, g_ScreenPixelOffset.y * PIXEL_STUFF, depthY); 
	//float3 ddxUVDepth = float3(g_ScreenPixelOffset.x * PIXEL_STUFF * 2.0f, 0.0f, depthX); //ddx(uvDepth) ;
	//float3 ddyUVDepth = float3(0.0f, g_ScreenPixelOffset.y * PIXEL_STUFF * 2.0f, depthY); 
	
	//float3 diffX	= ddx(worldPos);// normalize( float3(ddxUVDepth.x, 0.0f, ddxUVDepth.z) );
	//float3 diffY	= ddy(worldPos);//normalize( float3(0.0f, ddyUVDepth.y, ddyUVDepth.z) );
	float3 diffX	= ( ddxUVDepth );
	float3 diffY	= ( ddyUVDepth );
	//float3 worldNormal = normalize( cross( diffY, diffX ) );
	
	//return float4((diffY * diffY)* 1000000.0f, 1.0f);
	
	//return float4((worldNormal * 0.5f) + 0.5f, 1.0f);
	
	float3 screenNormal = normalize( cross( diffY, diffX ) );
	
	float totalOccl	= 0.0f;
	
	const static float NOISE_SCALE	= 70.0f;
	
	//float2 randomVec	= (tex2D( g_SampNoise, (worldPos.xx + worldPos.yy + worldPos.zz) * NOISE_SCALE).rg - 0.5f) * 2.0f;
	float2 randomVec	= (tex2D( g_SampNoise, (uv) * NOISE_SCALE ).rg - 0.5f) * 2.0f;
	randomVec = normalize(randomVec);
	
	const static float OFFSET_DEPTH_EPSILON = 0.001f;	// UV offset control based on distance
	
	float2 scale = //g_ScreenPixelOffset.xy * 1.641235f; 
		( g_ScreenPixelOffset.xy / (depth * 6.5f + OFFSET_DEPTH_EPSILON));
		//g_ScreenPixelOffset.xy * (1.0f + pow(1.0 - depth, 64.0f) * 20.0f );
		
	for(int i=0; i < NUM_SAMPLES; i++)
	{
		const static float NORMAL_SCALE			= 115.0f;	// Similar normals attenuation (to remove halo)
		const static float DEPTH_SCALE			= 12000.0f;
		const static float DEPTH_DIST_SQ_ATTEN	= 0.0021f;
		
		float2 rotateOffset = reflect( s_Offset[i], randomVec );
		float2 curUV 		= uv + (rotateOffset * scale);
		curUV = RoundToNearestUV(curUV, g_ZPosTexSize);
				
		float curDepth 		= tex2D(sampDepth, curUV).r;
		
		float3 diffUVDepth	= normalize( float3(curUV.x, curUV.y, curDepth) - uvDepth );
		float diff			= saturate(depth - curDepth);
		
		float2 attenNormalDepth = float2( saturate(dot(diffUVDepth, screenNormal)) * -NORMAL_SCALE, diff * -DEPTH_SCALE );
		attenNormalDepth = 1.0f - exp(attenNormalDepth);
	
		float distAtten = saturate(1.0f - (diff * diff)/(DEPTH_DIST_SQ_ATTEN * DEPTH_DIST_SQ_ATTEN));
		totalOccl += distAtten * attenNormalDepth.x * attenNormalDepth.y;		
	}

	float occl	= exp( totalOccl * -7.0f / (depth * 300.0f + NUM_SAMPLES) );
	
	//float occlAmount		=  1.0f - (totalOccl / NUM_SAMPLES) * exp(-depth * 10.0f);

	float depthCompressed = depth * 255.0f;
	float depthRemain = frac(depthCompressed);	
	return float4(occl, (depthCompressed - depthRemain) / 255.0f, depthRemain, 1.0f);
}

float4 FloatToVec4(float val)
{
	return float4(val, val, val, val);
}



//=============================== Vertex Shaders ==========================================

struct VS_INScreenSpaceVS
{
	float4 vsinput_0 : POSITION;
	float2 vsinput_1 : TEXCOORD0;
};

struct VS_OUTScreenSpaceVS
{
	float4 vsoutput_0 : POSITION0;
	float2 vsoutput_1 : TEXCOORD0;
	float3 vsoutput_2 : TEXCOORD1;
};

VS_OUTScreenSpaceVS ScreenSpaceVS(VS_INScreenSpaceVS input)
{
	VS_OUTScreenSpaceVS output;

	float2 Vec4XY_1 = Vec4XY(input.vsinput_0);

	float2 FixScreenSpacePos_4 = FixScreenSpacePos(Vec4XY_1, g_ScreenPixelOffset);

	float4 MergeVec2ToVec4XY_5 = MergeVec2ToVec4XY(FixScreenSpacePos_4, input.vsinput_0);

	float3 LerpFarPlaneCornerVecFromPos_0 = LerpFarPlaneCornerVecFromPos(FixScreenSpacePos_4, g_FarPlaneCornerVecs, g_ScreenPixelOffset);


	output.vsoutput_0 = MergeVec2ToVec4XY_5;
	output.vsoutput_1 = input.vsinput_1;
	output.vsoutput_2 = LerpFarPlaneCornerVecFromPos_0;
	return output;
}


//=============================== Pixel Shaders ==========================================

struct PS_INSSAOPS
{
	float2 psinput_0 : TEXCOORD0;
	float3 cornerVec : TEXCOORD1;
};

struct PS_OUTSSAOPS
{
	float4 psoutput_0 : COLOR0;
};

PS_OUTSSAOPS SSAOPS(PS_INSSAOPS input)
{
	PS_OUTSSAOPS output;

	float4 ComputeSSAO_0 = ComputeSSAO(input.psinput_0, input.cornerVec, g_SampZPosAccum);

	output.psoutput_0 = ComputeSSAO_0;
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

