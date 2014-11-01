// TestVertCacheOpt.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

IFFileSystemPtr g_pFileSystem;
IRRendererPtr g_pRenderer;
IBFXBaseFXPtr g_pBaseFX;
IMGMaterialGenPtr g_pMatGen;

class AppCallback : public IAppCallback
{
private: 

public: 
	AppCallback()
	{
	}

	~AppCallback()
	{
	}

public: 
	IByteBuffer* GetFileData(const wchar* pFileName)
	{
		wstring fileName;// = _W("src\\shaders\\");
		fileName += pFileName;

		if(g_pFileSystem->CheckFileExists(fileName.c_str()))
		{
			IFFilePtr pFile = g_pFileSystem->GetFile(fileName.c_str());

			IByteBuffer* pBuffer = _NEW CByteBuffer();
			uint bytesRead = AppendData(pBuffer, pFile);

			if(bytesRead == pFile->Length())
				return pBuffer;

			_DEBUG_ASSERT(FALSE);
			_DELETE( pBuffer );
		}

		return NULL;
	}
	
	void Log(const wchar* pMsg)
	{
		_TRACE(pMsg);
	}
};

AppCallback s_AppCallback;

void RunSim(IRIndexBuffer* pIB, uint cacheSize)
{	
	VertexCache oldCache(cacheSize);
	uint numTris = pIB->GetNumIndices() / 3;
	ushort* pIndices = pIB->Lock(0, 0);
	
	uint numMisses = 0;
	_LOOPi(numTris)
	{
		boolean isHit = oldCache.RegisterVertex(pIndices[i*3]);
		if(!isHit)
			numMisses++;
		
		isHit = oldCache.RegisterVertex(pIndices[i*3+1]);
		if(!isHit)
			numMisses++;

		isHit = oldCache.RegisterVertex(pIndices[i*3+2]);
		if(!isHit)
			numMisses++;		
	}
	
	_TRACE(_W("Cache Miss Per Triangle: %f\n"), ((float)numMisses) / numTris);
}

ICMMemAllocatorPtr g_pAllocator;
_DECL_CORE_MEM_ALLOC_FNS(g_pAllocator)

