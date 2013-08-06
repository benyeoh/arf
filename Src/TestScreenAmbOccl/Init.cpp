//==============================================================================
//
//		Init.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/11/2009
//
//==============================================================================

#include "stdafx.h"

boolean InitFileSystem()
{
	// Load Win32 Platform
	g_hFileSystem = InitializeModule(_W("FileSystemWin32.dll"), &g_MemAllocFns);
	if(!g_hFileSystem) return FALSE;

	CreateFileSystemFn CreateFileSystem = (CreateFileSystemFn) GetFunction(g_hFileSystem, "CreateFileSystem");

	_DEBUG_ASSERT(CreateFileSystem);

	g_pFileSystem = CreateFileSystem();
	g_pFileSystem->Initialize();	

	g_pFileSystem->AddMnemonic(_W("shd"), _W("data\\shaders"));
	g_pFileSystem->AddMnemonic(_W("mat"), _W("data\\materials"));
	g_pFileSystem->AddMnemonic(_W("msh"), _W("data\\meshes"));
	g_pFileSystem->AddMnemonic(_W("tex"), _W("data\\textures"));
	g_pFileSystem->AddMnemonic(_W("dat"), _W("data"));

	return TRUE;
}

boolean InitInput()
{
	// Load Win32 Platform
	g_hInput = InitializeModule(_W("InputWin32.dll"), &g_MemAllocFns);
	if(!g_hInput) return FALSE;

	CreateInputFn CreateInput = (CreateInputFn) GetFunction(g_hInput, "CreateInput");

	_DEBUG_ASSERT(CreateInput);

	g_pInput = CreateInput();
	g_pInput->Initialize();	

	g_FeedWindowsMessage = (FeedWindowsMessageFn) GetFunction(g_hInput, "FeedWindowsMessage");
	return TRUE;
}

boolean InitMatGen()
{
	g_hMatGen = InitializeModule(_W("MaterialGenD3D.dll"), &g_MemAllocFns);
	if(!g_hMatGen) return FALSE;

	CreateMaterialGenFn CreateMaterialGen = (CreateMaterialGenFn) GetFunction(g_hMatGen, "CreateMaterialGen");

	_DEBUG_ASSERT(CreateMaterialGen);
	g_pMatGen = CreateMaterialGen();
	g_pMatGen->Initialize(g_AppCallback);	

	return TRUE;
}

boolean InitBaseFX()
{
	g_hBaseFX = InitializeModule(_W("BaseFX.dll"), &g_MemAllocFns);
	if(!g_hBaseFX) return FALSE;

	CreateBaseFXFn CreateBaseFX = (CreateBaseFXFn) GetFunction(g_hBaseFX, "CreateBaseFX");

	_DEBUG_ASSERT(CreateBaseFX );
	g_pBaseFX = CreateBaseFX();
	g_pBaseFX->Initialize(*g_pRenderer, *g_pMatGen, g_AppCallback);

	g_pBFXPipeline	= g_pBaseFX->GetResourceMgr().CreatePipeline();
	g_pBFXPipeline->AddRenderTarget(g_pRenderer->GetBackBufferColor());
	g_pBFXPipeline->SetDepthBuffer(g_pRenderer->GetBackBufferDepth());

	g_pBFXParamPool = g_pBaseFX->GetResourceMgr().CreateParamPool(NULL);

	g_ParamContainer.SetParamCallback(g_pBFXParamPool, _GET_LIB_INDEX(BFX_EFFECT_PARAM_OFFSET));
	g_RenderContainer.SetRenderCallback(g_pBFXPipeline, _GET_LIB_INDEX_FROM_MAT_TYPE(BFX_MATERIAL_TYPE_OFFSET));

	return TRUE;
}

void InitShadowFX()
{
	g_hShadowFX = InitializeModule(_W("ShadowFX.dll"), &g_MemAllocFns);

	CreateShadowFXFn createShadowFX = (CreateShadowFXFn) GetFunction(g_hShadowFX, "CreateShadowFX");

	_DEBUG_ASSERT(createShadowFX);

	g_pShadowFX = createShadowFX();
	g_pShadowFX->Initialize(*g_pRenderer, g_AppCallback);

	g_pSHFXPipeline		= g_pShadowFX->GetResourceMgr().CreatePipeline();
	g_pSHFXParamPool	= g_pShadowFX->GetResourceMgr().CreateParamPool();

	IRTextureRT* pBB = g_pRenderer->GetBackBufferColor();
	g_pShadowFX->GetDebug().AddRenderTarget(pBB);
	g_pShadowFX->GetDebug().SetViewport(0, 0, pBB->GetWidth(), pBB->GetHeight());

	g_ParamContainer.SetParamCallback(g_pSHFXParamPool, _GET_LIB_INDEX(SHFX_EFFECT_PARAM_OFFSET));
	g_RenderContainer.SetRenderCallback(g_pSHFXPipeline, _GET_LIB_INDEX_FROM_MAT_TYPE(SHFX_MATERIAL_TYPE_OFFSET));
}

