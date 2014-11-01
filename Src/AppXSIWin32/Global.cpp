//==============================================================================
//
//		Global.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/6/2007
//
//==============================================================================

#include "stdafx.h"

IRRendererPtr g_pRenderer;
IFFileSystemPtr g_pFileSystem;

HMODULE g_hFileSystem = NULL;
HMODULE g_hRenderer = NULL;

wstring* s_pApplicationRoot;

AppCallback s_AppCallback;

void
AllocateGlobalData()
{
	g_hFileSystem = ::LoadLibrary(_W("FileSystem.dll"));
	_DEBUG_ASSERT(g_hFileSystem);

	CreateFileSystemFn CreateFileSystem = (CreateFileSystemFn) ::GetProcAddress(g_hFileSystem, "CreateFileSystem");
	_DEBUG_ASSERT(CreateFileSystem);

	g_pFileSystem = CreateFileSystem();	
	_DEBUG_ASSERT(g_pFileSystem);

	g_pFileSystem->Initialize();
	
	wchar filePath[256];
	GetModuleFileName(g_hFileSystem, filePath, 256);
	s_pApplicationRoot = _NEW wstring;
	*s_pApplicationRoot = filePath;
	std::transform(s_pApplicationRoot->begin(), s_pApplicationRoot->end(), s_pApplicationRoot->begin(), towlower);

	int index = (int) s_pApplicationRoot->find(_W("\\bin\\"));
	if(index != wstring::npos)
	{
		*s_pApplicationRoot = s_pApplicationRoot->substr(0, index);
	}
	
	g_pFileSystem->Open(s_pApplicationRoot->c_str());
	g_pFileSystem->AddMnemonic(_W("shd"), _W("data\\shaders"));
	g_pFileSystem->AddMnemonic(_W("msh"), _W("data\\meshes"));
	g_pFileSystem->AddMnemonic(_W("tex"), _W("data\\textures"));
	g_pFileSystem->AddMnemonic(_W("eff"), _W("data\\effects"));
	g_pFileSystem->AddMnemonic(_W("mat"), _W("data\\materials"));
	g_pFileSystem->AddMnemonic(_W("dat"), _W("data"));

	g_hRenderer = ::LoadLibrary(_W("Renderer.dll"));
	CreateRendererFn CreateRendererD3D = (CreateRendererFn)::GetProcAddress(g_hRenderer, "CreateRenderer");
	g_pRenderer = CreateRendererD3D();
	_DEBUG_ASSERT(g_pRenderer);
}

void
FreeGlobalData()
{
	_DELETE(s_pApplicationRoot);
	
	g_pFileSystem = NULL;
	g_pRenderer = NULL;

	::FreeLibrary(g_hFileSystem);
	::FreeLibrary(g_hRenderer);
}

void
InitializeRenderer()
{
	HWND hDesktop = GetDesktopWindow();

	RInitInfo info;
	info.presentInfo.backBufferHeight = 768;
	info.presentInfo.backBufferWidth = 1024;
	info.presentInfo.fullScreen = FALSE;
	info.presentInfo.hWnd = hDesktop;
	info.pApplication = &s_AppCallback;
	g_pRenderer->Initialize(info);
}

void
GetAllObjectsWithPolygonMeshes(XSI::CRefArray& toStore, XSI::X3DObject& curObj, boolean doGetHiddenObjects)
{
	XSI::CString name = curObj.GetName();

	if (!curObj.IsA(XSI::siLightID) && 
		!curObj.IsA(XSI::siNullID) && 
		!curObj.IsA(XSI::siModelID))
	{

		XSI::Primitive prim(curObj.GetActivePrimitive());
		if (prim.IsValid())
		{
			XSI::Geometry geom(prim.GetGeometry());
			if(geom.IsValid() && geom.IsA(XSI::siPolygonMeshID))
			{
				XSI::Property prop = curObj.GetLocalProperties().GetItem(L"Visibility");
				XSI::CValue viewvis = prop.GetParameterValue(L"viewvis");
				if(doGetHiddenObjects || viewvis == true) 
				{
					toStore.Add(curObj);
				}
			}
		}

	}

	// Cascade into children
	XSI::CRefArray children = curObj.GetChildren();
	for(long i = 0; i < children.GetCount(); i++)
	{
		XSI::X3DObject childObj = children[i];
		GetAllObjectsWithPolygonMeshes(toStore, childObj, doGetHiddenObjects);
	}
}	

XSI::CStatus
ShowProperty(XSI::CustomProperty& prop, const XSI::CString title, XSI::siInspectMode mode)
{
	XSI::Application app;

	if(!prop.IsValid())
	{	
		return XSI::CStatus::Fail;
	}

	// Show Property Window
	XSI::CValueArray args(5);
	XSI::CValue retVal;

	args[0] = prop;
	args[1] = _W("");
	args[2] = title;
	args[3] = (long) mode;
	args[4] = false;
	app.ExecuteCommand(L"InspectObj", args, retVal); 

	XSI::CStatus status = ((bool)retVal ? XSI::CStatus::Fail : XSI::CStatus::OK);

	return status;
}