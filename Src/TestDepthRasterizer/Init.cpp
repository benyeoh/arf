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

	//float val1;
	//const float orig = -5.0f;
	//const float tileSize = 32.0f;
	//const float rcpTileSize = 1.0f / tileSize;
	//__m128 ss = _mm_set_ss(orig);
	//__m128 rcpTile = _mm_set_ss(rcpTileSize);
	//ss = _mm_mul_ss(ss, rcpTile);
	//int sign = *((int*) &orig);
	//sign = sign >> 31;
	//int floor = _mm_cvtt_ss2si(ss);
	//floor += sign;
	//ss = _mm_cvt_si2ss(ss, floor);
	//__m128 tile = _mm_set_ss(tileSize);
	//ss = _mm_mul_ss(ss, tile);
	//_mm_store_ss(&val1, ss);
	//_TRACE(_W("val1 after: %f\n"), val1);

	//__m128 floorOffset = _mm_set_ss(-0.5f);
	//ss = _mm_set_ss(orig);
	//ss = _mm_mul_ss(ss, rcpTile);
	//__m128 minXSnap = _mm_add_ss(ss, ss);
	//minXSnap = _mm_add_ss(minXSnap, floorOffset);
	//int temp = _mm_cvt_ss2si(minXSnap);
	//temp >>= 1;
	//minXSnap = _mm_cvt_si2ss(minXSnap, temp);
	//minXSnap = _mm_mul_ss(minXSnap, tile);
	//_mm_store_ss(&val1, minXSnap);	
	//_TRACE(_W("val3 after: %f\n"), val1);

	//__m128 ceilOffset = _mm_set_ss(-0.5f);
	//ss = _mm_set_ss(orig);
	//ss = _mm_mul_ss(ss, rcpTile);
	//__m128 maxXSnap = _mm_add_ss(ss, ss);
	//maxXSnap = _mm_sub_ss(ceilOffset, maxXSnap);
	//temp = _mm_cvt_ss2si(maxXSnap);
	//temp >>= 1;	
	//temp = -temp;
	//maxXSnap = _mm_cvt_si2ss(maxXSnap, temp);
	//maxXSnap = _mm_mul_ss(maxXSnap, tile);
	//_mm_store_ss(&val1, maxXSnap);	
	//_TRACE(_W("val2 after: %f\n"), val1);


	//const float TOP_TILE_F = (float) 32 * 2.0f;
	//const float RCP_TOP_TILE_F = 1.0f / TOP_TILE_F;
	//const float MID_TILE_F = (float) 4 * 2.0f;
	//const float RCP_MID_TILE_F = 1.0f / MID_TILE_F;
	//const float FINAL_TILE_F = 2.0f;
	//const float RCP_FINAL_TILE_F = 1.0f / FINAL_TILE_F;

	////const _ALIGN(16) float SNAP_MUL[] = {1.0f, 1.0f, -1.0f, -1.0f};
	//const _ALIGN(16) float ROUND_OFFSET[] = {-0.5f, -0.5f, 1.5f, 1.5f};
	//const _ALIGN(16) float PIXEL_OFFSET[] = {0.5f, 0.5f, -0.5f, -0.5f};
	////const _ALIGN(16) float TOP_TILE_MAX_ADD = {0.0f, 0.0f, TOP_TILE_F, TOP_TILE_F};
	////const _ALIGN(16) float MID_TILE_MAX_ADD = {0.0f, 0.0f, MID_TILE_F, MID_TILE_F};
	////const _ALIGN(16) float FINAL_TILE_MAX_ADD = {0.0f, 0.0f, FINAL_TILE_F, FINAL_TILE_F};

	//const __m128 roundOffset = _mm_load_ps(ROUND_OFFSET);
	//const __m128 pixelOffset = _mm_load_ps(PIXEL_OFFSET);
	////const __m128 snapMul = _mm_load_ps(SNAP_MUL);
	//
	//const __m128 rcpTopTile = _mm_set_ps1(RCP_TOP_TILE_F);
	//const __m128 topTile = _mm_set_ps1(TOP_TILE_F);
	//const __m128 rcpMidTile = _mm_set_ps1(RCP_MID_TILE_F);
	//const __m128 midTile = _mm_set_ps1(MID_TILE_F);
	//const __m128 rcpFinalTile = _mm_set_ps1(RCP_FINAL_TILE_F);
	//const __m128 finalTile = _mm_set_ps1(FINAL_TILE_F);

	//__m128 minX = _mm_set_ss(0.5f);
	//__m128 minY = _mm_set_ss(0.5f);
	//__m128 maxX = _mm_set_ss(4.5f);
	//__m128 maxY = _mm_set_ss(8.1f);

	//__m128 bbMin = _mm_unpacklo_ps(minX, minY);
	//__m128 bbMax = _mm_unpacklo_ps(maxX, maxY);
	//__m128 bb = _mm_movelh_ps(bbMin, bbMax);

	//// Top
	//__m128 snapVal = _mm_mul_ps(bb, rcpTopTile);
	//snapVal = _mm_add_ps(snapVal, snapVal);
	//snapVal = _mm_add_ps(roundOffset, snapVal);
	//__m128i roundVal = _mm_cvtps_epi32(snapVal);
	//roundVal = _mm_srai_epi32(roundVal, 1);
	//snapVal = _mm_cvtepi32_ps(roundVal);
	//snapVal = _mm_mul_ps(snapVal, topTile);
	//snapVal = _mm_add_ps(snapVal, pixelOffset);

	//_ALIGN(16) float vals[4];
	//_mm_store_ps(vals, snapVal);

	//_TRACE(_W("%f, %f, %f, %f\n"), vals[0], vals[1], vals[2], vals[3]);

	//// Mid
	//snapVal = _mm_mul_ps(bb, rcpMidTile);
	//snapVal = _mm_add_ps(snapVal, snapVal);
	//snapVal = _mm_add_ps(roundOffset, snapVal);
	//roundVal = _mm_cvtps_epi32(snapVal);
	//roundVal = _mm_srai_epi32(roundVal, 1);
	//snapVal = _mm_cvtepi32_ps(roundVal);
	//snapVal = _mm_mul_ps(snapVal, midTile);
	//snapVal = _mm_add_ps(snapVal, pixelOffset);

	//_mm_store_ps(vals, snapVal);

	//_TRACE(_W("%f, %f, %f, %f\n"), vals[0], vals[1], vals[2], vals[3]);

	//// Final
	//snapVal = _mm_mul_ps(bb, rcpFinalTile);
	//snapVal = _mm_add_ps(snapVal, snapVal);
	//snapVal = _mm_add_ps(roundOffset, snapVal);
	//roundVal = _mm_cvtps_epi32(snapVal);
	//roundVal = _mm_srai_epi32(roundVal, 1);
	//snapVal = _mm_cvtepi32_ps(roundVal);
	//snapVal = _mm_mul_ps(snapVal, finalTile);
	//snapVal = _mm_add_ps(snapVal, pixelOffset);

	//_mm_store_ps(vals, snapVal);

	//_TRACE(_W("%f, %f, %f, %f\n"), vals[0], vals[1], vals[2], vals[3]);

	//float curX = 0.5f;
	//const float endX = 191.5f;
	//const float tileSize = 64.0f;
	//while(curX < (endX - tileSize))
	//{
	//	_TRACE(_W("%f\n"), curX);
	//	curX += tileSize;
	//}

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

	g_pHWGroup = g_pRenderer->GetRResourceMgr().CreateRenderGroup(NULL);
	g_pHWGroup->AddRenderTarget(g_pRenderer->GetBackBufferColor());
	g_pHWGroup->SetDepthBuffer(g_pRenderer->GetBackBufferDepth());
	g_pHWGroup->SetViewport(0, 0, 
							g_pRenderer->GetBackBufferColor()->GetWidth() / 2, 
							g_pRenderer->GetBackBufferColor()->GetHeight());
	
	g_pRasterizeTex = g_pRenderer->GetRResourceMgr().CreateTexture2D(NULL, RASTERIZE_BUFFER_W, RASTERIZE_BUFFER_H, 1, TEXF_R8, TEXU_DYNAMIC);
	g_pRasterizeBuffer = _NEW byte[RASTERIZE_BUFFER_W * RASTERIZE_BUFFER_H];
	g_pRasterizeDepthBuffer = (float*) _ALIGNED_MALLOC(16, RASTERIZE_BUFFER_W * RASTERIZE_BUFFER_H * sizeof(float));
	g_pFastDepthClearBuffer = (uint*) _ALIGNED_MALLOC(16, FAST_DEPTH_CLEAR_W * FAST_DEPTH_CLEAR_H * sizeof(uint));

	g_pSWGroup = g_pRenderer->GetRResourceMgr().CreateRenderGroup(NULL);
	g_pSWGroup->AddRenderTarget(g_pRenderer->GetBackBufferColor());
	g_pSWGroup->SetDepthBuffer(g_pRenderer->GetBackBufferDepth());
	g_pSWGroup->SetViewport(g_pRenderer->GetBackBufferColor()->GetWidth() / 2, 0, 
							g_pRenderer->GetBackBufferColor()->GetWidth() / 2, 
							g_pRenderer->GetBackBufferColor()->GetHeight());
	
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
	//g_pFontFX->SetDepthBuffer(g_pRenderer->GetBackBufferDepth());

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

	g_pThreadPool = g_pPlatform->GetResourceMgr().CreateThreadPool();

	IPThread* pThreads[NUM_THREADS];
	
	_LOOPi(NUM_THREADS)
	{
		pThreads[i] = g_pPlatform->GetResourceMgr().CreateThread();
		pThreads[i]->Initialize();

		g_pThreads[i] = g_pPlatform->GetResourceMgr().CreateThread();
		g_pThreads[i]->Initialize();
	}

	g_pThreadPool->Initialize(4, pThreads, NUM_THREADS);

	//SetThreadAffinityMask(GetCurrentThread(), 1);
	return TRUE;
}