boolean InitRenderer()
{
	g_hRenderer = InitializeModule(_W("RendererD3D.dll"), &g_MemAllocFns);
	if(!g_hRenderer ) return FALSE;

	CreateRendererFn CreateRendererD3D = (CreateRendererFn) GetFunction(g_hRenderer, "CreateRenderer");

	_DEBUG_ASSERT(CreateRendererD3D);

	g_pRenderer		= CreateRendererD3D();

	// Initialize renderer and renderFX
	RECT clRect;
	GetClientRect( g_hWnd, &clRect );

	RDisplayInfo pInfo;
	pInfo.backBufferWidth	= clRect.right - clRect.left;
	pInfo.backBufferHeight	= clRect.bottom - clRect.top;
	pInfo.fullScreen		= g_IsFullScreen ;
	pInfo.hWnd				= g_hWnd;

	RInitInfo rInfo;
	rInfo.presentInfo		= pInfo;
	rInfo.pApplication		= &g_AppCallback;

	if(!g_pRenderer->Initialize(rInfo))
		return FALSE;

	//GetD3DDeviceFn	GetD3DDevice = (GetD3DDeviceFn)GetFunction(g_hRenderer, "GetD3DDevice");
	//g_pD3DDevice	= GetD3DDevice(g_pRenderer);

	g_pMainClearGroup	= g_pRenderer->GetRResourceMgr().CreateRenderGroup(NULL);
	g_pMainClearGroup->AddRenderTarget(g_pRenderer->GetBackBufferColor());
	g_pMainClearGroup->SetDepthBuffer(g_pRenderer->GetBackBufferDepth());
	g_pMainClearGroup->SetClearColor(gmtl::Vec4f(0.1f, 0.12f, 0.16f, 0.0f));
	g_pMainClearGroup->SetClearDepth(1.0f);

	g_pZPosClearGroup	= g_pRenderer->GetRResourceMgr().CreateRenderGroup(NULL);
	g_pZPosClearGroup->SetClearColor(gmtl::Vec4f(1.0f, 1.0f, 1.0f, 1.0f)); 

	g_pNormalsClearGroup	= g_pRenderer->GetRResourceMgr().CreateRenderGroup(NULL);
	g_pNormalsClearGroup->SetClearColor(gmtl::Vec4f(0.0f, 1.0f, 0.0f, 0.0f)); 

	return TRUE;
}

void InitDebugFX()
{
	g_pDebugFX = CreateDebugFX();
	g_pDebugFX->Initialize(*g_pRenderer, g_AppCallback);
	g_pDebugFX->AddRenderTarget(g_pRenderer->GetBackBufferColor());
	g_pDebugFX->SetDepthBuffer(g_pRenderer->GetBackBufferDepth());
}

void InitFontFX()
{
	g_pFontFX = CreateFontFX();
	g_pFontFX->Initialize(*g_pRenderer, g_AppCallback);
	g_pFontFX->AddRenderTarget(g_pRenderer->GetBackBufferColor());

	IFFilePtr pFile = g_pFileSystem->GetFile(_W("[dat]\\Font\\verdana.ttf"));
	IByteBufferPtr pFontData = _NEW CByteBuffer(pFile->Length());
	AppendData(pFontData, pFile);

	g_pVerdanaFontFace = g_pFontFX->GetResourceMgr().CreateFontFace(pFontData, 12, 0);	

	pFile = g_pFileSystem->GetFile(_W("[dat]\\Font\\cour.ttf"));
	pFontData = _NEW CByteBuffer(pFile->Length());
	AppendData(pFontData, pFile);

	g_pCourierFontFace = g_pFontFX->GetResourceMgr().CreateFontFace(pFontData, 12, 0);
}

boolean InitPlatform()
{
	g_hPlatform = InitializeModule(_W("PlatformWin32.dll"), &g_MemAllocFns);
	if(!g_hPlatform ) return FALSE;

	CreatePlatformFn createPlatform = (CreatePlatformFn) GetFunction(g_hPlatform, "CreatePlatform");

	_DEBUG_ASSERT(createPlatform);

	g_pPlatform = createPlatform();

	return TRUE;
}

