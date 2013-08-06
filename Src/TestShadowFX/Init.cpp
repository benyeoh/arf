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

void InitLights()
{
	g_HemiLight.skyColor	= gmtl::Vec4f(0.14f, 0.15f, 0.17f, 1.0f);
	g_HemiLight.groundColor = gmtl::Vec4f(0.16f, 0.12f, 0.12f, 1.0f);

	g_DirLight.color		= gmtl::Vec4f(0.69f, 0.64f, 0.66f, 1.0f);
	g_DirLight.dir			= gmtl::Vec3f(0.5f, -1.0f, -1.0f);
	gmtl::normalize(g_DirLight.dir);

	g_DirLight.invSize		= 1.0f;

	g_PointLightPos			= gmtl::VecA3f(0.0, 5.0f, -1.0f);
	g_PointLight.pos		= _CAST_VEC3(g_PointLightPos);
	g_PointLight.color		= gmtl::Vec4f(1.3f, 0.8f, 0.0f, 1.0f);
	g_PointLight.invRange	= 1.0f / 80.0f;
	g_PointLight.invSize	= 1.0f;
	
	eRTexFormat smFormat = g_pShadowFX->GetResourceMgr().GetSMCompatibleFormat();
	eRDepthFormat smDepthFormat = g_pShadowFX->GetResourceMgr().GetSMCompatibleDepthFormat();

	g_pShadowDirLight = g_pShadowFX->GetResourceMgr().CreateCascadedDirLight();
	IRTexture2DPtr pDirLightSM1 = g_pRenderer->GetRResourceMgr().CreateTexture2DRT(1024, 1024, 1, smFormat);// g_pShadowFX->GetResourceMgr().CreateCascadedDirLightExpShadowMap(1024);
	IRTexture2DPtr pDirLightSM2 = g_pRenderer->GetRResourceMgr().CreateTexture2DRT(1024, 1024, 1, smFormat);
	IRTexture2DPtr pDirLightSMFinal = g_pRenderer->GetRResourceMgr().CreateTexture2DRT(2048, 2048, 1, smFormat);
	IRDepthBufferPtr pDirLightDB = g_pRenderer->GetRResourceMgr().CreateDepthBuffer(1024, 1024, smDepthFormat, 0);
	
	//float stdDevs[4];
	//uint spreadKernel[4];
	//stdDevs[0] = 2.0f;
	//stdDevs[1] = 1.2f;
	//stdDevs[2] = 0.7f;
	//stdDevs[3] = 0.4f;
	//spreadKernel[0] = 2;
	//spreadKernel[1] = 1;
	//spreadKernel[2] = 1;
	//spreadKernel[3] = 1;
	//g_pShadowDirLight->SetShadowMapParamsGaussianFilter(pDirLightSM1, pDirLightSM2, pDirLightSMFinal, pDirLightDB, stdDevs, spreadKernel);
	//g_pShadowDirLight->SetShadowMapParams(pDirLightSMFinal, pDirLightDB);
	eBFXUPoissonBlurSamples blurSamples[4];
	float spreadScales[4];
	blurSamples[0] = POISSON_8_SAMPLE;
	blurSamples[1] = POISSON_8_SAMPLE;
	blurSamples[2] = POISSON_8_SAMPLE;
	blurSamples[3] = POISSON_8_SAMPLE;
	spreadScales[0] = 0.0015f;
	spreadScales[1] = 0.00075f;
	spreadScales[2] = 0.0003f;
	spreadScales[3] = 0.0001f;

	g_pShadowDirLight->SetShadowMapParamsPoissonFilter(pDirLightSM1, pDirLightSMFinal, pDirLightDB, blurSamples, spreadScales, FALSE);

	//g_pShadowPointLight = g_pShadowFX->GetResourceMgr().CreatePointLight();
	//IRTexture2DPtr pPLSM1 = g_pRenderer->GetRResourceMgr().CreateTexture2DRT(256, 256, smFormat);//g_pShadowFX->GetResourceMgr().CreatePointLightExpShadowMap(256);
	//IRTexture2DPtr pPLSM2 = g_pRenderer->GetRResourceMgr().CreateTexture2DRT(256, 256, smFormat);
	//IRTexture2DPtr pPLSMFinal = g_pRenderer->GetRResourceMgr().CreateTexture2DRT(512, 256, smFormat);
	//IRDepthBufferPtr pPLDB = g_pRenderer->GetRResourceMgr().CreateDepthBuffer(256, 256, smDepthFormat, 0);
	////g_pShadowPointLight->SetShadowMapParamsPoissonFilter(pPLSM1, pPLSMFinal, pPLDB, POISSON_8_SAMPLE, 0.012f, FALSE);
	//g_pShadowPointLight->SetShadowMapParamsGaussianFilter(pPLSM1, pPLSM2, pPLSMFinal, pPLDB, 4.0f, 4);
	////g_pShadowPointLight->SetShadowMapParams(pPLSMFinal, pPLDB);

	//g_pShadowPointLight->Update(g_PointLight.pos);
	//g_pShadowPointLight->SetRange(1.0f / g_PointLight.invRange);

	g_pShadowCubePointLight = g_pShadowFX->GetResourceMgr().CreateCubePointLight();
	IRTexture2DPtr pPLSM1 = g_pRenderer->GetRResourceMgr().CreateTexture2DRT(128, 128, 1, smFormat);//g_pShadowFX->GetResourceMgr().CreatePointLightExpShadowMap(256);
	IRTexture2DPtr pPLSM2 = g_pRenderer->GetRResourceMgr().CreateTexture2DRT(128, 128, 1, smFormat);
	IRTextureCubePtr pCubePLSM1 = g_pRenderer->GetRResourceMgr().CreateTextureCubeRT(128, smFormat);
	IRTextureCubePtr pCubePLSMFinal = g_pRenderer->GetRResourceMgr().CreateTextureCubeRT(128, smFormat);
	IRDepthBufferPtr pPLDB = g_pRenderer->GetRResourceMgr().CreateDepthBuffer(128, 128, smDepthFormat, 0);
	//g_pShadowCubePointLight->SetShadowMapParams(pCubePLSMFinal, pPLDB);
	//g_pShadowCubePointLight->SetShadowMapParamsPoissonFilter(pPLSM1, pCubePLSMFinal, pPLDB, POISSON_8_SAMPLE, 0.015f, TRUE);
	g_pShadowCubePointLight->SetShadowMapParamsCubePoissonFilter(pCubePLSM1, pCubePLSMFinal, pPLDB, POISSON_8_SAMPLE, 0.02f, FALSE);
	//g_pShadowCubePointLight->SetShadowMapParamsGaussianFilter(pPLSM1, pPLSM2, pCubePLSMFinal, pPLDB, 4.0f, 4);

	g_pShadowCubePointLight->Update(g_PointLight.pos);
	g_pShadowCubePointLight->SetRange(1.0f / g_PointLight.invRange);

	g_pCubeDebugRT =  g_pRenderer->GetRResourceMgr().CreateTexture2DRT(128, 128, 1, smFormat);
}

