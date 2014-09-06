// TestVertCacheOpt.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

IFFileSystemPtr g_pFileSystem;

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
		// Assume it's a path if it has a mnemonic at the start
		// This is a bad assumption in general, but works only for
		// the script output parser
		if(pMsg[0] == (wchar) '[')
		{
			const static uint MAX_STR_SIZE = 512;
			wchar fullPath[MAX_STR_SIZE];
			uint pathLen = g_pFileSystem->ResolvePath(pMsg, fullPath, MAX_STR_SIZE);
			_DEBUG_ASSERT(pathLen < MAX_STR_SIZE);

			_TRACE(fullPath);
		}
		else
		{
			_TRACE(pMsg);
		}
	}
};

AppCallback s_AppCallback;

ICMMemAllocatorPtr g_pAllocator;
_DECL_CORE_MEM_ALLOC_FNS(g_pAllocator)

void PrintProperty(SGSProperty& prop)
{
	SGSValue resolveVal = prop.expr.Resolve();
	switch(resolveVal.type)
	{
	case STRING_VAL:	s_AppCallback.Log(_W("string ")); break;
	case BOOL_VAL:		s_AppCallback.Log(_W("bool ")); break;
	case DATA_VAL:		s_AppCallback.Log(_W("data ")); break;
	case FLOAT_VAL:		s_AppCallback.Log(_W("float ")); break;
	case INT_VAL:		s_AppCallback.Log(_W("int ")); break;
	default:	_DEBUG_ASSERT(FALSE); break;
	}

	wchar wStr[1024];
	ASCIIToUnicode((const char*) prop.pName->GetData(), wStr, 1024);
	s_AppCallback.Log(wStr);
	s_AppCallback.Log(_W(" = "));

	switch(resolveVal.type)
	{
	case STRING_VAL:
		{
			ASCIIToUnicode((const char*) resolveVal.pData->GetData(), wStr, 1024);
			s_AppCallback.Log(_W("\""));
			s_AppCallback.Log(wStr);
			s_AppCallback.Log(_W("\""));
			break;
		}
	case BOOL_VAL:
		{
			s_AppCallback.Log( resolveVal.bVal ? _W("true") : _W("false") );
			break;
		}

	case DATA_VAL:
		{
			s_AppCallback.Log(_W("..."));
			break;
		}
	case FLOAT_VAL:
		{
			swprintf(wStr, _W("%f"), resolveVal.fVal);
			s_AppCallback.Log(wStr);
			break;
		}
	case INT_VAL:
		{
			swprintf(wStr, _W("%d"), resolveVal.iVal);
			s_AppCallback.Log(wStr);
			break;
		}

	default:	_DEBUG_ASSERT(FALSE); break;

	}
}

void PrintParameter(SGSParameter& param, const wchar* pNewLinePreamble)
{
	s_AppCallback.Log(_W("parameter "));

	wchar wStr[1024];
	ASCIIToUnicode(param.GetName(), wStr, 1024);
	s_AppCallback.Log(wStr);
	s_AppCallback.Log(pNewLinePreamble);
	s_AppCallback.Log(_W("{"));

	wstring preamble = pNewLinePreamble;
	preamble += _W("\t");

	_LOOPi(param.GetNumProperties())
	{
		s_AppCallback.Log(preamble.c_str());
		PrintProperty(param.GetProperty(i));
		s_AppCallback.Log(_W(";"));
	}

	s_AppCallback.Log(pNewLinePreamble);
	s_AppCallback.Log(_W("}"));
}

