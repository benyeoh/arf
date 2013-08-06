//================================================================================
//
//		TestSHPRTFX..h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		5/21/2012
//
//================================================================================

#pragma once

_USE_NAMESPACE

const static uint NUM_MESHES = 4;

#include "BRDFDirs.h"
#include "AppCallback.h"
#include "BRDFCallback.h"
#include "SHPRTCallback.h"
#include "SHRadianceCallback.h"
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
extern Module g_hSHPRTCompute;
extern Module g_hAmbientFX;

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
extern ISHPRTComputePtr		g_pSHPRTCompute;
extern IAFXAmbientFXPtr		g_pAmbientFX;
extern IAFXParamPoolPtr		g_pAFXParamPool;
extern IPThreadPoolPtr		g_pThreadPool;

extern BFXUMeshList*	g_pRoomMeshList;

extern BFXUMeshListPtr	g_pMeshes[NUM_MESHES];
extern gmtl::MatrixA44f g_MeshesWorld[NUM_MESHES];

extern BFXUMeshListPtr	g_pSkyDome;

//extern uint		g_CurObjectIndex;

extern float	m_LastTimeElapsed;
extern uint		m_NumFrames;
extern uint		m_TotalNumFrames;
extern uint		m_LastNumOfFrames;
extern float	g_TimeDT;
extern boolean	g_IsDebugRenderObjects;
extern boolean	g_IsDebugRenderSceneDB;
extern boolean	g_IsWireFrameMode;

extern AppCallback	g_AppCallback;
extern SHPRTCallback g_SHPRTCallback;
extern BRDFCallback	 g_BRDFCallback;
extern SHRadianceCallback g_RadianceCallback;

extern BFXHemiLight		g_HemiLight;
extern BFXDirLight		g_DirLight;
extern BFXPointLight	g_PointLight;
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
extern AFXAmbientCoeffs		g_AmbientCoeffs;
extern AFXTransferMatrix	g_AmbientRed;
extern AFXTransferMatrix	g_AmbientGreen;
extern AFXTransferMatrix	g_AmbientBlue;
extern gmtl::VecA3f			g_AmbientZ;
extern gmtl::VecA3f			g_AmbientY;

extern float g_TripleProductTable[9 * 9 * 9];

const static uint NUM_BOUNCES = 4;
const static uint NUM_FILTERS = 1;
const static float POS_BIAS = 0.013f;
const static float DOT_THRESHOLD = 0.45f;

const static float FAR_PLANE = 500.0f;
const static float NEAR_PLANE = 0.01f;