void InitDeferredFX()
{
	g_hDeferredFX = InitializeModule(_W("DeferredFX.dll"), &g_MemAllocFns);

	CreateDeferredFXFn createDeferred = (CreateDeferredFXFn) GetFunction(g_hDeferredFX, "CreateDeferredFX");

	_DEBUG_ASSERT(createDeferred);

	g_pDeferredFX = createDeferred();
	_DEBUG_ASSERT(g_pDeferredFX);
	g_pDeferredFX->Initialize(*g_pRenderer, g_AppCallback);
	g_pDeferredPipeline = g_pDeferredFX->GetResourceMgr().CreatePipeline();
	uint screenWidth = g_pRenderer->GetDisplayInfo().backBufferWidth;
	uint screenHeight = g_pRenderer->GetDisplayInfo().backBufferHeight;
	IRTexture2D* pZPos = g_pRenderer->GetRResourceMgr().CreateTexture2DRT(screenWidth, screenHeight, NUM_Z_MIPS + 1, TEXF_R32F);
	IRTexture2D* pNormals = g_pRenderer->GetRResourceMgr().CreateTexture2DRT(screenWidth, screenHeight, 1, TEXF_A8R8G8B8);
	IRTexture2D* pMaterial = g_pRenderer->GetRResourceMgr().CreateTexture2DRT(screenWidth, screenHeight, 1, TEXF_A8R8G8B8);
	_DEBUG_ASSERT(pZPos->GetWidth(0) == g_pRenderer->GetBackBufferDepth()->GetWidth());
	_DEBUG_ASSERT(pZPos->GetHeight(0) == g_pRenderer->GetBackBufferDepth()->GetHeight());
	_DEBUG_ASSERT(screenWidth == g_pRenderer->GetBackBufferDepth()->GetWidth());
	_DEBUG_ASSERT(screenHeight == g_pRenderer->GetBackBufferDepth()->GetHeight());

	g_pZPos = pZPos;
	g_pNormals = pNormals;

	g_pZPosClearGroup->AddRenderTarget(pZPos->GetTextureRT(0));
	g_pNormalsClearGroup->AddRenderTarget(pNormals->GetTextureRT(0));

	g_pDeferredPipeline->SetViewport(0, 0, screenWidth, screenHeight);
	g_pDeferredPipeline->AddAccumulationTargets(pZPos->GetTextureRT(0), pNormals->GetTextureRT(0), pMaterial->GetTextureRT(0));
	g_pDeferredPipeline->SetDepthBuffer(g_pRenderer->GetBackBufferDepth());

	g_pDeferredParamPool = g_pDeferredFX->GetResourceMgr().CreateParamPool();

	_LOOPi(NUM_POINT_LIGHTS)
	{
		g_pDeferredLightGroup[i] = g_pDeferredFX->GetResourceMgr().CreateLightGroup();
		g_pDeferredLightGroup[i]->AddRenderTarget(g_pRenderer->GetBackBufferColor());
		g_pDeferredLightGroup[i]->SetDepthBuffer(g_pRenderer->GetBackBufferDepth());
		g_pDeferredLightGroup[i]->SetViewport(0, 0, screenWidth, screenHeight);
	}

	g_pDeferredDirLightGroup = g_pDeferredFX->GetResourceMgr().CreateLightGroup();
	g_pDeferredDirLightGroup->AddRenderTarget(g_pRenderer->GetBackBufferColor());
	g_pDeferredDirLightGroup->SetDepthBuffer(g_pRenderer->GetBackBufferDepth());
	g_pDeferredDirLightGroup->SetViewport(0, 0, screenWidth, screenHeight);

	g_ParamContainer.SetParamCallback(g_pDeferredParamPool, _GET_LIB_INDEX(DEFX_EFFECT_PARAM_OFFSET));
	g_RenderContainer.SetRenderCallback(g_pDeferredPipeline, _GET_LIB_INDEX_FROM_MAT_TYPE(DEFX_MATERIAL_TYPE_OFFSET));

	g_pDeferredParamPool->SetAccumulationBuffers(pZPos, pNormals, pMaterial);
}

IRTexture2D* InitNoiseTexture(IRRenderer* pRenderer, uint width, uint height)
{
	wchar dataBuffer[256];
	dataBuffer[0] = (wchar) width;
	dataBuffer[1] = (wchar) height;

	wstring dataName = _W("___TestScreenSpaceAmbOccl::Noise::");
	_DEBUG_ASSERT(dataName.length() < 256 - 2);
	memcpy(&(dataBuffer[2]), (void*) dataName.c_str(), dataName.length() * sizeof(wchar));

	srand((uint) GetCurrentTime());

	CRCDataKey key;
	key.Set(dataBuffer, sizeof(wchar) * (dataName.size() + 2));
	IRTexture2D* pNoise = pRenderer->GetRResourceMgr().LoadTexture2D(&key, NULL);
	if(!pNoise)
	{
		pNoise = pRenderer->GetRResourceMgr().CreateTexture2D(&key, width, height, 1, TEXF_R8, TEXU_DEFAULT);

		uint pitch;
		byte* pDest = pNoise->Lock(0, pitch, NULL);

		_LOOPi(pNoise->GetHeight(0))
		{
			_LOOPj(pNoise->GetWidth(0))
			{
				float x = float(::rand())/float(RAND_MAX);
				byte xVal = (byte) (x * 255.0f);

				pDest[j] = xVal;

				//float y = float(::rand())/float(RAND_MAX) * 2.0f - 1.0f;
				//float length = sqrt(x * x + y * y);
				//x = x / length;
				//y = y / length;

				//byte xVal = (byte) (x * 255.0f);
				//byte yVal = (byte) (y * 255.0f);
				//pDest[j*2]		= xVal;
				//pDest[j*2+1]	= yVal; 
			}

			pDest += pitch;
		}

		pNoise->Unlock(0);
	}

	return pNoise;
}

class NumSamplesEntry : public IVarParamEntry
{
private:
	double m_LastChangedTime;

public:
	NumSamplesEntry() { m_LastChangedTime = 0.0; }

public:
	const wchar* GetString() { return ToStringAuto(_W("Num Samples: %d"), g_NumSamples); }
	void Inc() 
	{ 
		double curTime = g_pPlatform->GetTimer().GetTime();
		if(curTime - m_LastChangedTime > 0.1f)
		{
			g_NumSamples++;
			if(g_NumSamples > 64) 
				g_NumSamples = 64;
			m_LastChangedTime = curTime;
		}
	}

