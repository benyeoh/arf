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

	g_pThreadPool->Initialize(8, pThreads, NUM_THREADS);

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

    switch(DB_USAGE)
    {
    case USE_ARRAY:
	    g_pSceneContainer = g_pSceneDB->GetResourceMgr().CreateArray();
	    g_pSceneLightFlockContainer = g_pSceneDB->GetResourceMgr().CreateArray();
	    g_pSceneEntityFlockContainer = g_pSceneDB->GetResourceMgr().CreateArray();
        break;
    case USE_SPARSE_LOOSE_OCTREE:
        g_pSceneContainer = g_pSceneDB->GetResourceMgr().CreateSparseLooseOctTree(6, 8.0f, 512);
        g_pSceneLightFlockContainer = g_pSceneDB->GetResourceMgr().CreateSparseLooseOctTree(6, 8.0f, 512);
        g_pSceneEntityFlockContainer = g_pSceneDB->GetResourceMgr().CreateSparseLooseOctTree(6, 8.0f, 512);
        break;
    case USE_OCTANT_ORDER_LOOSE_OCTREE:
		g_pSceneContainer = g_pSceneDB->GetResourceMgr().CreateOctantOrderLooseOctTree(6, 8.0f);
        g_pSceneLightFlockContainer = g_pSceneDB->GetResourceMgr().CreateOctantOrderLooseOctTree(6, 8.0f);
        g_pSceneEntityFlockContainer = g_pSceneDB->GetResourceMgr().CreateOctantOrderLooseOctTree(6, 8.0f);
        break;
    }
}

boolean InitSHPRTCompute()
{
	g_hSHPRTCompute = InitializeModule(_W("SHPRTCompute.dll"), &g_MemAllocFns);
	if(!g_hSHPRTCompute) return FALSE;

	CreateSHPRTComputeFn createSHPRTCompute = (CreateSHPRTComputeFn) GetFunction(g_hSHPRTCompute, "CreateSHPRTCompute");

	_DEBUG_ASSERT(createSHPRTCompute);
	g_pSHPRTCompute = createSHPRTCompute();
	g_pSHPRTCompute->Initialize(*g_pRenderer, g_AppCallback);

	return TRUE;
}

FlockingAIComponent* CreateFlockingAILights()
{
	FlockingAIComponent* pFlockingAI = _NEW FlockingAIComponent(&g_TimeDT, g_pSceneLightFlockContainer);
	pFlockingAI->SetSpeed(9.0f);
	pFlockingAI->SetSteering(1.4f);
	pFlockingAI->SetSearchRadius(100.0f);

	AABoxAvoidanceBehavior* pBoxAvoid = _NEW AABoxAvoidanceBehavior;
	pBoxAvoid->SetStrength(3.5f);
	pBoxAvoid->SetThreshold(0.0f);
	AABox bounds;
	bounds.min.set(-AREA_ROOT, 14.0f, -AREA_ROOT);
	bounds.max.set(AREA_ROOT, 20.0f, AREA_ROOT);
	pBoxAvoid->SetAABox(bounds);
	pFlockingAI->AddBehavior(pBoxAvoid);

	SeparationBehavior* pSep = _NEW SeparationBehavior;
	pSep->SetRadius(40.0f);
	pSep->SetStrength(1.0f);
	pFlockingAI->AddBehavior(pSep);

	CohesionBehavior* pCoh	= _NEW CohesionBehavior;
	pCoh->SetRadius(100.0f);
	pCoh->SetStrength(0.1f);
	pFlockingAI->AddBehavior(pCoh);

	return pFlockingAI;
}

FlockingAIComponent* CreateFlockingAIEntities()
{
	FlockingAIComponent* pFlockingAI = _NEW FlockingAIComponent(&g_TimeDT, g_pSceneEntityFlockContainer);
	pFlockingAI->SetSpeed(12.0f);
	pFlockingAI->SetSteering(2.4f);
	pFlockingAI->SetSearchRadius(10.0f);
	pFlockingAI->SetAxes(gmtl::VecA3f(1.0f, 0.0f, 1.0f));

	AABoxAvoidanceBehavior* pBoxAvoid = _NEW AABoxAvoidanceBehavior;
	pBoxAvoid->SetStrength(2.0f);
	pBoxAvoid->SetThreshold(0.0f);
	AABox bounds;
	bounds.min.set(-AREA_ROOT, -10000.0f, -AREA_ROOT);
	bounds.max.set(AREA_ROOT, 10000.0f, AREA_ROOT);
	pBoxAvoid->SetAABox(bounds);
	pFlockingAI->AddBehavior(pBoxAvoid);

	SeparationBehavior* pSep = _NEW SeparationBehavior;
	pSep->SetRadius(8.0f);
	pSep->SetStrength(2.f);
	pFlockingAI->AddBehavior(pSep);

	CohesionBehavior* pCoh	= _NEW CohesionBehavior;
	pCoh->SetRadius(10.0f);
	pCoh->SetStrength(0.1f);
	pFlockingAI->AddBehavior(pCoh);

    AlignmentBehavior* pAlign	= _NEW AlignmentBehavior;
    pAlign->SetRadius(9.0f);
    pAlign->SetStrength(0.2f);
    pFlockingAI->AddBehavior(pAlign);

	return pFlockingAI;
}