void InitMesh()
{
	RVertexDesc triDesc[] =
	{
		{ VDU_POSITION, 0 },
		//{ VDU_TEXCOORDF2, 0 },
		{ VDU_END, 0 },
	};

	IRVertexBuffer* pVB = g_pRenderer->GetRResourceMgr().CreateVertexBuffer(NULL, triDesc, 4, BUF_DEFAULT);
	
	g_pTriangleVBGroup	= g_pRenderer->GetRResourceMgr().CreateVertexBufferGroup(NULL);
	g_pTriangleIB		= g_pRenderer->GetRResourceMgr().CreateIndexBuffer(NULL, 6, BUF_DEFAULT);
	g_pTriangleVBGroup->AddVertexBuffer(pVB);

	byte* pTriData = pVB->Lock(0, 4);
	*((gmtl::Vec3f*)pTriData) = gmtl::Vec3f(-1.0f, 1.0f, 0.0f);
	pTriData += GetVertexDataSize(VDU_POSITION);
	*((gmtl::Vec3f*)pTriData) = gmtl::Vec3f(1.0f, 1.0f, 0.0f);
	pTriData += GetVertexDataSize(VDU_POSITION);
	*((gmtl::Vec3f*)pTriData) = gmtl::Vec3f(-1.0f, -1.0f, 0.0f);
	pTriData += GetVertexDataSize(VDU_POSITION);
	*((gmtl::Vec3f*)pTriData) = gmtl::Vec3f(0.5f, -0.5f, 0.0f);
	pTriData += GetVertexDataSize(VDU_POSITION);
	
	pVB->Unlock();

	ushort* pTriIndices = g_pTriangleIB->Lock(0, 6);
	*pTriIndices = (ushort) 0;		pTriIndices++;
	*pTriIndices = (ushort) 1;		pTriIndices++;
	*pTriIndices = (ushort) 2;		pTriIndices++;
	*pTriIndices = (ushort) 1;		pTriIndices++;
	*pTriIndices = (ushort) 3;		pTriIndices++;
	*pTriIndices = (ushort) 2;		pTriIndices++;
	
	g_pTriangleIB->Unlock();

	IByteBufferPtr pBuffer = g_AppCallback.GetFileData(_W("[shd]\\TestDepthRasterizer\\ScreenTriangle.fx"));
	g_pTriangleEffect = g_pRenderer->GetRResourceMgr().LoadEffect(
		NULL, 
		pBuffer,
		NULL,
		NULL,
		0);


	RVertexDesc quadDesc[] =
	{
		{ VDU_POSITION, 0 },
		{ VDU_TEXCOORDF2, 0 },
		{ VDU_END, 0 },
	};

	pVB = g_pRenderer->GetRResourceMgr().CreateVertexBuffer(NULL, quadDesc, 4, BUF_DEFAULT);
	
	g_pQuadVBGroup		= g_pRenderer->GetRResourceMgr().CreateVertexBufferGroup(NULL);
	g_pQuadIB			= g_pRenderer->GetRResourceMgr().CreateIndexBuffer(NULL, 6, BUF_DEFAULT);
	g_pQuadVBGroup->AddVertexBuffer(pVB);

	byte* pQuadData = pVB->Lock(0, 4);
	*((gmtl::Vec3f*)pQuadData) = gmtl::Vec3f(-1.0f, 1.0f, 0.0f);
	pQuadData += GetVertexDataSize(VDU_POSITION);
	*((gmtl::Vec2f*)pQuadData) = gmtl::Vec2f(0.0f, 0.0f);
	pQuadData += GetVertexDataSize(VDU_TEXCOORDF2);

	*((gmtl::Vec3f*)pQuadData) = gmtl::Vec3f(1.0f, 1.0f, 0.0f);
	pQuadData += GetVertexDataSize(VDU_POSITION);
	*((gmtl::Vec2f*)pQuadData) = gmtl::Vec2f(1.0f, 0.0f);
	pQuadData += GetVertexDataSize(VDU_TEXCOORDF2);
	
	*((gmtl::Vec3f*)pQuadData) = gmtl::Vec3f(-1.0f, -1.0f, 0.0f);
	pQuadData += GetVertexDataSize(VDU_POSITION);
	*((gmtl::Vec2f*)pQuadData) = gmtl::Vec2f(0.0f, 1.0f);
	pQuadData += GetVertexDataSize(VDU_TEXCOORDF2);

	*((gmtl::Vec3f*)pQuadData) = gmtl::Vec3f(1.0f, -1.0f, 0.0f);
	pQuadData += GetVertexDataSize(VDU_POSITION);
	*((gmtl::Vec2f*)pQuadData) = gmtl::Vec2f(1.0f, 1.0f);
	pQuadData += GetVertexDataSize(VDU_TEXCOORDF2);
	pVB->Unlock();

	ushort* pQuadIndices = g_pQuadIB->Lock(0, 6);
	*pQuadIndices = (ushort) 0;		pQuadIndices++;
	*pQuadIndices = (ushort) 1;		pQuadIndices++;
	*pQuadIndices = (ushort) 2;		pQuadIndices++;
	*pQuadIndices = (ushort) 1;		pQuadIndices++;
	*pQuadIndices = (ushort) 3;		pQuadIndices++;
	*pQuadIndices = (ushort) 2;		pQuadIndices++;
	
	g_pQuadIB->Unlock();

	REffectParam param;
	param.SetTexture2D(g_pRasterizeTex);
	uint semantic = 55;
	pBuffer = g_AppCallback.GetFileData(_W("[shd]\\TestDepthRasterizer\\ScreenTexturedQuad.fx"));
	g_pQuadTexEffect = g_pRenderer->GetRResourceMgr().LoadEffect(
		NULL, 
		pBuffer,
		&param,
		&semantic,
		1);

	RVertexDesc cubeDesc[] =
	{
		{ VDU_POSITION, 0 },
		{ VDU_TEXCOORDF2, 0 },
		{ VDU_END, 0 },
	};

	pVB = g_pRenderer->GetRResourceMgr().CreateVertexBuffer(NULL, cubeDesc, 24, BUF_DEFAULT);
	
	g_pCubeVBGroup		= g_pRenderer->GetRResourceMgr().CreateVertexBufferGroup(NULL);
	g_pCubeIB			= g_pRenderer->GetRResourceMgr().CreateIndexBuffer(NULL, 36, BUF_DEFAULT);
	g_pCubeVBGroup->AddVertexBuffer(pVB);

	byte* pCubeData = pVB->Lock(0, 24);
	memcpy(pCubeData, (void*) g_CubeVertices, sizeof(g_CubeVertices));
	pVB->Unlock();

	ushort* pCubeIndices = g_pCubeIB->Lock(0, 36);
	memcpy(pCubeIndices, (void*) g_CubeIndices, sizeof(g_CubeIndices));
	g_pCubeIB->Unlock();

	param.SetFloat(FAR_PLANE);
	semantic = 1;
	pBuffer = g_AppCallback.GetFileData(_W("[shd]\\TestDepthRasterizer\\CubeDepth.fx"));
	g_pCubeEffect = g_pRenderer->GetRResourceMgr().LoadEffect(
		NULL, 
		pBuffer,
		&param,
		&semantic,
		1);
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
	InitInput();
	InitFontFX();
	InitDebugFX();
	InitManualJobDispatch();

	const RDisplayInfo& dispInfo = g_pRenderer->GetDisplayInfo();

	gmtl::setPerspective(g_Proj, gmtl::Math::PI / 3.0f, ((float)dispInfo.backBufferWidth) / dispInfo.backBufferHeight, 0.01f, FAR_PLANE);
	_CAST_VEC3(g_EyePos) = gmtl::Vec3f(0.0f, 0.0f, 2.0f);
	_CAST_VEC3(g_EyeDir) = gmtl::Vec3f(0.0f, 0.0f, -1.f);
	gmtl::VecA3f at;
	_CAST_VEC3(at) = _CAST_VEC3(g_EyePos) + _CAST_VEC3(g_EyeDir);
	_CAST_VEC3(g_EyeUpVec) = gmtl::Vec3f(0.0f, 1.0f, 0.0f);
	//gmtl::identity( _CAST_MAT44(g_View) );
	gmtl::setView(_CAST_MAT44(g_View), 
		_CAST_VEC3(g_EyePos), 
		_CAST_VEC3(at), 
		_CAST_VEC3(g_EyeUpVec));

	g_TimeDT = 0.0f;

	InitMesh();

	gmtl::Math::seedRandom( (uint) (g_pPlatform->GetTimer().GetTime() * 10000.0) );
	_LOOPi(NUM_CUBES)
	{
		gmtl::identity(_CAST_MAT44(g_CubeWorld[i]));
		float x = gmtl::Math::rangeRandom(-4.0f, 4.0f);
		float y = gmtl::Math::rangeRandom(-4.0f, 4.0f);
		float z = gmtl::Math::rangeRandom(-10.0f, -6.0f);
		gmtl::setTrans(_CAST_MAT44(g_CubeWorld[i]), gmtl::Vec3f(x, y, z));
		g_CubeRotAxis[i] = gmtl::Vec3f(
			gmtl::Math::rangeRandom(-1.0f, 1.0f), 
			gmtl::Math::rangeRandom(-1.0f, 1.0f), 
			gmtl::Math::rangeRandom(-1.0f, 1.0f));
		gmtl::normalize(g_CubeRotAxis[i]);
	}

	return TRUE;
}	

