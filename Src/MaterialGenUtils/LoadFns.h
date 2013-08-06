//==============================================================================
//
//		LoadFns.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/17/2008
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

struct InputLink
{
	uint bindVal;
	uint altBindVal;
	IMGInputResource* pIn;
	
	InputLink(uint val, uint altVal, IMGInputResource* pInput)
		: bindVal(val)
		, altBindVal(altVal)
		, pIn(pInput)
	{
	}
	
	~InputLink()
	{
	}
};

typedef std::vector<InputLink> InputList;
typedef std::map<uint, IMGOutputResource*> OutputMap;
typedef std::map<uint, IMGFunction*> FunctionMap;
typedef std::map<uint, IMGVertexShader*> VertexShaderMap;
typedef std::map<uint, IMGPixelShader*> PixelShaderMap;

uint
LoadMaterialTemplate(IMGMaterialGen* pGen, IByteBuffer* pBuffer, uint offset, IMGMaterialTemplate** ppTemplate);

uint
LoadLocal(IMGMaterialGen* pGen, IByteBuffer* pBuffer, uint offset, IMGLocal** ppLocal);

uint
LoadGlobal(IMGMaterialGen* pGen, IByteBuffer* pBuffer, uint offset, IMGGlobal** ppGlobal, InputList* pInputs, OutputMap* pOutputs);

uint
LoadFunction(IMGMaterialGen* pGen, IByteBuffer* pBuffer, uint offset, IMGFunction** ppFunction, FunctionMap* pFunctions);

uint
LoadVertexShader(IMGMaterialGen* pGen, IByteBuffer* pBuffer, uint offset, IMGVertexShader** ppShader, 
				 InputList* pInputs, OutputMap* pOutputs, 
				 FunctionMap* pFunctions, VertexShaderMap* pVertexShaders);

uint
LoadVertexIn(IMGMaterialGen* pGen, IByteBuffer* pBuffer, uint offset, IMGVertexIn** ppVIn, OutputMap* pOutputs);

uint
LoadVertexOut(IMGMaterialGen* pGen, IByteBuffer* pBuffer, uint offset, IMGVertexOut** ppVOut, InputList* pInputs);

uint
LoadFunctionOp(IMGMaterialGen* pGen, IByteBuffer* pBuffer, uint offset, IMGFunctionOp** ppOp, 
			   InputList* pInputs, OutputMap* pOutputs,
			   FunctionMap* pFunctions);
			   
uint
LoadPixelShader(IMGMaterialGen* pGen, IByteBuffer* pBuffer, uint offset, IMGPixelShader** ppShader, 
				InputList* pInputs, OutputMap* pOutputs, 
				FunctionMap* pFunctions, PixelShaderMap* pPixelShaders);

uint
LoadPixelIn(IMGMaterialGen* pGen, IByteBuffer* pBuffer, uint offset, IMGPixelIn** ppPIn, OutputMap* pOutputs);

uint
LoadPixelOut(IMGMaterialGen* pGen, IByteBuffer* pBuffer, uint offset, IMGPixelOut** ppPOut, InputList* pInputs);

uint
LoadTechnique(IMGMaterialGen* pGen, IByteBuffer* pBuffer, uint offset, IMGTechnique** ppTech, InputList* pInputs,
	PixelShaderMap* pPixelShaders, VertexShaderMap* pVertexShaders);

uint
LoadPass(IMGMaterialGen* pGen, IByteBuffer* pBuffer, uint offset, IMGPass** ppPass, InputList* pInputs,
	PixelShaderMap* pPixelShaders, VertexShaderMap* pVertexShaders);

uint
LoadRenderState(IMGMaterialGen* pGen, IByteBuffer* pBuffer, uint offset, IMGRenderState** ppState, InputList* pInputs);

_NAMESPACE_END