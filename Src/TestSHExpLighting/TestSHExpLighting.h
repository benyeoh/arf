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
extern HMODULE g_hDeferredFX;
extern HMODULE g_hSHExpLightingFX;

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
extern IDEFXDeferredFXPtr			g_pDeferredFX;
extern ISEFXSHExpLightingFXPtr		g_pSHExpLightingFX;

extern BFXMesh		g_Bandit;
extern BFXMesh		g_Soldier;
extern BFXMesh		g_Floor;
extern BFXUMeshList*	g_pRoomSHMeshList;

const static uint NUM_MESHES = 2;
extern BFXUMeshListPtr	g_pMeshes[NUM_MESHES];
extern gmtl::MatrixA44f g_MeshesWorld[NUM_MESHES];
extern SEFXUVisibilityTreePtr	g_pVisTree[NUM_MESHES];
extern IBFXMaterialGroupPtr	g_pBackupNoTexMat[NUM_MESHES];

extern uint		g_CurObjectIndex;

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

const static uint NUM_BANDS = 3;

extern IRTextureCubePtr	g_pTexCube[4];
extern float			g_CurIndex;
extern float			g_IncRate;

extern IRTextureCubePtr		g_pRadianceCosineTex[NUM_BANDS*3];
extern IRTextureCubePtr		g_pYlmTex[NUM_BANDS];
extern IRTexture2DPtr		g_pVisSphereTex;
extern IRTexture2DPtr		g_pABTableTex;
extern float				g_UnitCoeffs[NUM_BANDS * NUM_BANDS];
extern float				g_CosineLobeCoeffs[NUM_BANDS * NUM_BANDS];
extern float				g_LightZH[NUM_BANDS];
extern void*				g_pD3DDevice;

extern gmtl::VecA3f			g_CameraFarCornerVecs[4];
