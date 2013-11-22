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

	_LOOPi(NUM_THREADS)
	{
		g_pBFXPipeline->AddContext();
	}

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
	g_pPlatform->Initialize();

	g_pThreadPool = g_pPlatform->GetResourceMgr().CreateThreadPool();

	IPThread* pThreads[NUM_THREADS+1];

	_LOOPi(NUM_THREADS)
	{
		pThreads[i] = g_pPlatform->GetResourceMgr().CreateThread();
		pThreads[i]->Initialize();
	}

	g_pThreadPool->Initialize(10, pThreads, NUM_THREADS);

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
	g_pZPos = g_pRenderer->GetRResourceMgr().CreateTexture2DRT(screenWidth, screenHeight, 1, TEXF_R32F);
	g_pNormals = g_pRenderer->GetRResourceMgr().CreateTexture2DRT(screenWidth, screenHeight, 1, TEXF_A8R8G8B8);
	g_pMaterial = g_pRenderer->GetRResourceMgr().CreateTexture2DRT(screenWidth, screenHeight, 1, TEXF_A8R8G8B8);
	_DEBUG_ASSERT(g_pZPos->GetWidth(0) == g_pRenderer->GetBackBufferDepth()->GetWidth());
	_DEBUG_ASSERT(g_pZPos->GetHeight(0) == g_pRenderer->GetBackBufferDepth()->GetHeight());
	_DEBUG_ASSERT(screenWidth == g_pRenderer->GetBackBufferDepth()->GetWidth());
	_DEBUG_ASSERT(screenHeight == g_pRenderer->GetBackBufferDepth()->GetHeight());

	g_pDeferredPipeline->SetViewport(0, 0, screenWidth, screenHeight);
	g_pDeferredPipeline->AddAccumulationTargets(g_pZPos->GetTextureRT(0), g_pNormals->GetTextureRT(0), g_pMaterial->GetTextureRT(0));
	g_pDeferredPipeline->AddRenderTarget(g_pRenderer->GetBackBufferColor());
	g_pDeferredPipeline->SetDepthBuffer(g_pRenderer->GetBackBufferDepth());
	
	_LOOPi(NUM_THREADS)
	{
		g_pDeferredPipeline->AddContext();
	}

	g_pDeferredParamPool = g_pDeferredFX->GetResourceMgr().CreateParamPool();

	g_ParamContainer.SetParamCallback(g_pDeferredParamPool, _GET_LIB_INDEX(DEFX_EFFECT_PARAM_OFFSET));
	g_RenderContainer.SetRenderCallback(g_pDeferredPipeline, _GET_LIB_INDEX_FROM_MAT_TYPE(DEFX_MATERIAL_TYPE_OFFSET));

	g_pDeferredParamPool->SetAccumulationBuffers(g_pZPos, g_pNormals, g_pMaterial);
}

void InitSceneDB()
{
	g_hSceneDB = InitializeModule(_W("SceneDB.dll"), &g_MemAllocFns);

	CreateSceneDBFn createSceneDB = (CreateSceneDBFn) GetFunction(g_hSceneDB, "CreateSceneDB");
	_DEBUG_ASSERT(createSceneDB);

	g_pSceneDB = createSceneDB();
	g_pSceneDB->Initialize();

    g_pSceneContainer = g_pSceneDB->GetResourceMgr().CreateOctantOrderLooseOctTree(6, 8.0f);
}

AHEntity* CreateEntity(BFXUMeshList* pMeshList, const gmtl::VecA3f& pos, const gmtl::VecA3f& dir)
{
    AHEntity* pEntity = _NEW AHEntity();

    AABox meshBounds;
    ComputeBounds(*pMeshList, meshBounds);

    AHSpatialComponent* pSpatial = _NEW AHSpatialComponent();
    pSpatial->SetLocalAABox(meshBounds);
    pSpatial->SetWorldPos(pos);
    pSpatial->SetWorldFacing(dir, gmtl::VecA3f(0.0f, 1.0f, 0.0f));

    pEntity->AddComponent(pSpatial);

    AHRenderComponent* pRender = _NEW AHRenderComponent();
    pRender->SetMeshList(pMeshList);
    pRender->SetLocalAABox(meshBounds);
    pRender->SetScene(g_pSceneContainer);

    pEntity->AddComponent(pRender);
    g_pEntityMgr->AddEntity(pEntity);

    return pEntity;
}