CircularMovementAIComponent* CreateCircularMovementAI(const gmtl::VecA3f& pos)
{
    CircularMovementAIComponent* pAI = _NEW CircularMovementAIComponent(&g_TimeDT);

    gmtl::VecA3f tetherPos; 
    float radius = (((float) rand()) / RAND_MAX) * 10.0f;
    float x = ((((float) rand()) / RAND_MAX) * 2.0f - 1.0f) * radius;
    float z = ((((float) rand()) / RAND_MAX) * 2.0f - 1.0f) * radius;
    float y = ((((float) rand()) / RAND_MAX) * 2.0f - 1.0f) * radius;
    VecVecAdd(&tetherPos, &pos, &gmtl::VecA3f(x, y, z));

    gmtl::VecA3f axis;
    axis[0] = ((((float) rand()) / RAND_MAX) * 2.0f - 1.0f);
    axis[1] = ((((float) rand()) / RAND_MAX) * 2.0f - 1.0f);
    axis[2] = ((((float) rand()) / RAND_MAX) * 2.0f - 1.0f);
    NormalizeVec(&axis, &axis);

    pAI->SetRotateAxis(axis);
    pAI->SetTetherPos(tetherPos);
    pAI->SetSpeed( (((float) rand()) / RAND_MAX) * 3.0f );

    return pAI;
}

AHEntity* CreateEntity(BFXUMeshList* pMeshList, const gmtl::VecA3f& pos, const gmtl::VecA3f& dir, boolean isUseFlocking=FALSE)
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
#ifndef _USE_PRT_VERSION
    if(isUseFlocking)
    {
        //pEntity->AddComponent( CreateCircularMovementAI(pos) );
        pEntity->AddComponent( CreateFlockingAIEntities() );
    }
#endif
    g_pEntityMgr->AddEntity(pEntity);

    return pEntity;
}

AHEntity* CreateEntity(const wchar* pPath, const gmtl::VecA3f& pos, const gmtl::VecA3f& dir, boolean isUseFlocking=FALSE)
{
	IByteBufferPtr pBuffer = g_AppCallback.GetFileData(pPath);
	BFXUMeshList* pMeshList = _NEW BFXUMeshList;
	LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *pMeshList);

	return CreateEntity(pMeshList, pos, dir, isUseFlocking);
}

