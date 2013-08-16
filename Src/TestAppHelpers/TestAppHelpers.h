//================================================================================
//
//		TestAppHelpers.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/10/2012
//
//================================================================================

#pragma once

_USE_NAMESPACE

enum
{
	COMP_AI_FLOCKING = COMP_APPHELPERS_NUM,
    COMP_AI_CIRCULAR_MOVE,
};

enum
{
	SDB_FILTER_FLOCKING_COMP = 0x00000008,
};

#include "AppCallback.h"
#include "Render.h"
#include "Update.h"
#include "Init.h"

#include "FlockingAIComponent.h"
#include "AABoxAvoidanceBehavior.h"
#include "AlignmentBehavior.h"
#include "CohesionBehavior.h"
#include "SeparationBehavior.h"
#include "SphereAvoidanceBehavior.h"

#include "CircularMovementAIComponent.h"

#include "BRDFDirs.h"
#include "BRDFCallback.h"

#ifdef _USE_PRT_VERSION
const static uint NUM_POINT_LIGHTS = 0;
const static uint NUM_THREADS = 3;
const static uint NUM_PRTCOMPUTE_PHASE_THREADS = 1;
const static uint NUM_PRTCOMPUTE_PROCESS_THREADS = 3;
const static uint NUM_ENTITIES = 6;
#else
const static uint NUM_POINT_LIGHTS = 0;
const static uint NUM_THREADS = 3;
const static uint NUM_PRTCOMPUTE_PHASE_THREADS = 1;
const static uint NUM_PRTCOMPUTE_PROCESS_THREADS = 1;
const static uint NUM_ENTITIES = 200000;
const static float AREA_ROOT = 7000.0f;

enum
{
    USE_ARRAY,
    USE_SPARSE_LOOSE_OCTREE,
    USE_OCTANT_ORDER_LOOSE_OCTREE,
};

const static uint DB_USAGE = USE_OCTANT_ORDER_LOOSE_OCTREE;
#endif

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
extern Module g_hSHPRTCompute;

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
extern IPThreadPoolPtr		g_pThreadPool;

extern ISDBSceneContainerPtr g_pSceneContainer;
extern ISDBSceneContainerPtr g_pSceneEntityFlockContainer;
extern ISDBSceneContainerPtr g_pSceneLightFlockContainer;

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
extern ISHPRTComputePtr		g_pSHPRTCompute;

extern IRTexture2D* g_pZPos;
extern IRTexture2D* g_pNormals;
extern IRTexture2D* g_pMaterial;

extern AHSceneRenderPhasePtr	g_pSceneRenderPhase;
extern AHEntityManagerPtr		g_pEntityMgr;
extern AHMaterialCreateCallbackImmedPtr g_pMaterialCreateCallback;
extern AHSpatialComponent*		g_pSkyDomeSpatial;
extern AHScenePRTComputePtr		g_pAHPRTCompute;

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

const static float FAR_PLANE = 1200.0f;
const static float NEAR_PLANE = 0.1f;