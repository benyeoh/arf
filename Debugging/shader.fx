//=============================== Locals ==========================================

struct DEFXPointLight
{
	float3 pos;
	float4 color;
	float invRange;
};
const static float PARABOLOID_Z_OFFSET = 1.0f;


//=============================== Globals ==========================================

float4x4 g_World <int semantic = 0; int usage = 0;>;
float4x4 g_ViewProj <int semantic = 3; int usage = 0;>;
textureCUBE g_CubePLSMTex1 <int semantic = 268; int usage = 0;>;
samplerCUBE g_SampCubePL1 = sampler_state
{
    Texture = <g_CubePLSMTex1>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU  = WRAP;
    ADDRESSV  = WRAP;
};
DEFXPointLight g_DEFXPointLight <int semantic = 774; int usage = 0;>;
float4x4 g_ParaView <int semantic = 512; int usage = 0;>;
texture g_ZPosAccumTex <int semantic = 768; int usage = 0;>;
sampler g_SampZPosAccum = sampler_state
{
    	Texture = <g_ZPosAccumTex>;
    	MipFilter = NONE;
    	MinFilter = POINT;
    	MagFilter = POINT;
	//MaxAnisotropy = 8;
    	ADDRESSU  = CLAMP;
    	ADDRESSV  = CLAMP;
};
float4 g_CameraPosWithViewFar <int semantic = 15; int usage = 0;>;


//=============================== Functions ==========================================

float4 VecMatMult4(float4 vec4, float4x4 mat44)
{
	return mul(vec4, mat44);
}

float4 ClampPostProjToFarPlane(float4 postProj)
{
	if((postProj.z / postProj.w) > 1.0f)
		postProj.z = postProj.w;
	
	return postProj;
}

float3 GetPointLightPos(DEFXPointLight pointLight)
{
	return pointLight.pos;
}

float3x3 CastMat44ToMat33(float4x4 mat44)
{
	return (float3x3)mat44;
}

float2 Vec4XY(float4 v4)
{
	return v4.xy;
}

float2 GetUVSpaceFromScreenSpace(float2 screenPos)
{
	float2 uv = screenPos * 0.5f + 0.5f;
	uv.y = 1.0f - uv.y;
	return uv;
}

float4 Tex2D(sampler samp, float2 uv)
{
	return tex2D(samp, uv);
}

float Vec4X(float4 v4)
{
	return v4.x;
}

float3 MergeVec2Float(float2 xy, float z)
{
	return float3(xy, z);
}

float Vec4W(float4 v4)
{
	return v4.w;
}

float3 InverseParaboloidProj(float3 postProj, float paraboloidFarPlane)
{
	float dotXY = dot(postProj.xy, postProj.xy);
	float denom = 1.0f / (dotXY + 1.0f);
	
	float3 paraViewDir;
	paraViewDir.xz = 2.0f * postProj.xy;
	paraViewDir.y = -(dotXY - 1.0f);
	paraViewDir *= denom;
	
	paraViewDir = normalize(paraViewDir);
	
	return paraViewDir * paraboloidFarPlane * postProj.z;
}

float3 MatVecMult3(float3x3 mat33, float3 vec3)
{
	return mul(mat33, vec3);
}

float3 Vec4XYZ(float4 vec4)
{
	return vec4.xyz;
}

float3 VecVecAdd3(float3 v1, float3 v2)
{
	return v1 + v2;
}

float3 VecVecSub3(float3 v1, float3 v2)
{
	return v1 - v2;
}

float Dot3(float3 v1, float3 v2)
{
	return dot(v1, v2);
}

float ReciprocalSqrt(float val)
{
	return rsqrt(val);
}

float3 VecFloatMult3(float k, float3 vec3)
{
	return k * vec3;
}

float3 Negate3(float3 val)
{
	return -val;
}

float4 SampleCubeMap(samplerCUBE sampCube, float3 normal)
{
	float3 dir = normal;
	
	// NOTE: D3D only
	dir.z = -dir.z;
	return texCUBE(sampCube, dir);
}

float Reciprocal1(float val)
{
	return rcp(val);
}

float GetPointLightInvRange(DEFXPointLight pointLight)
{
	return pointLight.invRange;
}

float FloatFloatMult(float s1, float s2)
{
	return s1 * s2;
}

float ComputeVarianceShadowPL(float2 moments, float shadowDepth)
{
	float diff = shadowDepth - moments.x;
	float p = diff <= 0.0f;
	float variance = moments.y - (moments.x * moments.x);
	
	// We set a minimum variance to reduce "sparklies" caused by bias/numerical imprecision
	// Needed esp if we are using a 16-bit per channel rather than 32-bit
	const static float MIN_VARIANCE = 1.5f / 65535.0f;
	variance = max(variance, MIN_VARIANCE);
	float pMax = variance / (variance + (diff * diff));
	
	// Reduce light bleeding
	const static float MIN_DARK_TAIL = 0.3f;
	const static float MIN_DARK_RANGE_RCP = 1.0f / (1.0f - MIN_DARK_TAIL);
	
	pMax = max( (pMax - MIN_DARK_TAIL) * MIN_DARK_RANGE_RCP, 0.0f );
	 
	float lightAmount = max(p, pMax);
	return lightAmount;
}

