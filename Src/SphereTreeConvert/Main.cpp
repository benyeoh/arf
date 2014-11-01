//==============================================================================
//
//		Main.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Monday, June 05, 2006
//
//==============================================================================

#include "stdafx.h"

_USE_NAMESPACE

HMODULE g_hFileSystem;

void MyRegisterClass(HINSTANCE hInstance);
void Uninitialize();
boolean Initialize();

_NAMESPACE_BEGIN

IFFileSystemPtr g_pFileSystem = NULL;

_NAMESPACE_END

void PrintSphereTree(SEFXUSphereTree* pSphereTree)
{
	wprintf(_W("%d %d\n"), pSphereTree->GetNumLevels() + 1, pSphereTree->GetNumBranches());
	wprintf(_W("%f %f %f %f\n"), pSphereTree->GetRoot().center[0], pSphereTree->GetRoot().center[1], pSphereTree->GetRoot().center[2], pSphereTree->GetRoot().center[3]);

	_LOOPi(pSphereTree->GetNumLevels())
	{
		SEFXUSphereTreeLevel& level = pSphereTree->GetLevel(i);
		wprintf(_W("%d\n"), level.numValidSpheres);

		_LOOPj(level.numValidSpheres)
		{
			wprintf(_W("%f %f %f %f\n"), level.pSpheres[j].center[0], 
				level.pSpheres[j].center[1], 
				level.pSpheres[j].center[2], 
				level.pSpheres[j].center[3]);
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	Initialize();

	_DEBUG_ASSERT(argc >= 2);
	SEFXUSphereTreePtr pSphereTree = ConvertFromSPH(argv[1]);
	
	//wprintf(_W("-------------- 1 ----------------\n"));
	//PrintSphereTree(pSphereTree);

	IByteBufferPtr pBuffer = _NEW CByteBuffer(2048);

	SaveSphereTree(pSphereTree, pBuffer);

	FILE* pOut = _wfopen(argv[2], _W("w+b"));
	_DEBUG_ASSERT(pOut);
	fwrite(pBuffer->GetData(), 1, pBuffer->GetDataLength(), pOut);
	fclose(pOut);

	//SEFXUSphereTree* pRaw = NULL;
	//LoadSphereTree(pBuffer, 0, &pRaw);
	
	//wprintf(_W("-------------- 2 ----------------\n"));
	//SEFXUSphereTreePtr pSphereTree2 = pRaw;	
	//PrintSphereTree(pSphereTree2);

	pBuffer = NULL;
	pSphereTree = NULL;
	
	Uninitialize();

	return 0;
}

void Uninitialize()
{
	g_pFileSystem = NULL;

	::FreeLibrary(g_hFileSystem);

	_DUMP_LEAKS;
}

boolean Initialize()
{
	// Load Win32 File system
	g_hFileSystem = ::LoadLibrary(_W("FileSystem.dll"));
	if(!g_hFileSystem) return FALSE;

	CreateFileSystemFn CreateFileSystem = (CreateFileSystemFn) ::GetProcAddress(g_hFileSystem, "CreateFileSystem");

	if(!CreateFileSystem)
	{
		wprintf(_W("Failed to create FileSystem!\n"));
		_DEBUG_ASSERT(FALSE);
		return FALSE;
	}

	g_pFileSystem = CreateFileSystem();

	wchar filePath[256];
	GetModuleFileName(NULL, filePath, 256);
	wstring path = filePath;
	std::transform(path.begin(), path.end(), path.begin(), towlower);

	int index = (int) path.find(_W("\\bin\\"));
	if(index != wstring::npos)
	{
		path = path.substr(0, index);
	}

	if(!g_pFileSystem->Open(path.c_str()))
	{
		wprintf(_W("Failed to init FileSystem!\n"));
		return FALSE;
	}

	g_pFileSystem->AddMnemonic(_W("shd"), _W("data\\shaders"));
	g_pFileSystem->AddMnemonic(_W("mat"), _W("data\\materials"));

	return TRUE;
}
