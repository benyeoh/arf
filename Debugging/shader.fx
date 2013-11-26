//=============================== Locals ==========================================

struct BakedSMAreaLight
{
	float4 posAndRadius;
	float4 lightColor;
};
struct SHCoeff4
{
	float4 coeffs[4];
};


//=============================== Globals ==========================================

int VertexDesc2_Binormal0 <int semantic = 138; int usage = 1; bool always_used = true; bool isInit = true;> = 0x00008000;
int VertexDesc1_Tangent0 <int semantic = 137; int usage = 1; bool always_used = true; bool isInit = true;> = 0x00010000;
float4x4 g_World <int semantic = 0; int usage = 0;>;
float4x4 g_ViewProj <int semantic = 3; int usage = 0;>;
float4 g_CameraPosWithViewFar <int semantic = 15; int usage = 0;>;
BakedSMAreaLight g_BakedSMAreaLight1 <int semantic = 2054; int usage = 0;>;
float g_BakedSMFarPlane <int semantic = 2049; int usage = 0;>;
texture g_VisSphereTableTex <int semantic = 2180; int usage = 1;>;
sampler g_SampVisSphereTable = sampler_state
{
    	Texture = <g_VisSphereTableTex>;
    	MipFilter = LINEAR;
    	MinFilter = LINEAR;
    	MagFilter = LINEAR;
	//MaxAnisotropy = 8;
    	ADDRESSU  = CLAMP;
    	ADDRESSV  = CLAMP;
};
texture g_BakedSMTex1 <int semantic = 2176; int usage = 1;>;
sampler g_SampBakedSM1 = sampler_state
{
    	Texture = <g_BakedSMTex1>;
    	MipFilter = LINEAR;
    	MinFilter = LINEAR;
    	MagFilter = LINEAR;
	//MaxAnisotropy = 8;
    	ADDRESSU  = CLAMP;
    	ADDRESSV  = CLAMP;
};
texture g_BakedSMTex2 <int semantic = 2177; int usage = 1;>;
sampler g_SampBakedSM2 = sampler_state
{
    	Texture = <g_BakedSMTex2>;
    	MipFilter = LINEAR;
    	MinFilter = LINEAR;
    	MagFilter = LINEAR;
	//MaxAnisotropy = 8;
    	ADDRESSU  = CLAMP;
    	ADDRESSV  = CLAMP;
};
texture g_BakedSMTex3 <int semantic = 2178; int usage = 1;>;
sampler g_SampBakedSM3 = sampler_state
{
    	Texture = <g_BakedSMTex3>;
    	MipFilter = LINEAR;
    	MinFilter = LINEAR;
    	MagFilter = LINEAR;
	//MaxAnisotropy = 8;
    	ADDRESSU  = CLAMP;
    	ADDRESSV  = CLAMP;
};
texture g_BakedSMTex4 <int semantic = 2179; int usage = 1;>;
sampler g_SampBakedSM4 = sampler_state
{
    	Texture = <g_BakedSMTex4>;
    	MipFilter = LINEAR;
    	MinFilter = LINEAR;
    	MagFilter = LINEAR;
	//MaxAnisotropy = 8;
    	ADDRESSU  = CLAMP;
    	ADDRESSV  = CLAMP;
};
float g_BakedSMExp <int semantic = 2181; int usage = 1;>;
BakedSMAreaLight g_BakedSMAreaLight2 <int semantic = 2055; int usage = 0;>;
BakedSMAreaLight g_BakedSMAreaLight3 <int semantic = 2056; int usage = 0;>;
BakedSMAreaLight g_BakedSMAreaLight4 <int semantic = 2057; int usage = 0;>;
BakedSMAreaLight g_BakedSMAreaLight5 <int semantic = 2058; int usage = 0;>;
BakedSMAreaLight g_BakedSMAreaLight6 <int semantic = 2059; int usage = 0;>;
BakedSMAreaLight g_BakedSMAreaLight7 <int semantic = 2060; int usage = 0;>;
BakedSMAreaLight g_BakedSMAreaLight8 <int semantic = 2061; int usage = 0;>;


