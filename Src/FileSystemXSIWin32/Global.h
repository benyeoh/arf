//==============================================================================
//
//		Global.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/13/2007
//
//==============================================================================

#pragma once 

extern IFFileSystemPtr g_pFileSystem;
extern IDataStoreForXSIPlugin* g_pDataStore;
extern wstring* s_pApplicationRoot;

void
AllocateGlobalData();

void
FreeGlobalData();

void
InitializeFileSystem();

void
Initialize();

void
Shutdown();

XSI::CString
GetRelativePath(const XSI::CString absolutePath, const XSI::CString pathToRemove);

XSI::CStatus
ShowProperty(XSI::CustomProperty& prop, const XSI::CString title, XSI::siInspectMode mode=XSI::siRecycle);