	void Dec() 
	{ 
		double curTime = g_pPlatform->GetTimer().GetTime();
		if(curTime - m_LastChangedTime > 0.1f)
		{
			g_NumSamples--; 
			if(g_NumSamples < 4) 
				g_NumSamples = 4;
			m_LastChangedTime = curTime;
		}
	}
};

class AlchemyIntensityEntry : public IVarParamEntry
{
public:
	const wchar* GetString() { return ToStringAuto(_W("Alchemy Intensity: %f"), g_AlchemyAOParams[0]); }
	void Inc() { g_AlchemyAOParams[0] += 1.0f * g_TimeDT; }
	void Dec() { g_AlchemyAOParams[0] -= 1.0f * g_TimeDT; }
};

class AlchemyContrastEntry : public IVarParamEntry
{
public:
	const wchar* GetString() { return ToStringAuto(_W("Alchemy Contrast: %f"), g_AlchemyAOParams[1]); }
	void Inc() { g_AlchemyAOParams[1] += 1.0f * g_TimeDT; }
	void Dec() { g_AlchemyAOParams[1] -= 1.0f * g_TimeDT; }
};

class AlchemyBiasEntry : public IVarParamEntry
{
public:
	const wchar* GetString() { return ToStringAuto(_W("Alchemy Bias: %f"), g_AlchemyAOParams[2]); }
	void Inc() { g_AlchemyAOParams[2] += 0.0001f * 3.0f * g_TimeDT; }
	void Dec() { g_AlchemyAOParams[2] -= 0.0001f * 3.0f * g_TimeDT; }
};

class DistributedSampleRadiusEntry : public IVarParamEntry
{
public:
	const wchar* GetString() { return ToStringAuto(_W("Distributed Sample Radius: %f"), g_DistributedSampleParams[0]); }
	void Inc() { g_DistributedSampleParams[0] += 1.0f * g_TimeDT; }
	void Dec() { g_DistributedSampleParams[0] -= 1.0f * g_TimeDT; }
};

class DistributedNumTurnsEntry : public IVarParamEntry
{
public:
	const wchar* GetString() { return ToStringAuto(_W("Distributed Sample Turns: %f"), g_DistributedSampleParams[1]); }
	void Inc() { g_DistributedSampleParams[1] += 1.0f * g_TimeDT; }
	void Dec() { g_DistributedSampleParams[1] -= 1.0f * g_TimeDT; }
};

class DistributedMipFactorEntry : public IVarParamEntry
{
public:
	const wchar* GetString() { return ToStringAuto(_W("Distributed Sample Mip Factor: %f"), g_DistributedSampleParams[2]); }
	void Inc() { g_DistributedSampleParams[2] += 12.0f * g_TimeDT;; }
	void Dec() { g_DistributedSampleParams[2] -= 12.0f * g_TimeDT;; }
};

class TechniqueEntry : public IVarParamEntry
{
private:
	double m_LastChangedTime;

public:
	TechniqueEntry() { m_LastChangedTime = 0.0; }

public:
	const wchar* GetString() 
	{ 
		switch(g_CurTechnique)
		{
		case 0: return _W("Alchemy Falloff");
		case 1: return _W("Updated Alchemy Falloff");
		}

		return _W("Error");
	}

	void Inc() 
	{ 
		double curTime = g_pPlatform->GetTimer().GetTime();
		if(curTime - m_LastChangedTime > 0.5f)
		{
			g_CurTechnique++; 
			g_CurTechnique %= NUM_TECHS; 

			m_LastChangedTime = curTime;
		}
	}

	void Dec() 
	{ 
		double curTime = g_pPlatform->GetTimer().GetTime();
		if(curTime - m_LastChangedTime > 0.5f)
		{
			g_CurTechnique--;
			g_CurTechnique %= NUM_TECHS; 
			g_CurTechnique = abs(g_CurTechnique);

			m_LastChangedTime = curTime;
		}
	}
};

class DownsampleTechniqueEntry : public IVarParamEntry
{
private:
	double m_LastChangedTime;

public:
	DownsampleTechniqueEntry() { m_LastChangedTime = 0.0; }

public:
	const wchar* GetString() 
	{ 
		switch(g_CurDownsampleTechnique)
		{
		case 0: return _W("Z Downsample - Rotated Grid");
		case 1: return _W("Z Downsample - Average");
		}

		return _W("Error");
	}

	void Inc() 
	{ 
	}

	void Dec() 
	{ 
	}
};

static TechniqueEntry s_TechniqueEntry;
static DownsampleTechniqueEntry s_DownsampleTechniqueEntry;

static NumSamplesEntry s_NumSamplesEntry;
static AlchemyIntensityEntry s_AlchemyIntensityEntry;
static AlchemyContrastEntry s_AlchemyContrastEntry;
static AlchemyBiasEntry s_AlchemyBiasEntry;
static DistributedSampleRadiusEntry s_DistributedSampleRadiusEntry;
static DistributedNumTurnsEntry s_DistributedNumTurnsEntry;
static DistributedMipFactorEntry s_DistributedMipFactorEntry;