void InitMesh()
{
	//g_pRoomMeshList = _NEW BFXUMeshList;

	IByteBufferPtr pBuffer = g_AppCallback.GetFileData(_W("[msh]\\TestShadowFX\\floor-parallax.mls")); 
	g_pRoomMeshList = _NEW BFXUMeshList;
	LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *g_pRoomMeshList);

	pBuffer = g_AppCallback.GetFileData(_W("[msh]\\TestShadowFX\\bandit.mls"));
	BFXUMeshList* pMeshList = _NEW BFXUMeshList;
	LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *pMeshList);
	g_pMeshes[0] = pMeshList;
	gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[0]), gmtl::Vec3f(0.0f, 0.0f, 0.0f));

	pBuffer = g_AppCallback.GetFileData(_W("[msh]\\TestShadowFX\\bandit.mls"));
	pMeshList = _NEW BFXUMeshList;
	LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *pMeshList);
	g_pMeshes[1] = pMeshList;
	gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[1]), gmtl::Vec3f(0.0f, 0.0f, -10.0f));

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
	//int xi = 0x402aaaa8;
	//float xf = *(float*)(&xi);
	//float x = 2.666666;
	//float y = 0.333333;

	//float xoy = x / y;
	//int q = (int) xoy;
	//float r = xoy - q;

	//_TRACE(_W("XX: %f\n"), r);

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
	InitShadowFX();
	//g_pCube = _NEW TestCube();
	//g_pCube->Initialize();

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

	g_pShadowDirLight->UpdateCascadedFrustumBounds(g_CascadeFrustum, 1.35f, _CAST_MAT44(g_Proj));

	return TRUE;
}	

void Shutdown()
{
	_LOOPi(NUM_MESHES)
	{
		g_pMeshes[i] = 0;
	}

	_DELETE( g_pRoomMeshList );

	g_pCubeDebugRT = NULL;
	g_pShadowCubePointLight = NULL;
	g_pShadowPointLight = NULL;
	g_pShadowDirLight = NULL;
	g_pShadowFX = NULL;
	g_pSHFXPipeline = NULL;
	g_pSHFXParamPool = NULL;

    g_pDebugFX = NULL;

	g_pVerdanaFontFace = NULL;
	g_pCourierFontFace = NULL;
	g_pFontFX = NULL;
	
	g_pBFXPipeline = NULL;
	g_pBFXParamPool = NULL;

	g_pBaseFX = NULL;
	g_pMatGen = NULL;

	g_pMainClearGroup = NULL;

	g_pRenderer = NULL;

	g_pInput = NULL;
	g_pFileSystem->Close();
	g_pFileSystem = NULL;

	g_pPlatform = NULL;

	g_pAllocator->DumpLeaks();

	//FreeLibrary( g_hScreenAmbOcclFX );
	//FreeLibrary( g_hDeferredFX );
	ShutdownModule( g_hShadowFX );
	ShutdownModule( g_hBaseFX );
	ShutdownModule( g_hMatGen );
	ShutdownModule( g_hRenderer );
	ShutdownModule( g_hFileSystem );
	ShutdownModule( g_hInput );
	ShutdownModule( g_hPlatform );
}

