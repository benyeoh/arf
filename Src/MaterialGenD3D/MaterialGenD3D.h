//==============================================================================
//
//		MaterialGenD3D.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/8/2008
//
//==============================================================================

#pragma once

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
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

// D3D stuff
//#include <d3d9.h>
//#include <d3dx9.h>

// Platform independent stuff
#include <CoreMemory\CoreMemory.h>
#include <MaterialGen\MaterialGenFramework.h>

_NAMESPACE_BEGIN

class CMGMaterialGenD3D;

_NAMESPACE_END

#include "CMGMaterialTemplateD3D.h"
#include "CMGResourceMgrD3D.h"
#include "CMGMaterialGenD3D.h"