AHEntity* CreateEntity(const wchar* pPath, const gmtl::VecA3f& pos, const gmtl::VecA3f& dir)
{
	IByteBufferPtr pBuffer = g_AppCallback.GetFileData(pPath);
	BFXUMeshList* pMeshList = _NEW BFXUMeshList;
	LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *pMeshList);

	return CreateEntity(pMeshList, pos, dir);
}

AHEntity* CreateLightEntity(const gmtl::VecA3f& pos, const gmtl::Vec3f& color)
{
	eRTexFormat smFormat = g_pShadowFX->GetResourceMgr().GetSMCompatibleFormat();
	eRDepthFormat smDepthFormat = g_pShadowFX->GetResourceMgr().GetSMCompatibleDepthFormat();

	IByteBufferPtr pBuffer = g_AppCallback.GetFileData(_W("[msh]\\TestBakedSM\\pointlight.mls")); 
	BFXUMeshListPtr pMeshList = _NEW BFXUMeshList;
	LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *pMeshList);

	IRTextureCubePtr pPLSM1 = g_pRenderer->GetRResourceMgr().CreateTextureCubeRT(128, smFormat);
	IRTextureCubePtr pPLSMFinal = g_pRenderer->GetRResourceMgr().CreateTextureCubeRT(128, smFormat);
	IRDepthBufferPtr pPLDB = g_pRenderer->GetRResourceMgr().CreateDepthBuffer(128, 128, smDepthFormat, 0);
	AHEntity* pEntity = _NEW AHEntity();

	AABox meshBounds;
	ComputeBounds(*pMeshList, meshBounds);

	AHSpatialComponent* pSpatial = _NEW AHSpatialComponent();
	pSpatial->SetLocalAABox(meshBounds);
	pSpatial->SetWorldPos(pos);
	pSpatial->SetWorldFacing(gmtl::VecA3f(0.0f, 0.0f, 1.0f), gmtl::VecA3f(0.0f, 1.0f, 0.0f));

	pEntity->AddComponent(pSpatial);

	AHRenderComponent* pRender = _NEW AHRenderComponent();
	pRender->SetMeshList(pMeshList);
	pRender->SetLocalAABox(meshBounds);

	AHPointLightComponent* pPointLight = _NEW AHPointLightComponent();

	pPointLight->SetColor(color);
	pPointLight->SetRadius(40.0f);

	IDEFXLightGroupPtr pLightGroup = g_pDeferredFX->GetResourceMgr().CreateLightGroup();
	pLightGroup->AddRenderTarget(g_pRenderer->GetBackBufferColor());
	pLightGroup->SetDepthBuffer(g_pRenderer->GetBackBufferDepth());
	pLightGroup->SetViewport(0, 0, g_pRenderer->GetDisplayInfo().backBufferWidth, g_pRenderer->GetDisplayInfo().backBufferHeight);

	ISHFXCubePointLightPtr pShadowPL = g_pShadowFX->GetResourceMgr().CreateCubePointLight();
	pShadowPL->SetShadowMapParamsCubePoissonFilter(pPLSM1, pPLSMFinal, pPLDB, POISSON_8_SAMPLE, 0.03f, FALSE);
	//pShadowPL->SetShadowMapParams(pPLSMFinal, pPLDB);

	_LOOPi(NUM_THREADS)
	{
		pLightGroup->AddContext();
		pShadowPL->AddContext();
	}

	pPointLight->SetShadowPointLight(pShadowPL, TRUE);
	pPointLight->SetDeferredPointLight(pLightGroup, pRender);
	pPointLight->SetScene(g_pSceneContainer);

	pEntity->AddComponent(pPointLight);
	g_pEntityMgr->AddEntity(pEntity);

	return pEntity;
}

