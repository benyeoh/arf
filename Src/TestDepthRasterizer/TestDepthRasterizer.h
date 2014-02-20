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

extern MemAllocFns g_MemAllocFns;

extern HWND	g_hWnd;

typedef LRESULT (*FeedWindowsMessageFn)(HWND, UINT, WPARAM, LPARAM);
extern FeedWindowsMessageFn g_FeedWindowsMessage;

typedef void* (*GetD3DDeviceFn)(IRRenderer* pRenderer);

extern IRRendererPtr		g_pRenderer;
extern IRRenderGroupPtr		g_pMainClearGroup;
extern IRRenderGroupPtr		g_pHWGroup;
extern IRRenderGroupPtr		g_pSWGroup;

extern IFFileSystemPtr		g_pFileSystem;
extern ICMMemAllocatorPtr	g_pAllocator;

extern IIInputPtr			g_pInput;
extern IFFXFontFXPtr		g_pFontFX;
extern IFFXFontFacePtr		g_pVerdanaFontFace;
extern IFFXFontFacePtr		g_pCourierFontFace;
extern IPPlatformPtr		g_pPlatform;
extern IDFXDebugFXPtr		g_pDebugFX;

extern IRVertexBufferGroupPtr	g_pTriangleVBGroup;
extern IRIndexBufferPtr			g_pTriangleIB;
extern IREffectPtr				g_pTriangleEffect;
extern IRTexture2DPtr			g_pRasterizeTex;
extern float*					g_pRasterizeBuffer;
extern float*					g_pRasterizeDepthBuffer;
extern uint*					g_pFastDepthClearBuffer;
extern IRVertexBufferGroupPtr	g_pQuadVBGroup;
extern IRIndexBufferPtr			g_pQuadIB;
extern IREffectPtr				g_pQuadTexEffect;
extern IRVertexBufferGroupPtr	g_pCubeVBGroup;
extern IRIndexBufferPtr			g_pCubeIB;
extern IREffectPtr				g_pCubeEffect;

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
extern double			g_SWTimeElapsed;
extern float			g_VertTransTimeElapsed;
extern float            g_SWQuadTimeElapsed;
extern void*			g_pD3DDevice;
extern uint				g_NumTriangles;
extern uint             g_NumPixelsProcessed;

const static uint NUM_CUBES = 512;
const static int NUM_THREADS = 7;

extern IPThreadPoolPtr	g_pThreadPool;
extern IPThreadPtr		g_pThreads[NUM_THREADS];

extern gmtl::MatrixA44f g_CubeWorld[NUM_CUBES];
extern gmtl::MatrixA44f g_CubeWorldViewProj[NUM_CUBES];
extern gmtl::Vec3f		g_CubeRotAxis[NUM_CUBES];

//const static uint RASTERIZE_BUFFER_W = 1280;
//const static uint RASTERIZE_BUFFER_H = 720;
//const static uint BIN_WIDTH     = 320;
//const static uint BIN_HEIGHT    = 180;
const static uint RASTERIZE_BUFFER_W = 256;
const static uint RASTERIZE_BUFFER_H = 128;
const static uint BIN_WIDTH     = 64;
const static uint BIN_HEIGHT    = 32;
const static uint NUM_BINS_X = RASTERIZE_BUFFER_W / BIN_WIDTH;
const static uint NUM_BINS_Y = RASTERIZE_BUFFER_H / BIN_HEIGHT;

const static uint       NUM_BIN_CONTEXTS = NUM_THREADS + 1;
extern TriangleBin*     g_pTriBins;
extern uint*            g_pNumTrisInBins;

const static uint FAST_DEPTH_CLEAR_W = (RASTERIZE_BUFFER_W / 2) / 32;
const static uint FAST_DEPTH_CLEAR_H = RASTERIZE_BUFFER_H / 2;

//const static float CUBE_SIZE = 1.0f;
const static float FAR_PLANE = 20.0f;
const static float FAR_PLANE_RCP = 1.0f / FAR_PLANE;
const static float NEAR_PLANE = 0.1f;
const static uint CUBE_VERTEX_STRIDE	= 5;
const static uint CUBE_TEXUV_OFFSET		= 3;

const static float CUBE_SIZE = 1.0f;
const static float CUBE_DISTANCE_MULT = 1.0f;

const static uint NUM_CUBE_VERTICES = 24;
const static float g_CubeVertices[CUBE_VERTEX_STRIDE * NUM_CUBE_VERTICES] = 
{
	// Front
	-CUBE_SIZE, CUBE_SIZE, CUBE_SIZE,		0.0f, 0.0f,		// 0
	CUBE_SIZE, CUBE_SIZE, CUBE_SIZE,		1.0f, 0.0f,		// 1
	-CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE,		0.0f, 1.0f,		// 2
	CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE,		1.0f, 1.0f,		// 3

	// Back
	CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE,		0.0f, 0.0f,		// 4
	-CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE,		1.0f, 0.0f,		// 5
	CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE,		0.0f, 1.0f,		// 6
	-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE,		1.0f, 1.0f,		// 7

	// Top
	-CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE,		0.0f, 0.0f,		// 8
	CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE,		1.0f, 0.0f,		// 9
	-CUBE_SIZE, CUBE_SIZE, CUBE_SIZE,		0.0f, 1.0f,		// 10
	CUBE_SIZE, CUBE_SIZE, CUBE_SIZE,		1.0f, 1.0f,		// 11

	// Bottom
	-CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE,		0.0f, 0.0f,		// 12
	CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE,		1.0f, 0.0f,		// 13
	-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE,		0.0f, 1.0f,		// 14
	CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE,		1.0f, 1.0f,		// 15
	
	// Left
	-CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE,		0.0f, 0.0f,		// 16
	-CUBE_SIZE, CUBE_SIZE, CUBE_SIZE,		1.0f, 0.0f,		// 17
	-CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE,		0.0f, 1.0f,		// 18
	-CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE,		1.0f, 1.0f,		// 19

	// Right
	CUBE_SIZE, CUBE_SIZE, CUBE_SIZE,		0.0f, 0.0f,		// 20
	CUBE_SIZE, CUBE_SIZE, -CUBE_SIZE,		1.0f, 0.0f,		// 21
	CUBE_SIZE, -CUBE_SIZE, CUBE_SIZE,		0.0f, 1.0f,		// 22
	CUBE_SIZE, -CUBE_SIZE, -CUBE_SIZE,		1.0f, 1.0f,		// 23
};

const static ushort g_CubeIndices[] = 
{
	// Front
	0, 1, 2,
	1, 3, 2,

	// Back
	4, 5, 6,
	5, 7, 6,

	// Top
	8, 9, 10,
	9, 11, 10,

	// Bottom
	12, 13, 14,
	13, 15, 14,

	// Left
	16, 17, 18,
	17, 19, 18,

	// Right
	20, 21, 22,
	21, 23, 22,
};