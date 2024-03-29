// TestVertCacheOpt.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

IFFileSystemPtr g_pFileSystem;
IPPlatformPtr g_pPlatform;

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

			_TRACE_NOARG(fullPath);
		}
		else
		{
			_TRACE_NOARG(pMsg);
		}
	}
};

AppCallback s_AppCallback;

ICMMemAllocatorPtr g_pAllocator;
_DECL_CORE_MEM_ALLOC_FNS(g_pAllocator)

struct TestCompileFn : public ISGSUserFunction
{
	IAppCallback* pCallback;
	eSGSValueType m_ParamSigTypes[2];		

	TestCompileFn()
	{
		pCallback = NULL;
		m_ParamSigTypes[0] = STRING_VAL;
		m_ParamSigTypes[1] = STRING_VAL;
	}

	const char* GetName() { return "Compile"; }

	void GetParameterSig(eSGSValueType** ppParamTypes, uint& numParams)
	{
		*ppParamTypes = m_ParamSigTypes;
		numParams = 2;
	}

	boolean Execute(const SGSScript* pSrc, SGSValue* parameters, int numParameters, SGSValue* pResults, int& numResults)
	{
		_DEBUG_ASSERT(numParameters == 2);

		_LOOPi(pSrc->GetNumOfShaders())
		{
			const SGSShader* pShader = pSrc->GetShader(i);
			if( strcmp(pShader->GetName(), (const char*) parameters[0].pData->GetData()) == 0 )
			{
				CByteBuffer* pBuf = _NEW CByteBuffer();
				AppendData(pBuf, pShader->GetShaderCode(), 32);
				AppendData(pBuf, (char)0);
				pResults[0].SetString(pBuf);
				numResults = 1;
				return TRUE;
			}
		}

		return FALSE;
	}
};

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

void PrintPass(SGSPass& pass, const wchar* pNewLinePreamble)
{
	s_AppCallback.Log(_W("pass "));

	wchar wStr[1024];
	ASCIIToUnicode(pass.GetName(), wStr, 1024);
	s_AppCallback.Log(wStr);
	s_AppCallback.Log(pNewLinePreamble);
	s_AppCallback.Log(_W("{"));

	wstring preamble = pNewLinePreamble;
	preamble += _W("\t");

	_LOOPi(pass.GetNumProperties())
	{
		s_AppCallback.Log(preamble.c_str());
		PrintProperty(pass.GetProperty(i));
		s_AppCallback.Log(_W(";"));
	}

	s_AppCallback.Log(pNewLinePreamble);
	s_AppCallback.Log(_W("}"));
}

void PrintTechnique(SGSTechnique& tech, const wchar* pNewLinePreamble)
{
	s_AppCallback.Log(_W("technique "));

	wchar wStr[1024];
	ASCIIToUnicode(tech.GetName(), wStr, 1024);
	s_AppCallback.Log(wStr);
	s_AppCallback.Log(pNewLinePreamble);
	s_AppCallback.Log(_W("{"));

	wstring preamble = pNewLinePreamble;
	preamble += _W("\t");

	_LOOPi(tech.GetNumPasses())
	{
		s_AppCallback.Log(preamble.c_str());
		PrintPass(tech.GetPass(i), preamble.c_str());
	}

	s_AppCallback.Log(pNewLinePreamble);
	s_AppCallback.Log(_W("}"));
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
	TestCompileFn testCompileFn;
	testCompileFn.pCallback = &s_AppCallback;

	SGSScriptParser scriptParser;
	scriptParser.Initialize();
	scriptParser.AddUserFunction(&testCompileFn);

	IFFilePtr pTestScriptFile = g_pFileSystem->GetFile(pFilePath);
	IByteBufferPtr pTestScriptData = _NEW CByteBuffer(256);
	AppendData(pTestScriptData, pTestScriptFile);

	SGSScript script;
	g_pPlatform->GetTimer().BeginSample();
	boolean isSuccess = scriptParser.Compile(script, pTestScriptData, &s_AppCallback, pFilePath);
	g_pPlatform->GetTimer().EndSample();
	
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

				_LOOPi(pShader->GetShaderCodeLength())
				{
					toPrint	+= pCodeStr[i];
				}

				s_AppCallback.Log(toPrint.c_str());
			}

			s_AppCallback.Log(_W("\n\t}\n"));
			s_AppCallback.Log(_W("}\n\n"));
		}

		// Print parameters
		_LOOPi(script.GetNumOfParameters())
		{
			s_AppCallback.Log(_W("\n"));
			SGSParameter* pParam = script.GetParameter(i);
			PrintParameter(*pParam, _W("\n"));	
		}

		// Print technique
		_LOOPi(script.GetNumOfTechniques())
		{
			s_AppCallback.Log(_W("\n"));
			SGSTechnique* pTech = script.GetTechnique(i);
			PrintTechnique(*pTech, _W("\n"));	
		}

		s_AppCallback.Log(_W("\n\n ================= Script Dump End ================================\n\n"));
		wchar buf[256];
		swprintf(buf, _W("Compile Time: %f ms\n\n"), (float) g_pPlatform->GetTimer().GetTimeElapsed() * 1000.0f);
		s_AppCallback.Log(buf);
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

	Module hFileSystem	= InitializeModule(_W("FileSystem.dll"), &allocFns);
		
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

	Module hPlatform = InitializeModule(_W("Platform.dll"), &allocFns);

	CreatePlatformFn CreatePlatform = (CreatePlatformFn) GetFunction(hPlatform, "CreatePlatform");
	g_pPlatform = CreatePlatform();

	RunTest(_W("[dat]\\UnitTestMisc\\TestShaderGroupScript\\TestScript.sgs"));

	g_pPlatform = NULL;
	g_pFileSystem = NULL;

	ShutdownModule(hPlatform);
	ShutdownModule(hFileSystem);

	g_pAllocator->DumpLeaks();
	g_pAllocator = NULL;
	
	ShutdownModule(hCoreMem);
	
	return 0;
}