void InitAppHelpers()
{
	g_pMaterialCreateCallback = _NEW AHMaterialCreateCallbackImmed;

	g_pSceneRenderPhase = _NEW AHSceneRenderPhase(g_pRenderer, 
		g_pBaseFX, g_pDeferredFX, g_pShadowFX, g_pThreadPool, g_pBFXPipeline, g_pDeferredPipeline, g_pSHFXPipeline, g_pMaterialCreateCallback,
		g_pZPos, g_pNormals, g_pMaterial,
		g_pSceneContainer, &g_View, &g_Proj, NEAR_PLANE, FAR_PLANE);

	g_pEntityMgr = _NEW AHEntityManager(g_pThreadPool);

	//// Skydome
	//IByteBufferPtr pBuffer = g_AppCallback.GetFileData(_W("[msh]\\TestBakedSM\\skydome_radiance.mls")); 
	//BFXUMeshList* pMeshList = _NEW BFXUMeshList;
	//LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *pMeshList);
	//AHEntity* pEntity = _NEW AHEntity();

	//AABox meshBounds;
	//meshBounds.min[0] = -100000.0f;
	//meshBounds.min[1] = -100000.0f;
	//meshBounds.min[2] = -100000.0f;
	//meshBounds.max[0] = 100000.0f;
	//meshBounds.max[1] = 100000.0f;
	//meshBounds.max[2] = 100000.0f;

	//AHSpatialComponent* pSpatial = _NEW AHSpatialComponent();
	//pSpatial->SetLocalAABox(meshBounds);
	//g_pSkyDomeSpatial = pSpatial;

	//pEntity->AddComponent(pSpatial);
	//AHRenderComponent* pRender = _NEW AHRenderComponent();
	//pRender->SetMeshList(pMeshList);
	//pRender->SetLocalAABox(meshBounds);
	//pRender->SetScene(g_pSceneContainer);
	//pEntity->AddComponent(pRender);
	//g_pEntityMgr->AddEntity(pEntity);

	//// Dir light
	//eRTexFormat smFormat = g_pShadowFX->GetResourceMgr().GetSMCompatibleFormat();
	//eRDepthFormat smDepthFormat = g_pShadowFX->GetResourceMgr().GetSMCompatibleDepthFormat();

	//pBuffer = g_AppCallback.GetFileData(_W("[msh]\\TestBakedSM\\dirlight.mls")); 
	//pMeshList = _NEW BFXUMeshList;
	//LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *pMeshList);

	//pEntity = _NEW AHEntity();
	//ComputeBounds(*pMeshList, meshBounds);

	//AABox lightBounds;
	//lightBounds.min[0] = -100000.0f;
	//lightBounds.min[1] = -100000.0f;
	//lightBounds.min[2] = -100000.0f;

	//lightBounds.max[0] = 100000.0f;
	//lightBounds.max[1] = 100000.0f;
	//lightBounds.max[2] = 100000.0f;

	//pSpatial = _NEW AHSpatialComponent();
	//pSpatial->SetLocalAABox(lightBounds);
	//pSpatial->SetWorldPos(gmtl::VecA3f(0.0f, 0.0f, 0.0f));
	////gmtl::VecA3f dir(0.7f, 1.0f, -1.0f);
	//gmtl::VecA3f dir(-1.0f, 5.5f, -1.0f);
	//NormalizeVec(&dir, &dir);
	//pSpatial->SetWorldFacing(dir, gmtl::VecA3f(0.0f, 1.0f, 0.0f));

	//pEntity->AddComponent(pSpatial);

	//pRender = _NEW AHRenderComponent();
	//pRender->SetMeshList(pMeshList);
	//pRender->SetLocalAABox(meshBounds);

	//AHDirLightComponent* pDirLight = _NEW AHDirLightComponent();
	//pDirLight->SetLocalAABox(lightBounds);

	//float r = 0.6f;
	//float g = 0.5f;
	//float b = 0.4f;
	////float r = 0.14f;
	////float g = 0.145f;
	////float b = 0.2f;
	//pDirLight->SetColor(gmtl::Vec3f(r, g, b));
	//
	//IDEFXLightGroupPtr pLightGroup = g_pDeferredFX->GetResourceMgr().CreateLightGroup();
	//pLightGroup->AddRenderTarget(g_pRenderer->GetBackBufferColor());
	//pLightGroup->SetDepthBuffer(g_pRenderer->GetBackBufferDepth());
	//pLightGroup->SetViewport(0, 0, g_pRenderer->GetDisplayInfo().backBufferWidth, g_pRenderer->GetDisplayInfo().backBufferHeight);

	//IRTexture2DPtr pDirLightSM1 = g_pRenderer->GetRResourceMgr().CreateTexture2DRT(1024, 1024, 1, smFormat);// g_pShadowFX->GetResourceMgr().CreateCascadedDirLightExpShadowMap(1024);
	//IRTexture2DPtr pDirLightSMFinal = g_pRenderer->GetRResourceMgr().CreateTexture2DRT(2048, 2048, 1, smFormat);
	//IRDepthBufferPtr pDirLightDB = g_pRenderer->GetRResourceMgr().CreateDepthBuffer(1024, 1024, smDepthFormat, 0);
	//ISHFXCascadedDirLightPtr pShadowDL = g_pShadowFX->GetResourceMgr().CreateCascadedDirLight();
	//eBFXUPoissonBlurSamples blurSamples[4];
	//float spreadScales[4];
	//blurSamples[0] = POISSON_4_SAMPLE;
	//blurSamples[1] = POISSON_4_SAMPLE;
	//blurSamples[2] = POISSON_4_SAMPLE;
	//blurSamples[3] = POISSON_4_SAMPLE;
	//spreadScales[0] = 0.0010f;
	//spreadScales[1] = 0.00055f;
	//spreadScales[2] = 0.00025f;
	//spreadScales[3] = 0.0001f;
	//pShadowDL->SetShadowMapParamsPoissonFilter(pDirLightSM1, pDirLightSMFinal, pDirLightDB, blurSamples, spreadScales, FALSE);

	////pShadowDL->SetShadowMapParams(pDirLightSMFinal, pDirLightDB);

	//_LOOPi(NUM_THREADS)
	//{
	//	pLightGroup->AddContext();
	//	pShadowDL->AddContext();
	//}

	//pDirLight->SetShadowDirLight(pShadowDL, 1.5f, 800.0f, FALSE);
	//pDirLight->SetDeferredDirLight(pLightGroup, pRender);
	//pDirLight->SetScene(g_pSceneContainer);

	//pEntity->AddComponent(pDirLight);
	//g_pEntityMgr->AddEntity(pEntity);

	//g_pEntityMgr->UpdateComponentsTemp();
}

