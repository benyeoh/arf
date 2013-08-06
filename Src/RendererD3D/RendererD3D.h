//==============================================================================
//
//		RendererD3D.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Tuesday, February 07, 2006
//
//==============================================================================

#pragma once

//// Modify the following defines if you have to target a platform prior to the ones specified below.
//// Refer to MSDN for the latest info on corresponding values for different platforms.
//#ifndef WINVER				// Allow use of features specific to Windows XP or later.
//#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
//#endif
//
//#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
//#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
//#endif						
//
//#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
//#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
//#endif
//
//#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
//#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
//#endif

#include <windows.h>

// Platform independent stuff
#include <CoreMemory\CoreMemory.h>
#include <Renderer\RendererFramework.h>

// D3D stuff
#include <d3d9.h>
#include <d3dx9.h>

_NAMESPACE_BEGIN

_DECLARE_SMARTPTR(IDirect3D9)
_DECLARE_SMARTPTR(IDirect3DDevice9)
_DECLARE_SMARTPTR(IDirect3DTexture9)
_DECLARE_SMARTPTR(IDirect3DCubeTexture9)
_DECLARE_SMARTPTR(IDirect3DSurface9)
_DECLARE_SMARTPTR(IDirect3DVolume9)
_DECLARE_SMARTPTR(IDirect3DVolumeTexture9)
_DECLARE_SMARTPTR(ID3DXEffect)
_DECLARE_SMARTPTR(IDirect3DVertexBuffer9)
_DECLARE_SMARTPTR(IDirect3DVertexDeclaration9)
_DECLARE_SMARTPTR(IDirect3DIndexBuffer9)
_DECLARE_SMARTPTR(ID3DXConstantTable)

class CRRendererD3D;

const static uint MAX_SIZE_INSTANCED_VB = 4096;

inline uint
GetEffectParamSize(eREffectParamType paramType)
{
	switch(paramType)
	{
		case EPT_MAT44_P:	return (sizeof(float) * 4 * 4);	
		case EPT_MAT33_P:	return (sizeof(float) * 3 * 3);	
		case EPT_MAT22_P:	return (sizeof(float) * 2 * 2);	
		case EPT_VEC4_P:	return (sizeof(float) * 4);		
		case EPT_VEC3_P:	return (sizeof(float) * 3);		
		case EPT_VEC2_P:	return (sizeof(float) * 2);		
		case EPT_FLOAT:		return (sizeof(float));			
		case EPT_INT:		return (sizeof(int));
		
		default:	_DEBUG_ASSERT(FALSE); break;
	}
	
	return -1;
}

_NAMESPACE_END

#include "D3DConversionFns.h"
#include "RD3DEffectStateManager.h"
#include "CRIndexBufferD3D.h"
#include "CRVertexBufferD3D.h"
#include "CRVertexBufferGroupD3D.h"
#include "CREffectD3DTemplate.h"
#include "CRTextureRTD3D.h"
#include "CRTexture2DD3D.h"
#include "CRTexture3DD3D.h"
#include "CRTextureCubeD3D.h"
#include "CRDepthBufferD3D.h"
//#include "CRPipelineD3D.h"
#include "CRResourceMgrD3D.h"
#include "CRRendererD3D.h"
#include "CRRenderGroupD3D.h"

extern "C"
{
	_DECL_DYNAMIC_EXPORT( void* GetD3DDevice(_NAMESPACE IRRenderer* pRendererD3D) )
}

