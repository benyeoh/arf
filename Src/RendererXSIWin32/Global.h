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

extern IRRendererPtr g_pRenderer;
extern IFFileSystemPtr g_pFileSystem;

void
AllocateGlobalData();

void
FreeGlobalData();

void
InitializeRenderer();

void
InitializeFileSystem();

void
GetAllObjectsWithPolygonMeshes(XSI::CRefArray& toStore, XSI::X3DObject& curObj, boolean doGetHiddenObjects=FALSE);

XSI::CString
GetRelativePath(const XSI::CString absolutePath, const XSI::CString pathToRemove);

const wchar*
GetApplicationRoot();

// Math conversion
inline gmtl::Matrix44f& 
XSIToGMTL(gmtl::Matrix44f& dest, const XSI::MATH::CMatrix4 src)
{
	dest[0][0] = (float)src.GetValue(0,0);
	dest[1][0] = (float)src.GetValue(0,1);
	dest[2][0] = (float)src.GetValue(0,2);
	dest[3][0] = (float)src.GetValue(0,3);
	dest[0][1] = (float)src.GetValue(1,0);
	dest[1][1] = (float)src.GetValue(1,1);
	dest[2][1] = (float)src.GetValue(1,2);
	dest[3][1] = (float)src.GetValue(1,3);
	dest[0][2] = (float)src.GetValue(2,0);
	dest[1][2] = (float)src.GetValue(2,1);
	dest[2][2] = (float)src.GetValue(2,2);
	dest[3][2] = (float)src.GetValue(2,3);
	dest[0][3] = (float)src.GetValue(3,0);
	dest[1][3] = (float)src.GetValue(3,1);
	dest[2][3] = (float)src.GetValue(3,2);
	dest[3][3] = (float)src.GetValue(3,3);
	
//	dest.setState(gmtl::Matrix44f::FULL);
	
	return dest;	
}

XSI::CStatus
ShowProperty(XSI::CustomProperty& prop, const XSI::CString title, XSI::siInspectMode mode=XSI::siRecycle);

void Initialize();
void Shutdown();