//================================================================================
//
//		TestBakedSM.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/10/2012
//
//================================================================================

#pragma once

_USE_NAMESPACE

#include "AppCallback.h"
#include "Render.h"
#include "Update.h"
#include "Init.h"

#include "BakedSMDefines.h"
#include "BakedSMParamPool.h"
#include "BakedSMComputePipeline.h"
#include "BakedSMCompute.h"

#include "BakedSMMeshCreate.h"
#include "BakedSMRenderCallback.h"

const static uint NUM_POINT_LIGHTS = 0;
const static uint NUM_THREADS = 3;

const static uint MAX_GRID_DEBUG = 64000;
extern AABox* g_pSparseGridCells;

extern HINSTANCE hInst;								// current instance
extern Module g_hRenderer;
extern Module g_hPlatform;
extern Module g_hFileSystem;
extern Module g_hInput;
extern Module g_hCoreMem;
extern Module g_hMatGen;
extern Module g_hBaseFX;
extern Module g_hShadowFX;
extern Module g_hDeferredFX;
extern Module g_hSceneDB;

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
extern BakedSMComputePtr    g_pBakedSMCompute;
extern BakedSMRenderCallback g_BakedSMCallback;

extern BFXParamContainer	g_ParamContainer;
extern BFXRenderContainer	g_RenderContainer;

extern ICMMemAllocatorPtr	g_pAllocator;
extern IPThreadPoolPtr		g_pThreadPool;

extern ISDBSceneContainerPtr g_pSceneContainer;

extern ISDBSceneDBPtr		g_pSceneDB;
extern IIInputPtr			g_pInput;
extern IFFXFontFXPtr		g_pFontFX;
extern IFFXFontFacePtr		g_pVerdanaFontFace;
extern IFFXFontFacePtr		g_pCourierFontFace;
extern IPPlatformPtr		g_pPlatform;
extern IDFXDebugFXPtr		g_pDebugFX;
extern IDEFXDeferredFXPtr	g_pDeferredFX;
extern IDEFXPipelinePtr		g_pDeferredPipeline;
extern IDEFXParamPoolPtr	g_pDeferredParamPool;
extern ISHFXShadowFXPtr		g_pShadowFX;
extern ISHFXPipelinePtr		g_pSHFXPipeline;
extern ISHFXParamPoolPtr	g_pSHFXParamPool;

extern IRTexture2D* g_pZPos;
extern IRTexture2D* g_pNormals;
extern IRTexture2D* g_pMaterial;

extern AHSceneRenderPhasePtr	g_pSceneRenderPhase;
extern AHEntityManagerPtr		g_pEntityMgr;
extern AHMaterialCreateCallbackImmedPtr g_pMaterialCreateCallback;
extern AHSpatialComponent*		g_pSkyDomeSpatial;

extern uint		g_CurObjectIndex;

extern float	m_LastTimeElapsed;
extern uint		m_NumFrames;
extern uint		m_TotalNumFrames;
extern uint		m_LastNumOfFrames;
extern float	g_TimeDT;
extern boolean	g_IsDebugRenderObjects;
extern boolean	g_IsDebugRenderSceneDB;
extern boolean	g_IsDebugRenderLights;
extern boolean	g_IsWireFrameMode;

extern AppCallback	g_AppCallback;

extern BFXHemiLight		g_HemiLight;

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

extern float			g_CurIndex;
extern float			g_IncRate;

const static uint NUM_TIMES_AVERAGE = 64;
extern double			g_RenderTimes[NUM_TIMES_AVERAGE];
extern double			g_UpdateTimes[NUM_TIMES_AVERAGE];
extern uint				g_TimeCounter;

extern void*				g_pD3DDevice;

extern gmtl::VecA3f		g_CameraFarCornerVecs[4];
extern gmtl::VecA3f		g_ViewFarCornerVecs[4];

const static float FAR_PLANE = 1000.0f;
const static float NEAR_PLANE = 0.01f;

const static uint NUM_CUBES = 15;
const static uint NUM_MESHES = NUM_CUBES * 6 + 3;
extern gmtl::MatrixA44f	g_MeshesWorld[NUM_MESHES];
extern BFXMesh g_Meshes[NUM_MESHES];

extern float g_TestLightSize;
extern float g_TestLightDist;

extern BakedSMAreaLight g_AreaLights[16];