void CreateCube(int indexStart, float cubeSize, const gmtl::Vec3f& pos, float rotAmount)
{
    const static uint NUM_SUBDIVS = 60;
    const static float CUBE_SIZE = 2.0f;

    const wchar* pFileName = _W("[shd]\\TestBakedSM\\TestBakedSMSH.mgp");
    IFFilePtr pFile = g_pFileSystem->GetFile(pFileName);
    IByteBufferPtr pMatData = _NEW CByteBuffer(pFile->Length());
    AppendData(pMatData, pFile);

    gmtl::AxisAnglef rotY;
    rotY.set(rotAmount, gmtl::Vec3f(0.0f, 1.0f, 0.0f));
    gmtl::MatrixA44f rotMat;
    gmtl::identity(_CAST_MAT44(rotMat));
    gmtl::setRot(_CAST_MAT44(rotMat), rotY);

    // Make cube
    g_Meshes[indexStart] = CreatePlanePosY(g_pRenderer, cubeSize, cubeSize, NUM_SUBDIVS, NUM_SUBDIVS);
    IBFXMaterialGroupTemplatePtr pMatGroupTemplate = g_pBaseFX->GetResourceMgr().CreateMaterialGroupTemplate(NULL, pMatData, pFileName);
    g_Meshes[indexStart].pMatGroup = g_pBaseFX->GetResourceMgr().CreateMaterialGroup(NULL, pMatGroupTemplate, NULL, NULL, 0);
    gmtl::identity(_CAST_MAT44(g_MeshesWorld[indexStart]));
    gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[indexStart]), gmtl::Vec3f(0.0f, cubeSize * 0.5f, 0.0f));
    MatMatMult(&g_MeshesWorld[indexStart], &rotMat, &g_MeshesWorld[indexStart]);
    gmtl::Vec4f trans;
    gmtl::setTrans(trans, _CAST_MAT44(g_MeshesWorld[indexStart]));
    _CAST_VEC3(trans) += pos;
    gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[indexStart]), trans);

    g_Meshes[indexStart+1] = CreatePlaneNegY(g_pRenderer, cubeSize, cubeSize, NUM_SUBDIVS, NUM_SUBDIVS);
    pMatGroupTemplate = g_pBaseFX->GetResourceMgr().CreateMaterialGroupTemplate(NULL, pMatData, pFileName);
    g_Meshes[indexStart+1].pMatGroup = g_pBaseFX->GetResourceMgr().CreateMaterialGroup(NULL, pMatGroupTemplate, NULL, NULL, 0);
    gmtl::identity(_CAST_MAT44(g_MeshesWorld[indexStart+1]));
    gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[indexStart+1]), gmtl::Vec3f(0.0f, -cubeSize * 0.5f, 0.0f));
    MatMatMult(&g_MeshesWorld[indexStart+1], &rotMat, &g_MeshesWorld[indexStart+1]);
    gmtl::setTrans(trans, _CAST_MAT44(g_MeshesWorld[indexStart+1]));
    _CAST_VEC3(trans) += pos;
    gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[indexStart+1]), trans);

    g_Meshes[indexStart+2] = CreatePlanePosZ(g_pRenderer, cubeSize, cubeSize, NUM_SUBDIVS, NUM_SUBDIVS);
    pMatGroupTemplate = g_pBaseFX->GetResourceMgr().CreateMaterialGroupTemplate(NULL, pMatData, pFileName);
    g_Meshes[indexStart+2].pMatGroup = g_pBaseFX->GetResourceMgr().CreateMaterialGroup(NULL, pMatGroupTemplate, NULL, NULL, 0);
    gmtl::identity(_CAST_MAT44(g_MeshesWorld[indexStart+2]));
    gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[indexStart+2]), gmtl::Vec3f(0.0f, 0.0f, cubeSize * 0.5f));
    MatMatMult(&g_MeshesWorld[indexStart+2], &rotMat, &g_MeshesWorld[indexStart+2]);
    gmtl::setTrans(trans, _CAST_MAT44(g_MeshesWorld[indexStart+2]));
    _CAST_VEC3(trans) += pos;
    gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[indexStart+2]), trans);

    g_Meshes[indexStart+3] = CreatePlaneNegZ(g_pRenderer, cubeSize, cubeSize, NUM_SUBDIVS, NUM_SUBDIVS);
    pMatGroupTemplate = g_pBaseFX->GetResourceMgr().CreateMaterialGroupTemplate(NULL, pMatData, pFileName);
    g_Meshes[indexStart+3].pMatGroup = g_pBaseFX->GetResourceMgr().CreateMaterialGroup(NULL, pMatGroupTemplate, NULL, NULL, 0);
    gmtl::identity(_CAST_MAT44(g_MeshesWorld[indexStart+3]));
    gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[indexStart+3]), gmtl::Vec3f(0.0f, 0.0f, -cubeSize * 0.5f));
    MatMatMult(&g_MeshesWorld[indexStart+3], &rotMat, &g_MeshesWorld[indexStart+3]);
    gmtl::setTrans(trans, _CAST_MAT44(g_MeshesWorld[indexStart+3]));
    _CAST_VEC3(trans) += pos;
    gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[indexStart+3]), trans);

    g_Meshes[indexStart+4] = CreatePlanePosX(g_pRenderer, cubeSize, cubeSize, NUM_SUBDIVS, NUM_SUBDIVS);
    pMatGroupTemplate = g_pBaseFX->GetResourceMgr().CreateMaterialGroupTemplate(NULL, pMatData, pFileName);
    g_Meshes[indexStart+4].pMatGroup = g_pBaseFX->GetResourceMgr().CreateMaterialGroup(NULL, pMatGroupTemplate, NULL, NULL, 0);
    gmtl::identity(_CAST_MAT44(g_MeshesWorld[indexStart+4]));
    gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[indexStart+4]), gmtl::Vec3f(cubeSize * 0.5f, 0.0f, 0.0f));
    MatMatMult(&g_MeshesWorld[indexStart+4], &rotMat, &g_MeshesWorld[indexStart+4]);
    gmtl::setTrans(trans, _CAST_MAT44(g_MeshesWorld[indexStart+4]));
    _CAST_VEC3(trans) += pos;
    gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[indexStart+4]), trans);

    g_Meshes[indexStart+5] = CreatePlaneNegX(g_pRenderer, cubeSize, cubeSize, NUM_SUBDIVS, NUM_SUBDIVS);
    pMatGroupTemplate = g_pBaseFX->GetResourceMgr().CreateMaterialGroupTemplate(NULL, pMatData, pFileName);
    g_Meshes[indexStart+5].pMatGroup = g_pBaseFX->GetResourceMgr().CreateMaterialGroup(NULL, pMatGroupTemplate, NULL, NULL, 0);
    gmtl::identity(_CAST_MAT44(g_MeshesWorld[indexStart+5]));
    gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[indexStart+5]), gmtl::Vec3f(-cubeSize * 0.5f, 0.0f, 0.0f));
    MatMatMult(&g_MeshesWorld[indexStart+5], &rotMat, &g_MeshesWorld[indexStart+5]);
    gmtl::setTrans(trans, _CAST_MAT44(g_MeshesWorld[indexStart+5]));
    _CAST_VEC3(trans) += pos;
    gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[indexStart+5]), trans);

}