AHEntity* CreateLightEntity(const gmtl::VecA3f& pos, const gmtl::Vec3f& color)
{
	eRTexFormat smFormat = g_pShadowFX->GetResourceMgr().GetSMCompatibleFormat();
	eRDepthFormat smDepthFormat = g_pShadowFX->GetResourceMgr().GetSMCompatibleDepthFormat();

	IByteBufferPtr pBuffer = g_AppCallback.GetFileData(_W("[msh]\\TestAppHelpers\\pointlight.mls")); 
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
#ifndef _USE_PRT_VERSION
	pEntity->AddComponent( CreateFlockingAILights() );
#endif
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
	
#ifdef _USE_PRT_VERSION
	CreateEntity(_W("[msh]\\TestAppHelpers\\floor_radiance.mls"), gmtl::VecA3f(0.0f, 0.0f, 0.0f), gmtl::VecA3f(0.0f, 0.0f, 1.0f));
#else

    const static float GRID_SIZE = 79.0f;
    float numFloorsRoot = (2 * AREA_ROOT / GRID_SIZE) + 1;
	float curX = -GRID_SIZE * (numFloorsRoot / 2);
	float curY = -GRID_SIZE * (numFloorsRoot / 2);

    BFXUMeshList* pCachedMeshList = NULL;

	_LOOPi(numFloorsRoot)
	{
		_LOOPj(numFloorsRoot)
		{
            if(!pCachedMeshList)
            {
			    AHEntity* pEntity = CreateEntity(_W("[msh]\\TestAppHelpers\\floor.mls"), gmtl::VecA3f(curX, 0.0f, curY), gmtl::VecA3f(0.0f, 0.0f, 1.0f));
                AHRenderComponent* pComp = (AHRenderComponent*) pEntity->GetComponent(COMP_RENDER);
                pCachedMeshList = pComp->GetMeshList();
            }
            else
            {
                CreateEntity(pCachedMeshList, gmtl::VecA3f(curX, 0.0f, curY), gmtl::VecA3f(0.0f, 0.0f, 1.0f));                
            }

			curX += GRID_SIZE;
		}

		curX = -GRID_SIZE * (numFloorsRoot / 2);
		curY += GRID_SIZE;
	}
#endif
	//IByteBufferPtr pBuffer = g_AppCallback.GetFileData(_W("[msh]\\TestAppHelpers\\floor_radiance.mls")); 
	//BFXUMeshList* pMeshList = _NEW BFXUMeshList;
	//LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *pMeshList);
	//AHEntity* pEntity = _NEW AHEntity();

	//AABox meshBounds;
	//ComputeBounds(*pMeshList, meshBounds);

	//gmtl::VecA3f pos;
	//pos[0] = 0.0f;
	//pos[1] = 0.0f;
	//pos[2] = 0.0f;

	//AHSpatialComponent* pSpatial = _NEW AHSpatialComponent();
	//pSpatial->SetLocalAABox(meshBounds);
	//pSpatial->SetWorldPos(pos);
	//pSpatial->SetWorldFacing(gmtl::VecA3f(0.0f, 0.0f, 1.0f), gmtl::VecA3f(0.0f, 1.0f, 0.0f));
	//pEntity->AddComponent(pSpatial);
	//AHRenderComponent* pRender = _NEW AHRenderComponent();
	//pRender->SetMeshList(pMeshList);
	//pRender->SetLocalAABox(meshBounds);
	//pRender->SetScene(g_pSceneContainer);
	//pEntity->AddComponent(pRender);
	//g_pEntityMgr->AddEntity(pEntity);

	// Skydome
	IByteBufferPtr pBuffer = g_AppCallback.GetFileData(_W("[msh]\\TestAppHelpers\\skydome_radiance.mls")); 
	BFXUMeshList* pMeshList = _NEW BFXUMeshList;
	LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *pMeshList);
	AHEntity* pEntity = _NEW AHEntity();

	AABox meshBounds;
	meshBounds.min[0] = -100000.0f;
	meshBounds.min[1] = -100000.0f;
	meshBounds.min[2] = -100000.0f;
	meshBounds.max[0] = 100000.0f;
	meshBounds.max[1] = 100000.0f;
	meshBounds.max[2] = 100000.0f;

	AHSpatialComponent* pSpatial = _NEW AHSpatialComponent();
	pSpatial->SetLocalAABox(meshBounds);
	g_pSkyDomeSpatial = pSpatial;

	pEntity->AddComponent(pSpatial);
	AHRenderComponent* pRender = _NEW AHRenderComponent();
	pRender->SetMeshList(pMeshList);
	pRender->SetLocalAABox(meshBounds);
	pRender->SetScene(g_pSceneContainer);
	pEntity->AddComponent(pRender);
	g_pEntityMgr->AddEntity(pEntity);

#ifdef _USE_PRT_VERSION
	CreateEntity(_W("[msh]\\TestAppHelpers\\cube_room_radiance_2.mls"), gmtl::VecA3f(0.0f, -0.4f, 0.0f), gmtl::VecA3f(0.0f, 0.0f, 1.0f));
	CreateEntity(_W("[msh]\\TestAppHelpers\\cube_room_radiance.mls"), gmtl::VecA3f(-35.0f, -0.4f, 23.0f), gmtl::VecA3f(1.0f, 0.0f, 0.0f));
	CreateEntity(_W("[msh]\\TestAppHelpers\\statue_radiance_2.mls"), gmtl::VecA3f(-21.0f, 0.0f, 14.8f), gmtl::VecA3f(1.0f, 0.0f, 0.0f));
	CreateEntity(_W("[msh]\\TestAppHelpers\\woman_head_radiance.mls"), gmtl::VecA3f(-7.0f, -0.5f, 14.0f), gmtl::VecA3f(0.0f, 0.0f, 1.0f));
	//CreateEntity(_W("[msh]\\TestAppHelpers\\woman_head_radiance.mls"), gmtl::VecA3f(6.7f, 0.0f, 32.4f), gmtl::VecA3f(0.707f, 0.0f, 0.707f));
	//CreateEntity(_W("[msh]\\TestAppHelpers\\king_kong_radiance.mls"), gmtl::VecA3f(3.0f, 0.0f, 4.8f), gmtl::VecA3f(0.0f, 0.0f, 1.0f));
	CreateEntity(_W("[msh]\\TestAppHelpers\\king_kong_radiance.mls"), gmtl::VecA3f(10.0f, 0.0f, 16.8f), gmtl::VecA3f(0.0f, 0.0f, 1.0f));

