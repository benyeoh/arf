
#include "stdafx.h"

//------------------------------------------------------------------------------
//	Misc
//
//------------------------------------------------------------------------------
#define MAX_LOADSTRING 100

TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Global Variables:
HINSTANCE hInst;								// current instance
Module	g_hRenderer;
Module g_hPlatform;
Module g_hFileSystem;
Module g_hInput;
Module g_hCoreMem;
Module g_hMatGen;
Module g_hBaseFX;
Module g_hDeferredFX;
Module g_hScreenAmbOcclFX;
Module g_hShadowFX;

HWND	g_hWnd			= NULL;

MemAllocFns g_MemAllocFns;

typedef LRESULT (*FeedWindowsMessageFn)(HWND, UINT, WPARAM, LPARAM);
FeedWindowsMessageFn g_FeedWindowsMessage;

//------------------------------------------------------------------------------
//	Globals
//
//------------------------------------------------------------------------------

IRRendererPtr		g_pRenderer;
IRRenderGroupPtr	g_pMainClearGroup;

IFFileSystemPtr		g_pFileSystem;
IMGMaterialGenPtr	g_pMatGen;
IBFXBaseFXPtr		g_pBaseFX;
IBFXPipelinePtr		g_pBFXPipeline;
IBFXParamPoolPtr	g_pBFXParamPool;
BFXParamContainer	g_ParamContainer;
BFXRenderContainer	g_RenderContainer;

ICMMemAllocatorPtr	g_pAllocator;

IIInputPtr			g_pInput;
IFFXFontFXPtr		g_pFontFX;
IFFXFontFacePtr		g_pVerdanaFontFace;
IFFXFontFacePtr		g_pCourierFontFace;
IPPlatformPtr		g_pPlatform;
IDFXDebugFXPtr		g_pDebugFX;
ISHFXShadowFXPtr	g_pShadowFX;
ISHFXPipelinePtr	g_pSHFXPipeline;
ISHFXParamPoolPtr	g_pSHFXParamPool;
IDEFXDeferredFXPtr	g_pDeferredFX;
IDEFXPipelinePtr	g_pDeferredPipeline;
IDEFXLightGroupPtr	g_pDeferredLightGroup[NUM_POINT_LIGHTS+1];
IDEFXParamPoolPtr	g_pDeferredParamPool;
IDEFXLightGroupPtr	g_pDeferredDirLightGroup;

BFXUMeshList*	g_pRoomMeshList;

uint		g_CurObjectIndex = 0;

float	m_LastTimeElapsed;
uint	m_NumFrames;
uint	m_TotalNumFrames = 0;
uint	m_LastNumOfFrames;
float	g_TimeDT;
boolean g_IsDebugRenderObjects = TRUE;
boolean g_IsDebugRenderSceneDB = TRUE;
boolean g_IsWireFrameMode = FALSE;

AppCallback	g_AppCallback;

BFXHemiLight		g_HemiLight;
//BFXDirLight			g_DirLight;

DEFXPointLight		g_PointLight[NUM_POINT_LIGHTS+1];
ISHFXCubePointLightPtr	g_pShadowPointLight[NUM_POINT_LIGHTS+1];
DEFXDirLight		g_DirLight;
ISHFXCascadedDirLightPtr g_pShadowDirLight;
SHFXCascadedFrustumBounds g_CascadeFrustum;

gmtl::VecA3f		g_EyePos;
gmtl::VecA3f		g_EyeDir;
gmtl::VecA3f		g_EyeUpVec;
gmtl::MatrixA44f	g_View;
gmtl::MatrixA44f	g_Proj;
gmtl::VecA3f		g_PointLightPos[NUM_POINT_LIGHTS+1];

BOOL				g_IsFullScreen = FALSE;
boolean				g_IsRenderReady = FALSE;
float				g_CurIndex = 0.0f;
float				g_IncRate = 0.0f;

void*				g_pD3DDevice;

gmtl::VecA3f		g_CameraFarCornerVecs[4];
gmtl::VecA3f		g_ViewFarCornerVecs[4];

BFXUMeshListPtr		g_pMeshes[NUM_MESHES];
gmtl::MatrixA44f	g_MeshesWorld[NUM_MESHES];

BFXUMeshListPtr		g_pPointLightMesh;
gmtl::MatrixA44f	g_PointLightWorld[NUM_POINT_LIGHTS+1];