void InitBakedSM()
{
    g_pBakedSMCompute = _NEW BakedSMCompute;
    g_pBakedSMCompute->Initialize(g_pRenderer, &g_AppCallback);

    g_ParamContainer.SetParamCallback(g_pBakedSMCompute->GetBakedSMComputeParamPool(), _GET_LIB_INDEX(BAKEDSMCOMPUTE_EFFECT_PARAM_OFFSET));

    const wchar* pFileName = _W("[shd]\\TestBakedSM\\TestBakedSMSH.mgp");
    IFFilePtr pFile = g_pFileSystem->GetFile(pFileName);
    IByteBufferPtr pMatData = _NEW CByteBuffer(pFile->Length());
    AppendData(pMatData, pFile);

    const static uint NUM_SUBDIVS = 50;
    const static float PLANE_SIZE = 27.0f;
   
    g_Meshes[0] = CreatePlanePosY(g_pRenderer, PLANE_SIZE, PLANE_SIZE, NUM_SUBDIVS, NUM_SUBDIVS);
    IBFXMaterialGroupTemplatePtr pMatGroupTemplate = g_pBaseFX->GetResourceMgr().CreateMaterialGroupTemplate(NULL, pMatData, pFileName);
    g_Meshes[0].pMatGroup = g_pBaseFX->GetResourceMgr().CreateMaterialGroup(NULL, pMatGroupTemplate, NULL, NULL, 0);
    gmtl::identity(_CAST_MAT44(g_MeshesWorld[0]));
    gmtl::AxisAnglef rotY;
    rotY.set(0.2f, gmtl::Vec3f(1.0f, 1.0f, 0.0f));
    gmtl::MatrixA44f rotMat;
    gmtl::identity(_CAST_MAT44(rotMat));
    gmtl::setRot(_CAST_MAT44(rotMat), rotY);
    MatMatMult(&g_MeshesWorld[0], &rotMat, &g_MeshesWorld[0]);

    CreateCube(1, 4.0f, gmtl::Vec3f(1.0f, 1.0f, 0.0f), 0.8f);
    CreateCube(7, 1.0f, gmtl::Vec3f(-7.5f, 0.5f, 0.0f), 0.3f);
    CreateCube(13, 1.0f, gmtl::Vec3f(-3.0f, 1.7f, 4.0f), 1.2f);
    CreateCube(19, 1.3f, gmtl::Vec3f(-1.0f, 1.1f, 8.0f), 2.3f);

    g_SphereLight.set(0.0f, 8.0f, 0.0f, 1.0f);

    g_BakedSMCallback.Compute();
// gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[0]), gmtl::Vec4f(0.0f, -0.0f, 0.0f, 0.0f));
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
	InitSceneDB();

	const RDisplayInfo& dispInfo = g_pRenderer->GetDisplayInfo();

	gmtl::setPerspective(g_Proj, gmtl::Math::PI / 3.0f, ((float)dispInfo.backBufferWidth) / dispInfo.backBufferHeight, NEAR_PLANE, FAR_PLANE);
	_CAST_VEC3(g_EyePos) = gmtl::Vec3f(0.0f, 1.0f, 0.0f);
	_CAST_VEC3(g_EyeDir) = gmtl::Vec3f(0.0f, 0.0f, 1.0f);
	gmtl::VecA3f at;
	_CAST_VEC3(at) = _CAST_VEC3(g_EyePos) + _CAST_VEC3(g_EyeDir);
	_CAST_VEC3(g_EyeUpVec) = gmtl::Vec3f(0.0f, 1.0f, 0.0f);
	gmtl::setView(_CAST_MAT44(g_View), 
		_CAST_VEC3(g_EyePos), 
		_CAST_VEC3(at), 
		_CAST_VEC3(g_EyeUpVec));

	g_TimeDT = 0.0f;

	InitAppHelpers();
    InitBakedSM();

    g_pSparseGridCells = (AABox*) _ALIGNED_MALLOC(16, sizeof(AABox) * MAX_GRID_DEBUG);

	return TRUE;
}	

