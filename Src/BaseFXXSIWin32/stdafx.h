//==============================================================================
//
//		stdafx.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/9/2008
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

// XSI headers

#include <xsi_pluginregistrar.h>
#include <xsi_eventinfo.h>

// menu
#include <xsi_application.h>
#include <xsi_context.h>
#include <xsi_menu.h>
#include <xsi_model.h>

// property
#include <xsi_ppglayout.h>
#include <xsi_ppgeventcontext.h>
#include <xsi_uitoolkit.h>
#include <xsi_progressbar.h>
#include <xsi_desktop.h>
//
#include <xsi_scene.h>
#include <xsi_project.h>
#include <xsi_material.h>
#include <xsi_materiallibrary.h>
#include <xsi_shader.h>
#include <xsi_imageclip2.h>
#include <xsi_texture.h>
#include <xsi_kinematics.h>
#include <xsi_kinematicstate.h> 
#include <xsi_geometryaccessor.h>
#include <xsi_geometry.h>
#include <xsi_primitive.h>
#include <xsi_userdatamap.h>
#include <xsi_polygonmesh.h>
#include <xsi_triangle.h>
#include <xsi_trianglevertex.h>

// command
#include <xsi_command.h>
#include <xsi_argument.h> 
#include <xsi_selection.h>

#include "BaseFXXSIWin32.h"

_USE_NAMESPACE

#include "AppCallback.h"
#include "Global.h"
#include "BaseFXMenu.h"
#include "ExportBFXMeshFn.h"
#include "ExportBFXUMeshListFn.h"
#include "CreateBaseFXMaterialProperty.h"
#include "ExportBFXUMeshListProperty.h"
#include "ExportBFXMaterialFn.h"
#include "ExportBFXMaterialCommand.h"
#include "ExportBFXMeshCommand.h"


// TODO: reference additional headers your program requires here