void RunTest(const wchar* pFilePath)
{
	SGSScriptParser scriptParser;
	scriptParser.Initialize();

	IFFilePtr pTestScriptFile = g_pFileSystem->GetFile(pFilePath);
	IByteBufferPtr pTestScriptData = _NEW CByteBuffer(256);
	AppendData(pTestScriptData, pTestScriptFile);

	SGSScript script;
	boolean isSuccess = scriptParser.Compile(script, pTestScriptData, &s_AppCallback, pFilePath);

	if(!isSuccess)
	{
		s_AppCallback.Log(_W(">>>>>>>>>>>>>>>>>>> Failed to compile script: "));

		const static uint MAX_STR_SIZE = 512;
		wchar fullPath[MAX_STR_SIZE];
		uint pathLen = g_pFileSystem->ResolvePath(pFilePath, fullPath, MAX_STR_SIZE);
		_DEBUG_ASSERT(pathLen < MAX_STR_SIZE);

		s_AppCallback.Log(fullPath);
		s_AppCallback.Log(_W("\n"));
	}
	else
	{
		// Print shader
		s_AppCallback.Log(_W("\n\n ================= Script Dump Begin ================================\n\n"));

		// Print shaders
		_LOOPi(script.GetNumOfShaders())
		{
			s_AppCallback.Log(_W("shader "));
			SGSShader* pShader = script.GetShader(i);

			wchar wStr[1024];
			ASCIIToUnicode(pShader->GetName(), wStr, 1024);
			s_AppCallback.Log(wStr);
			s_AppCallback.Log(_W("\n{\n"));

			s_AppCallback.Log(_W("\tcode\n\t{\n"));

			const char* pCodeStr = pShader->GetShaderCode();
			if(pCodeStr)
			{
				wstring toPrint = _W("");

				uint i = 0;
				while(pCodeStr[i])
				{
					if(pCodeStr[i] != '\n')
						toPrint	+= pCodeStr[i];
					else
					{
						s_AppCallback.Log(toPrint.c_str());
						toPrint = _W("\n");
					}

					i++;
				}

				s_AppCallback.Log(toPrint.c_str());
			}

			s_AppCallback.Log(_W("\n\t}\n"));
			s_AppCallback.Log(_W("}\n\n"));
		}

		// Print annotations
		_LOOPi(script.GetNumOfAnnotations())
		{
			s_AppCallback.Log(_W("annotation "));
			SGSAnnotation* pAnnot = script.GetAnnotation(i);

			wchar wStr[1024];
			ASCIIToUnicode(pAnnot->GetName(), wStr, 1024);
			s_AppCallback.Log(wStr);
			s_AppCallback.Log(_W("\n{"));

			_LOOPj(pAnnot->GetNumParameters())
			{
				s_AppCallback.Log(_W("\n\t"));
				SGSParameter& param = pAnnot->GetParameter(j);
				PrintParameter(param, _W("\n\t"));	
			}

			s_AppCallback.Log(_W("\n}\n\n"));
		}

		s_AppCallback.Log(_W("\n\n ================= Script Dump End ================================\n\n"));
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	Module hCoreMem		= InitializeModule(_W("CoreMemory.dll"), NULL);

	CreateMemAllocatorFn CallCreateMemAllocator = (CreateMemAllocatorFn) GetFunction(hCoreMem, "CreateMemAllocator");
	g_pAllocator = CallCreateMemAllocator();

	_DEBUG_ASSERT(g_pAllocator);
	MemAllocFns allocFns = GetCoreMemoryBindings();
	_BIND_TO_GLOBAL_ALLOC(allocFns);

	Module hFileSystem	= InitializeModule(_W("FileSystemWin32.dll"), &allocFns);
		
	CreateFileSystemFn CreateFileSystem = (CreateFileSystemFn) GetFunction(hFileSystem, "CreateFileSystem");
	g_pFileSystem = CreateFileSystem();
	
	g_pFileSystem->Initialize();	
	g_pFileSystem->AddMnemonic(_W("shd"), _W("data\\shaders"));
	g_pFileSystem->AddMnemonic(_W("mat"), _W("data\\materials"));
	g_pFileSystem->AddMnemonic(_W("msh"), _W("data\\meshes"));
	g_pFileSystem->AddMnemonic(_W("tex"), _W("data\\textures"));
	g_pFileSystem->AddMnemonic(_W("dat"), _W("data"));
		
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

	RunTest(_W("[dat]\\UnitTestMisc\\TestShaderGroupScript\\TestScript.sgs"));

	g_pFileSystem = NULL;
	
	ShutdownModule(hFileSystem);

	g_pAllocator->DumpLeaks();
	g_pAllocator = NULL;
	
	ShutdownModule(hCoreMem);
	
	return 0;
}

