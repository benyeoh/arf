//==============================================================================
//
//		BlurAODepth.fxh
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		10/1/2007
//
//==============================================================================

float2 g_FilterTextureOffsets[11]	< int semantic = 4267; 
									int usage = 1; 
									string name = "Filter Texture Offsets"; >;
															
float g_FilterWeights[11]			< int semantic = 4268; 
									int usage = 1; 
									string name = "Filter Weights"; >;

texture		g_TexAO < int semantic = 4265; 
						int usage = 1; 
						string name = "AO"; >;

sampler		g_SampAO = sampler_state
{
    Texture = (g_TexAO);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = CLAMP;
    ADDRESSV  = CLAMP;
};

/*sampler		g_SampAODepth = sampler_state
{
    Texture = (g_TexAO);
    MipFilter = POINT;
    MinFilter = POINT;
    MagFilter = POINT;
    ADDRESSU  = CLAMP;
    ADDRESSV  = CLAMP;
};

texture		g_TexDepth < int semantic = 4266; 
						int usage = 1; >;

sampler		g_SampDepth = sampler_state
{
    Texture = (g_TexDepth);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = CLAMP;
    ADDRESSV  = CLAMP;
};
*/

texture		g_TexNormal < int semantic = 4273; 
						int usage = 1; >;

sampler		g_SampNormal = sampler_state
{
    Texture = (g_TexNormal);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = CLAMP;
    ADDRESSV  = CLAMP;
};

float2 g_ScreenPixelOffset <int semantic = 4270; int usage = 1; >;
float4 g_CameraPosWithViewFar <int semantic = 15; int usage = 0;>;

struct FarPlaneOffsets
{
	float4 offsets[3];
};

FarPlaneOffsets g_FarPlaneOffsets <int semantic = 205; int usage = 0;>;


struct VS_OUT_BlurAODepth
{
	float4 pos			: POSITION0;
	float4 texUVTap0	: TEXCOORD0;
	
	float4 farPlaneOffset0	: TEXCOORD1;
	float4 farPlaneOffset1	: TEXCOORD2;
	float4 farPlaneOffset2	: TEXCOORD3;
	float4 farPlaneOffset3	: TEXCOORD4;
	float4 farPlaneOffset4	: TEXCOORD5;
	float4 farPlaneOffset5	: TEXCOORD6;
	float4 farPlaneOffset6	: TEXCOORD7;
	float4 farPlaneOffset7	: TEXCOORD8;
	float4 farPlaneOffset8	: TEXCOORD9;
};

/*
struct VS_OUT_BlurAODepth
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
*/

struct VS_IN_2D
{
	float4 pos		: POSITION;
	float2 texUV	: TEXCOORD0;
};

struct PS_OUT_Color
{
	float4 color : COLOR0;
};

float UncompressDepth(float3 depthCompressed)
{
	return dot(depthCompressed, float3(1.0f, 1.0f / 255.0f, 1.0f / (255.0f * 255.0f)));
	
//	return depthCompressed.x + ( depthCompressed.y / 255.0f ) + (depthCompressed.z / (255.0f * 255.0f));
}

float3 GetFarPlanePosFromUV(float2 uv, FarPlaneOffsets planeOffsets)
{
	return planeOffsets.offsets[0].xyz + uv.x * planeOffsets.offsets[1].xyz + uv.y * planeOffsets.offsets[2].xyz;
}

float3 GetWorldPos(float2 uv, float depth)
{
	return depth * GetFarPlanePosFromUV(uv, g_FarPlaneOffsets);// + g_CameraPosWithViewFar.xyz;
}

float FindProjection(float3 diff, float3 normal)
{
	// Finds the similarity of the sample with the current surface
	// Basically does a distance and normal check
	const static float SIMILAR_SURFACE_CUTOFF = 0.01f;
	 
	float proj = dot((diff), normal);
	return (1.0f - saturate( (proj * proj) / (SIMILAR_SURFACE_CUTOFF * SIMILAR_SURFACE_CUTOFF) ));
}

float3 GetWorldPosFromVert(float3 offset, float depth)
{
	return depth * offset;
}

