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
	g_hFileSystem = InitializeModule(_W("FileSystem.dll"), &g_MemAllocFns);
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
	g_hInput = InitializeModule(_W("Input.dll"), &g_MemAllocFns);
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
	g_hMatGen = InitializeModule(_W("MaterialGen.dll"), &g_MemAllocFns);
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
	g_hRenderer = InitializeModule(_W("Renderer.dll"), &g_MemAllocFns);
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

boolean InitAmbientFX()
{
	g_hAmbientFX = InitializeModule(_W("AmbientFX.dll"), &g_MemAllocFns);
	if(!g_hAmbientFX) return FALSE;

	CreateAmbientFXFn createFn = (CreateAmbientFXFn) GetFunction(g_hAmbientFX, "CreateAmbientFX");

	_DEBUG_ASSERT(createFn);
	g_pAmbientFX = createFn();
	g_pAmbientFX->Initialize(g_AppCallback);

	g_pAFXParamPool = g_pAmbientFX->GetResourceMgr().CreateParamPool();
	g_ParamContainer.SetParamCallback(g_pAFXParamPool, _GET_LIB_INDEX(AFX_EFFECT_PARAM_OFFSET));

	IByteBufferPtr pBuffer = _NEW CByteBuffer();
	IFFilePtr pFile = g_pFileSystem->GetFile(_W("[tex]\\grand_canyon_cube.dds"));
	pFile->Open(FO_READ_ONLY);
	AppendData(pBuffer, pFile);
	pFile->Close();

	IRTextureCubePtr pAmbientCube = g_pRenderer->GetRResourceMgr().LoadTextureCube(NULL, pBuffer);
	g_pAmbientFX->GetResourceMgr().CompressAmbientCubeMap(pAmbientCube, &g_AmbientCoeffs, 0.0f, 1.0f / 3.142f);

	g_AmbientZ.set(0.0f, 0.0f, 1.0f);
	g_AmbientY.set(0.0f, 1.0f, 0.0f);

	EvaluateSHTripleProductTable(g_TripleProductTable, 3, NULL);

	Matrix99f transfer;
	float sh[9];
	//g_pAmbientFX->GetSHToolKit().CreateHemiLobeCoeffs(unitSH, 128, 3);
	sh[0] = g_AmbientCoeffs.rTerm1[0];
	sh[1] = g_AmbientCoeffs.rTerm1[1];
	sh[2] = g_AmbientCoeffs.rTerm1[2];
	sh[3] = g_AmbientCoeffs.rTerm1[3];
	sh[4] = g_AmbientCoeffs.rTerm2[0];
	sh[5] = g_AmbientCoeffs.rTerm2[1];
	sh[6] = g_AmbientCoeffs.rTerm2[2];
	sh[7] = g_AmbientCoeffs.rTerm2[3];
	sh[8] = g_AmbientCoeffs.rgbTerm[0];
	
	EvaluateSHTripleProductTransfer3(transfer, g_TripleProductTable, sh);
	g_AmbientRed.Set(transfer);

	sh[0] = g_AmbientCoeffs.gTerm1[0];
	sh[1] = g_AmbientCoeffs.gTerm1[1];
	sh[2] = g_AmbientCoeffs.gTerm1[2];
	sh[3] = g_AmbientCoeffs.gTerm1[3];
	sh[4] = g_AmbientCoeffs.gTerm2[0];
	sh[5] = g_AmbientCoeffs.gTerm2[1];
	sh[6] = g_AmbientCoeffs.gTerm2[2];
	sh[7] = g_AmbientCoeffs.gTerm2[3];
	sh[8] = g_AmbientCoeffs.rgbTerm[1];

	EvaluateSHTripleProductTransfer3(transfer, g_TripleProductTable, sh);
	g_AmbientGreen.Set(transfer);

	sh[0] = g_AmbientCoeffs.bTerm1[0];
	sh[1] = g_AmbientCoeffs.bTerm1[1];
	sh[2] = g_AmbientCoeffs.bTerm1[2];
	sh[3] = g_AmbientCoeffs.bTerm1[3];
	sh[4] = g_AmbientCoeffs.bTerm2[0];
	sh[5] = g_AmbientCoeffs.bTerm2[1];
	sh[6] = g_AmbientCoeffs.bTerm2[2];
	sh[7] = g_AmbientCoeffs.bTerm2[3];
	sh[8] = g_AmbientCoeffs.rgbTerm[2];

	EvaluateSHTripleProductTransfer3(transfer, g_TripleProductTable, sh);
	g_AmbientBlue.Set(transfer);

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
	g_hPlatform = InitializeModule(_W("Platform.dll"), &g_MemAllocFns);
	if(!g_hPlatform ) return FALSE;

	CreatePlatformFn createPlatform = (CreatePlatformFn) GetFunction(g_hPlatform, "CreatePlatform");

	_DEBUG_ASSERT(createPlatform);

	g_pPlatform = createPlatform();
	g_pThreadPool = g_pPlatform->GetResourceMgr().CreateThreadPool();
	
	const static int NUM_THREADS = 3;

	IPThread* pThreads[NUM_THREADS];

	_LOOPi(NUM_THREADS)
	{
		pThreads[i] = g_pPlatform->GetResourceMgr().CreateThread();
		pThreads[i]->Initialize();
	}

	g_pThreadPool->Initialize(10, pThreads, NUM_THREADS);

	return TRUE;
}

