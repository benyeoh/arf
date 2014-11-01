//==============================================================================
//
//		ShaderVerfier.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Monday, June 05, 2006
//
//==============================================================================

#include "stdafx.h"
#include "ShaderVerifier.h"

_USE_NAMESPACE

HMODULE g_hRenderer;
HMODULE g_hFileSystem;
HMODULE g_hBaseFX;
HMODULE g_hMatGen;

RendererFeedback g_RFeedback;

void MyRegisterClass(HINSTANCE hInstance);
void Uninitialize();
boolean Initialize();
boolean Compile(const wchar* pFileName);
uint BuildFailed();
uint BuildSuccess();
void PrintEffect(IREffectTemplate* pTemplate);

_NAMESPACE_BEGIN

IRRendererPtr g_pRenderer = NULL;
IFFileSystemPtr g_pFileSystem = NULL;
IBFXBaseFXPtr g_pBaseFX = NULL;
IMGMaterialGenPtr g_pMatGen = NULL;

_NAMESPACE_END


int _tmain(int argc, _TCHAR* argv[])
{
	if(!Initialize())
	{
		return BuildFailed();
	}
	else
	{
		wprintf(_W("Verifying Shaders...\n"));
		
		for(int i=1; i < argc; ++i)
		{
			if(!Compile(argv[i]))
				return BuildFailed();
		}		
	}
	
	return BuildSuccess();
}

void MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= DefWindowProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= NULL; 
	wcex.lpszClassName	= _T("ShaderVerifier");
	wcex.hIconSm		= NULL; 

	RegisterClassEx(&wcex);
}

void Uninitialize()
{
	g_pMatGen = NULL;
	g_pBaseFX = NULL;
	g_pRenderer = NULL;
	g_pFileSystem = NULL;
	
	::FreeLibrary(g_hMatGen);
	::FreeLibrary(g_hBaseFX);
	::FreeLibrary(g_hFileSystem);
	::FreeLibrary(g_hRenderer);
	
	_DUMP_LEAKS;
}

uint BuildFailed()
{
	wprintf(_W("=============================== BUILD FAILED ================================\n"));
	Uninitialize();
	return 1;
}

uint BuildSuccess()
{
	wprintf(_W("=============================== BUILD SUCCESS ================================\n"));
	Uninitialize();
	return 0;	
}

const wchar* GetParamStringValue(const REffectParam* pParam, wchar* pStr)
{
	switch(pParam->type)
	{
		case EPT_STRING:	swprintf(pStr, 512,_W("%s"), pParam->pStr);		break;
		case EPT_INT:		swprintf(pStr, 512, _W("%i"), pParam->intVal);	break;
		case EPT_FLOAT:		swprintf(pStr, 512, _W("%f"), pParam->floatVal);	break;
		case EPT_VOID_P:	swprintf(pStr, 512, _W("VOID_P"));				break;
		case EPT_MAT22_P:	
			swprintf(pStr, 512, _W("%f %f | %f %f"), 
				(*(pParam->pMat22))[0][0], (*(pParam->pMat22))[0][1], 
				(*(pParam->pMat22))[1][0], (*(pParam->pMat22))[1][1]);	
			break;
		case EPT_MAT33_P:	
			swprintf(pStr, 512, _W("%f %f %f | %f %f %f | %f %f %f"), 
				(*(pParam->pMat33))[0][0], (*(pParam->pMat33))[0][1], (*(pParam->pMat33))[0][2],
				(*(pParam->pMat33))[1][0], (*(pParam->pMat33))[1][1], (*(pParam->pMat33))[1][2],
				(*(pParam->pMat33))[2][0], (*(pParam->pMat33))[2][1], (*(pParam->pMat33))[2][2]
				);	
			break;
		case EPT_MAT44_P:	
			swprintf(pStr, 512, _W("%f %f %f %f | %f %f %f %f | %f %f %f %f | %f %f %f %f"), 
				(*(pParam->pMat44))[0][0], (*(pParam->pMat44))[0][1], (*(pParam->pMat44))[0][2], (*(pParam->pMat44))[0][3],
				(*(pParam->pMat44))[1][0], (*(pParam->pMat44))[1][1], (*(pParam->pMat44))[1][2], (*(pParam->pMat44))[1][3],
				(*(pParam->pMat44))[2][0], (*(pParam->pMat44))[2][1], (*(pParam->pMat44))[2][2], (*(pParam->pMat44))[2][3],
				(*(pParam->pMat44))[3][0], (*(pParam->pMat44))[3][1], (*(pParam->pMat44))[3][2], (*(pParam->pMat44))[3][3]
				);	
			break;
		case EPT_VEC2_P:	
			swprintf(pStr, 512, _W("%f %f"), 
				(*(pParam->pVec2))[0], (*(pParam->pVec2))[1]);	
			break;
		case EPT_VEC3_P:	
			swprintf(pStr, 512, _W("%f %f %f"), 
				(*(pParam->pVec3))[0], (*(pParam->pVec3))[1], (*(pParam->pVec3))[2]);	
			break;
		case EPT_VEC4_P:	
			swprintf(pStr, 512, _W("%f %f %f %f"), 
				(*(pParam->pVec4))[0], (*(pParam->pVec4))[1], (*(pParam->pVec4))[2], (*(pParam->pVec4))[3]);	
			break;
		case EPT_BOOL:		swprintf(pStr, 512, _W("%b"), pParam->boolVal ? true : false);	break;
		case EPT_TEX2D_P:	swprintf(pStr, 512, _W("%s"), pParam->pTex->GetFileName());		break;
		case EPT_TEXCUBE_P:	swprintf(pStr, 512, _W("%s"), pParam->pCube->GetFileName());		break;
	
		default : _DEBUG_ASSERT(FALSE);
	}
	
	return pStr;
}

