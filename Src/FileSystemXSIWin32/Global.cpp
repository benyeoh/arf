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

IFFileSystemPtr g_pFileSystem;
IDataStoreForXSIPlugin* g_pDataStore;

Module g_hFileSystem = NULL;
Module g_hDataStore = NULL;

wstring* s_pApplicationRoot;

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
	
	//ShutdownModule(g_hFileSystem);
}

void
InitializeFileSystem()
{
	g_pFileSystem = (IFFileSystem*) g_pDataStore->GetRefCountData(FILE_SYSTEM_XSI);

	if(!g_pFileSystem)
	{
		g_hFileSystem = InitializeModule(_W("FileSystem.dll"), NULL);
		_DEBUG_ASSERT(g_hFileSystem);

		CreateFileSystemFn CreateFileSystem = (CreateFileSystemFn) GetFunction(g_hFileSystem, "CreateFileSystem");
		_DEBUG_ASSERT(CreateFileSystem);

		g_pFileSystem = CreateFileSystem();	
		_DEBUG_ASSERT(g_pFileSystem);

		g_pFileSystem->Initialize();

		wchar filePath[256];
		GetModuleFileName((HMODULE)g_hFileSystem, filePath, 256);
		s_pApplicationRoot = _NEW wstring;
		*s_pApplicationRoot = filePath;
		std::transform(s_pApplicationRoot->begin(), s_pApplicationRoot->end(), s_pApplicationRoot->begin(), towlower);

		int index = (int) s_pApplicationRoot->find(_W("\\bin\\"));
		if(index != wstring::npos)
		{
			*s_pApplicationRoot = s_pApplicationRoot->substr(0, index);
		}

		g_pFileSystem->Open(s_pApplicationRoot->c_str());
		g_pFileSystem->AddMnemonic(_W("shd"), DEF_SHADERS_PATH);
		g_pFileSystem->AddMnemonic(_W("msh"), DEF_MESHES_PATH);
		g_pFileSystem->AddMnemonic(_W("tex"), DEF_TEXTURES_PATH);
		g_pFileSystem->AddMnemonic(_W("mat"), DEF_MATERIALS_PATH);
		g_pFileSystem->AddMnemonic(_W("dat"), DEF_DATA_PATH);

		g_pDataStore->AddRefCountData(FILE_SYSTEM_XSI, g_pFileSystem);
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
	g_pDataStore->ClearRefCountData(FILE_SYSTEM_XSI);
}
