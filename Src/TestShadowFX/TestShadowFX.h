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

#include "AppCallback.h"
#include "Render.h"
#include "Update.h"
#include "Init.h"

extern HINSTANCE hInst;								// current instance
extern Module g_hRenderer;
extern Module g_hPlatform;
extern Module g_hFileSystem;
extern Module g_hInput;
extern Module g_hCoreMem;
extern Module g_hMatGen;
extern Module g_hBaseFX;
extern Module g_hShadowFX;

extern MemAllocFns g_MemAllocFns;

extern HWND	g_hWnd;

typedef LRESULT (*FeedWindowsMessageFn)(HWND, UINT, WPARAM, LPARAM);
extern FeedWindowsMessageFn g_FeedWindowsMessage;

typedef void* (*GetD3DDeviceFn)(IRRenderer* pRenderer);

extern IRRendererPtr		g_pRenderer;
extern IRRenderGroupPtr		g_pMainClearGroup;

extern IFFileSystemPtr		g_pFileSystem;
//IRFXRenderFXPtr		g_pRenderFX;
extern IMGMaterialGenPtr	g_pMatGen;
extern IBFXBaseFXPtr		g_pBaseFX;
extern IBFXPipelinePtr		g_pBFXPipeline;
extern IBFXParamPoolPtr		g_pBFXParamPool;
extern BFXParamContainer	g_ParamContainer;
extern BFXRenderContainer	g_RenderContainer;

extern ICMMemAllocatorPtr	g_pAllocator;

extern IIInputPtr			g_pInput;
extern IFFXFontFXPtr		g_pFontFX;
extern IFFXFontFacePtr		g_pVerdanaFontFace;
extern IFFXFontFacePtr		g_pCourierFontFace;
extern IPPlatformPtr		g_pPlatform;
extern IDFXDebugFXPtr		g_pDebugFX;
extern ISHFXShadowFXPtr		g_pShadowFX;
extern ISHFXPipelinePtr		g_pSHFXPipeline;
extern ISHFXParamPoolPtr	g_pSHFXParamPool;

extern BFXUMeshList*	g_pRoomMeshList;

const static uint NUM_MESHES = 2;
extern BFXUMeshListPtr	g_pMeshes[NUM_MESHES];
extern gmtl::MatrixA44f g_MeshesWorld[NUM_MESHES];

extern uint		g_CurObjectIndex;

extern float	m_LastTimeElapsed;
extern uint		m_NumFrames;
extern uint		m_TotalNumFrames;
extern uint		m_LastNumOfFrames;
extern float	g_TimeDT;
extern boolean	g_IsDebugRenderObjects;
extern boolean	g_IsDebugRenderSceneDB;
extern boolean	g_IsWireFrameMode;

extern AppCallback	g_AppCallback;

extern BFXHemiLight		g_HemiLight;
extern BFXDirLight		g_DirLight;
extern ISHFXCascadedDirLightPtr g_pShadowDirLight;
extern BFXPointLight	g_PointLight;
extern ISHFXPointLightPtr g_pShadowPointLight;
extern ISHFXCubePointLightPtr g_pShadowCubePointLight;
extern IRTexture2DPtr	g_pCubeDebugRT;

extern SHFXCascadedFrustumBounds g_CascadeFrustum;

extern gmtl::VecA3f		g_EyePos;
extern gmtl::VecA3f		g_EyeDir;
extern gmtl::VecA3f		g_EyeUpVec;
extern gmtl::MatrixA44f	g_View;
extern gmtl::MatrixA44f	g_Proj;
extern gmtl::MatrixA44f	g_DebugViewProj;
extern gmtl::MatrixA44f	g_DebugView;
extern gmtl::MatrixA44f	g_DebugProj;
extern gmtl::VecA3f		g_DebugEyePos;
extern gmtl::VecA3f		g_PointLightPos;

extern BOOL				g_IsFullScreen;
extern boolean			g_IsRenderReady;

extern float			g_CurIndex;
extern float			g_IncRate;

extern void*				g_pD3DDevice;

extern gmtl::VecA3f			g_CameraFarCornerVecs[4];

const static float FAR_PLANE = 800.0f;
const static float NEAR_PLANE = 0.1f;