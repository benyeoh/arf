//==============================================================================
//
//		Global.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/6/2007
//
//==============================================================================

#pragma once 

extern IRRendererPtr g_pRenderer;
extern IFFileSystemPtr g_pFileSystem;

void
AllocateGlobalData();

void
FreeGlobalData();

void
GetAllObjectsWithPolygonMeshes(XSI::CRefArray& toStore, XSI::X3DObject& curObj, boolean doGetHiddenObjects=FALSE);

XSI::CStatus
ShowProperty(XSI::CustomProperty& prop, const XSI::CString title, XSI::siInspectMode mode=XSI::siRecycle);

//void
//InitializeRenderer();
//
//void
//InitializeFileSystem();
//
//
