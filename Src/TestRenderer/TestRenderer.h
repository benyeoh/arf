//================================================================================
//
//		TestRenderer.h
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

extern HINSTANCE hInst;								// current instance
extern Module g_hRenderer;
extern Module g_hPlatform;
extern Module g_hFileSystem;
extern Module g_hInput;
extern Module g_hCoreMem;

extern MemAllocFns g_MemAllocFns;

extern HWND	g_hWnd;

typedef LRESULT (*FeedWindowsMessageFn)(HWND, UINT, WPARAM, LPARAM);
extern FeedWindowsMessageFn g_FeedWindowsMessage;

typedef void* (*GetD3DDeviceFn)(IRRenderer* pRenderer);

extern IRRendererPtr		g_pRenderer;
extern IRRenderGroupPtr		g_pMainClearGroup;

extern IFFileSystemPtr		g_pFileSystem;

extern ICMMemAllocatorPtr	g_pAllocator;

extern IIInputPtr			g_pInput;
//extern IFFXFontFXPtr		g_pFontFX;
//extern IFFXFontFacePtr		g_pVerdanaFontFace;
//extern IFFXFontFacePtr		g_pCourierFontFace;
extern IPPlatformPtr		g_pPlatform;
//extern IDFXDebugFXPtr		g_pDebugFX;

extern float	m_LastTimeElapsed;
extern uint		m_NumFrames;
extern uint		m_TotalNumFrames;
extern uint		m_LastNumOfFrames;
extern float	g_TimeDT;

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

const static float FAR_PLANE = 600.0f;
const static float NEAR_PLANE = 0.01f;