#else
    pCachedMeshList = NULL;

	if(NUM_ENTITIES > 0)
	{
		//pBuffer = g_AppCallback.GetFileData(_W("[msh]\\TestAppHelpers\\bandit.mls"));
		//pMeshList = _NEW BFXUMeshList;
		//LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *pMeshList);
		//g_pMeshes[0] = pMeshList;
		//gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[0]), gmtl::Vec3f(0.0f, 3.0f, 0.0f));

        srand((uint) (g_pPlatform->GetTimer().GetTime() * 10000000.0));
		_LOOPi(NUM_ENTITIES)
		{
			float x = ((((float) rand()) / RAND_MAX) * 2.0f - 1.0f) * AREA_ROOT;
			float z = ((((float) rand()) / RAND_MAX) * 2.0f - 1.0f) * AREA_ROOT;
            float y = ((((float) rand()) / RAND_MAX)) * 100.0f;

			gmtl::VecA3f pos;
			pos[0] = x;
			pos[1] = 0.0f;
			pos[2] = z;

			//int xMod = i % 16;
			//int zMod = i / 16;
			//pos[0] = FastCeiling(xMod * 0.5f) * ((xMod % 2) * 2.0f - 1.0f) * 5.0f;
			//pos[1] = 0.0f;
			//pos[2] = FastCeiling(zMod * 0.5f) * ((zMod % 2) * 2.0f - 1.0f) * 5.0f;

            if(!pCachedMeshList)
            {
                AHEntity* pEntity = CreateEntity(_W("[msh]\\TestAppHelpers\\bandit_inst.mls"), pos, gmtl::VecA3f(0.0f, 0.0f, 1.0f), TRUE);
                AHRenderComponent* pComp = (AHRenderComponent*) pEntity->GetComponent(COMP_RENDER);
                pCachedMeshList = pComp->GetMeshList();
            }
            else
            {
                CreateEntity(pCachedMeshList, pos, gmtl::VecA3f(0.0f, 0.0f, 1.0f), TRUE);
            }		
		}
	}

	// Test one light
	//srand((uint) (g_pPlatform->GetTimer().GetTime() * 1000000000.0));
	//float r = (((float) rand()) / RAND_MAX) * 0.8f + 0.2f;
	//float g = (((float) rand()) / RAND_MAX) * 0.8f + 0.2f;
	//float b = (((float) rand()) / RAND_MAX) * 0.8f + 0.2f;
	//CreateLightEntity(gmtl::VecA3f(2.0f, 8.0f, -2.0f), gmtl::Vec3f(1.0f, 1.0f, 1.0f));

	_LOOPi(NUM_POINT_LIGHTS)
	{
		srand((uint) (g_pPlatform->GetTimer().GetTime() * 1000000000.0));
		float r = (((float) rand()) / RAND_MAX) * 0.7f + 0.3f;
		float g = (((float) rand()) / RAND_MAX) * 0.7f + 0.3f;
		float b = (((float) rand()) / RAND_MAX) * 0.7f + 0.3f;
		gmtl::VecA3f pos;
		//int xMod = i % 4;
		//int zMod = i / 4;
		//pos[0] = FastCeiling(xMod * 0.5f) * ((xMod % 2) * 2.0f - 1.0f) * 20.0f;
		//pos[1] = 6.0f;
		//pos[2] = FastCeiling(zMod * 0.5f) * ((zMod % 2) * 2.0f - 1.0f) * 20.0f + 3.0f;

		float x = ((((float) rand()) / RAND_MAX) * 2.0f - 1.0f) * 30.0f;
		float z = ((((float) rand()) / RAND_MAX) * 2.0f - 1.0f) * 30.0f;
		float y = ((((float) rand()) / RAND_MAX)) * 10.0f;

		pos[0] = x;
		pos[1] = y;
		pos[2] = z;

		CreateLightEntity(pos, gmtl::Vec3f(r, g, b));

		//pBuffer = g_AppCallback.GetFileData(_W("[msh]\\TestAppHelpers\\pointlight.mls")); 
		//pMeshList = _NEW BFXUMeshList;
		//LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *pMeshList);

		//srand((uint) (g_pPlatform->GetTimer().GetTime() * 1000000000.0));
	
		//IRTextureCubePtr pPLSM1 = g_pRenderer->GetRResourceMgr().CreateTextureCubeRT(128, smFormat);
		//IRTextureCubePtr pPLSMFinal = g_pRenderer->GetRResourceMgr().CreateTextureCubeRT(128, smFormat);
		//IRDepthBufferPtr pPLDB = g_pRenderer->GetRResourceMgr().CreateDepthBuffer(128, 128, smDepthFormat, 0);
		//_LOOPi(NUM_POINT_LIGHTS)
		//{
		//	AHEntity* pEntity = _NEW AHEntity();

		//	AABox meshBounds;
		//	ComputeBounds(*pMeshList, meshBounds);

		//	gmtl::VecA3f pos;
		//	int xMod = i % 4;
		//	int zMod = i / 4;
		//	pos[0] = FastCeiling(xMod * 0.5f) * ((xMod % 2) * 2.0f - 1.0f) * 12.0f;
		//	pos[1] = 6.0f;
		//	pos[2] = FastCeiling(zMod * 0.5f) * ((zMod % 2) * 2.0f - 1.0f) * 12.0f + 2.0f;

		//	AHSpatialComponent* pSpatial = _NEW AHSpatialComponent();
		//	pSpatial->SetLocalAABox(meshBounds);
		//	pSpatial->SetWorldPos(pos);
		//	pSpatial->SetWorldFacing(gmtl::VecA3f(0.0f, 0.0f, 1.0f), gmtl::VecA3f(0.0f, 1.0f, 0.0f));

		//	pEntity->AddComponent(pSpatial);

		//	AHRenderComponent* pRender = _NEW AHRenderComponent();
		//	pRender->SetMeshList(pMeshList);
		//	pRender->SetLocalAABox(meshBounds);
		//
		//	AHPointLightComponent* pPointLight = _NEW AHPointLightComponent();

		//	float r = (((float) rand()) / RAND_MAX) * 0.8f + 0.2f;
		//	float g = (((float) rand()) / RAND_MAX) * 0.8f + 0.2f;
		//	float b = (((float) rand()) / RAND_MAX) * 0.8f + 0.2f;
		//	pPointLight->SetColor(gmtl::Vec3f(r, g, b));
		//	pPointLight->SetRadius(20.0f);

		//	IDEFXLightGroupPtr pLightGroup = g_pDeferredFX->GetResourceMgr().CreateLightGroup();
		//	pLightGroup->AddRenderTarget(g_pRenderer->GetBackBufferColor());
		//	pLightGroup->SetDepthBuffer(g_pRenderer->GetBackBufferDepth());
		//	pLightGroup->SetViewport(0, 0, g_pRenderer->GetDisplayInfo().backBufferWidth, g_pRenderer->GetDisplayInfo().backBufferHeight);
		//
		//	ISHFXCubePointLightPtr pShadowPL = g_pShadowFX->GetResourceMgr().CreateCubePointLight();
		//	pShadowPL->SetShadowMapParamsCubePoissonFilter(pPLSM1, pPLSMFinal, pPLDB, POISSON_8_SAMPLE, 0.03f, FALSE);
		//	//pShadowPL->SetShadowMapParams(pPLSMFinal, pPLDB);

		//	_LOOPi(NUM_THREADS)
		//	{
		//		pLightGroup->AddContext();
		//		pShadowPL->AddContext();
		//	}

		//	pPointLight->SetShadowPointLight(pShadowPL, TRUE);
		//	pPointLight->SetDeferredPointLight(pLightGroup, pRender);
		//	pPointLight->SetScene(g_pSceneContainer);

		//	pEntity->AddComponent(pPointLight);
		//	//pEntity->AddComponent( CreateFlockingAILights() );

		//	g_pEntityMgr->AddEntity(pEntity);
		//}
	}