void Shutdown()
{
	ShutdownManualJobDispatch();

	_LOOPi(NUM_THREADS)
	{
		g_pThreads[i]->Join();
		g_pThreads[i] = NULL;
	}

	g_pThreadPool = NULL;

	_DELETE_ARRAY(g_pRasterizeBuffer);
	_ALIGNED_FREE(g_pRasterizeDepthBuffer);
	_ALIGNED_FREE(g_pFastDepthClearBuffer);

	g_pRasterizeTex = NULL;
	g_pTriangleEffect = NULL;
	g_pTriangleVBGroup = NULL;
	g_pTriangleIB = NULL;
	g_pQuadVBGroup = NULL;
	g_pQuadIB = NULL;
	g_pQuadTexEffect = NULL;
	g_pCubeVBGroup = NULL;
	g_pCubeIB = NULL;
	g_pCubeEffect = NULL;

    g_pDebugFX = NULL;

	g_pVerdanaFontFace = NULL;
	g_pCourierFontFace = NULL;
	g_pFontFX = NULL;
	
	g_pSWGroup = NULL;
	g_pHWGroup = NULL;
	g_pMainClearGroup = NULL;

	g_pRenderer = NULL;

	g_pInput = NULL;
	g_pFileSystem->Close();
	g_pFileSystem = NULL;

	g_pPlatform = NULL;

	g_pAllocator->DumpLeaks();

	ShutdownModule( g_hRenderer );
	ShutdownModule( g_hFileSystem );
	ShutdownModule( g_hInput );
	ShutdownModule( g_hPlatform );
}