//=============================== Functions ==========================================

float4 VecMatMult4(float4 vec4, float4x4 mat44)
{
	return mul(vec4, mat44);
}

float3 Vec4XYZ(float4 vec4)
{
	return vec4.xyz;
}

float3 Negate3(float3 val)
{
	return -val;
}

float3 VecVecAdd3(float3 v1, float3 v2)
{
	return v1 + v2;
}

float Vec4W(float4 v4)
{
	return v4.w;
}

float4 MergeVec3Float(float3 v3, float val)
{
	return float4(v3, val);
}

float3x3 CastMat44ToMat33(float4x4 mat44)
{
	return (float3x3)mat44;
}

float3 VecMatMult3(float3 vec3, float3x3 mat33)
{
	return mul(vec3, mat33);
}

float3 Normalize3(float3 vec3)
{
	return normalize(vec3);
}

float3 VecVecSub3(float3 v1, float3 v2)
{
	return v1 - v2;
}

float4 Tex2D(sampler samp, float2 uv)
{
	return tex2D(samp, uv);
}

SHCoeff4 InitSHCoeff4(float4 coeff0, float4 coeff1, float4 coeff2, float4 coeff3)
{
	const float BIAS_DATA[16]   = { 0.0f, 
	                               -2.0f, -2.0f, -2.0f,
				       -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 
	                               -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, - 1.0f };
	                                                                   
	const float SCALE_DATA[16]  = { 2.0f,
	                                4.0f, 4.0f, 4.0f, 
	                                2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 
	                                2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f };
	
	SHCoeff4 sh;
	sh.coeffs[0] = coeff0 * float4(SCALE_DATA[0], SCALE_DATA[1], SCALE_DATA[2], SCALE_DATA[3])
		              + float4(BIAS_DATA[0], BIAS_DATA[1], BIAS_DATA[2], BIAS_DATA[3]);
	
	sh.coeffs[1] = coeff1 * float4(SCALE_DATA[4], SCALE_DATA[5], SCALE_DATA[6], SCALE_DATA[7])
			      + float4(BIAS_DATA[4], BIAS_DATA[5], BIAS_DATA[6], BIAS_DATA[7]);
	
	sh.coeffs[2] = coeff2 * float4(SCALE_DATA[8], SCALE_DATA[9], SCALE_DATA[10], SCALE_DATA[11])
	                      + float4(BIAS_DATA[8], BIAS_DATA[9], BIAS_DATA[10], BIAS_DATA[11]);
	
	sh.coeffs[3] = coeff3 * float4(SCALE_DATA[12], SCALE_DATA[13], SCALE_DATA[14], SCALE_DATA[15])
	                      + float4(BIAS_DATA[12], BIAS_DATA[13], BIAS_DATA[14], BIAS_DATA[15]);
	return sh;
}

SHCoeff4 MultScalarSHCoeff4(SHCoeff4 shCoeff, float expK)
{
	SHCoeff4 res;
	
	res.coeffs[0] = shCoeff.coeffs[0] * expK;
	res.coeffs[1] = shCoeff.coeffs[1] * expK;
	res.coeffs[2] = shCoeff.coeffs[2] * expK;
	res.coeffs[3] = shCoeff.coeffs[3] * expK;
	
	return res;
}