boolean InitSHPRTCompute()
{
	g_hSHPRTCompute = InitializeModule(_W("SHPRTCompute.dll"), &g_MemAllocFns);
	if(!g_hSHPRTCompute) return FALSE;

	CreateSHPRTComputeFn createSHPRTCompute = (CreateSHPRTComputeFn) GetFunction(g_hSHPRTCompute, "CreateSHPRTCompute");

	_DEBUG_ASSERT(createSHPRTCompute);
	g_pSHPRTCompute = createSHPRTCompute();
	g_pSHPRTCompute->Initialize(*g_pRenderer, g_AppCallback);

	//// Evaluate test BRDF
	//BRDFData brdf;
	//_LOOPi(3)
	//{
	//	brdf.lobe[i].Cx = 0.0f;
	//	brdf.lobe[i].Cy = 0.0f;
	//	brdf.lobe[i].Cz = 0.0f;
	//	brdf.lobe[i].n = 1.0f;
	//}

	////brdf.lobe[0].Cx = 0.0f;
	////brdf.lobe[0].Cy = 0.8f;
	////brdf.lobe[0].Cz = 0.3f;
	////brdf.lobe[0].n = 2.3f;

	//brdf.lobe[0].Cx = -1.0f;
	//brdf.lobe[0].Cy = 0.0f;
	//brdf.lobe[0].Cz = 1.0f;
	//brdf.lobe[0].n = 16.0f;

	//brdf.diffuseScale = 0.04f;
	//brdf.lobeScale = 1.0f;
	//brdf.metalScale = 0.0f;
	//brdf.nDotLScale = 0.0f;
	//g_BRDFCallback.SetBRDFParameters(brdf);
	//g_BRDFCallback.NormalizeBRDFParameters();

	//SHPRTLocationEntry* pLocEntries = _NEW SHPRTLocationEntry[64 * 64];
	//SHPRTDataEntry* pDataEntries = _NEW SHPRTDataEntry[64 * 64];
	//g_pSHPRTCompute->ComputeBRDFLocationEntries(pLocEntries, 64, 64);
	//g_pSHPRTCompute->ComputeBRDFTransfer(pLocEntries, pDataEntries, 64 * 64, g_BRDFCallback);

	//IRTexture2D* pBRDFTex1;
	//IRTexture2D* pBRDFTex2;
	//g_pSHPRTCompute->CreateBRDFTexturesZH(pLocEntries, pDataEntries, 64 * 64, 64, 64, &pBRDFTex1, &pBRDFTex2);

	//IByteBufferPtr pTempSaveTex1 = _NEW CByteBuffer();
	//IByteBufferPtr pTempSaveTex2 = _NEW CByteBuffer();
	//g_pRenderer->GetRResourceMgr().SaveTexture2D(pBRDFTex1, pTempSaveTex1);
	//g_pRenderer->GetRResourceMgr().SaveTexture2D(pBRDFTex2, pTempSaveTex2);

	//IFFilePtr pFile = g_pFileSystem->GetFile(_W("[tex]\\TestDiffusePhongBRDF_1.dds"));
	//pFile->Open(FO_WRITE_ONLY);
	//pFile->Write(pTempSaveTex1->GetData(), pTempSaveTex1->GetDataLength());
	//pFile->Close();

	//pFile = g_pFileSystem->GetFile(_W("[tex]\\TestDiffusePhongBRDF_2.dds"));
	//pFile->Open(FO_WRITE_ONLY);
	//pFile->Write(pTempSaveTex2->GetData(), pTempSaveTex2->GetDataLength());
	//pFile->Close();

	//pBRDFTex1->Release();
	//pBRDFTex2->Release();

	//_DELETE_ARRAY(pLocEntries);
	//_DELETE_ARRAY(pDataEntries);

	return TRUE;
}

