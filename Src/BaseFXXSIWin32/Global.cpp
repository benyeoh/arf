//==============================================================================
//
//		Global.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/9/2008
//
//==============================================================================

#include "stdafx.h"

//HMODULE g_hFileSystem = NULL;
//HMODULE g_hRenderer = NULL;
Module g_hBaseFX = NULL;
Module g_hMatGen = NULL;
Module g_hDataStore = NULL;

IRRendererPtr g_pRenderer;
IFFileSystemPtr g_pFileSystem;
IBFXBaseFXPtr g_pBaseFX;
IMGMaterialGenPtr g_pMatGen;
IDataStoreForXSIPlugin* g_pDataStore = NULL;

std::vector<IBFXMaterialGroupTemplatePtr>* g_pMatGroups;
std::vector<wstring>* g_pMatGroupFileNames;

XSI::CValueArray* s_pParameterValArray; 
XSI::CStringArray* s_pParameterNameArray; 

AppCallback g_Callback;

const wchar*
GetApplicationRoot()
{
	InitializeFileSystem();
	
	return g_pFileSystem->GetResourcePath();
}

void 
InitializeMaterialGroups()
{
	(*g_pMatGroups).clear();
	(*g_pMatGroupFileNames).clear();
	
	InitializeFileSystem();
	InitializeMaterialGen();
	InitializeBaseFX();	
	
	IFFileEnumeratorPtr pFileEnum = g_pFileSystem->GetFiles(_W("[shd]"), _W("*.mgp"), TRUE);
	IFFilePtr pFile = pFileEnum->GetNextFile();
	while(pFile)
	{
		IByteBufferPtr pBuffer = _NEW CByteBuffer(pFile->Length());
		AppendData(pBuffer, pFile);
		
		CRCDataKey id;
		wstring fileName = pFile->GetFileName();
		wchar normalizedPath[256];
		g_pFileSystem->NormalizePath(fileName.c_str(), normalizedPath, 256);
		wstring normalizedName = normalizedPath;
			
		id.Set(normalizedName.c_str(), (uint)normalizedName.size() * sizeof(wchar));
		IBFXMaterialGroupTemplatePtr pMatGroup = g_pBaseFX->GetResourceMgr().
			CreateMaterialGroupTemplate(&id, pBuffer, normalizedName.c_str());

		_DEBUG_ASSERT(pMatGroup);

		if(pMatGroup)
		{			
			(*g_pMatGroups).push_back(pMatGroup);		
		
			//wchar normalizedPath[256];
			//g_pFileSystem->NormalizePath(pFile->GetFileName(), normalizedPath, 256);
			(*g_pMatGroupFileNames).push_back(normalizedName);
		}
			
		pFile = pFileEnum->GetNextFile();		
	}
}

void
AllocateGlobalData()
{
	g_hDataStore = InitializeModule(_W("DataStoreForXSIPlugin.dll"), NULL);
	GetDataStoreFn CallGetDataStore = (GetDataStoreFn) GetFunction(g_hDataStore, "GetDataStore");
	_DEBUG_ASSERT(CallGetDataStore);

	g_pDataStore = CallGetDataStore();
	_DEBUG_ASSERT(g_pDataStore);
	
	g_pMatGroups = _NEW std::vector<IBFXMaterialGroupTemplatePtr>;
	g_pMatGroupFileNames = _NEW std::vector<wstring>;
	
	s_pParameterValArray = _NEW XSI::CValueArray;
	s_pParameterNameArray = _NEW XSI::CStringArray;
	
	//CreateFileSystemFn CreateFileSystem = (CreateFileSystemFn) ::GetProcAddress(g_hFileSystem, "CreateFileSystem");
	//_DEBUG_ASSERT(CreateFileSystem);

	//g_pFileSystem = CreateFileSystem();	
	//_DEBUG_ASSERT(g_pFileSystem);

	//g_pFileSystem->Initialize();
	//
	//wchar filePath[256];
	//GetModuleFileName(g_hFileSystem, filePath, 256);
	//s_ApplicationRoot = filePath;
	//std::transform(s_ApplicationRoot.begin(), s_ApplicationRoot.end(), s_ApplicationRoot.begin(), towlower);

	//int index = (int) s_ApplicationRoot.find(_W("\\bin\\"));
	//if(index != wstring::npos)
	//{
	//	s_ApplicationRoot = s_ApplicationRoot.substr(0, index);
	//}
	//
	//g_pFileSystem->Open(s_ApplicationRoot.c_str());
	//g_pFileSystem->AddMnemonic(_W("shd"), _W("src\\shaders"));
	//g_pFileSystem->AddMnemonic(_W("msh"), _W("data\\meshes"));
	//g_pFileSystem->AddMnemonic(_W("tex"), _W("data\\textures"));
	//g_pFileSystem->AddMnemonic(_W("eff"), _W("data\\effects"));
	//g_pFileSystem->AddMnemonic(_W("dat"), _W("data"));

	//g_hRenderer = ::LoadLibrary(_W("RendererD3D.dll"));
	//CreateRendererFn CreateRendererD3D = (CreateRendererFn)::GetProcAddress(g_hRenderer, "CreateRenderer");
	//g_pRenderer = CreateRendererD3D();
	//_DEBUG_ASSERT(g_pRenderer);	
}