SHCoeff4 WindowSHCoeff4(SHCoeff4 orig)
{
	// Compute SH exponential depth
	const static float WINDOW_SIZE = 2.5f;
	const static float WINDOW_1 = (1.0 + cos( (3.14159265358979 * 1.0/4.0) / (max(1.0/4.0, WINDOW_SIZE)) )) * 0.5; //0.95;
	const static float WINDOW_2 = (1.0 + cos( (3.14159265358979 * 2.0/4.0) / (max(2.0/4.0, WINDOW_SIZE)) )) * 0.5; //0.82;
	const static float WINDOW_3 = (1.0 + cos( (3.14159265358979 * 3.0/4.0) / (max(3.0/4.0, WINDOW_SIZE)) )) * 0.5; //0.62;
	//const static float WINDOW_1 = 0.95;
	//const static float WINDOW_2 = 0.82;
	//const static float WINDOW_3 = 0.62;
	const static float WINDOW_1_SQ = WINDOW_1 * WINDOW_1;
	const static float WINDOW_2_SQ = WINDOW_2 * WINDOW_2;
	const static float WINDOW_3_SQ = WINDOW_3 * WINDOW_3;
	        
	SHCoeff4 toReturn;
	toReturn.coeffs[0] = orig.coeffs[0] * float4(1.0, WINDOW_1_SQ, WINDOW_1_SQ, WINDOW_1_SQ);
	toReturn.coeffs[1] = orig.coeffs[1] * float4(WINDOW_2_SQ, WINDOW_2_SQ, WINDOW_2_SQ, WINDOW_2_SQ);
	toReturn.coeffs[2] = orig.coeffs[2] * float4(WINDOW_2_SQ, WINDOW_3_SQ, WINDOW_3_SQ, WINDOW_3_SQ);
	toReturn.coeffs[3] = orig.coeffs[3] * float4(WINDOW_3_SQ, WINDOW_3_SQ, WINDOW_3_SQ, WINDOW_3_SQ);
	
	return toReturn;
}

float3 ComputeAreaLightBakedSM4(BakedSMAreaLight areaLight, float3 normal, float3 worldPos, float farPlane, sampler sampVisSphereTable, SHCoeff4 shadowCoeff)
{
	float3 lightPos = areaLight.posAndRadius.xyz;
	float lightRadius = areaLight.posAndRadius.w;
	    
	float3 posToLight = lightPos - worldPos;
	  
	// Compute new sphere on tangent plane
	float projDist = ( dot(normal, posToLight) );
	float newLightRadius = 0.5 * (lightRadius + projDist);
	
	float3 colorOut = 0.0;
	
	[branch]
	if(newLightRadius > 0.0)
	{
		float tangentOffsetAmount = max(0.0, 0.5 * (lightRadius - projDist));
		float3 newPosToLight = posToLight + normal * tangentOffsetAmount;
	
		float newPosToLightDistRcp = rsqrt( dot(newPosToLight, newPosToLight) );
	        float radiusOverDist = lightRadius * newPosToLightDistRcp;
	        float newToOldRadiusRatio = saturate(newLightRadius / lightRadius);
	        
	        radiusOverDist = max(0.1, radiusOverDist) * newToOldRadiusRatio;
	        
	        float3 posToLightDir = newPosToLight.xyz * newPosToLightDistRcp;
	        float normDist = rcp(farPlane * newPosToLightDistRcp);
	        
	        // Compute visibility sphere
	        float4 sphereZH4 = tex2Dlod(sampVisSphereTable, float4(radiusOverDist, normDist, 0, 0));
	        
	        // Expand to SH
	        const static float4 y1 = float4(0.28209479177387814, -0.4886025119029199, 0.4886025119029199, -0.4886025119029199);
	        const static float4 y2 = float4(1.092548430592079, -1.092548430592079, 0.3153915652525200, -1.092548430592079);
	        const static float4 y3 = float4(0.5462742152960395, -0.590043589926643, 2.89061144264055405, -0.45704579946446573);
	        const static float4 y4 = float4(0.37317630648613, -0.45704579946446573, 1.44530572132027702, -0.590043589926643);
	        
	        const static float4 gTerm = float4(3.544907701811, 2.04665341589297, 1.585330919042, 1.339849171381);
	        
	        float x = posToLightDir.x;
	        float y = -posToLightDir.z;
	        float z = posToLightDir.y;
	        
	        float4 sphereZH4Trans = sphereZH4 * gTerm;
	
		SHCoeff4 depthCoeff;
	        depthCoeff.coeffs[0] = float4(1.0, y, z, x) * y1 * sphereZH4Trans.xyyy;
	        
	        float zSq = z * z;
	        depthCoeff.coeffs[1] = float4((y * x), 
	                                (y * z), 
	                                (3.0 * zSq - 1.0), 
	                                (x * z)) * y2 * sphereZH4Trans.zzzz;
	
	        float xSq = x * x;                                
	        float ySq = y * y;                             
	        depthCoeff.coeffs[2] = float4((xSq - ySq), 
	                                (y * (3.0 * xSq - ySq)), 
	                                (y * x * z), 
	                                (y * (-1.0 + 5.0 * zSq))) * y3 * sphereZH4Trans.zwww;
	        
	        depthCoeff.coeffs[3] = float4(z * (5.0 * zSq - 3.0), 
	                                (x * (-1.0 + 5.0 * zSq)), 
	                                (z * (xSq - ySq)), 
	                                (x * (xSq - 3.0 * ySq))) * y4 * sphereZH4Trans.wwww;
	
	
		// Compute SH exponential depth
	        float res = //shadowCoeff.coeffs[0].x;
	                //dot(shadowCoeff.coeffs[0], depthCoeff.coeffs[0]);
	        /*
	                dot(shadowCoeff.coeffs[0], depthCoeff.coeffs[0]) + 
	                dot(shadowCoeff.coeffs[1], depthCoeff.coeffs[1]) +
	                shadowCoeff.coeffs[2].x * depthCoeff.coeffs[2].x;
	        */
	        
	                dot(shadowCoeff.coeffs[0], depthCoeff.coeffs[0]) + 
	                dot(shadowCoeff.coeffs[1], depthCoeff.coeffs[1]) +
	                dot(shadowCoeff.coeffs[2], depthCoeff.coeffs[2]) +
	                dot(shadowCoeff.coeffs[3], depthCoeff.coeffs[3]);
	        
	        //float scaleFactor = ((2.0 * 3.141592653589793238) / (radiusOverDist * (1.0 - 0) + 0.0001));
	        //float scaleFactor = 1.0 / ( shadowCoeff.coeffs[0].x * depthCoeff.coeffs[0].x );
	        //float scaleFactor = ((2.0 * 3.141592653589793238) / (asin(radiusOverDist) * 4.0 + 0.0001));
	        
	        //float scaleFactor = (1.0 / ( 2.0 * 3.141592653589793238 * (1.0 - cos(asin(radiusOverDist)))) );
	        float scaleFactor = 22.0 / (2.0 * 3.141592653589793238);
	        
	        float final = res * scaleFactor;
		colorOut = final * areaLight.lightColor.xyz;
	}
	
	return colorOut;
}