VS_OUT_BlurAODepth
BlurAODepthVS(VS_IN_2D input)
{
	VS_OUT_BlurAODepth output = (VS_OUT_BlurAODepth) 0;
		
	// NOTE: Assumes either position or texUV has been offset appropriately
	output.pos = float4(input.pos.xy, 0.0f, 1.0f);
	
	output.texUVTap0.xy = input.texUV + g_FilterTextureOffsets[0];	
	output.texUVTap0.zw = input.texUV + g_FilterTextureOffsets[1];	

	float4 texUVTap1;
	texUVTap1.xy = input.texUV + g_FilterTextureOffsets[2];
	texUVTap1.zw = input.texUV + g_FilterTextureOffsets[3];
	
	float4 texUVTap2;
	texUVTap2.xy = input.texUV + g_FilterTextureOffsets[4];
	texUVTap2.zw = input.texUV + g_FilterTextureOffsets[5];

	float4 texUVTap3;
	texUVTap3.xy = input.texUV + g_FilterTextureOffsets[6];
	texUVTap3.zw = input.texUV + g_FilterTextureOffsets[7];
	
	float4 texUVTap4;
	texUVTap4.xy = input.texUV + g_FilterTextureOffsets[8];
	texUVTap4.zw = input.texUV + g_FilterTextureOffsets[9];
	
	float4 texUVTap5;
	texUVTap5.xy = input.texUV + g_FilterTextureOffsets[10];
	//output.texUVTap5.zw = input.texUV + g_FilterTextureOffsets[11];

	float3 res;
	res = GetFarPlanePosFromUV(output.texUVTap0.xy, g_FarPlaneOffsets);	// 0
	output.farPlaneOffset0.xyz	= res.xyz;
	res = GetFarPlanePosFromUV(output.texUVTap0.zw, g_FarPlaneOffsets); // 1
	output.farPlaneOffset0.w = res.x;
	output.farPlaneOffset1.xy = res.yz;
	res = GetFarPlanePosFromUV(texUVTap1.xy, g_FarPlaneOffsets); // 2
	output.farPlaneOffset1.zw = res.xy;
	output.farPlaneOffset2.x = res.z;
	res = GetFarPlanePosFromUV(texUVTap1.zw, g_FarPlaneOffsets); // 3
	output.farPlaneOffset2.yzw = res.xyz;
	res = GetFarPlanePosFromUV(texUVTap2.xy, g_FarPlaneOffsets); // 4
	output.farPlaneOffset3.xyz = res.xyz;
	res = GetFarPlanePosFromUV(texUVTap2.zw, g_FarPlaneOffsets); // 5
	output.farPlaneOffset3.w = res.x;
	output.farPlaneOffset4.xy = res.yz;
	res = GetFarPlanePosFromUV(texUVTap3.xy, g_FarPlaneOffsets); // 6
	output.farPlaneOffset4.zw = res.xy;
	output.farPlaneOffset5.x = res.z;
	res = GetFarPlanePosFromUV(texUVTap3.zw, g_FarPlaneOffsets); // 7
	output.farPlaneOffset5.yzw = res.xyz;
	res = GetFarPlanePosFromUV(texUVTap4.xy, g_FarPlaneOffsets); // 8
	output.farPlaneOffset6.xyz = res.xyz;
	res = GetFarPlanePosFromUV(texUVTap4.zw, g_FarPlaneOffsets); // 9
	output.farPlaneOffset6.w = res.x;
	output.farPlaneOffset7.xy = res.yz;
	res = GetFarPlanePosFromUV(texUVTap5.xy, g_FarPlaneOffsets); // 10
	output.farPlaneOffset7.zw = res.xy;
	output.farPlaneOffset8.x = res.z;
	
	return output;
}

#define DoInit \
	float curWeight; \
	float4 center		= tex2D(g_SampAO, input.texUVTap0.xy); \
	float curDepth		= UncompressDepth(center.yzw); \
	float occl			= center.x; \
	float	zDiffMaxDist = 0.0001f + 0.00275f * curDepth; \
	float	zFactor = 1.f / zDiffMaxDist;


/*
	texUVOffset.xy = input.texUVTap0.xy + float2(g_ScreenPixelOffset.x, 0.0f); \
	texUVOffset.zw = input.texUVTap0.xy + float2(0.0f, g_ScreenPixelOffset.y); \
	float4 xData		= tex2D(g_SampAO, texUVOffset.xy); \
	float4 yData		= tex2D(g_SampAO, texUVOffset.zw); \
	float3 diffX		= GetWorldPos(texUVOffset.xy, UncompressDepth(xData.yzw)) - worldPos; \
	float3 diffY		= GetWorldPos(texUVOffset.zw, UncompressDepth(yData.yzw)) - worldPos; \
	float3 normal		= normalize( cross(diffY, diffX) );

	float3 normal		= tex2D(g_SampNormal, input.texUVTap0.xy).xyz;
*/