#endif	

	// Dir light
	eRTexFormat smFormat = g_pShadowFX->GetResourceMgr().GetSMCompatibleFormat();
	eRDepthFormat smDepthFormat = g_pShadowFX->GetResourceMgr().GetSMCompatibleDepthFormat();

	pBuffer = g_AppCallback.GetFileData(_W("[msh]\\TestAppHelpers\\dirlight.mls")); 
	pMeshList = _NEW BFXUMeshList;
	LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *pMeshList);

	pEntity = _NEW AHEntity();
	ComputeBounds(*pMeshList, meshBounds);

	AABox lightBounds;
	lightBounds.min[0] = -100000.0f;
	lightBounds.min[1] = -100000.0f;
	lightBounds.min[2] = -100000.0f;

	lightBounds.max[0] = 100000.0f;
	lightBounds.max[1] = 100000.0f;
	lightBounds.max[2] = 100000.0f;

	pSpatial = _NEW AHSpatialComponent();
	pSpatial->SetLocalAABox(lightBounds);
	pSpatial->SetWorldPos(gmtl::VecA3f(0.0f, 0.0f, 0.0f));
	//gmtl::VecA3f dir(0.7f, 1.0f, -1.0f);
	gmtl::VecA3f dir(-1.0f, 5.5f, -1.0f);
	NormalizeVec(&dir, &dir);
	pSpatial->SetWorldFacing(dir, gmtl::VecA3f(0.0f, 1.0f, 0.0f));

	pEntity->AddComponent(pSpatial);

	pRender = _NEW AHRenderComponent();
	pRender->SetMeshList(pMeshList);
	pRender->SetLocalAABox(meshBounds);

	AHDirLightComponent* pDirLight = _NEW AHDirLightComponent();
	pDirLight->SetLocalAABox(lightBounds);

	float r = 0.6f;
	float g = 0.5f;
	float b = 0.4f;
	//float r = 0.14f;
	//float g = 0.145f;
	//float b = 0.2f;
	pDirLight->SetColor(gmtl::Vec3f(r, g, b));
	
	IDEFXLightGroupPtr pLightGroup = g_pDeferredFX->GetResourceMgr().CreateLightGroup();
	pLightGroup->AddRenderTarget(g_pRenderer->GetBackBufferColor());
	pLightGroup->SetDepthBuffer(g_pRenderer->GetBackBufferDepth());
	pLightGroup->SetViewport(0, 0, g_pRenderer->GetDisplayInfo().backBufferWidth, g_pRenderer->GetDisplayInfo().backBufferHeight);

	IRTexture2DPtr pDirLightSM1 = g_pRenderer->GetRResourceMgr().CreateTexture2DRT(1024, 1024, 1, smFormat);// g_pShadowFX->GetResourceMgr().CreateCascadedDirLightExpShadowMap(1024);
	IRTexture2DPtr pDirLightSMFinal = g_pRenderer->GetRResourceMgr().CreateTexture2DRT(2048, 2048, 1, smFormat);
	IRDepthBufferPtr pDirLightDB = g_pRenderer->GetRResourceMgr().CreateDepthBuffer(1024, 1024, smDepthFormat, 0);
	ISHFXCascadedDirLightPtr pShadowDL = g_pShadowFX->GetResourceMgr().CreateCascadedDirLight();
	eBFXUPoissonBlurSamples blurSamples[4];
	float spreadScales[4];
	blurSamples[0] = POISSON_4_SAMPLE;
	blurSamples[1] = POISSON_4_SAMPLE;
	blurSamples[2] = POISSON_4_SAMPLE;
	blurSamples[3] = POISSON_4_SAMPLE;
	spreadScales[0] = 0.0010f;
	spreadScales[1] = 0.00055f;
	spreadScales[2] = 0.00025f;
	spreadScales[3] = 0.0001f;
	pShadowDL->SetShadowMapParamsPoissonFilter(pDirLightSM1, pDirLightSMFinal, pDirLightDB, blurSamples, spreadScales, FALSE);

	//pShadowDL->SetShadowMapParams(pDirLightSMFinal, pDirLightDB);

	_LOOPi(NUM_THREADS)
	{
		pLightGroup->AddContext();
		pShadowDL->AddContext();
	}

	pDirLight->SetShadowDirLight(pShadowDL, 1.8f, 700.0f, FALSE);
	pDirLight->SetDeferredDirLight(pLightGroup, pRender);