void InitSSAO()
{
	uint screenWidth = g_pRenderer->GetDisplayInfo().backBufferWidth;
	uint screenHeight = g_pRenderer->GetDisplayInfo().backBufferHeight;
	
	g_pAOBuffer = g_pRenderer->GetRResourceMgr().CreateTexture2DRT(screenWidth, screenHeight, 1, TEXF_A8R8G8B8);
	g_pNoiseTex = InitNoiseTexture(g_pRenderer, 64, 64);
	
	// Setup AO effect
	IByteBufferPtr pBuffer = g_AppCallback.GetFileData(_W("[shd]\\TestScreenAmbOccl\\AOPass.fx")); 	
	IREffectTemplatePtr pTemplate = g_pRenderer->GetRResourceMgr().LoadEffectTemplate(NULL, pBuffer);

	REffectParam constantParams[4];
	uint semantics[4];
	
	constantParams[0].SetTexture2D(g_pNoiseTex);
	semantics[0] = NOISE_TEX;
	constantParams[1].SetTexture2D(g_pZPos);
	semantics[1] = Z_POS_TEX;
	constantParams[2].SetTexture2D(g_pNormals);
	semantics[2] = NORMALS_TEX;

	gmtl::Vec4f projParams;
	projParams[0] = g_Proj[0][0];
	projParams[1] = g_Proj[1][1];
	projParams[2] = (float) screenWidth;
	projParams[3] = (float) screenHeight;

	constantParams[3].SetVector4(&projParams);
	semantics[3] = PROJ_PARAMS;

	g_pAOEffect = g_pRenderer->GetRResourceMgr().LoadEffect(pTemplate, constantParams, semantics, 4);
	
	// Setup downsample effect
	_LOOPi(NUM_Z_MIPS)
	{
		pBuffer = g_AppCallback.GetFileData(_W("[shd]\\TestScreenAmbOccl\\DownsampleZPass.fx")); 	
		IREffectTemplatePtr pTemplate = g_pRenderer->GetRResourceMgr().LoadEffectTemplate(NULL, pBuffer);

		REffectParam constantParams[4];
		uint semantics[4];

		constantParams[0].SetTexture2D(g_pNoiseTex);
		semantics[0] = NOISE_TEX;
		constantParams[1].SetTexture2D(g_pZPos);
		semantics[1] = Z_POS_TEX;
		
		gmtl::Vec2f halfPixelOffset;
		halfPixelOffset[0] = 0.5f / g_pZPos->GetWidth(i);
		halfPixelOffset[1] = 0.5f / g_pZPos->GetHeight(i);

		constantParams[2].SetVector2(&halfPixelOffset);
		semantics[2] = HALF_PIXEL_OFFSET;

		constantParams[3].SetInt(i);
		semantics[3] = SAMPLE_LOD;

		g_pDownsampleEffect[i] = g_pRenderer->GetRResourceMgr().LoadEffect(pTemplate, constantParams, semantics, 4);
	}
	
	// Setup main AO group
	g_pAOGroup = g_pRenderer->GetRResourceMgr().CreateRenderGroup(NULL);

	g_pAOGroup->AddRenderTarget(g_pAOBuffer->GetTextureRT(0));
	g_pAOGroup->SetViewport(0, 0, g_pAOBuffer->GetWidth(0), g_pAOBuffer->GetHeight(0));
	g_pAOGroup->SetDepthBuffer(g_pRenderer->GetBackBufferDepth());
	g_pAOGroup->SetClearColor(gmtl::Vec4f(0.4f, 0.4f, 0.4f, 0.4f));

	g_AOScreenMesh.Initialize(g_pZPos->GetWidth(0), g_pZPos->GetHeight(0), g_pRenderer);

	// Setup downsample group
	_LOOPi(NUM_Z_MIPS)
	{
		g_pDownsampleGroups[i] = g_pRenderer->GetRResourceMgr().CreateRenderGroup(NULL);

		g_pDownsampleGroups[i]->AddRenderTarget(g_pZPos->GetTextureRT(i+1));
		g_pDownsampleGroups[i]->SetViewport(0, 0, g_pZPos->GetWidth(i+1), g_pZPos->GetHeight(i+1));

		g_ZDownsampleMesh[i].Initialize(g_pZPos->GetWidth(i+1), g_pZPos->GetHeight(i+1), g_pRenderer);
	}

	g_NumSamples = 16;
	g_AlchemyAOParams[0] = 1.0f; g_AlchemyAOParams[1] = 1.0f; g_AlchemyAOParams[2] = 0.0001f;
	g_DistributedSampleParams[0] = 1.0f; g_DistributedSampleParams[1] = 9.5f; g_DistributedSampleParams[2] = 12.0f;

	g_pAOParamEntries[0] = &s_NumSamplesEntry;
	g_pAOParamEntries[1] = &s_AlchemyIntensityEntry;
	g_pAOParamEntries[2] = &s_AlchemyContrastEntry;
	g_pAOParamEntries[3] = &s_AlchemyBiasEntry;
	g_pAOParamEntries[4] = &s_DistributedSampleRadiusEntry;
	g_pAOParamEntries[5] = &s_DistributedNumTurnsEntry;
	g_pAOParamEntries[6] = &s_DistributedMipFactorEntry;

	g_pTechEntry = &s_TechniqueEntry;
	g_pTechDownsampleEntry = &s_DownsampleTechniqueEntry;
}