BFXUMeshListPtr	g_pDirLightMesh;

//------------------------------------------------------------------------------
//	Forward Decls
//
//------------------------------------------------------------------------------
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
boolean				RunApp();

_DECL_CORE_MEM_ALLOC_FNS(g_pAllocator)

	int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	g_hCoreMem = InitializeModule(_W("CoreMemory.dll"), NULL);

	CreateMemAllocatorFn CallCreateMemAllocator = (CreateMemAllocatorFn) GetFunction(g_hCoreMem, "CreateMemAllocator");
	g_pAllocator = CallCreateMemAllocator();

	_DEBUG_ASSERT(g_pAllocator);
	g_MemAllocFns = GetCoreMemoryBindings();
	_BIND_TO_GLOBAL_ALLOC(g_MemAllocFns);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	//HACCEL hAccelTable;

	// Initialize global strings
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	if(!Initialize())
		return FALSE;

	while(RunApp())
	{
		// Main message loop:
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			//if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if(msg.message == WM_QUIT)
				break;
		}

	}

	Shutdown();

	g_pAllocator = NULL;
	ShutdownModule( g_hCoreMem );

	_DUMP_LEAKS;

	return 0;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= _W("TestDeferredFX");
	wcex.lpszClassName	= _W("TestDeferredFX");
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	g_hWnd = CreateWindow(_W("TestDeferredFX"), _W("TestDeferredFX"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 1024, 768, NULL, NULL, hInstance, NULL);

	_DEBUG_ASSERT(g_hWnd);

	if (!g_hWnd)
	{
		return FALSE;
	}

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	//	PAINTSTRUCT ps;
	//	HDC hdc;

	switch (message)
	{
	case WM_ERASEBKGND: 
		return TRUE;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		//switch (wmId)
		//{
		//case IDM_ABOUT:
		//	DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
		//	break;
		//case IDM_EXIT:
		//	DestroyWindow(hWnd);
		//	break;
		//case ID_OPTIONS_FULLSCREEN:
		//	{
		//		g_IsFullScreen = !g_IsFullScreen;
		//		//UINT state = CheckMenuItem(GetMenu(hWnd), ID_OPTIONS_FULLSCREEN, 
		//		//							g_IsFullScreen ? MF_CHECKED : MF_UNCHECKED);
		//		RECT winRect;
		//		HWND hDesktopWnd = GetDesktopWindow();
		//		GetWindowRect(hDesktopWnd, &winRect);

		//		SetScreenMode(winRect.right - winRect.left, winRect.bottom - winRect.top, g_IsFullScreen);
		//		break;
		//	}
		//default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		//}
		break;
		//case WM_PAINT:
		//	hdc = BeginPaint(hWnd, &ps);
		//	// TODO: Add any drawing code here...
		//	EndPaint(hWnd, &ps);
		//	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		if(g_FeedWindowsMessage)
			if(!g_FeedWindowsMessage(hWnd, message, wParam, lParam))
				return 0;
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
//INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	UNREFERENCED_PARAMETER(lParam);
//	switch (message)
//	{
//	case WM_INITDIALOG:
//		return (INT_PTR)TRUE;
//
//	case WM_COMMAND:
//		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
//		{
//			EndDialog(hDlg, LOWORD(wParam));
//			return (INT_PTR)TRUE;
//		}
//		break;
//	}
//	return (INT_PTR)FALSE;
//}

boolean RunApp()
{
	g_pPlatform->GetTimer().BeginSample();

	RenderAll();

	// Updates here
	ProcessInput();	

	// DisplayFrame is 1 frame late to maximize parallism
	g_pRenderer->DisplayFrame();

	g_pPlatform->GetTimer().EndSample();

	g_TimeDT = (float) g_pPlatform->GetTimer().GetTimeElapsed();
	if(g_TimeDT > 0.1f)
		g_TimeDT = 0.1f;
	m_LastTimeElapsed += g_TimeDT;
	m_NumFrames++;
	m_TotalNumFrames++;

	if(m_LastTimeElapsed >= 1.0f)
	{
		m_LastTimeElapsed = 0.0f;
		m_LastNumOfFrames = m_NumFrames;
		m_NumFrames = 0;
	}

	return TRUE;
}