#ifdef USE_DIR_LIGHT
    pDirLight->SetScene(g_pSceneContainer);
	pEntity->AddComponent(pDirLight);
	g_pEntityMgr->AddEntity(pEntity);
#endif
	g_pEntityMgr->UpdateComponentsTemp();

#ifdef _USE_PRT_VERSION
	// Init PRT Compute
	pBuffer = g_AppCallback.GetFileData(_W("[msh]\\TestAppHelpers\\prtcompute_dirlight.mls")); 
	BFXUMeshList* pPRTDirLightMeshList = _NEW BFXUMeshList;
	LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *pPRTDirLightMeshList);

	pBuffer = g_AppCallback.GetFileData(_W("[msh]\\TestAppHelpers\\prtcompute_pointlight.mls")); 
	BFXUMeshList* pPRTPointLightMeshList = _NEW BFXUMeshList;
	LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *pPRTPointLightMeshList);

	IPThreadPool* pComputePhasePool = g_pPlatform->GetResourceMgr().CreateThreadPool();
	IPThread* pComputePhaseThreads[NUM_PRTCOMPUTE_PHASE_THREADS];

	_LOOPi(NUM_PRTCOMPUTE_PHASE_THREADS)
	{
		pComputePhaseThreads[i] = g_pPlatform->GetResourceMgr().CreateThread();
		pComputePhaseThreads[i]->Initialize();
	}

	pComputePhasePool->Initialize(7, pComputePhaseThreads, NUM_PRTCOMPUTE_PHASE_THREADS);

	IPThreadPool* pComputeProcessPool = g_pPlatform->GetResourceMgr().CreateThreadPool();
	IPThread* pComputeProcessThreads[NUM_PRTCOMPUTE_PROCESS_THREADS];

	_LOOPi(NUM_PRTCOMPUTE_PROCESS_THREADS)
	{
		pComputeProcessThreads[i] = g_pPlatform->GetResourceMgr().CreateThread();
		pComputeProcessThreads[i]->Initialize();
	}

	pComputeProcessPool->Initialize(10, pComputeProcessThreads, NUM_PRTCOMPUTE_PROCESS_THREADS);
	g_pAHPRTCompute = _NEW AHScenePRTCompute(g_pRenderer, g_pBaseFX, g_pDeferredFX, g_pShadowFX, g_pSHPRTCompute, pComputePhasePool, pComputeProcessPool, pPRTDirLightMeshList, pPRTPointLightMeshList);
	g_pAHPRTCompute->Compute(g_pSceneContainer, 100.0f, 20.0f, 6, 0.0175f, 0.75f, 2, 0.45f, TRUE);