void InitLights()
{
	g_HemiLight.skyColor	= gmtl::Vec4f(0.14f, 0.15f, 0.17f, 1.0f);
	g_HemiLight.groundColor = gmtl::Vec4f(0.16f, 0.12f, 0.12f, 1.0f);

	g_DirLight.color		= gmtl::Vec4f(0.6f, 0.58f, 0.5f, 1.0f);
	g_DirLight.dir			= gmtl::Vec3f(0.3f, -1.0f, -0.8f);
	gmtl::normalize(g_DirLight.dir);

	//g_PointLight[0].color	= gmtl::Vec4f(1.0f, 0.1f, 0.1f, 1.0f);
	//g_PointLight[1].color	= gmtl::Vec4f(0.1f, 1.0f, 0.1f, 1.0f);
	//g_PointLight[2].color	= gmtl::Vec4f(0.1f, 0.1f, 1.0f, 1.0f);
	//g_PointLight[3].color	= gmtl::Vec4f(0.0f, 0.8f, 0.8f, 1.0f);

	//g_PointLight[4].color	= gmtl::Vec4f(0.0f, 0.8f, 0.8f, 1.0f);
	//g_PointLight[5].color	= gmtl::Vec4f(0.0f, 0.8f, 0.8f, 1.0f);
	//g_PointLight[6].color	= gmtl::Vec4f(0.0f, 0.8f, 0.8f, 1.0f);
	//g_PointLight[7].color	= gmtl::Vec4f(0.0f, 0.8f, 0.8f, 1.0f);

	float piInc = gmtl::Math::TWO_PI / NUM_POINT_LIGHTS;
	_LOOPi(NUM_POINT_LIGHTS)
	{
		float x = cos(piInc * i) * 2.0f;
		float z = sin(piInc * i) * 2.0f;

		float r = (((float) rand()) / RAND_MAX) * 0.6f + 0.1f;
		float g = (((float) rand()) / RAND_MAX) * 0.6f + 0.1f;
		float b = (((float) rand()) / RAND_MAX) * 0.6f + 0.1f;

		g_PointLight[i].color	= gmtl::Vec4f(r, g, b, 1.0f);

		g_PointLightPos[i]			= gmtl::VecA3f(x, 4.0f, z);
		g_PointLight[i].invRange	= 1.0f / 8.0f;
		g_PointLight[i].pos			= _CAST_VEC3(g_PointLightPos[i]);

		gmtl::identity(_CAST_MAT44(g_PointLightWorld[i]));
		gmtl::setTrans(_CAST_MAT44(g_PointLightWorld[i]), g_PointLight[i].pos);
		gmtl::setScale(_CAST_MAT44(g_PointLightWorld[i]), 1.0f / g_PointLight[i].invRange);
	}

	//gmtl::VecA3f posPLViewSpace;
	//TransformVec(&g_View, &g_PointLightPos, &posPLViewSpace);
	//g_PointLight.pos		= _CAST_VEC3(g_PointLightPos);
	//g_PointLight.color		= gmtl::Vec4f(1.3f, 0.6f, 0.3f, 1.0f);
	//g_PointLight.invRange	= 1.0f / 8.0f;
	//g_PointLight.invSize	= 1.0f;

	IByteBufferPtr pBuffer = g_AppCallback.GetFileData(_W("[msh]\\TestScreenAmbOccl\\pointlight.mls")); 
	g_pPointLightMesh = _NEW BFXUMeshList;
	LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *g_pPointLightMesh);

	pBuffer = g_AppCallback.GetFileData(_W("[msh]\\TestScreenAmbOccl\\dirlight.mls")); 
	g_pDirLightMesh = _NEW BFXUMeshList;
	LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *g_pDirLightMesh);

	//gmtl::identity(_CAST_MAT44(g_PointLightWorld));
	//gmtl::setTrans(_CAST_MAT44(g_PointLightWorld), g_PointLight.pos);
	//gmtl::setScale(_CAST_MAT44(g_PointLightWorld), 1.0f / g_PointLight.invRange);

	eRTexFormat smFormat = g_pShadowFX->GetResourceMgr().GetSMCompatibleFormat();
	eRDepthFormat smDepthFormat = g_pShadowFX->GetResourceMgr().GetSMCompatibleDepthFormat();

	g_pShadowDirLight = g_pShadowFX->GetResourceMgr().CreateCascadedDirLight();
	IRTexture2DPtr pDirLightSM1 = g_pRenderer->GetRResourceMgr().CreateTexture2DRT(1024, 1024, 1, smFormat);// g_pShadowFX->GetResourceMgr().CreateCascadedDirLightExpShadowMap(1024);
	//IRTextureRT* pDirLightSM2 = g_pRenderer->GetRResourceMgr().CreateTexture2DRT(1024, 1024, smFormat);
	IRTexture2DPtr pDirLightSMFinal = g_pRenderer->GetRResourceMgr().CreateTexture2DRT(2048, 2048, 1, smFormat);
	IRDepthBufferPtr pDirLightDB = g_pRenderer->GetRResourceMgr().CreateDepthBuffer(1024, 1024, smDepthFormat, 0);

	eBFXUPoissonBlurSamples blurSamples[4];
	float spreadScales[4];
	blurSamples[0] = POISSON_4_SAMPLE;
	blurSamples[1] = POISSON_4_SAMPLE;
	blurSamples[2] = POISSON_4_SAMPLE;
	blurSamples[3] = POISSON_4_SAMPLE;
	spreadScales[0] = 0.0015f;
	spreadScales[1] = 0.00075f;
	spreadScales[2] = 0.0003f;
	spreadScales[3] = 0.0001f;
	g_pShadowDirLight->SetShadowMapParamsPoissonFilter(pDirLightSM1, pDirLightSMFinal, pDirLightDB, blurSamples, spreadScales, FALSE);

	IRTextureCubePtr pPLSM1 = g_pRenderer->GetRResourceMgr().CreateTextureCubeRT(128, smFormat);//g_pShadowFX->GetResourceMgr().CreatePointLightExpShadowMap(256);
	//IRTextureRTPtr pPLSM2 = g_pRenderer->GetRResourceMgr().CreateTexture2DRT(256, 256, smFormat);
	IRTextureCubePtr pPLSMFinal = g_pRenderer->GetRResourceMgr().CreateTextureCubeRT(128, smFormat);
	IRDepthBufferPtr pPLDB = g_pRenderer->GetRResourceMgr().CreateDepthBuffer(128, 128, smDepthFormat, 0);

	_LOOPi(NUM_POINT_LIGHTS)
	{
		g_pShadowPointLight[i] = g_pShadowFX->GetResourceMgr().CreateCubePointLight();
		g_pShadowPointLight[i]->SetShadowMapParamsCubePoissonFilter(pPLSM1, pPLSMFinal, pPLDB, POISSON_8_SAMPLE, 0.02f, FALSE);
		g_pShadowPointLight[i]->Update(g_PointLight[i].pos);
		g_pShadowPointLight[i]->SetRange(1.0f / g_PointLight[i].invRange);
	}
}