float3 TonemapColor(float3 color)
{
	return float3(1.0, 1.0, 1.0) - exp(-color.xyz);
}

float GetUnitFloat()
{
	return 1.0f;
}

float4 IsExistVec4Vec4(float4 vecToCheck, float4 vec)
{
	return vec;
}



//=============================== Vertex Shaders ==========================================

struct VS_INObjectVS
{
	float4 vsinput_0 : POSITION;
	float2 vsinput_1 : TEXCOORD0;
	float3 vsinput_2 : TANGENT;
	float3 vsinput_3 : BINORMAL;
	float3 vsinput_4 : NORMAL;
};

struct VS_OUTObjectVS
{
	float4 vsoutput_0 : POSITION0;
	float2 vsoutput_1 : TEXCOORD0;
	float4 vsoutput_2 : TEXCOORD1;
	float3 vsoutput_3 : TEXCOORD3;
	float3 vsoutput_4 : TEXCOORD4;
	float3 vsoutput_5 : TEXCOORD2;
	float3 vsoutput_6 : TEXCOORD5;
};

VS_OUTObjectVS ObjectVS(VS_INObjectVS input)
{
	VS_OUTObjectVS output;

	float4 VecMatMult4_9__fn = VecMatMult4(input.vsinput_0, g_World);

	float4 VecMatMult4_10__fn = VecMatMult4(VecMatMult4_9__fn, g_ViewProj);

	float3 Vec4XYZ_12__fn = Vec4XYZ(VecMatMult4_9__fn);

	float3 Negate3_11__fn = Negate3(Vec4XYZ_12__fn);

	float3 Vec4XYZ_16__fn = Vec4XYZ(g_CameraPosWithViewFar);

	float3 VecVecAdd3_13__fn = VecVecAdd3(Negate3_11__fn, Vec4XYZ_16__fn);

	float Vec4W_14__fn = Vec4W(g_CameraPosWithViewFar);

	float4 MergeVec3Float_15__fn = MergeVec3Float(VecVecAdd3_13__fn, Vec4W_14__fn);

	float3x3 CastMat44ToMat33_3__fn = CastMat44ToMat33(g_World);

	float3 VecMatMult3_4__fn = VecMatMult3(input.vsinput_2, CastMat44ToMat33_3__fn);

	float3 Normalize3_5__fn = Normalize3(VecMatMult3_4__fn);

	float3x3 CastMat44ToMat33_6__fn = CastMat44ToMat33(g_World);

	float3 VecMatMult3_7__fn = VecMatMult3(input.vsinput_3, CastMat44ToMat33_6__fn);

	float3 Normalize3_8__fn = Normalize3(VecMatMult3_7__fn);

	float3x3 CastMat44ToMat33_0__fn = CastMat44ToMat33(g_World);

	float3 VecMatMult3_1__fn = VecMatMult3(input.vsinput_4, CastMat44ToMat33_0__fn);

	float3 Normalize3_2__fn = Normalize3(VecMatMult3_1__fn);


	output.vsoutput_0 = VecMatMult4_10__fn;
	output.vsoutput_1 = input.vsinput_1;
	output.vsoutput_2 = MergeVec3Float_15__fn;
	output.vsoutput_3 = Normalize3_5__fn;
	output.vsoutput_4 = Normalize3_8__fn;
	output.vsoutput_5 = Normalize3_2__fn;
	output.vsoutput_6 = Vec4XYZ_12__fn;
	return output;
}


