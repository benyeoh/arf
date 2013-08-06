//================================================================================
//
//		TestScreenAmbOccl.h
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
#include "ParamCallback.h"

const static uint NUM_POINT_LIGHTS = 0;

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

extern MemAllocFns g_MemAllocFns;

extern HWND	g_hWnd;

typedef LRESULT (*FeedWindowsMessageFn)(HWND, UINT, WPARAM, LPARAM);
extern FeedWindowsMessageFn g_FeedWindowsMessage;

typedef void* (*GetD3DDeviceFn)(IRRenderer* pRenderer);

extern IRRendererPtr		g_pRenderer;
extern IRRenderGroupPtr		g_pMainClearGroup;
extern IRRenderGroupPtr		g_pZPosClearGroup;
extern IRRenderGroupPtr		g_pNormalsClearGroup;

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
extern IDEFXDeferredFXPtr	g_pDeferredFX;
extern IDEFXPipelinePtr		g_pDeferredPipeline;
extern IDEFXLightGroupPtr	g_pDeferredLightGroup[NUM_POINT_LIGHTS+1];
extern IDEFXParamPoolPtr	g_pDeferredParamPool;
extern ISHFXShadowFXPtr		g_pShadowFX;
extern ISHFXPipelinePtr		g_pSHFXPipeline;
extern ISHFXParamPoolPtr	g_pSHFXParamPool;
extern IDEFXLightGroupPtr	g_pDeferredDirLightGroup;

extern IRTexture2D* g_pZPos;
extern IRTexture2D* g_pNormals;

extern BFXUMeshList*	g_pRoomMeshList;

const static uint NUM_MESHES = 1;
extern BFXUMeshListPtr	g_pMeshes[NUM_MESHES];
extern gmtl::MatrixA44f g_MeshesWorld[NUM_MESHES];

extern BFXUMeshListPtr	g_pPointLightMesh;
extern gmtl::MatrixA44f	g_PointLightWorld[NUM_POINT_LIGHTS+1];

extern BFXUMeshListPtr	g_pDirLightMesh;

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
//extern BFXDirLight		g_DirLight;
//extern BFXPointLight	g_PointLight;

extern DEFXPointLight	g_PointLight[NUM_POINT_LIGHTS+1];
extern ISHFXCubePointLightPtr g_pShadowPointLight[NUM_POINT_LIGHTS+1];
extern DEFXDirLight		g_DirLight;
extern ISHFXCascadedDirLightPtr g_pShadowDirLight;
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
extern gmtl::VecA3f		g_PointLightPos[NUM_POINT_LIGHTS+1];

extern BOOL				g_IsFullScreen;
extern boolean			g_IsRenderReady;

extern float			g_CurIndex;
extern float			g_IncRate;

extern void*				g_pD3DDevice;

extern gmtl::VecA3f		g_CameraFarCornerVecs[4];
extern gmtl::VecA3f		g_ViewFarCornerVecs[4];

const static float FAR_PLANE = 600.0f;
const static float NEAR_PLANE = 0.01f;

struct IVarParamEntry
{
	virtual const wchar* GetString() = 0;
	virtual void Inc() = 0;
	virtual void Dec() = 0;
};

const static int NUM_AO_PARAM_ENTRIES = 7;
extern int g_CurAOParamEntry;
extern IVarParamEntry* g_pAOParamEntries[NUM_AO_PARAM_ENTRIES];

// Dynamic params
const static int NUM_SAMPLES = 0xFFFF0000;
const static int ALCHEMY_AO_PARAMS	= NUM_SAMPLES + 1;
const static int DISTRIBUTED_SAMPLE_PARAMS = ALCHEMY_AO_PARAMS + 1;
const static int CAMERA_POS_WITH_VIEW_FAR = DISTRIBUTED_SAMPLE_PARAMS + 1;
const static int FAR_PLANE_OFFSETS = CAMERA_POS_WITH_VIEW_FAR + 1;

// Constant params
const static int NOISE_TEX = 0xFFFFAAAA;
const static int Z_POS_TEX = NOISE_TEX + 1;
const static int NORMALS_TEX = Z_POS_TEX + 1;
const static int PROJ_PARAMS = NORMALS_TEX + 1;
const static int SAMPLE_LOD = PROJ_PARAMS + 1;
const static int HALF_PIXEL_OFFSET = SAMPLE_LOD + 1;

const static int NUM_Z_MIPS = 6;

extern BFXUScreenSpaceMesh g_AOScreenMesh;
extern BFXUScreenSpaceMesh g_ZDownsampleMesh[NUM_Z_MIPS];
extern IRTexture2DPtr g_pAOBuffer;
extern IRTexture2DPtr g_pNoiseTex;
extern IREffectPtr g_pAOEffect;
extern IREffectPtr g_pDownsampleEffect[NUM_Z_MIPS];
extern IRRenderGroupPtr g_pAOGroup;
extern IRRenderGroupPtr g_pDownsampleGroups[NUM_Z_MIPS];

const static int NUM_TECHS = 2;
extern int g_CurTechnique;
extern IVarParamEntry* g_pTechEntry;

const static int NUM_DOWNSAMPLE_TECHS = 2;
extern int g_CurDownsampleTechnique;
extern IVarParamEntry* g_pTechDownsampleEntry;

struct FarPlaneOffsets
{
	gmtl::Vec4f offset[3];
};

extern int g_NumSamples;
extern gmtl::Vec4f g_AlchemyAOParams;
extern gmtl::Vec4f g_DistributedSampleParams;
extern gmtl::Vec4f g_CameraPosWithViewFar;
extern FarPlaneOffsets g_FarPlaneOffsets;