float4 FloatToVec4(float val)
{
	return float4(val, val, val, val);
}



//=============================== Vertex Shaders ==========================================

struct VS_INLightVS
{
	float4 vsinput_0 : POSITION;
};

struct VS_OUTLightVS
{
	float4 vsoutput_0 : POSITION0;
	float4 vsoutput_1 : TEXCOORD0;
};

VS_OUTLightVS LightVS(VS_INLightVS input)
{
	VS_OUTLightVS output;

	float4 VecMatMult4_0__fn = VecMatMult4(input.vsinput_0, g_World);

	float4 VecMatMult4_1__fn = VecMatMult4(VecMatMult4_0__fn, g_ViewProj);

	float4 ClampPostProjToFarPlane_2__fn = ClampPostProjToFarPlane(VecMatMult4_1__fn);


	output.vsoutput_0 = ClampPostProjToFarPlane_2__fn;
	output.vsoutput_1 = VecMatMult4_1__fn;
	return output;
}


//=============================== Pixel Shaders ==========================================

struct PS_INScreenSpacePLShadowParaboloidPS
{
	float4 psinput_0 : TEXCOORD0;
};

struct PS_OUTScreenSpacePLShadowParaboloidPS
{
	float4 psoutput_0 : COLOR0;
};

PS_OUTScreenSpacePLShadowParaboloidPS ScreenSpacePLShadowParaboloidPS(PS_INScreenSpacePLShadowParaboloidPS input)
{
	PS_OUTScreenSpacePLShadowParaboloidPS output;

	float3 GetPointLightPos_65__fn = GetPointLightPos(g_DEFXPointLight);

	float3x3 CastMat44ToMat33_80__fn = CastMat44ToMat33(g_ParaView);

	float2 Vec4XY_82__fn = Vec4XY(input.psinput_0);

	float2 GetUVSpaceFromScreenSpace_60__fn = GetUVSpaceFromScreenSpace(Vec4XY_82__fn);

	float4 Tex2D_43__fn = Tex2D(g_SampZPosAccum, GetUVSpaceFromScreenSpace_60__fn);

	float Vec4X_45__fn = Vec4X(Tex2D_43__fn);

	float3 MergeVec2Float_83__fn = MergeVec2Float(Vec4XY_82__fn, Vec4X_45__fn);

	float Vec4W_84__fn = Vec4W(g_CameraPosWithViewFar);

	float3 InverseParaboloidProj_81__fn = InverseParaboloidProj(MergeVec2Float_83__fn, Vec4W_84__fn);

	float3 MatVecMult3_85__fn = MatVecMult3(CastMat44ToMat33_80__fn, InverseParaboloidProj_81__fn);

	float3 Vec4XYZ_67__fn = Vec4XYZ(g_CameraPosWithViewFar);

	float3 VecVecAdd3_68__fn = VecVecAdd3(MatVecMult3_85__fn, Vec4XYZ_67__fn);

	float3 VecVecSub3_50__fn = VecVecSub3(GetPointLightPos_65__fn, VecVecAdd3_68__fn);

	float Dot3_73__fn = Dot3(VecVecSub3_50__fn, VecVecSub3_50__fn);

	float ReciprocalSqrt_74__fn = ReciprocalSqrt(Dot3_73__fn);

	float3 VecFloatMult3_75__fn = VecFloatMult3(ReciprocalSqrt_74__fn, VecVecSub3_50__fn);

	float3 Negate3_72__fn = Negate3(VecFloatMult3_75__fn);

	float4 SampleCubeMap_41__fn = SampleCubeMap(g_SampCubePL1, Negate3_72__fn);

	float2 Vec4XY_40__fn = Vec4XY(SampleCubeMap_41__fn);

	float Reciprocal1_76__fn = Reciprocal1(ReciprocalSqrt_74__fn);

	float GetPointLightInvRange_66__fn = GetPointLightInvRange(g_DEFXPointLight);

	float FloatFloatMult_77__fn = FloatFloatMult(Reciprocal1_76__fn, GetPointLightInvRange_66__fn);

	float ComputeVarianceShadowPL_42__fn = ComputeVarianceShadowPL(Vec4XY_40__fn, FloatFloatMult_77__fn);

	float4 FloatToVec4_87__fn = FloatToVec4(ComputeVarianceShadowPL_42__fn);


	output.psoutput_0 = FloatToVec4_87__fn;
	return output;
}


//=============================== Techniques ==========================================

technique defaultTech
{
	pass defaultPass
	{
		StencilFunc = EQUAL;
		ZEnable = FALSE;
		StencilRef = 0x1;
		StencilMask = 0x1;
		VertexShader = compile vs_3_0 LightVS();
		PixelShader = compile ps_3_0 ScreenSpacePLShadowParaboloidPS();
	}

};