#endif
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
	InitSHPRTCompute();

	//// Evaluate test BRDF
	//BRDFCallback brdfCallback;
	//BRDFData brdf;
	//_LOOPi(3)
	//{
	//	brdf.lobe[i].Cx = 0.0f;
	//	brdf.lobe[i].Cy = 0.0f;
	//	brdf.lobe[i].Cz = 0.0f;
	//	brdf.lobe[i].n = 1.0f;
	//}

	//// Brush metal like material
	//brdf.lobe[0].Cx = 0.2f;
	//brdf.lobe[0].Cy = 1.0f;
	//brdf.lobe[0].Cz = 0.0f;
	//brdf.lobe[0].n = 2.1f;
	//brdf.nDotLScale = 0.9f;
	//brdf.diffuseScale = 0.0f;
	//brdf.lobeScale = 1.0f;
	//brdf.metalScale = 0.8f;
	//const wchar* pStr1 = _W("[tex]\\TestAppHelpers\\TestBrushMetalLobe_1.dds");
	//const wchar* pStr2 = _W("[tex]\\TestAppHelpers\\TestBrushMetalLobe_2.dds");

	////// Velvet like material
	////brdf.lobe[0].Cx = 0.99f;
	////brdf.lobe[0].Cy = 0.9f;
	////brdf.lobe[0].Cz = 0.0f;
	////brdf.lobe[0].n = 2.25f;
	////brdf.nDotLScale = 1.0f;
	////brdf.diffuseScale = 0.205f;
	////brdf.lobeScale = 1.0f;
	////brdf.metalScale = 0.0f;
	////const wchar* pStr1 = _W("[tex]\\TestAppHelpers\\TestAnisoLobe_1.dds");
	////const wchar* pStr2 = _W("[tex]\\TestAppHelpers\\TestAnisoLobe_2.dds");

	////// Phong with offspecular
	////brdf.lobe[1].Cx = -0.93f;
	////brdf.lobe[1].Cy = 0.0f;
	////brdf.lobe[1].Cz = 0.0f;
	////brdf.lobe[1].n = 4.0f;

	////brdf.lobe[0].Cx = -1.0f;
	////brdf.lobe[0].Cy = 0.0f;
	////brdf.lobe[0].Cz = 1.0f;
	////brdf.lobe[0].n = 3.0f;
	////brdf.nDotLScale = 0.6f;
	////brdf.diffuseScale = 0.0f;
	////brdf.lobeScale = 1.0f;
	////brdf.metalScale = 1.0f;
	////const wchar* pStr1 = _W("[tex]\\TestAppHelpers\\TestPhongOffSpecularLobe_1.dds");
	////const wchar* pStr2 = _W("[tex]\\TestAppHelpers\\TestPhongOffSpecularLobe_2.dds");

	//brdfCallback.SetBRDFParameters(brdf);
	//brdfCallback.NormalizeBRDFParameters();

	//float lobeScale = brdfCallback.GetLobeScale();
	//float diffuseScale = brdfCallback.GetDiffuseScale();

	//_TRACE(_W("\n>>> Diffuse Scale: [ %f ], Lobe Scale: [ %f ]\n"), diffuseScale, lobeScale);

	//SHPRTLocationEntry* pLocEntries = _NEW SHPRTLocationEntry[64 * 64];
	//SHPRTDataEntry* pDataEntries = _NEW SHPRTDataEntry[64 * 64];
	//g_pSHPRTCompute->ComputeBRDFLocationEntries(pLocEntries, 64, 64);
	//g_pSHPRTCompute->ComputeBRDFTransfer(pLocEntries, pDataEntries, 64 * 64, brdfCallback);

	//IRTexture2D* pBRDFTex1;
	//IRTexture2D* pBRDFTex2;
	//g_pSHPRTCompute->CreateBRDFTexturesZH(pLocEntries, pDataEntries, 64 * 64, 64, 64, &pBRDFTex1, &pBRDFTex2);

	//IByteBufferPtr pTempSaveTex1 = _NEW CByteBuffer();
	//IByteBufferPtr pTempSaveTex2 = _NEW CByteBuffer();
	//g_pRenderer->GetRResourceMgr().SaveTexture2D(pBRDFTex1, pTempSaveTex1);
	//g_pRenderer->GetRResourceMgr().SaveTexture2D(pBRDFTex2, pTempSaveTex2);

	//IFFilePtr pFile = g_pFileSystem->GetFile(pStr1);
	//pFile->Open(FO_WRITE_ONLY);
	//pFile->Write(pTempSaveTex1->GetData(), pTempSaveTex1->GetDataLength());
	//pFile->Close();

	//pFile = g_pFileSystem->GetFile(pStr2);
	//pFile->Open(FO_WRITE_ONLY);
	//pFile->Write(pTempSaveTex2->GetData(), pTempSaveTex2->GetDataLength());
	//pFile->Close();

	//pBRDFTex1->Release();
	//pBRDFTex2->Release();
	//_DELETE_ARRAY(pLocEntries);
	//_DELETE_ARRAY(pDataEntries);

	const RDisplayInfo& dispInfo = g_pRenderer->GetDisplayInfo();

	gmtl::setPerspective(g_Proj, gmtl::Math::PI / 3.0f, ((float)dispInfo.backBufferWidth) / dispInfo.backBufferHeight, NEAR_PLANE, FAR_PLANE);
	_CAST_VEC3(g_EyePos) = gmtl::Vec3f(13.0f, 19.5f, 33.0f);
	_CAST_VEC3(g_EyeDir) = gmtl::Vec3f(-0.5f, -0.6f, -1.f);
	gmtl::VecA3f at;
	_CAST_VEC3(at) = _CAST_VEC3(g_EyePos) + _CAST_VEC3(g_EyeDir);
	_CAST_VEC3(g_EyeUpVec) = gmtl::Vec3f(0.0f, 1.0f, 0.0f);
	gmtl::setView(_CAST_MAT44(g_View), 
		_CAST_VEC3(g_EyePos), 
		_CAST_VEC3(at), 
		_CAST_VEC3(g_EyeUpVec));

	g_TimeDT = 0.0f;

	InitAppHelpers();

    g_pSparseGridCells = (AABox*) _ALIGNED_MALLOC(16, sizeof(AABox) * MAX_GRID_DEBUG);

	return TRUE;
}	

void Shutdown()
{
    _ALIGNED_FREE(g_pSparseGridCells);

	g_pAHPRTCompute = NULL;

	g_pSceneRenderPhase = NULL;
	g_pEntityMgr = NULL;
	g_pMaterialCreateCallback = NULL;

	g_pShadowFX = NULL;
	g_pSHFXPipeline = NULL;
	g_pSHFXParamPool = NULL;

	g_pSceneEntityFlockContainer = NULL;
	g_pSceneLightFlockContainer = NULL;
	g_pSceneContainer = NULL;
	g_pSceneDB = NULL;

	g_pSHPRTCompute = NULL;

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
	ShutdownModule( g_hSHPRTCompute );
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