void Shutdown()
{
    _LOOPi(NUM_MESHES)
    {
        g_Meshes[i].pIB = NULL;
        g_Meshes[i].pMatGroup = NULL;
        g_Meshes[i].pVBGroup = NULL;
    }

    g_pBakedSMCompute = NULL;

    _ALIGNED_FREE(g_pSparseGridCells);

	g_pSceneRenderPhase = NULL;
	g_pEntityMgr = NULL;
	g_pMaterialCreateCallback = NULL;

	g_pShadowFX = NULL;
	g_pSHFXPipeline = NULL;
	g_pSHFXParamPool = NULL;

	g_pSceneContainer = NULL;
	g_pSceneDB = NULL;

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

	g_pMainClearGroup = NULL;

	g_pRenderer = NULL;

	g_pInput = NULL;
	g_pFileSystem->Close();
	g_pFileSystem = NULL;

	g_pThreadPool = NULL;
	g_pPlatform = NULL;

	g_pAllocator->DumpLeaks();

	//FreeLibrary( g_hScreenAmbOcclFX );
	ShutdownModule( g_hSceneDB );
	ShutdownModule( g_hDeferredFX );
	ShutdownModule( g_hShadowFX );
	ShutdownModule( g_hBaseFX );
	ShutdownModule( g_hMatGen );
	ShutdownModule( g_hRenderer );
	ShutdownModule( g_hFileSystem );
	ShutdownModule( g_hInput );
	ShutdownModule( g_hPlatform );
}

