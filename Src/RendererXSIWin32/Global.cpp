//==============================================================================
//
//		Global.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/13/2007
//
//==============================================================================

#include "stdafx.h"

IRRendererPtr g_pRenderer;
IFFileSystemPtr g_pFileSystem = NULL;
IDataStoreForXSIPlugin* g_pDataStore = NULL;

RendererCallbacks g_Callback;

Module g_hDataStore = NULL;
Module g_hRenderer = NULL;

const wchar*
GetApplicationRoot()
{
	InitializeFileSystem();

	return g_pFileSystem->GetResourcePath();
}

void
AllocateGlobalData()
{
	g_hDataStore = InitializeModule(_W("DataStoreForXSIPlugin.dll"), NULL);
	GetDataStoreFn CallGetDataStore = (GetDataStoreFn) GetFunction(g_hDataStore, "GetDataStore");
	_DEBUG_ASSERT(CallGetDataStore);

	g_pDataStore = CallGetDataStore();
	_DEBUG_ASSERT(g_pDataStore);
}

void
FreeGlobalData()
{
	g_pFileSystem = NULL;
	g_pRenderer = NULL;

	//ShutdownModule(g_hRenderer);
}

void
InitializeFileSystem()
{
	if(!g_pFileSystem)
	{
		XSI::Application app;
		XSI::CValue returnVal;
		XSI::CValueArray args;
		XSI::CStatus status = app.ExecuteCommand(_W("AppComGetFileSystem"), args, returnVal);
		if(status == XSI::CStatus::OK)
		{
			// TODO: FileSystem will not be valid if the plugins are not cached
			g_pFileSystem = (IFFileSystem*) ((ULONG)returnVal);
			_DEBUG_ASSERT(g_pFileSystem);
		}
	}
}

void
InitializeRenderer()
{
	g_pRenderer = (IRRenderer*) g_pDataStore->GetRefCountData(RENDERER_XSI);

	if(!g_pRenderer)
	{
		g_hRenderer = InitializeModule(_W("RendererD3D.dll"), NULL);

		CreateRendererFn CreateRendererD3D = (CreateRendererFn) GetFunction(g_hRenderer, "CreateRenderer");
		g_pRenderer = CreateRendererD3D();
		_DEBUG_ASSERT(g_pRenderer);

		HWND hDesktop = GetDesktopWindow();

		RInitInfo info;
		info.presentInfo.backBufferHeight = 768;
		info.presentInfo.backBufferWidth = 1024;
		info.presentInfo.fullScreen = FALSE;
		info.presentInfo.hWnd = hDesktop;
		info.pApplication = &g_Callback;
		g_pRenderer->Initialize(info);

		g_pDataStore->AddRefCountData(RENDERER_XSI, g_pRenderer);
	}
}

void
GetAllObjectsWithPolygonMeshes(XSI::CRefArray& toStore, XSI::X3DObject& curObj, boolean doGetHiddenObjects)
{
	XSI::CString name = curObj.GetName() ;

	if (!curObj.IsA(XSI::siLightID) && 
		!curObj.IsA(XSI::siNullID) && 
		!curObj.IsA(XSI::siModelID))
	{

		XSI::Primitive prim(curObj.GetActivePrimitive());
		if (prim.IsValid())
		{
			XSI::Geometry geom(prim.GetGeometry());
			if(geom.IsValid())
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

XSI::CString
GetRelativePath(const XSI::CString absolutePath, const XSI::CString pathToRemove)
{
	std::wstring absPath = absolutePath.GetWideString();
	std::wstring toFind = pathToRemove.GetWideString();

	toFind += _W("\\");
	
	int size = (int) toFind.length();
	int index = (int) absPath.find(toFind);
	if(index != (int)(std::wstring::npos))
	{
		 _DEBUG_ASSERT(index == 0);
		 
		 std::wstring relPath = absPath.substr(index + size);
		 
		 return relPath.c_str();
	}
	
	return absPath.c_str();
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

void Initialize()
{
}

void Shutdown()
{
	// Clear the persistent data store if the plugin is unloaded
	g_pDataStore->ClearRefCountData(RENDERER_XSI);
}