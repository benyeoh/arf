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
	g_hFileSystem = ::LoadLibrary(_W("FileSystemWin32.dll"));
	if(!g_hFileSystem) return FALSE;

	CreateFileSystemFn CreateFileSystem = (CreateFileSystemFn) ::GetProcAddress(g_hFileSystem, "CreateFileSystem");

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
	g_hInput = ::LoadLibrary(_W("InputWin32.dll"));
	if(!g_hInput) return FALSE;

	CreateInputFn CreateInput = (CreateInputFn) ::GetProcAddress(g_hInput, "CreateInput");

	_DEBUG_ASSERT(CreateInput);

	g_pInput = CreateInput();
	g_pInput->Initialize();	

	g_FeedWindowsMessage = (FeedWindowsMessageFn) ::GetProcAddress(g_hInput, "FeedWindowsMessage");
	return TRUE;
}

boolean InitMatGen()
{
	g_hMatGen = ::LoadLibrary(_W("MaterialGenD3D.dll"));
	if(!g_hMatGen) return FALSE;

	CreateMaterialGenFn CreateMaterialGen = (CreateMaterialGenFn)::GetProcAddress(g_hMatGen, "CreateMaterialGen");

	_DEBUG_ASSERT(CreateMaterialGen);
	g_pMatGen = CreateMaterialGen();
	g_pMatGen->Initialize(g_RendererCallback);	

	return TRUE;
}

boolean InitBaseFX()
{
	g_hBaseFX = ::LoadLibrary(_W("BaseFX.dll"));
	if(!g_hBaseFX) return FALSE;

	CreateBaseFXFn CreateBaseFX = (CreateBaseFXFn)::GetProcAddress(g_hBaseFX, "CreateBaseFX");

	_DEBUG_ASSERT(CreateBaseFX );
	g_pBaseFX = CreateBaseFX();
	g_pBaseFX->Initialize(*g_pRenderer, *g_pMatGen, g_RendererCallback);

	return TRUE;
}

boolean InitRenderer()
{
	g_hRenderer = ::LoadLibrary(_W("RendererD3D.dll"));
	if(!g_hRenderer ) return FALSE;

	CreateRendererFn CreateRendererD3D = (CreateRendererFn)::GetProcAddress(g_hRenderer, "CreateRenderer");

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
	rInfo.pApplication		= &g_RendererCallback;

	if(!g_pRenderer->Initialize(rInfo))
		return FALSE;

	GetD3DDeviceFn	GetD3DDevice = (GetD3DDeviceFn)::GetProcAddress(g_hRenderer, "GetD3DDevice");
	g_pD3DDevice	= GetD3DDevice(g_pRenderer);

	return TRUE;
}

void InitDebugFX()
{
	g_pDebugFX = CreateDebugFX();
	g_pDebugFX->Initialize(*g_pRenderer, g_RendererCallback);
}

void InitFontFX()
{
	g_pFontFX = CreateFontFX();
	g_pFontFX->Initialize(*g_pRenderer, g_RendererCallback);

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
	g_hPlatform = ::LoadLibrary(_W("PlatformWin32.dll"));
	if(!g_hPlatform ) return FALSE;

	CreatePlatformFn createPlatform = (CreatePlatformFn)::GetProcAddress(g_hPlatform, "CreatePlatform");

	_DEBUG_ASSERT(createPlatform);

	g_pPlatform = createPlatform();

	return TRUE;
}

void InitLights()
{
	g_HemiLight.skyColor	= gmtl::Vec4f(0.2f, 0.4f, 0.5f, 1.0f);
	g_HemiLight.groundColor = gmtl::Vec4f(0.475f, 0.2f, 0.1f, 1.0f);

	g_DirLight.color		= gmtl::Vec4f(0.8f, 0.74f, 0.66f, 1.0f);
	g_DirLight.dir			= gmtl::Vec3f(-2.0f, -1.0f, -1.0f);
	g_DirLight.invSize		= 1.0f;
}

void InitDeferredFX()
{
	g_hDeferredFX = ::LoadLibrary(_W("DeferredFX.dll"));
	_DEBUG_ASSERT(g_hDeferredFX != NULL);

	CreateDeferredFXFn CreateDeferredFX = (CreateDeferredFXFn)::GetProcAddress(g_hDeferredFX, "CreateDeferredFX");

	_DEBUG_ASSERT(CreateDeferredFX);

	g_pDeferredFX = CreateDeferredFX();
	g_pDeferredFX->Initialize(*g_pBaseFX, *g_pRenderer, g_RendererCallback);
}