void InitLights()
{
	g_HemiLight.skyColor	= gmtl::Vec4f(0.2f, 0.4f, 0.5f, 1.0f);
	g_HemiLight.groundColor = gmtl::Vec4f(0.475f, 0.2f, 0.1f, 1.0f);

	g_DirLight.color		= gmtl::Vec4f(0.8f, 0.74f, 0.66f, 1.0f);
	g_DirLight.dir			= gmtl::Vec3f(-2.0f, -1.0f, -1.0f);
	gmtl::normalize(g_DirLight.dir);
	g_DirLight.invSize		= 1.0f;

	g_PointLightPos			= gmtl::VecA3f(0.0, 4.0f, 0.0f);
	g_PointLight.pos		= _CAST_VEC3(g_PointLightPos);
	g_PointLight.color		= gmtl::Vec4f(1.0f, 0.0f, 0.0f, 1.0f);
	g_PointLight.invRange	= 1.0f / 20.0f;
	g_PointLight.invSize	= 1.0f;
}

//#include <xmmintrin.h>
//
//struct sVal
//{
//	__m128 v1;
//	__m128 v2;
//	__m128 v3[3];
//};
//
//__m128 val;
//__m128 val2;
//
//void DoSomething(__m128 v1, __m128 v2)
//{
//	val = v1;
//	val2 = v2;
//}
//
//void DoSomethingAgain(__m128 v1, __m128 v2)
//{
//	volatile __m128 out = _mm_add_ps(v1, v2);
//
//}
//
//void DoSomethingStruct(sVal* pS)
//{
//	volatile __m128 out = _mm_add_ps(pS->v1, pS->v2);
//
//}