void
FreeGlobalData()
{
	_DELETE(s_pParameterValArray);
	_DELETE(s_pParameterNameArray);
	
	_DELETE(g_pMatGroups);
	_DELETE(g_pMatGroupFileNames)
	
	g_pBaseFX = NULL;
	g_pMatGen = NULL;
	g_pFileSystem = NULL;
	g_pRenderer = NULL;

	//ShutdownModule(g_hMatGen);
	//ShutdownModule(g_hBaseFX);
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
			g_pFileSystem = (IFFileSystem*) ((XSI::CValue::siPtrType)returnVal);
			_DEBUG_ASSERT(g_pFileSystem);
		}
	}
}

void
InitializeBaseFX()
{
	g_pBaseFX = (IBFXBaseFX*) g_pDataStore->GetRefCountData(BASE_FX_XSI);

	if(!g_pBaseFX)
	{
		g_hBaseFX = InitializeModule(_W("BaseFX.dll"), NULL);
		_DEBUG_ASSERT(g_hBaseFX);

		CreateBaseFXFn CreateBaseFX = (CreateBaseFXFn) GetFunction(g_hBaseFX, "CreateBaseFX");
		_DEBUG_ASSERT(CreateBaseFX);
		g_pBaseFX = CreateBaseFX();

		XSI::Application app;
		XSI::CValue returnVal;
		XSI::CValueArray args;
		XSI::CStatus status = app.ExecuteCommand(_W("AppComGetRenderer"), args, returnVal);
		if(status == XSI::CStatus::OK)
		{
			// TODO: Renderer will not be valid if the plugins are not cached
			g_pRenderer = (IRRenderer*) ((XSI::CValue::siPtrType)returnVal);
			_DEBUG_ASSERT(g_pRenderer);
		}

		InitializeMaterialGen();

		g_pBaseFX->Initialize(*g_pRenderer, *g_pMatGen, g_Callback);
		g_pDataStore->AddRefCountData(BASE_FX_XSI, g_pBaseFX);
	}
}

void
InitializeMaterialGen()
{
	g_pMatGen = (IMGMaterialGen*) g_pDataStore->GetRefCountData(MAT_GEN_XSI);

	if(!g_pMatGen)
	{
		g_hMatGen = InitializeModule(_W("MaterialGenD3D.dll"), NULL);
		_DEBUG_ASSERT(g_hMatGen);

		CreateMaterialGenFn CreateMaterialGen = (CreateMaterialGenFn) GetFunction(g_hMatGen, "CreateMaterialGen");
		_DEBUG_ASSERT(CreateMaterialGen);
		g_pMatGen = CreateMaterialGen();

		g_pMatGen->Initialize(g_Callback);

		g_pDataStore->AddRefCountData(MAT_GEN_XSI, g_pMatGen);
	}
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
	g_pDataStore->ClearRefCountData(BASE_FX_XSI);
	g_pDataStore->ClearRefCountData(MAT_GEN_XSI);
}