// TestMaterialGen.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

_USE_NAMESPACE

IFFileSystemPtr g_pFileSystem;
IMGMaterialGenPtr g_pMaterialGen;

void TestMaterial();

ICMMemAllocatorPtr g_pAllocator;

_DECL_CORE_MEM_ALLOC_FNS(g_pAllocator)

int _tmain(int argc, _TCHAR* argv[])
{
	Module hCoreMem = InitializeModule(_W("CoreMemory.dll"), NULL);

	CreateMemAllocatorFn CallCreateMemAllocator = (CreateMemAllocatorFn) GetFunction(hCoreMem, "CreateMemAllocator");
	g_pAllocator = CallCreateMemAllocator();

	_DEBUG_ASSERT(g_pAllocator);
	MemAllocFns allocFns = GetCoreMemoryBindings();
	_BIND_TO_GLOBAL_ALLOC(allocFns);

	AppCallback* pCallback = _NEW AppCallback;
	
	Module hFileSystem = InitializeModule(_W("FileSystemWin32.dll"), &allocFns);

	CreateFileSystemFn CreateFileSystem = (CreateFileSystemFn) GetFunction(hFileSystem, "CreateFileSystem");

	_DEBUG_ASSERT(CreateFileSystem);

	g_pFileSystem = CreateFileSystem();
	g_pFileSystem->Initialize();	
	g_pFileSystem->Open(_W("E:\\Dev\\X"));
	g_pFileSystem->AddMnemonic(_W("shd"), _W("src\\shaders"));
	g_pFileSystem->AddMnemonic(_W("msh"), _W("data\\meshes"));
	g_pFileSystem->AddMnemonic(_W("tex"), _W("data\\textures"));
	g_pFileSystem->AddMnemonic(_W("dat"), _W("data"));

	Module hMaterialGen = InitializeModule(_W("MaterialGenD3D.dll"), &allocFns);
	
	CreateMaterialGenFn CreateMaterialGen = (CreateMaterialGenFn) GetFunction(hMaterialGen, "CreateMaterialGen");
	_DEBUG_ASSERT(CreateMaterialGen);
	
	g_pMaterialGen = CreateMaterialGen();
	g_pMaterialGen->Initialize(*pCallback);	
		
	TestMaterial();

	g_pFileSystem->Close();	
	g_pFileSystem = NULL;
	g_pMaterialGen = NULL;
	
	_DELETE(pCallback);
	
	ShutdownModule(hMaterialGen);
	ShutdownModule(hFileSystem);

	g_pAllocator->DumpLeaks();
	g_pAllocator = NULL;

	ShutdownModule(hCoreMem);

	_DUMP_LEAKS;
	
	return 0;
}

