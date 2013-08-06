//==============================================================================
//
//		TestAmbientCubeSH.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/31/2007
//
//==============================================================================

#pragma once

_USE_NAMESPACE

#include "RendererCallback.h"
#include "Render.h"
#include "Update.h"
#include "Init.h"

extern HINSTANCE hInst;								// current instance
extern HMODULE	g_hRenderer;
extern HMODULE g_hPlatform;
extern HMODULE g_hFileSystem;
extern HMODULE g_hInput;
extern HMODULE g_hCoreMem;
extern HMODULE g_hMatGen;
extern HMODULE g_hBaseFX;
extern HMODULE g_hAmbientFX;

extern HWND	g_hWnd;

extern GetCoreMemoryMetricsFn g_GetCoreMemoryMetrics;
typedef LRESULT (*FeedWindowsMessageFn)(HWND, UINT, WPARAM, LPARAM);
extern FeedWindowsMessageFn g_FeedWindowsMessage;

typedef void* (*GetD3DDeviceFn)(IRRenderer* pRenderer);

extern IRRendererPtr		g_pRenderer;
extern IFFileSystemPtr		g_pFileSystem;
//IRFXRenderFXPtr		g_pRenderFX;
extern IMGMaterialGenPtr	g_pMatGen;
extern IBFXBaseFXPtr		g_pBaseFX;
extern IIInputPtr			g_pInput;
extern IFFXFontFXPtr		g_pFontFX;
extern IFFXFontFacePtr		g_pVerdanaFontFace;
extern IFFXFontFacePtr		g_pCourierFontFace;
extern IPPlatformPtr		g_pPlatform;
extern IDFXDebugFXPtr		g_pDebugFX;
extern IAFXAmbientFXPtr		g_pAmbientFX;


extern BFXMesh		g_Sphere;
extern BFXMesh		g_Bandit;
extern BFXMesh		g_Soldier;
extern BFXMesh		g_Floor;
extern BFXUMeshList*	g_pRoomSHMeshList;

extern float	m_LastTimeElapsed;
extern uint		m_NumFrames;
extern uint		m_TotalNumFrames;
extern uint		m_LastNumOfFrames;
extern float	g_TimeDT;
extern boolean	g_IsDebugRenderObjects;
extern boolean	g_IsDebugRenderSceneDB;
extern boolean	g_IsWireFrameMode;

extern RendererCallback	g_RendererCallback;

extern BFXHemiLight		g_HemiLight;
extern BFXDirLight		g_DirLight;
extern gmtl::VecA3f		g_EyePos;
extern gmtl::VecA3f		g_EyeDir;
extern gmtl::VecA3f		g_EyeUpVec;
extern gmtl::MatrixA44f	g_View;
extern gmtl::MatrixA44f	g_Proj;
extern gmtl::MatrixA44f	g_DebugViewProj;
extern gmtl::MatrixA44f	g_DebugView;
extern gmtl::MatrixA44f	g_DebugProj;
extern gmtl::VecA3f		g_DebugEyePos;

extern BOOL				g_IsFullScreen;
extern boolean			g_IsRenderReady;

extern IRTextureCubePtr	g_pTexCube[4];
extern AFXAmbientCoeffs g_Coeffs[4];
extern AFXAmbientCoeffs g_CurCoeff;
extern float			g_CurIndex;
extern float			g_IncRate;

extern IRTextureCubePtr		g_pRadianceCosineTex[4*3];
extern float				g_UnitCoeffs[16];
extern float				g_CosineLobeCoeffs[16];
extern float				g_LightZH[4];
extern void*				g_pD3DDevice;