void InitMesh()
{
	//g_pRoomMeshList = _NEW BFXUMeshList;

	//IByteBufferPtr pBuffer = g_AppCallback.GetFileData(_W("[msh]\\TestScreenAmbOccl\\floor.mls")); 
	//g_pRoomMeshList = _NEW BFXUMeshList;
	//LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *g_pRoomMeshList);

	IByteBufferPtr pBuffer = g_AppCallback.GetFileData(_W("[msh]\\TestScreenAmbOccl\\sponza.mls"));
	BFXUMeshList* pMeshList = _NEW BFXUMeshList;
	LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *pMeshList);
	g_pMeshes[0] = pMeshList;
	gmtl::setScale(_CAST_MAT44(g_MeshesWorld[0]), gmtl::Vec3f(0.02f, 0.02f, 0.02f));
	gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[0]), gmtl::Vec3f(0.0f, 0.0f, 0.0f));

	//pBuffer = g_AppCallback.GetFileData(_W("[msh]\\TestDeferredFX\\king_kong_1.mls"));
	//pMeshList = _NEW BFXUMeshList;
	//LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *pMeshList);
	//g_pMeshes[1] = pMeshList;
	//gmtl::setScale(_CAST_MAT44(g_MeshesWorld[1]), gmtl::Vec3f(0.3f, 0.3f, 0.3f));
	//gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[1]), gmtl::Vec3f(-3.0f, 1.5f, 0.0f));

	//pBuffer = g_AppCallback.GetFileData(_W("[msh]\\TestDeferredFX\\king_kong_2.mls"));
	//pMeshList = _NEW BFXUMeshList;
	//LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *pMeshList);
	//g_pMeshes[2] = pMeshList;
	//gmtl::setScale(_CAST_MAT44(g_MeshesWorld[2]), gmtl::Vec3f(0.3f, 0.3f, 0.3f));
	//gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[2]), gmtl::Vec3f(-1.0f, 1.5f, 0.0f));

	//pBuffer = g_AppCallback.GetFileData(_W("[msh]\\TestDeferredFX\\king_kong_3.mls"));
	//pMeshList = _NEW BFXUMeshList;
	//LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *pMeshList);
	//g_pMeshes[3] = pMeshList;
	//gmtl::setScale(_CAST_MAT44(g_MeshesWorld[3]), gmtl::Vec3f(0.3f, 0.3f, 0.3f));
	//gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[3]), gmtl::Vec3f(1.0f, 1.5f, 0.0f));

	//pBuffer = g_AppCallback.GetFileData(_W("[msh]\\TestDeferredFX\\king_kong_4.mls"));
	//pMeshList = _NEW BFXUMeshList;
	//LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *pMeshList);
	//g_pMeshes[4] = pMeshList;
	//gmtl::setScale(_CAST_MAT44(g_MeshesWorld[4]), gmtl::Vec3f(0.3f, 0.3f, 0.3f));
	//gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[4]), gmtl::Vec3f(3.0f, 1.5f, 0.0f));

	//pBuffer = g_AppCallback.GetFileData(_W("[msh]\\TestShadowFX\\armor-merchant-parallax.mls"));
	//pMeshList = _NEW BFXUMeshList;
	//LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *pMeshList);
	//g_pMeshes[1] = pMeshList;
	//gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[1]), gmtl::Vec3f(0.0f, 0.0f, -10.0f));

	//pBuffer = g_AppCallback.GetFileData(_W("[msh]\\bandit_deferred.mls"));
	//pMeshList = _NEW BFXUMeshList;
	//LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *pMeshList);
	//g_pMeshes[2] = pMeshList;
	//gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[0]), gmtl::Vec3f(0.0f, -1.0f, 1.0f));

	//pBuffer = g_AppCallback.GetFileData(_W("[msh]\\raider.mls"));
	//pMeshList = _NEW BFXUMeshList;
	//LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *pMeshList);
	//g_pMeshes[3] = pMeshList;
	//gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[1]), gmtl::Vec3f(1.0f, -1.0f, 2.0f));
}