void
TestMaterial()
{
	//IMGMaterialTemplatePtr pTemplate = g_pMaterialGen->GetResourceMgr().CreateMaterialTemplate();

	MGParamSig paramSig;
	MGInputSig inputSigs[16];
	MGLocalSig localSig;
	
	// Locals
	localSig.pType = _W("struct");
	localSig.pName = _W("LightDir");
	localSig.pPostamble = _W("{\n")
		_W("\tfloat3 dir;\n")
		_W("\tfloat4 color;\n")
		_W("\tfloat invSize;\n")
		_W("}");
	IMGLocalPtr pDirLight = g_pMaterialGen->GetResourceMgr().CreateLocal(&localSig);
	localSig.pType = _W("struct");
	localSig.pName = _W("LightHemi");
	localSig.pPostamble = _W("{\n")
		_W("\tfloat4 skyColor;\n")
		_W("\tfloat4 groundColor;\n")
		_W("}");
	IMGLocalPtr pHemiLight = g_pMaterialGen->GetResourceMgr().CreateLocal(&localSig);

	// Globals	
	paramSig.pName = _W("g_matWorld");
	paramSig.pType = _W("float4x4");
	IMGGlobalPtr pWorld = g_pMaterialGen->GetResourceMgr().CreateGlobal(&paramSig, NULL, 0);
	pWorld->SetUsage(MGU_DYNAMIC);
	pWorld->SetSemantic(23);
	
	paramSig.pName = _W("g_matViewProj");
	paramSig.pType = _W("float4x4");
	IMGGlobalPtr pViewProj = g_pMaterialGen->GetResourceMgr().CreateGlobal(&paramSig, NULL, 0);
	pViewProj->SetUsage(MGU_DYNAMIC);
	pViewProj->SetSemantic(24);
	
	paramSig.pName = _W("g_LightDir");
	paramSig.pType = _W("LightDir");
	IMGGlobalPtr pDirLightGlobal = g_pMaterialGen->GetResourceMgr().CreateGlobal(&paramSig, NULL, 0);
	pDirLightGlobal->SetUsage(MGU_DYNAMIC);
	pDirLightGlobal->SetSemantic(25);
	
	paramSig.pName = _W("g_LightHemi");
	paramSig.pType = _W("LightHemi");
	IMGGlobalPtr pHemiLightGlobal = g_pMaterialGen->GetResourceMgr().CreateGlobal(&paramSig, NULL, 0);
	pHemiLightGlobal->SetUsage(MGU_DYNAMIC);
	pHemiLightGlobal->SetSemantic(26);

	paramSig.pName = _W("EffectType");
	paramSig.pType = _W("int");
	IMGGlobalPtr pEffectType = g_pMaterialGen->GetResourceMgr().CreateGlobal(&paramSig, NULL, 0);
	pEffectType->SetUsage(MGU_CONSTANT);
	pEffectType->SetSemantic(3);
	pEffectType->SetIsAlwaysUsed(TRUE);
	pEffectType->SetValue(_W("999"));
	
	paramSig.pName = _W("g_TexDiffuse");
	paramSig.pType = _W("texture");
	IMGGlobalPtr pDiffuseTex = g_pMaterialGen->GetResourceMgr().CreateGlobal(&paramSig, NULL, 0);
	pDiffuseTex->SetUsage(MGU_CONSTANT);
	pDiffuseTex->SetSemantic(32);
	inputSigs[0].pDefaultVal = _W("\"Diffuse Texture\"");
	inputSigs[0].pName = _W("name");
	inputSigs[0].pType = _W("string");
	pDiffuseTex->AddDescription(inputSigs);
	
	paramSig.pName = _W("g_SampDiffuse");
	paramSig.pType = _W("sampler");
	inputSigs[0].pDefaultVal = NULL;
	inputSigs[0].pName = _W("diffuseTexture");
	inputSigs[0].pType = _W("texture");
	IMGGlobalPtr pSampDiffuse = g_pMaterialGen->GetResourceMgr().CreateGlobal(&paramSig, inputSigs, 1);
	pSampDiffuse->SetValue(_W("sampler_state\n")
		_W("{\n")
		_W("\tTexture = diffuseTexture;\n")
		_W("\tMipFilter = LINEAR;\n")
		_W("\tMinFilter = LINEAR;\n")
		_W("\tMagFilter = LINEAR;\n")
		_W("\tADDRESSU = WRAP;\n")
		_W("\tADDRESSV = WRAP;\n")
		_W("}"));
	
	pSampDiffuse->GetInput(0).BindValue(&pDiffuseTex->GetOutput());
	
	// Vertex IO
	paramSig.pName = _W("POSITION");
	paramSig.pType = _W("float4");
	IMGVertexInPtr pVInPos = g_pMaterialGen->GetResourceMgr().CreateVertexIn(&paramSig);
	paramSig.pName = _W("NORMAL");
	paramSig.pType = _W("float3");
	IMGVertexInPtr pVInNormal = g_pMaterialGen->GetResourceMgr().CreateVertexIn(&paramSig);
	paramSig.pName = _W("TEXCOORD0");
	paramSig.pType = _W("float2");
	IMGVertexInPtr pVInTex0 = g_pMaterialGen->GetResourceMgr().CreateVertexIn(&paramSig);
	
	paramSig.pName = _W("POSITION0");
	paramSig.pType = _W("float4");
	IMGVertexOutPtr pVOutPos = g_pMaterialGen->GetResourceMgr().CreateVertexOut(&paramSig);
	paramSig.pName = _W("COLOR0");
	paramSig.pType = _W("float4");
	IMGVertexOutPtr pVOutColor = g_pMaterialGen->GetResourceMgr().CreateVertexOut(&paramSig);
	paramSig.pName = _W("TEXCOORD0");
	paramSig.pType = _W("float2");
	IMGVertexOutPtr pVOutTex0 = g_pMaterialGen->GetResourceMgr().CreateVertexOut(&paramSig);

	// Pixel IO
	paramSig.pName = _W("POSITION0");
	paramSig.pType = _W("float4");
	IMGPixelInPtr pPInPos = g_pMaterialGen->GetResourceMgr().CreatePixelIn(&paramSig);
	paramSig.pName = _W("COLOR0");
	paramSig.pType = _W("float4");
	IMGPixelInPtr pPInColor = g_pMaterialGen->GetResourceMgr().CreatePixelIn(&paramSig);
	paramSig.pName = _W("TEXCOORD0");
	paramSig.pType = _W("float2");
	IMGPixelInPtr pPInTex0 = g_pMaterialGen->GetResourceMgr().CreatePixelIn(&paramSig);

	paramSig.pName = _W("COLOR0");
	paramSig.pType = _W("float4");
	IMGPixelOutPtr pPOutColor = g_pMaterialGen->GetResourceMgr().CreatePixelOut(&paramSig);
			
	// Functions
	paramSig.pName = _W("Vec4Mult");
	paramSig.pType = _W("float4");
	inputSigs[0].pDefaultVal = NULL;
	inputSigs[0].pName = _W("vec");
	inputSigs[0].pType = _W("float4");
	inputSigs[1].pDefaultVal = NULL;
	inputSigs[1].pName = _W("mat");
	inputSigs[1].pType = _W("float4x4");
	IMGFunctionPtr pVec4Mult = g_pMaterialGen->GetResourceMgr().CreateFunction(&paramSig, inputSigs, 2);
	pVec4Mult->SetValue(
		_W("float4 val = mul(vec, mat);\n")
		_W("return val;")
		);
	
	paramSig.pName = _W("Vec4Mod");
	paramSig.pType = _W("float4");
	inputSigs[0].pDefaultVal = NULL;
	inputSigs[0].pName = _W("vec1");
	inputSigs[0].pType = _W("float4");
	inputSigs[1].pDefaultVal = NULL;
	inputSigs[1].pName = _W("vec2");
	inputSigs[1].pType = _W("float4");
	IMGFunctionPtr pVec4Mod = g_pMaterialGen->GetResourceMgr().CreateFunction(&paramSig, inputSigs, 2);
	pVec4Mod->SetValue(
		_W("float4 val = vec1 * vec2;\n")
		_W("return val;")
		);
		
	paramSig.pName = _W("Vec3Mult");
	paramSig.pType = _W("float3");
	inputSigs[0].pDefaultVal = NULL;
	inputSigs[0].pName = _W("vec");
	inputSigs[0].pType = _W("float3");
	inputSigs[1].pDefaultVal = NULL;
	inputSigs[1].pName = _W("mat");
	inputSigs[1].pType = _W("float4x4");
	IMGFunctionPtr pVec3Mult = g_pMaterialGen->GetResourceMgr().CreateFunction(&paramSig, inputSigs, 2);
	pVec3Mult->SetValue(
		_W("float3 val = mul(vec, mat);\n")
		_W("return val;")
		);
	
	paramSig.pName = _W("Vec4Add");
	paramSig.pType = _W("float4");
	inputSigs[0].pDefaultVal = NULL;
	inputSigs[0].pName = _W("vec1");
	inputSigs[0].pType = _W("float4");
	inputSigs[1].pDefaultVal = NULL;
	inputSigs[1].pName = _W("vec2");
	inputSigs[1].pType = _W("float4");
	IMGFunctionPtr pVec4Add = g_pMaterialGen->GetResourceMgr().CreateFunction(&paramSig, inputSigs, 2);
	pVec4Add->SetValue(
		_W("float4 val = vec1 + vec2;\n")
		_W("return val;")
		);
		
	paramSig.pName = _W("Normalize3");
	paramSig.pType = _W("float3");
	inputSigs[0].pDefaultVal = NULL;
	inputSigs[0].pName = _W("vec");
	inputSigs[0].pType = _W("float3");
	IMGFunctionPtr pNormalize = g_pMaterialGen->GetResourceMgr().CreateFunction(&paramSig, inputSigs, 1);
	pNormalize->SetValue(
		_W("return normalize(vec);")
		);

	paramSig.pName = _W("ComputeHemiAmbientColor");
	paramSig.pType = _W("float4");
	inputSigs[0].pDefaultVal = NULL;
	inputSigs[0].pName = _W("N");
	inputSigs[0].pType = _W("float3");
	inputSigs[1].pDefaultVal = NULL;
	inputSigs[1].pName = _W("hemi");
	inputSigs[1].pType = _W("LightHemi");
	IMGFunctionPtr pComputeHemiAmbient = g_pMaterialGen->GetResourceMgr().CreateFunction(&paramSig, inputSigs, 2);
	pComputeHemiAmbient->SetValue(
		_W("return lerp(hemi.groundColor, hemi.skyColor, N.y * 0.5f + 0.5f);")		
	);

	paramSig.pName = _W("ComputeDirColor");
	paramSig.pType = _W("float4");
	inputSigs[0].pDefaultVal = NULL;
	inputSigs[0].pName = _W("N");
	inputSigs[0].pType = _W("float3");
	inputSigs[1].pDefaultVal = NULL;
	inputSigs[1].pName = _W("dirLight");
	inputSigs[1].pType = _W("LightDir");
	IMGFunctionPtr pComputeDirColor = g_pMaterialGen->GetResourceMgr().CreateFunction(&paramSig, inputSigs, 2);
	pComputeDirColor->SetValue(
		_W("float NDotL = saturate( dot(N, -dirLight.dir) );\n")
		_W("float4 dirColor = NDotL * dirLight.color;\n")
		_W("return dirColor;")	
		);
		
	paramSig.pName = _W("SampleRGBA2D");
	paramSig.pType = _W("float4");
	inputSigs[0].pDefaultVal = NULL;
	inputSigs[0].pName = _W("uv");
	inputSigs[0].pType = _W("float2");
	inputSigs[1].pDefaultVal = NULL;
	inputSigs[1].pName = _W("samp");
	inputSigs[1].pType = _W("sampler");
	IMGFunctionPtr pSamplerRGBA2D = g_pMaterialGen->GetResourceMgr().CreateFunction(&paramSig, inputSigs, 2);
	pSamplerRGBA2D->SetValue(
		_W("return tex2D(samp, uv);\n")
		);
		
	// Link vertex shader
	IMGFunctionOpPtr pMult1 = pVec4Mult->CreateFunctionOp();
	pMult1->GetInput(0).BindValue(&pVInPos->GetOutput());
	pMult1->GetInput(1).BindValue(&pWorld->GetOutput());
	
	IMGFunctionOpPtr pMult2 = pVec4Mult->CreateFunctionOp();
	pMult2->GetInput(0).BindValue(&pMult1->GetOutput());
	pMult2->GetInput(1).BindValue(&pViewProj->GetOutput());
	
	pVOutPos->GetInput().BindValue(&pMult2->GetOutput());
	
	IMGFunctionOpPtr pWorldNormMult = pVec3Mult->CreateFunctionOp();
	pWorldNormMult->GetInput(0).BindValue(&pVInNormal->GetOutput());
	pWorldNormMult->GetInput(1).BindValue(&pWorld->GetOutput());
	
	IMGFunctionOpPtr pNormNormalize = pNormalize->CreateFunctionOp();
	pNormNormalize->GetInput(0).BindValue(&pWorldNormMult->GetOutput());
	
	IMGFunctionOpPtr pComputeHemiOp = pComputeHemiAmbient->CreateFunctionOp();
	pComputeHemiOp->GetInput(0).BindValue(&pNormNormalize->GetOutput());
	pComputeHemiOp->GetInput(1).BindValue(&pHemiLightGlobal->GetOutput());
	
	IMGFunctionOpPtr pComputeDirLightOp = pComputeDirColor->CreateFunctionOp();
	pComputeDirLightOp->GetInput(0).BindValue(&pNormNormalize->GetOutput());
	pComputeDirLightOp->GetInput(1).BindValue(&pDirLightGlobal->GetOutput());

	IMGFunctionOpPtr pTotalLight = pVec4Add->CreateFunctionOp();
	pTotalLight->GetInput(0).BindValue(&pComputeHemiOp->GetOutput());
	pTotalLight->GetInput(1).BindValue(&pComputeDirLightOp->GetOutput());

	pVOutColor->GetInput().BindValue(&pTotalLight->GetOutput());
	pVOutTex0->GetInput().BindValue(&pVInTex0->GetOutput());
	
	// Link Pixel Shader
	IMGFunctionOpPtr pSampOp = pSamplerRGBA2D->CreateFunctionOp();
	pSampOp->GetInput(1).BindValue(&pSampDiffuse->GetOutput());
	pSampOp->GetInput(0).BindValue(&pPInTex0->GetOutput());

	IMGFunctionOpPtr pColorMod = pVec4Mod->CreateFunctionOp();	
	pColorMod->GetInput(0).BindValue(&pSampOp->GetOutput());
	pColorMod->GetInput(1).BindValue(&pPInColor->GetOutput());

	pPOutColor->GetInput().BindValue(&pColorMod->GetOutput());
		
	// Link Shaders
	IMGVertexShaderPtr pVS = g_pMaterialGen->GetResourceMgr().CreateVertexShader(_W("TestVS"));
	pVS->AddFunctionOp(pMult1);
	pVS->AddFunctionOp(pMult2);
	pVS->AddFunctionOp(pWorldNormMult);
	pVS->AddFunctionOp(pNormNormalize);
	pVS->AddFunctionOp(pComputeHemiOp);
	pVS->AddFunctionOp(pComputeDirLightOp);
	pVS->AddFunctionOp(pTotalLight);
	pVS->AddVertexIn(pVInPos);
	pVS->AddVertexIn(pVInNormal);
	pVS->AddVertexIn(pVInTex0);
	pVS->AddVertexOut(pVOutPos);
	pVS->AddVertexOut(pVOutColor);
	pVS->AddVertexOut(pVOutTex0);
	pVS->SetVersion(_W("2"), _W("0"));

	IMGPixelShaderPtr pPS = g_pMaterialGen->GetResourceMgr().CreatePixelShader(_W("TestPS"));
	pPS->AddFunctionOp(pSampOp);
	pPS->AddFunctionOp(pColorMod);
	pPS->AddPixelIn(pPInPos);
	pPS->AddPixelIn(pPInColor);
	pPS->AddPixelIn(pPInTex0);
	pPS->AddPixelOut(pPOutColor);
	pPS->SetVersion(_W("2"), _W("0"));
	
	// Link Passes
	IMGPassPtr pPass = g_pMaterialGen->GetResourceMgr().CreatePass(_W("p0"));
	pPass->SetPixelShaderOp(pPS->CreateShaderOp());
	pPass->SetVertexShaderOp(pVS->CreateShaderOp());
	
	// Link Technique
	IMGTechniquePtr pTech = g_pMaterialGen->GetResourceMgr().CreateTechnique(_W("t0"));
	pTech->AddPass(pPass);
	
	// Link Material
	IMGMaterialTemplatePtr pTemplate = g_pMaterialGen->GetResourceMgr().CreateMaterialTemplate();
	pTemplate->AddFunction(pVec4Mult);
	pTemplate->AddFunction(pVec3Mult);
	pTemplate->AddFunction(pVec4Mod);
	pTemplate->AddFunction(pVec4Add);
	pTemplate->AddFunction(pNormalize);
	pTemplate->AddFunction(pComputeHemiAmbient);
	pTemplate->AddFunction(pComputeDirColor);
	pTemplate->AddFunction(pSamplerRGBA2D);
	pTemplate->AddGlobal(pWorld);
	pTemplate->AddGlobal(pViewProj);
	pTemplate->AddGlobal(pSampDiffuse);
	pTemplate->AddGlobal(pDiffuseTex);
	pTemplate->AddGlobal(pHemiLightGlobal);
	pTemplate->AddGlobal(pDirLightGlobal);
	pTemplate->AddGlobal(pEffectType);
	pTemplate->AddLocal(pDirLight);
	pTemplate->AddLocal(pHemiLight);
	pTemplate->AddTechnique(pTech);
	pTemplate->AddVertexShader(pVS);
	pTemplate->AddPixelShader(pPS);
	
	boolean available[64];
	_LOOPi(64)
	{
		available[i] = TRUE;
	}
	
	
	IByteBufferPtr pSave = _NEW CByteBuffer(1024);
	SaveMaterialTemplate(pTemplate, pSave);
	
	IFFilePtr pSaveFile = g_pFileSystem->GetFile(_W("[shd]\\TestMat.mte"));
	pSaveFile->Open(FO_WRITE_ONLY);
	pSaveFile->Write(pSave->GetData(), pSave->GetDataLength());
	pSaveFile->Close();
	
	IMGMaterialTemplate* pLoadTemplate = NULL;
	LoadMaterialTemplate(g_pMaterialGen, pSave, 0, &pLoadTemplate);

	IByteBufferPtr pBuffer = _NEW CByteBuffer(1024);
	pLoadTemplate->Compile(pBuffer, available);

	IFFilePtr pFile = g_pFileSystem->GetFile(_W("[shd]\\stuff2.fx"));
	pFile->Open(FO_WRITE_ONLY);
	pFile->Write(pBuffer->GetData(), pBuffer->GetDataLength());
	pFile->Close();
	
	pLoadTemplate->Release();
}