void InitMesh()
{
	gmtl::AxisAnglef meshRot;

	IByteBufferPtr pBuffer = g_AppCallback.GetFileData(_W("[msh]\\TestSHPRTFX\\sphereAndTorusSHRadianceBRDF.mls"));
	BFXUMeshList* pMeshList = _NEW BFXUMeshList;
	LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *pMeshList);
	meshRot.setAxis(gmtl::Vec3f(1.0f, 0.0f, 0.0f));
	meshRot.setAngle(gmtl::Math::PI_OVER_2);
	//gmtl::setRot(_CAST_MAT44(g_MeshesWorld[0]), meshRot);
	g_pMeshes[0] = pMeshList;
	gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[0]), gmtl::Vec3f(0.0f, 3.0f, 0.0f));

	pBuffer = g_AppCallback.GetFileData(_W("[msh]\\TestSHPRTFX\\room2RadianceBRDF.mls"));
	pMeshList = _NEW BFXUMeshList;
	LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *pMeshList);
	gmtl::setScale(_CAST_MAT44(g_MeshesWorld[1]), 3.0f);
	g_pMeshes[1] = pMeshList;
	gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[1]), gmtl::Vec3f(0.0f, 0.0f, 0.0f));

	pBuffer = g_AppCallback.GetFileData(_W("[msh]\\TestSHPRTFX\\raiderRadianceBRDF.mls"));
	pMeshList = _NEW BFXUMeshList;
	LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *pMeshList);
	meshRot.setAxis(gmtl::Vec3f(0.0f, 1.0f, 0.0f));
	meshRot.setAngle(gmtl::Math::PI_OVER_2);
	//gmtl::setRot(_CAST_MAT44(g_MeshesWorld[2]), meshRot);
	g_pMeshes[2] = pMeshList;
	gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[2]), gmtl::Vec3f(13.0f, 4.0f, 0.0f));

	pBuffer = g_AppCallback.GetFileData(_W("[msh]\\TestSHPRTFX\\tokkaRadianceBRDF.mls"));
	pMeshList = _NEW BFXUMeshList;
	LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *pMeshList);
	g_pMeshes[3] = pMeshList;
	meshRot.setAxis(gmtl::Vec3f(1.0f, 0.0f, 0.0f));
	meshRot.setAngle(gmtl::Math::PI_OVER_2);
	//gmtl::setRot(_CAST_MAT44(g_MeshesWorld[3]), meshRot);
	gmtl::setScale(_CAST_MAT44(g_MeshesWorld[3]), 1.7f);
	gmtl::setTrans(_CAST_MAT44(g_MeshesWorld[3]), gmtl::Vec3f(-15.0f, 4.0f, 0.0f));

	// Skydome
	pBuffer = g_AppCallback.GetFileData(_W("[msh]\\TestSHPRTFX\\skydome.mls"));
	pMeshList = _NEW BFXUMeshList;
	LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pBuffer, 0, *pMeshList);
	g_pSkyDome = pMeshList;
	
	//int out1 = FastRound(0.5f);
	//int out2 = FastRound(-0.5f);
	//int out3 = FastRound(-0.51f);
	//int out4 = FastRound(-1.5f);
	//int out5 = FastRound(-1.51f);
	//_TRACE(_W("out1: %d, out2: %d, out3: %d, out4: %d, out5: %d\n"), out1, out2, out3, out4, out5);

	//volatile float toDiv = 0.0f;
	//float inf = 1.0f / toDiv;
	//float amount = inf * 5.0f - 10.0f * inf + 20;

	//_TRACE(_W("amount: %f \n"), amount);

	// 2^0  * 1.11111111111111111111111
	// 2^24 * 0.000000000000000000000001 <- overflow
	// 2^23 * 0.00000000000000000000001
	// 2^22 * 0.00000000000000000000011  <- 1 bit decimal place
	// if add 2^22
	// 2^22 * 1.00000000000000000000011
	// then minus 2^22
	// 2^22 * 0.00000000000000000000011
	// 2^0  * 1.10000000000000000000000

	//volatile float toRound = 16777216.0f / 4096.0f; // Implies 2^(24-n) where n is the number of bits saved
	//volatile float init =  4095.0f + 1.0f / 2048.0f;
	//_TRACE(_W("init: %f \n"), init);
	//float amount = init;
	//amount += toRound;
	//_TRACE(_W("amount1: %f \n"), amount);
	//amount -= toRound;
	//_TRACE(_W("amount2: %f \n"), amount);

	//__m128 test = _mm_set1_ps( rand() / 10.0f );
	//__m128 test2 = _mm_set1_ps( rand() / 10.0f );
	//sVal s;
	//s.v1 = _mm_set1_ps( rand() / 10.0f );
	//s.v2 = _mm_set1_ps( rand() / 10.0f );
	//s.v3[0] = _mm_set1_ps( rand() / 10.0f );
	//s.v3[1] = _mm_set1_ps( rand() / 10.0f );
	//s.v3[2] = _mm_set1_ps( rand() / 10.0f );

	//DoSomething(test, test2);
	//DoSomethingAgain(val, val2);
	//DoSomethingStruct(&s);
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
	//g_pCube = _NEW TestCube();
	//g_pCube->Initialize();
	InitAmbientFX();
	InitSHPRTCompute();
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

	//float unitSH[9];
	//g_pAmbientFX->GetSHToolKit().CreateCosineLobeCoeffs(unitSH, 128, 3);
	//float zh[3];

	InitMesh();

	
	//CompressSHToZH(unitSH, zh, 3, gmtl::Vec3f(0.0f, 1.0f, 0.0f));

	//_LOOPi(9)
	//{
	//	_DEBUG_TRACE(_W("%f\n"), unitSH[i]);
	//}

	//_DEBUG_TRACE(_W("\n\n"));

	//_LOOPi(3)
	//{
	//	_DEBUG_TRACE(_W("%f\n"), zh[i]);
	//}

	// Compute SHPRT
	g_RadianceCallback.Compute();

	return TRUE;
}	

void Shutdown()
{
	g_pSkyDome = NULL;

	_LOOPi(NUM_MESHES)
	{
		g_pMeshes[i] = 0;
	}

	_DELETE( g_pRoomMeshList );

	g_pThreadPool = NULL;

	g_pAFXParamPool = NULL;

	g_pSHPRTCompute = NULL;
	g_pAmbientFX = NULL;
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
	ShutdownModule( g_hSHPRTCompute );
	ShutdownModule( g_hAmbientFX );
	ShutdownModule( g_hBaseFX );
	ShutdownModule( g_hMatGen );
	ShutdownModule( g_hRenderer );
	ShutdownModule( g_hFileSystem );
	ShutdownModule( g_hInput );
	ShutdownModule( g_hPlatform );
}