boolean Compile(const wchar* pFileName)
{
	wprintf(_W("-- %s --\n"), pFileName);
	
	wstring fileName = _W("[shd]\\");
	fileName += pFileName;
	
	//if(fileName.find(_W(".rfx")) != wstring::npos)
	//{
	//	IByteBufferPtr pFileBuffer = g_RFeedback.GetFileData(fileName.c_str());
	//	IRFXEffectGroupPtr pEffectGroup = g_pRenderFX->GetRFXResourceMgr().LoadRFXEffectGroup(pFileBuffer, NULL, NULL, 0);
	//	if(!pEffectGroup)
	//		return FALSE;
	//		
	//	const wchar* desc = pEffectGroup->GetDesc();
	//	wprintf(_W(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> %s <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n"), desc);
	//	
	//	_LOOP(curBlock, RFX_MAX_EFFECT_GROUP_BLOCKS)
	//	{
	//		wprintf(_W("Begin Block: %d\n"), curBlock);
	//		uint numEffects = pEffectGroup->GetNumOfEffects(curBlock);
	//		
	//		_LOOPi(numEffects)
	//		{
	//			IREffect& effect = pEffectGroup->GetEffect(curBlock, i);
	//			IREffectTemplate* pTemplate = effect.GetTemplate();
	//			PrintEffect(pTemplate);
	//		}
	//		
	//		wprintf(_W("End Block: %d\n"), curBlock);
	//	}
	//	
	//	wprintf(_W(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> END <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n"), desc);

	//}
	if(fileName.find(_W(".mgp")) != wstring::npos)
	{
		// Material group
		IByteBufferPtr pFileBuffer = g_RFeedback.GetFileData(fileName.c_str());
		
		IBFXMaterialGroupTemplatePtr pTemplate = 
			g_pBaseFX->GetResourceMgr().CreateMaterialGroupTemplate(NULL, pFileBuffer, fileName.c_str());
		wprintf(_W(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> %s <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n"), pTemplate->GetDesc());
		uint numMaterials = pTemplate->GetNumOfMaterials();
		wprintf(_W("Num Of Materials: %d\n"), numMaterials);
		_LOOPi(numMaterials)
		{
			wprintf(_W("......................... %d ..............................\n"), i);
			
			IBFXMaterialPtr pMat = pTemplate->GetMaterialByIndex(i);
			uint availability = 0x7FFFFFFF;
			//PrintEffect(pMat->GetEffectTemplate(availability));
		}
		
		wprintf(_W(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> END <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n"));
	}
	else if(fileName.find(_W(".fx")) != wstring::npos)
	{		
		IByteBufferPtr pFileBuffer = g_RFeedback.GetFileData(fileName.c_str());
		IREffectPtr pEffect = g_pRenderer->GetRResourceMgr().LoadEffect(NULL, pFileBuffer, NULL, NULL, 0);
		if(!pEffect)
			return FALSE;
			
		//PrintEffect(pEffect->GetTemplate());
	}
			
	return TRUE;
}

void 
PrintEffect(IREffectTemplate* pTemplate)
{
	wprintf(_W(" ===================================== \n"));
	//uint numEffectDesc = pTemplate->GetNumEffectDesc();
	//_LOOPj(numEffectDesc)
	//{
	//	REffectParam effectDesc = pTemplate->GetEffectDesc(j);
	//	const wchar* pEffectDescName = pTemplate->GetEffectDescName(j);
	//	wchar valueStr[128];
	//	wprintf(_W("Effect Desc -> %s: %s\n"), pEffectDescName, GetParamStringValue(&effectDesc, valueStr));
	//}

	uint numTechs = pTemplate->GetNumOfTechs();	
	_LOOPj(numTechs)
	{
		uint numDynamicParams = pTemplate->GetNumOfDynamicParams(j);
		_LOOPk(numDynamicParams)
		{
			uint semantic = pTemplate->GetDynamicParamSemantic(j, k);
			//const wchar* pSemanticName = GetParamSemanticName(semantic);

			wprintf(_W("Dynamic -> %d\n"), semantic);
		}
	}

	uint numConstantParams = pTemplate->GetNumOfConstantParams();
	_LOOPj(numConstantParams)
	{
		uint semantic = pTemplate->GetConstantParamSemantic(j);
		//const wchar* pSemanticName = GetParamSemanticName(semantic);
		wprintf(_W("Constant -> %d\n"), semantic);

		uint numParamDesc = pTemplate->GetConstantParamNumDesc(j);
		_LOOPk(numParamDesc)
		{
			wchar valueStr[128];
			REffectParam desc = pTemplate->GetConstantParamDesc(j, k);
			const wchar* pDescName = pTemplate->GetConstantParamDescName(j, k);
			wprintf(_W("Param Desc -> %s: %s\n"), pDescName, GetParamStringValue(&desc, valueStr));
		}
	}

	wprintf(_W(" ===================================== \n"));
}

boolean Initialize()
{
	MyRegisterClass(NULL);
	
	HWND hWnd;

//	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(_T("ShaderVerifier"), _T("ShaderVerifier"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, NULL, NULL);

	if (!hWnd)
	{
		wprintf(_W("Failed to create window!\n"));
		_DEBUG_ASSERT(FALSE);
		return FALSE;
	}
	
	// Hide the window
	ShowWindow(hWnd, SW_HIDE);
	
	// Load Win32 File system
	g_hFileSystem = ::LoadLibrary(_W("FileSystem.dll"));
	if(!g_hFileSystem) return FALSE;

	CreateFileSystemFn CreateFileSystem = (CreateFileSystemFn) ::GetProcAddress(g_hFileSystem, "CreateFileSystem");

	if(!CreateFileSystem)
	{
		wprintf(_W("Failed to create FileSystem!\n"));
		_DEBUG_ASSERT(FALSE);
		return FALSE;
	}

	g_pFileSystem = CreateFileSystem();
	
	wchar filePath[256];
	GetModuleFileName(NULL, filePath, 256);
	wstring path = filePath;
	std::transform(path.begin(), path.end(), path.begin(), towlower);

	int index = (int) path.find(_W("\\bin\\"));
	if(index != wstring::npos)
	{
		path = path.substr(0, index);
	}

	if(!g_pFileSystem->Open(path.c_str()))
	{
		wprintf(_W("Failed to init FileSystem!\n"));
		return FALSE;
	}

	g_pFileSystem->AddMnemonic(_W("shd"), _W("data\\shaders"));
	g_pFileSystem->AddMnemonic(_W("mat"), _W("data\\materials"));
	
	g_hRenderer = ::LoadLibrary(_W("Renderer.dll"));
	if(!g_hRenderer ) return FALSE;

	CreateRendererFn CreateRendererD3D = (CreateRendererFn)::GetProcAddress(g_hRenderer, "CreateRenderer");

	if(!CreateRendererD3D)
	{
		wprintf(_W("Failed to create Renderer!\n"));
		_DEBUG_ASSERT(FALSE);
		return FALSE;
	}
	
	g_pRenderer = CreateRendererD3D();
	
	RDisplayInfo pInfo;    
	pInfo.backBufferWidth	= 800;
	pInfo.backBufferHeight	= 600;
	pInfo.fullScreen		= FALSE;
	pInfo.hWnd				= hWnd;

	RInitInfo rInfo;
	rInfo.presentInfo		= pInfo;
	rInfo.pApplication		= &g_RFeedback;
	if(!g_pRenderer->Initialize(rInfo))
	{
		wprintf(_W("Failed to init Renderer!\n"));
		return FALSE;
	}
	
	IMGMaterialGen* pGen = NULL;
	g_hMatGen = LoadDLL(&pGen, _W("MaterialGen.dll"), "CreateMaterialGen");
	
	g_pMatGen = pGen;	
	g_pMatGen->Initialize(g_RFeedback);
	
	IBFXBaseFX* pBase = NULL;
	g_hBaseFX = LoadDLL(&pBase, _W("BaseFX.dll"), "CreateBaseFX");
	g_pBaseFX = pBase;
	g_pBaseFX->Initialize(*g_pRenderer, *g_pMatGen, g_RFeedback);
	
	//g_pBaseFX->Initialize(g_pRenderer, 
	
	
	//if(!g_pPlatform->Initialize())
	//{
	//	wprintf(_W("Failed to init Platform!\n"));
	//	return FALSE;
	//}
	
	//wchar path[1024];
	//GetCurrentDirectoryW(1024, path);
	//
	//_TRACE(_W("%s\n"), path);
	//
			
	return TRUE;
}