//=============================== Pixel Shaders ==========================================

struct PS_INTestBakedSMSHPS
{
	float3 psinput_0 : TEXCOORD2;
	float4 psinput_1 : TEXCOORD1;
	float2 psinput_2 : TEXCOORD0;
};

struct PS_OUTTestBakedSMSHPS
{
	float4 psoutput_0 : COLOR0;
};

PS_OUTTestBakedSMSHPS TestBakedSMSHPS(PS_INTestBakedSMSHPS input)
{
	PS_OUTTestBakedSMSHPS output;

	float3 Normalize3_42__fn = Normalize3(input.psinput_0);

	float3 Vec4XYZ_35__fn = Vec4XYZ(g_CameraPosWithViewFar);

	float3 Vec4XYZ_34__fn = Vec4XYZ(input.psinput_1);

	float3 VecVecSub3_43__fn = VecVecSub3(Vec4XYZ_35__fn, Vec4XYZ_34__fn);

	float4 Tex2D_15__fn = Tex2D(g_SampBakedSM1, input.psinput_2);

	float4 Tex2D_31__fn = Tex2D(g_SampBakedSM2, input.psinput_2);

	float4 Tex2D_30__fn = Tex2D(g_SampBakedSM3, input.psinput_2);

	float4 Tex2D_29__fn = Tex2D(g_SampBakedSM4, input.psinput_2);

	SHCoeff4 InitSHCoeff4_16__fn = InitSHCoeff4(Tex2D_15__fn, Tex2D_31__fn, Tex2D_30__fn, Tex2D_29__fn);

	SHCoeff4 MultScalarSHCoeff4_32__fn = MultScalarSHCoeff4(InitSHCoeff4_16__fn, g_BakedSMExp);

	SHCoeff4 WindowSHCoeff4_46__fn = WindowSHCoeff4(MultScalarSHCoeff4_32__fn);

	float3 ComputeAreaLightBakedSM4_0__fn = ComputeAreaLightBakedSM4(g_BakedSMAreaLight1, Normalize3_42__fn, VecVecSub3_43__fn, g_BakedSMFarPlane, g_SampVisSphereTable, WindowSHCoeff4_46__fn);

	float3 ComputeAreaLightBakedSM4_1__fn = ComputeAreaLightBakedSM4(g_BakedSMAreaLight2, Normalize3_42__fn, VecVecSub3_43__fn, g_BakedSMFarPlane, g_SampVisSphereTable, WindowSHCoeff4_46__fn);

	float3 VecVecAdd3_8__fn = VecVecAdd3(ComputeAreaLightBakedSM4_0__fn, ComputeAreaLightBakedSM4_1__fn);

	float3 ComputeAreaLightBakedSM4_2__fn = ComputeAreaLightBakedSM4(g_BakedSMAreaLight3, Normalize3_42__fn, VecVecSub3_43__fn, g_BakedSMFarPlane, g_SampVisSphereTable, WindowSHCoeff4_46__fn);

	float3 ComputeAreaLightBakedSM4_3__fn = ComputeAreaLightBakedSM4(g_BakedSMAreaLight4, Normalize3_42__fn, VecVecSub3_43__fn, g_BakedSMFarPlane, g_SampVisSphereTable, WindowSHCoeff4_46__fn);

	float3 VecVecAdd3_9__fn = VecVecAdd3(ComputeAreaLightBakedSM4_2__fn, ComputeAreaLightBakedSM4_3__fn);

	float3 VecVecAdd3_12__fn = VecVecAdd3(VecVecAdd3_8__fn, VecVecAdd3_9__fn);

	float3 ComputeAreaLightBakedSM4_4__fn = ComputeAreaLightBakedSM4(g_BakedSMAreaLight5, Normalize3_42__fn, VecVecSub3_43__fn, g_BakedSMFarPlane, g_SampVisSphereTable, WindowSHCoeff4_46__fn);

	float3 ComputeAreaLightBakedSM4_5__fn = ComputeAreaLightBakedSM4(g_BakedSMAreaLight6, Normalize3_42__fn, VecVecSub3_43__fn, g_BakedSMFarPlane, g_SampVisSphereTable, WindowSHCoeff4_46__fn);

	float3 VecVecAdd3_10__fn = VecVecAdd3(ComputeAreaLightBakedSM4_4__fn, ComputeAreaLightBakedSM4_5__fn);

	float3 ComputeAreaLightBakedSM4_6__fn = ComputeAreaLightBakedSM4(g_BakedSMAreaLight7, Normalize3_42__fn, VecVecSub3_43__fn, g_BakedSMFarPlane, g_SampVisSphereTable, WindowSHCoeff4_46__fn);

	float3 ComputeAreaLightBakedSM4_7__fn = ComputeAreaLightBakedSM4(g_BakedSMAreaLight8, Normalize3_42__fn, VecVecSub3_43__fn, g_BakedSMFarPlane, g_SampVisSphereTable, WindowSHCoeff4_46__fn);

	float3 VecVecAdd3_11__fn = VecVecAdd3(ComputeAreaLightBakedSM4_6__fn, ComputeAreaLightBakedSM4_7__fn);

	float3 VecVecAdd3_13__fn = VecVecAdd3(VecVecAdd3_10__fn, VecVecAdd3_11__fn);

	float3 VecVecAdd3_14__fn = VecVecAdd3(VecVecAdd3_12__fn, VecVecAdd3_13__fn);

	float3 TonemapColor_47__fn = TonemapColor(VecVecAdd3_14__fn);

	float GetUnitFloat_45__fn = GetUnitFloat();

	float4 MergeVec3Float_44__fn = MergeVec3Float(TonemapColor_47__fn, GetUnitFloat_45__fn);

	float4 IsExistVec4Vec4_21__fn = IsExistVec4Vec4(MergeVec3Float_44__fn, MergeVec3Float_44__fn);


	output.psoutput_0 = IsExistVec4Vec4_21__fn;
	return output;
}


//=============================== Techniques ==========================================

technique defaultTech
{
	pass defaultPass
	{
		VertexShader = compile vs_3_0 ObjectVS();
		PixelShader = compile ps_3_0 TestBakedSMSHPS();
	}

};