void InitSHExpLightingFX()
{
	g_hSHExpLightingFX = ::LoadLibrary(_W("SHExpLightingFX.dll"));
	_DEBUG_ASSERT(g_hSHExpLightingFX!= NULL);

	CreateSHExpLightingFXFn CreateSHExpLightingFX = (CreateSHExpLightingFXFn)::GetProcAddress(g_hSHExpLightingFX, "CreateSHExpLightingFX");

	_DEBUG_ASSERT(CreateSHExpLightingFX);

	g_pSHExpLightingFX = CreateSHExpLightingFX();
	g_pSHExpLightingFX->Initialize(*g_pDeferredFX, *g_pBaseFX, *g_pRenderer, g_RendererCallback);

	IByteBufferPtr pBuffer = g_RendererCallback.GetFileData(_W("[tex]\\uffizi_cross_tiny.dds"));
	g_pTexCube[0] = g_pRenderer->GetRResourceMgr().LoadTextureCube(NULL, pBuffer);

	//pBuffer = g_RendererCallback.GetFileData(_W("[tex]\\rnl_cross.dds"));
	//g_pTexCube[1] = g_pRenderer->GetRResourceMgr().LoadTextureCube(NULL, pBuffer);

	//pBuffer = g_RendererCallback.GetFileData(_W("[tex]\\grace_cross.dds"));
	//g_pTexCube[2] = g_pRenderer->GetRResourceMgr().LoadTextureCube(NULL, pBuffer);

	//pBuffer = g_RendererCallback.GetFileData(_W("[tex]\\stpeters_cross.dds"));
	//g_pTexCube[3] = g_pRenderer->GetRResourceMgr().LoadTextureCube(NULL, pBuffer);

	// g_pAmbientFX->GetResourceMgr().CompressAmbientCubeMap(g_pTexCube[0], &g_Coeffs[0]);
	//g_pAmbientFX->GetResourceMgr().CompressAmbientCubeMap(g_pTexCube[1], &g_Coeffs[1]);
	//g_pAmbientFX->GetResourceMgr().CompressAmbientCubeMap(g_pTexCube[2], &g_Coeffs[2]);
	//g_pAmbientFX->GetResourceMgr().CompressAmbientCubeMap(g_pTexCube[3], &g_Coeffs[3]);

	//g_pSHExpLightingFX->GetResourceMgr().CreateUnitSHCoeffs(g_UnitCoeffs, 128, NUM_BANDS);
	//g_pSHExpLightingFX->GetResourceMgr().CreateCosineLobeCoeffs(g_CosineLobeCoeffs, 128, NUM_BANDS);

	IRTextureCube* pTextureDest[NUM_BANDS*3];
	_LOOPi(NUM_BANDS*3)
	{
		pTextureDest[i] = g_pRenderer->GetRResourceMgr().CreateTextureCube(NULL, 32, 1, TEXF_A32B32G32R32F, TEXU_DEFAULT);
	}

	IRTextureCube* pYlmTex[NUM_BANDS];
	_LOOPi(NUM_BANDS)
	{
		pYlmTex[i] = g_pRenderer->GetRResourceMgr().CreateTextureCube(NULL, 64, 1, TEXF_A32B32G32R32F, TEXU_DEFAULT);
	}

	g_pVisSphereTex = g_pRenderer->GetRResourceMgr().CreateTexture2D(NULL, 256, 1, 1, TEXF_A32B32G32R32F, TEXU_DEFAULT);
	g_pABTableTex = g_pRenderer->GetRResourceMgr().CreateTexture2D(NULL, 256, 1, 1, TEXF_G32R32F, TEXU_DEFAULT);

	g_pSHExpLightingFX->GetResourceMgr().CreateRadianceCosineLobeSHCoeffs(pTextureDest, g_pTexCube[0], NUM_BANDS);

	_LOOPi(NUM_BANDS*3)
	{
		g_pRadianceCosineTex[i] = pTextureDest[i];
	}

	_LOOPi(NUM_BANDS)
	{
		g_pYlmTex[i] = pYlmTex[i];
	}


	g_pSHExpLightingFX->GetResourceMgr().CreateYlmCoeffs(pYlmTex, NUM_BANDS);
	g_pSHExpLightingFX->GetResourceMgr().CreateSphereVisibilityCoeffs(g_pVisSphereTex, g_pABTableTex, NUM_BANDS);

	//g_CurCoeff = g_Coeffs[0];
}