boolean Initialize()
{
	m_LastNumOfFrames = 0;
	m_NumFrames = 0;
	m_LastTimeElapsed = 0.0f;

	// Get the application path	
	wchar filePath[256];
	GetModuleFileName(NULL, filePath, 256);
	wstring path = filePath;
	std::transform(path.begin(), path.end(), path.begin(), towlower);

	int index = (int) path.find(_W("\\bin\\"));
	if(index != wstring::npos)
	{
		path = path.substr(0, index);
	}

	InitFileSystem();

	g_pFileSystem->Open(path.c_str());	

	InitPlatform();
	InitRenderer();
	InitMatGen();
	InitBaseFX();
	InitInput();
	InitFontFX();
	InitDebugFX();
	InitDeferredFX();
	InitShadowFX();
	InitLights();
	
	const RDisplayInfo& dispInfo = g_pRenderer->GetDisplayInfo();

	gmtl::setPerspective(g_Proj, gmtl::Math::PI / 3.0f, ((float)dispInfo.backBufferWidth) / dispInfo.backBufferHeight, NEAR_PLANE, FAR_PLANE);
	_CAST_VEC3(g_EyePos) = gmtl::Vec3f(-3.0f, 0.5f, 33.0f);
	_CAST_VEC3(g_EyeDir) = gmtl::Vec3f(0.0f, 0.0f, -1.f);
	gmtl::VecA3f at;
	_CAST_VEC3(at) = _CAST_VEC3(g_EyePos) + _CAST_VEC3(g_EyeDir);
	_CAST_VEC3(g_EyeUpVec) = gmtl::Vec3f(0.0f, 1.0f, 0.0f);
	gmtl::setView(_CAST_MAT44(g_View), 
		_CAST_VEC3(g_EyePos), 
		_CAST_VEC3(at), 
		_CAST_VEC3(g_EyeUpVec));

	g_TimeDT = 0.0f;

	InitMesh();
	InitSSAO();

	g_pShadowDirLight->UpdateCascadedFrustumBounds(g_CascadeFrustum, 2.0f, _CAST_MAT44(g_Proj));

	return TRUE;
}	

void Shutdown()
{
	g_pAOGroup = NULL;
	g_pAOEffect = NULL;

	g_pAOBuffer = NULL;
	g_pNoiseTex = NULL;

	g_AOScreenMesh.~BFXUScreenSpaceMesh();
	_LOOPi(NUM_Z_MIPS)
	{
		g_pDownsampleEffect[i] = NULL;
		g_pDownsampleGroups[i] = NULL;
		g_ZDownsampleMesh[i].~BFXUScreenSpaceMesh();
	}

	g_pDirLightMesh = NULL;
	g_pPointLightMesh = NULL;

	_LOOPi(NUM_MESHES)
	{
		g_pMeshes[i] = 0;
	}

	_DELETE( g_pRoomMeshList );

	g_pDeferredDirLightGroup = NULL;
	_LOOPi(NUM_POINT_LIGHTS)
	{
		g_pShadowPointLight[i] = NULL;
		g_pDeferredLightGroup[i] = NULL;
	}

	g_pShadowDirLight = NULL;
	g_pShadowFX = NULL;
	g_pSHFXPipeline = NULL;
	g_pSHFXParamPool = NULL;

	g_pDeferredPipeline = NULL;
	g_pDeferredParamPool = NULL;
	g_pDeferredFX = NULL;
	g_pDebugFX = NULL;

	g_pVerdanaFontFace = NULL;
	g_pCourierFontFace = NULL;
	g_pFontFX = NULL;

	g_pBFXPipeline = NULL;
	g_pBFXParamPool = NULL;

	g_pBaseFX = NULL;
	g_pMatGen = NULL;

	g_pNormalsClearGroup = NULL;
	g_pZPosClearGroup = NULL;
	g_pMainClearGroup = NULL;

	g_pRenderer = NULL;

	g_pInput = NULL;
	g_pFileSystem->Close();
	g_pFileSystem = NULL;

	g_pPlatform = NULL;

	g_pAllocator->DumpLeaks();

	//FreeLibrary( g_hScreenAmbOcclFX );
	ShutdownModule( g_hDeferredFX );
	ShutdownModule( g_hShadowFX );
	ShutdownModule( g_hBaseFX );
	ShutdownModule( g_hMatGen );
	ShutdownModule( g_hRenderer );
	ShutdownModule( g_hFileSystem );
	ShutdownModule( g_hInput );
	ShutdownModule( g_hPlatform );
}