int _tmain(int argc, _TCHAR* argv[])
{
	Module hCoreMem		= InitializeModule(_W("CoreMemory.dll"), NULL);

	CreateMemAllocatorFn CallCreateMemAllocator = (CreateMemAllocatorFn) GetFunction(hCoreMem, "CreateMemAllocator");
	g_pAllocator = CallCreateMemAllocator();

	_DEBUG_ASSERT(g_pAllocator);
	MemAllocFns allocFns = GetCoreMemoryBindings();
	_BIND_TO_GLOBAL_ALLOC(allocFns);

	Module hRenderer	= InitializeModule(_W("Renderer.dll"), &allocFns);
	Module hFileSystem	= InitializeModule(_W("FileSystem.dll"), &allocFns);
	Module hBaseFX		= InitializeModule(_W("BaseFX.dll"), &allocFns);
	Module hMatGen		= InitializeModule(_W("MaterialGen.dll"), &allocFns);
		
	CreateFileSystemFn CreateFileSystem = (CreateFileSystemFn) GetFunction(hFileSystem, "CreateFileSystem");
	g_pFileSystem = CreateFileSystem();

	CreateRendererFn CreateRenderer = (CreateRendererFn) GetFunction(hRenderer, "CreateRenderer");
	g_pRenderer = CreateRenderer();

	CreateMaterialGenFn CreateMaterialGen = (CreateMaterialGenFn) GetFunction(hMatGen, "CreateMaterialGen");
	g_pMatGen = CreateMaterialGen();

	CreateBaseFXFn CreateBaseFX = (CreateBaseFXFn) GetFunction(hBaseFX, "CreateBaseFX");
	g_pBaseFX = CreateBaseFX();
	
	g_pFileSystem->Initialize();	
	g_pFileSystem->AddMnemonic(_W("shd"), _W("data\\shaders"));
	g_pFileSystem->AddMnemonic(_W("mat"), _W("data\\materials"));
	g_pFileSystem->AddMnemonic(_W("msh"), _W("data\\meshes"));
	g_pFileSystem->AddMnemonic(_W("tex"), _W("data\\textures"));
	g_pFileSystem->AddMnemonic(_W("dat"), _W("data"));
	
	// Initialize renderer
	HWND hDesktop = GetDesktopWindow();

	RInitInfo info;
	info.presentInfo.backBufferHeight = 768;
	info.presentInfo.backBufferWidth = 1024;
	info.presentInfo.fullScreen = FALSE;
	info.presentInfo.hWnd = hDesktop;
	info.pApplication = &s_AppCallback;
	g_pRenderer->Initialize(info);
	
	g_pMatGen->Initialize(s_AppCallback);	
	g_pBaseFX->Initialize(*g_pRenderer, *g_pMatGen, s_AppCallback);
	
	// Get the application path	
	{
		wchar filePath[256];
		GetModuleFileName(NULL, filePath, 256);
		wstring path = filePath;
		std::transform(path.begin(), path.end(), path.begin(), towlower);

		int index = (int) path.find(_W("\\bin\\"));
		if(index != wstring::npos)
		{
			path = path.substr(0, index);
		}
	
		g_pFileSystem->Open(path.c_str());
	}

	//IFFilePtr pObjFile = g_pFileSystem->GetFile(_W("[msh]\\TestVertCacheOpt\\armor-merchant-parallax.mls"));
	//pObjFile->Open(FO_READ_ONLY);
	//IByteBufferPtr pObjBuffer = _NEW CByteBuffer(256);
	//AppendData(pObjBuffer, pObjFile);
	//pObjFile->Close();
	//pObjFile = NULL;
	
	{
		BFXUMeshList meshList;
		//LoadBFXUMeshList(g_pRenderer, g_pBaseFX, pObjBuffer, 0, meshList);
		//pObjBuffer = NULL;

		IByteBufferPtr pVBGroupBuffer = _NEW CByteBuffer();
		IFFilePtr pVBGroupFile = g_pFileSystem->GetFile(_W("[msh]\\TestVertCacheOpt\\armor-merchant.vbgroup"));
		pVBGroupFile->Open(FO_READ_ONLY);
		AppendData(pVBGroupBuffer, pVBGroupFile);
		pVBGroupFile->Close();
		pVBGroupFile = NULL;

		IRVertexBufferGroup* pVBGroup;
		LoadVBGroup(g_pRenderer, pVBGroupBuffer, 0, &pVBGroup);
		pVBGroupBuffer = NULL;

		IByteBufferPtr pIBBuffer = _NEW CByteBuffer();
		IFFilePtr pIBFile = g_pFileSystem->GetFile(_W("[msh]\\TestVertCacheOpt\\armor-merchant.ib"));
		pIBFile->Open(FO_READ_ONLY);
		AppendData(pIBBuffer, pIBFile);
		pIBFile->Close();
		pIBFile = NULL;

		IRIndexBuffer* pIB;
		LoadIB(g_pRenderer, pIBBuffer, 0, &pIB);
		pIBBuffer = NULL;

		BFXMesh mesh;
		mesh.pVBGroup = pVBGroup;
		mesh.pIB = pIB;
		meshList.meshes.push_back(mesh);

		uint numMeshes = (uint) meshList.meshes.size();
		_LOOPi(numMeshes)
		{
			_TRACE(_W(">>>>>>>>>>>>>>>>>>>>>>> Mesh: %d <<<<<<<<<<<<<<<<<<<<<<<\n"), i);

			_TRACE(_W("------- Before: cachesize %d ---------\n"), 4);
			RunSim(meshList.meshes[i].pIB, 4);

			_TRACE(_W("\n------- Before: cachesize %d ---------\n"), 8);
			RunSim(meshList.meshes[i].pIB, 8);

			_TRACE(_W("\n------- Before: cachesize %d ---------\n"), 16);
			RunSim(meshList.meshes[i].pIB, 16);

			_TRACE(_W("\n------- Before: cachesize %d ---------\n"), 32);
			RunSim(meshList.meshes[i].pIB, 32);

			_TRACE(_W("\n------- Before: cachesize %d ---------\n"), 1024);
			RunSim(meshList.meshes[i].pIB, 1024);
	
			RUPostTransVertCacheOpt optimizer;
			optimizer.Optimize(meshList.meshes[i].pIB, meshList.meshes[i].pVBGroup->GetVertexBuffer(0)->GetNumVertices());
			RUPreTransVertCacheOpt preTrans;
			preTrans.Optimize(meshList.meshes[i].pVBGroup, meshList.meshes[i].pIB);
			
			_TRACE(_W("\n\n======== After: cacheSize %d =========\n"), 4);
			RunSim(meshList.meshes[i].pIB, 4);

			_TRACE(_W("\n======== After: cacheSize %d =========\n"), 8);
			RunSim(meshList.meshes[i].pIB, 8);

			_TRACE(_W("\n======== After: cacheSize %d =========\n"), 16);
			RunSim(meshList.meshes[i].pIB, 16);

			_TRACE(_W("\n======== After: cacheSize %d =========\n"), 32);
			RunSim(meshList.meshes[i].pIB, 32);

			_TRACE(_W("\n======== After: cacheSize %d =========\n"), 1024);
			RunSim(meshList.meshes[i].pIB, 1024);
			
			_TRACE(_W("\n.............. Index order ..............\n"), 1024);
			ushort* pIndices = meshList.meshes[i].pIB->Lock(meshList.meshes[i].pIB->GetStartIndex(), 0);
			_LOOPj(meshList.meshes[i].pIB->GetNumIndices() / 3)
			{
				_TRACE(_W("%d, %d, %d\n"), pIndices[j*3], pIndices[j*3+1], pIndices[j*3+2]);
			}
			meshList.meshes[i].pIB->Unlock(FALSE);			
		}
		
		meshList.meshes.clear();
	}

	g_pBaseFX = NULL;
	g_pMatGen = NULL;
	g_pRenderer	= NULL;
	g_pFileSystem = NULL;
	
	ShutdownModule(hBaseFX);
	ShutdownModule(hMatGen);
	ShutdownModule(hRenderer);
	ShutdownModule(hFileSystem);

	g_pAllocator->DumpLeaks();
	g_pAllocator = NULL;
	
	ShutdownModule(hCoreMem);
	
	return 0;
}