void InitMesh()
{
	g_pRoomSHMeshList = _NEW BFXUMeshList;

	IByteBufferPtr pBuffer = g_RendererCallback.GetFileData(_W("[msh]\\roomSHNoTex.mls")); 
	LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *g_pRoomSHMeshList);
	
	pBuffer = g_RendererCallback.GetFileData(_W("[shd]\\ObjectAccum.mgp"));
	IBFXMaterialGroupTemplate* pTemplate = g_pBaseFX->GetResourceMgr().CreateMaterialGroupTemplate(NULL, pBuffer, _W("[shd]\\ObjectAccum.mgp"));
	IBFXMaterialGroup* pGroup = g_pBaseFX->GetResourceMgr().CreateMaterialGroup(NULL, pTemplate, NULL, NULL, 0);

	_LOOPi(g_pRoomSHMeshList->meshes.size())
	{
		g_pRoomSHMeshList->meshes[i].pMatGroup = pGroup;
	}

	_LOOPi(NUM_MESHES)
	{
		g_pBackupNoTexMat[i] = pGroup;
	}

	pBuffer = g_RendererCallback.GetFileData(_W("[msh]\\rahzar.mls"));
	BFXUMeshList* pMeshList = _NEW BFXUMeshList;
	LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *pMeshList);
	g_pMeshes[0] = pMeshList;
	gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[0]), gmtl::Vec3f(0.0f, -1.0f, 0.0f));

	pBuffer = g_RendererCallback.GetFileData(_W("[msh]\\tokka.mls"));
	pMeshList = _NEW BFXUMeshList;
	LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *pMeshList);
	g_pMeshes[1] = pMeshList;
	gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[1]), gmtl::Vec3f(1.0f, -1.0f, 0.0f));

	pBuffer = g_RendererCallback.GetFileData(_W("[msh]\\rahzar.spt"));
	SEFXUSphereTree* pSphereTree = NULL;
	LoadSphereTree(pBuffer, 0, &pSphereTree);
	g_pVisTree[0] = _NEW SEFXUVisibilityTree(pSphereTree);

	pBuffer = g_RendererCallback.GetFileData(_W("[msh]\\tokka.spt"));
	LoadSphereTree(pBuffer, 0, &pSphereTree);
	g_pVisTree[1] = _NEW SEFXUVisibilityTree(pSphereTree);

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
	InitSHExpLightingFX();
	//g_pCube = _NEW TestCube();
	//g_pCube->Initialize();

	InitLights();

	const RDisplayInfo& dispInfo = g_pRenderer->GetDisplayInfo();

	gmtl::setPerspective(g_Proj, gmtl::Math::PI / 3.0f, ((float)dispInfo.backBufferWidth) / dispInfo.backBufferHeight, 0.01f, 450.0f);
	_CAST_VEC3(g_EyePos) = gmtl::Vec3f(-3.0f, 0.0f, 33.0f);
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

	return TRUE;
}	

void Shutdown()
{
	_LOOPi(NUM_MESHES)
	{
		g_pMeshes[i] = 0;
		g_pVisTree[i] = 0;
		g_pBackupNoTexMat[i] = 0;
	}

	_DELETE( g_pRoomSHMeshList );

	g_pABTableTex = NULL;

	_LOOPi(NUM_BANDS*3)
	{
		g_pRadianceCosineTex[i] = NULL;
	}

	_LOOPi(NUM_BANDS)
	{
		g_pYlmTex[i] = NULL;
	}

	g_pVisSphereTex = NULL;

	g_pTexCube[0] = NULL;
	g_pTexCube[1] = NULL;
	g_pTexCube[2] = NULL;
	g_pTexCube[3] = NULL;

	g_Floor.pIB			= NULL;
	g_Floor.pMatGroup	= NULL;
	g_Floor.pVBGroup	= NULL;

	//g_Sphere.pIB		= NULL;
	//g_Sphere.pMatGroup	= NULL;
	//g_Sphere.pVBGroup	= NULL;

	g_Soldier.pIB		= NULL;
	g_Soldier.pMatGroup	= NULL;
	g_Soldier.pVBGroup	= NULL;

	g_Bandit.pIB		= NULL;
	g_Bandit.pMatGroup	= NULL;
	g_Bandit.pVBGroup	= NULL;

	g_pDebugFX = NULL;

	g_pSHExpLightingFX = NULL;
	g_pDeferredFX = NULL;

	g_pVerdanaFontFace = NULL;
	g_pCourierFontFace = NULL;
	g_pFontFX = NULL;
	g_pBaseFX = NULL;
	g_pMatGen = NULL;

	g_pRenderer = NULL;

	g_pInput = NULL;
	g_pFileSystem->Close();
	g_pFileSystem = NULL;

	g_pPlatform = NULL;

	FreeLibrary( g_hSHExpLightingFX );
	FreeLibrary( g_hDeferredFX );
	FreeLibrary( g_hBaseFX );
	FreeLibrary( g_hMatGen );
	FreeLibrary( g_hRenderer );
	FreeLibrary( g_hFileSystem );
	FreeLibrary( g_hInput );
	FreeLibrary( g_hPlatform );
}