/*
	float curWeight; \
	float4 center = tex2D(g_SampAO, input.texUV); \
	float curDepth		= UncompressDepth(center.yzw); \
	float occl			= center.x * CENTER_WEIGHT; \
	float weightTotal	= CENTER_WEIGHT; \
*/	
	
#define DoBlurSample( i, inUV, offset ) \
	float4 tap##i			= tex2D(g_SampAO, inUV); \
	float curDepth##i		= UncompressDepth(tap##i.yzw); \
	float tap##i##Diff		= abs(curDepth##i - curDepth); \
	float weightScale##i	= saturate(tap##i##Diff * zFactor); \
	occl					+= lerp(tap##i.x, center.x, weightScale##i);
	
	
PS_OUT_Color
BlurAODepth7SamplesPS(VS_OUT_BlurAODepth input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
		
	DoInit
		
	DoBlurSample( 1, input.texUVTap0.zw, float3(input.farPlaneOffset0.w, input.farPlaneOffset1.xy) )
	DoBlurSample( 2, input.texUVTap0.xy + g_FilterTextureOffsets[2], float3(input.farPlaneOffset1.zw, input.farPlaneOffset2.x)  )
	DoBlurSample( 3, input.texUVTap0.xy + g_FilterTextureOffsets[3], float3(input.farPlaneOffset2.yzw)  )
	DoBlurSample( 4, input.texUVTap0.xy + g_FilterTextureOffsets[4], float3(input.farPlaneOffset3.xyz)  )
	DoBlurSample( 5, input.texUVTap0.xy + g_FilterTextureOffsets[5], float3(input.farPlaneOffset3.w, input.farPlaneOffset4.xy)  )
	DoBlurSample( 6, input.texUVTap0.xy + g_FilterTextureOffsets[6], float3(input.farPlaneOffset4.zw, input.farPlaneOffset5.x)  )
	
	output.color	= float4(occl / 7, center.y, center.z, center.w);
	
	return output;
}

PS_OUT_Color
BlurAODepth11SamplesPS(VS_OUT_BlurAODepth input)
{
	PS_OUT_Color output = (PS_OUT_Color) 0;
		
	DoInit
		
	DoBlurSample( 1, input.texUVTap0.zw, float3(input.farPlaneOffset0.w, input.farPlaneOffset1.xy) )
	DoBlurSample( 2, input.texUVTap0.xy + g_FilterTextureOffsets[2], float3(input.farPlaneOffset1.zw, input.farPlaneOffset2.x)  )
	DoBlurSample( 3, input.texUVTap0.xy + g_FilterTextureOffsets[3], float3(input.farPlaneOffset2.yzw)  )
	DoBlurSample( 4, input.texUVTap0.xy + g_FilterTextureOffsets[4], float3(input.farPlaneOffset3.xyz)  )
	DoBlurSample( 5, input.texUVTap0.xy + g_FilterTextureOffsets[5], float3(input.farPlaneOffset3.w, input.farPlaneOffset4.xy)  )
	DoBlurSample( 6, input.texUVTap0.xy + g_FilterTextureOffsets[6], float3(input.farPlaneOffset4.zw, input.farPlaneOffset5.x)  )
	DoBlurSample( 7, input.texUVTap0.xy + g_FilterTextureOffsets[7], float3(input.farPlaneOffset5.yzw) )
	DoBlurSample( 8, input.texUVTap0.xy + g_FilterTextureOffsets[8], float3(input.farPlaneOffset6.xyz) )
	DoBlurSample( 9, input.texUVTap0.xy + g_FilterTextureOffsets[9], float3(input.farPlaneOffset6.w, input.farPlaneOffset7.xy) )
	DoBlurSample( 10, input.texUVTap0.xy + g_FilterTextureOffsets[10], float3(input.farPlaneOffset7.zw, input.farPlaneOffset8.x) )
	
	output.color	= float4(occl / 11, center.y, center.z, center.w);
	
	return output;
}

technique t0
{
	pass p0
	{
		VertexShader	=	compile vs_3_0 BlurAODepthVS();
		PixelShader		=	compile ps_3_0 BlurAODepth7SamplesPS();
	}
};

technique t1
{
	pass p0
	{
		VertexShader	=	compile vs_3_0 BlurAODepthVS();
		PixelShader		=	compile ps_3_0 BlurAODepth11SamplesPS();
	}
};
