//==============================================================================
//
//		SaveFns.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/14/2008
//
//==============================================================================

#include "MaterialGenUtils.h"

_NAMESPACE_BEGIN

uint 
SaveMaterialTemplate(IMGMaterialTemplate* pTemplate, IByteBuffer* pBuffer)
{
	_DEBUG_ASSERT(pTemplate);
	_DEBUG_ASSERT(pBuffer);

	uint totalBytesWritten = 0;

	totalBytesWritten += AppendData(pBuffer, MGU_MAT_TEMPLATE_CHUNK_H);
	
	uint numLocals = pTemplate->GetNumOfLocals();
	uint numGlobals = pTemplate->GetNumOfGlobals();
	uint numFunctions = pTemplate->GetNumOfFunctions();
	uint numTechs = pTemplate->GetNumOfTechniques();
	uint numVS = pTemplate->GetNumOfVertexShaders();
	uint numPS = pTemplate->GetNumOfPixelShaders();
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('n','l','o','c'), &numLocals);
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('n','g','l','o'), &numGlobals);
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('n','f','u','n'), &numFunctions);
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('n','t','e','c'), &numTechs);
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('n','v','s',' '), &numVS);
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('n','p','s',' '), &numPS);

	_LOOPi(numLocals)
	{
		IMGLocal* pLocal = pTemplate->GetLocal(i);
		totalBytesWritten += SaveLocal(pLocal, pBuffer);
	}
	
	_LOOPi(numGlobals)
	{
		IMGGlobal* pGlobal = pTemplate->GetGlobal(i);
		totalBytesWritten += SaveGlobal(pGlobal, pBuffer);
	}
		
	_LOOPi(numFunctions)
	{
		IMGFunction* pFunction = pTemplate->GetFunction(i);
		totalBytesWritten += SaveFunction(pFunction, pBuffer);
	}
	
	_LOOPi(numVS)
	{
		IMGVertexShader* pVS = pTemplate->GetVertexShader(i);
		totalBytesWritten += SaveVertexShader(pVS, pBuffer);
	}

	_LOOPi(numPS)
	{
		IMGPixelShader* pPS = pTemplate->GetPixelShader(i);
		totalBytesWritten += SavePixelShader(pPS, pBuffer);
	}
	
	_LOOPi(numTechs)
	{
		IMGTechnique* pTech = pTemplate->GetTechnique(i);
		totalBytesWritten += SaveTechnique(pTech, pBuffer);
	}	
		
	totalBytesWritten += AppendData(pBuffer, END_CHUNK_H);	
	return totalBytesWritten;
}

uint
SaveLocal(IMGLocal* pLocal, IByteBuffer* pBuffer)
{
	uint totalBytesWritten = 0;

	totalBytesWritten += AppendData(pBuffer, MGU_LOCAL_CHUNK_H);
	
	const wchar* pName = pLocal->GetName();
	if(pName)
		totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('n','a','m','e'), pName);
	const wchar* pType = pLocal->GetType();
	if(pType)
		totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('t','y','p','e'), pType);
	const wchar* pPre = pLocal->GetPreamble();
	if(pPre)
		totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('p','r','e',' '), pPre);
	const wchar* pPost = pLocal->GetPostamble();
	if(pPost)
		totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('p','o','s','t'), pPost);
	const wchar* pVal = pLocal->GetValue();
	if(pVal)
		totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('v','a','l',' '), pVal);
		
	totalBytesWritten += AppendData(pBuffer, END_CHUNK_H);	
	return totalBytesWritten;
}

uint
SaveGlobal(IMGGlobal* pGlobal, IByteBuffer* pBuffer)
{
	uint totalBytesWritten = 0;

	totalBytesWritten += AppendData(pBuffer, MGU_GLOBAL_CHUNK_H);

	const wchar* pName = pGlobal->GetName();
	totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('n','a','m','e'), pName);
	const wchar* pType = pGlobal->GetType();
	totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('t','p','y','e'), pType);
	const wchar* pValue = pGlobal->GetValue();
	if(pValue)
		totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('v','a','l','u'), pValue);
	eMGUsage usage = pGlobal->GetUsage();
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('u','s','a','g'), &usage);
	uint semantic = pGlobal->GetSemantic();
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('s','e','m','a'), &semantic);
	boolean isAlwaysUsed = pGlobal->GetIsAlwaysUsed();
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('a','l','u','s'), &isAlwaysUsed);
	boolean isVolatile = pGlobal->GetIsVolatile();
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('v','o','l','a'), &isVolatile);

	uint numInputs = pGlobal->GetNumOfInputs();
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('n','i','m','p'), &numInputs);
	_DEBUG_ASSERT(numInputs < 65535);
	_LOOPi(numInputs)
	{
		IMGInputResource& in = pGlobal->GetInput(i);
		const wchar* pInType = in.GetType();
		uint tag = _MAKE_ID('t','y',0x00,0x00);
		tag |= i;
		if(pInType)
			totalBytesWritten += SaveDataString(pBuffer, tag, pInType);
		const wchar* pInName = in.GetName();
		tag =  _MAKE_ID('n','a',0x00,0x00);
		tag |= i;
		if(pInName)
			totalBytesWritten += SaveDataString(pBuffer, tag, pInName);
		const wchar* pInVal = in.GetDefaultVal();
		tag =  _MAKE_ID('v','a',0x00,0x00);
		tag |= i;
		if(pInVal)
			totalBytesWritten += SaveDataString(pBuffer, tag, pInVal);
		IMGOutputResource* pOut = in.GetBinding();
		uint outVal = (uint)(size_t) pOut;
		tag =  _MAKE_ID('b','i',0x00,0x00);
		tag |= i;
		if(pOut)
			totalBytesWritten += SaveData(pBuffer, tag, &outVal);
			
		IMGOutputResource* pAltOut = in.GetAltBinding();
		uint altOutVal = (uint)(size_t) pAltOut;
		tag =  _MAKE_ID('a','l',0x00,0x00);
		tag |= i;
		if(pAltOut)
			totalBytesWritten += SaveData(pBuffer, tag, &altOutVal);
	}

	IMGOutputResource* pOut = &(pGlobal->GetOutput());
	uint outVal = (uint)(size_t) pOut;
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('o','u','t',' '), &outVal);
	
	uint numDescs = pGlobal->GetNumOfDesc();
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('n','d','e','s'), &numDescs);
	_LOOPi(numDescs)
	{	
		const wchar* pDescName = pGlobal->GetDescName(i);
		uint tag = _MAKE_ID('d','n',0x00,0x00);
		tag |= i;
		if(pDescName)
			totalBytesWritten += SaveDataString(pBuffer, tag, pDescName);
		const wchar* pDescType = pGlobal->GetDescType(i);
		tag = _MAKE_ID('d','t',0x00,0x00);
		tag |= i;
		if(pDescType)
			totalBytesWritten += SaveDataString(pBuffer, tag, pDescType);
		const wchar* pDescVal = pGlobal->GetDescVal(i);
		tag = _MAKE_ID('d','v',0x00,0x00);
		tag |= i;
		if(pDescVal)
			totalBytesWritten += SaveDataString(pBuffer, tag, pDescVal);
	}
	
	totalBytesWritten += AppendData(pBuffer, END_CHUNK_H);	
	return totalBytesWritten;
}

uint
SaveTechnique(IMGTechnique* pTech, IByteBuffer* pBuffer)
{
	uint totalBytesWritten = 0;

	totalBytesWritten += AppendData(pBuffer, MGU_TECHNIQUE_CHUNK_H);

	const wchar* pName = pTech->GetName();
	totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('n','a','m','e'), pName);
	
	uint numPasses = pTech->GetNumOfPass();
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('n','p','a','s'), &numPasses);
	_LOOPi(numPasses)
	{
		IMGPass* pPass = pTech->GetPass(i);
		totalBytesWritten += SavePass(pPass, pBuffer);
	}
	
	totalBytesWritten += AppendData(pBuffer, END_CHUNK_H);	
	return totalBytesWritten;
}

uint
SavePass(IMGPass* pPass, IByteBuffer* pBuffer)
{
	uint totalBytesWritten = 0;

	totalBytesWritten += AppendData(pBuffer, MGU_PASS_CHUNK_H);

	IMGVertexShader* pVS = pPass->GetVertexShaderOp()->GetShader();
	uint vsVal = (uint)(size_t)(pVS);
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('v','s','v','a'), &vsVal);

	IMGPixelShader* pPS = pPass->GetPixelShaderOp()->GetShader();
	uint psVal = (uint)(size_t)(pPS);
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('p','s','v','a'), &psVal);

	const wchar* pName = pPass->GetName();
	totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('n','a','m','e'), pName);

	uint numStates = pPass->GetNumOfRenderStates();
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('n','s','t','a'), &numStates);
	
	_LOOPi(numStates)
	{
		IMGRenderState* pState = pPass->GetRenderState(i);
		totalBytesWritten += SaveRenderState(pState, pBuffer);
	}
	
	totalBytesWritten += AppendData(pBuffer, END_CHUNK_H);	
	return totalBytesWritten;
}

uint
SaveVertexShader(IMGVertexShader* pShader, IByteBuffer* pBuffer)
{
	uint totalBytesWritten = 0;

	totalBytesWritten += AppendData(pBuffer, MGU_VS_CHUNK_H);

	const wchar* pName = pShader->GetName();
	totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('n','a','m','e'), pName);
	const wchar* pMaj = pShader->GetVersionMajor();
	totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('m','a','j','r'), pMaj);
	const wchar* pMin = pShader->GetVersionMinor();
	totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('m','i','n','r'), pMin);

	uint numVIns = pShader->GetNumOfVertexIn();
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('n','v','i','n'), &numVIns);
	uint numVOuts = pShader->GetNumOfVertexOut();
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('n','v','o','u'), &numVOuts);
	uint numOps = pShader->GetNumOfFunctionOp();
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('n','o','p','s'), &numOps);
	
	uint shaderVal = (uint)(size_t)pShader;
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('s','h','v','a'), &shaderVal);
	
	_LOOPi(numVIns)
	{
		IMGVertexIn* pVIn = pShader->GetVertexIn(i);
		totalBytesWritten += SaveVertexIn(pVIn, pBuffer);
	}
	
	_LOOPi(numVOuts)
	{
		IMGVertexOut* pVOut = pShader->GetVertexOut(i);
		totalBytesWritten += SaveVertexOut(pVOut, pBuffer);
	}

	_LOOPi(numOps)
	{
		IMGFunctionOp* pOp = pShader->GetFunctionOp(i);
		totalBytesWritten += SaveFunctionOp(pOp, pBuffer);
	}
	
	totalBytesWritten += AppendData(pBuffer, END_CHUNK_H);	
	return totalBytesWritten;
}

uint
SavePixelShader(IMGPixelShader* pShader, IByteBuffer* pBuffer)
{
	uint totalBytesWritten = 0;

	totalBytesWritten += AppendData(pBuffer, MGU_PS_CHUNK_H);

	const wchar* pName = pShader->GetName();
	totalBytesWritten += SaveDataString(pBuffer,  _MAKE_ID('n','a','m','e'), pName);
	const wchar* pMaj = pShader->GetVersionMajor();
	totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('m','a','j','r'), pMaj);
	const wchar* pMin = pShader->GetVersionMinor();
	totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('m','i','n','r'), pMin);

	uint numPIns = pShader->GetNumOfPixelIn();
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('n','p','i','n'), &numPIns);
	uint numPOuts = pShader->GetNumOfPixelOut();
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('n','p','o','u'), &numPOuts);
	uint numOps = pShader->GetNumOfFunctionOp();
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('n','o','p','s'), &numOps);

	uint shaderVal = (uint)(size_t)pShader;
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('s','h','v','a'), &shaderVal);

	_LOOPi(numPIns)
	{
		IMGPixelIn* pPIn = pShader->GetPixelIn(i);
		totalBytesWritten += SavePixelIn(pPIn, pBuffer);
	}

	_LOOPi(numPOuts)
	{
		IMGPixelOut* pPOut = pShader->GetPixelOut(i);
		totalBytesWritten += SavePixelOut(pPOut, pBuffer);
	}

	_LOOPi(numOps)
	{
		IMGFunctionOp* pOp = pShader->GetFunctionOp(i);
		totalBytesWritten += SaveFunctionOp(pOp, pBuffer);
	}

	totalBytesWritten += AppendData(pBuffer, END_CHUNK_H);	
	return totalBytesWritten;
}

uint
SaveRenderState(IMGRenderState* pRenderState, IByteBuffer* pBuffer)
{
	uint totalBytesWritten = 0;

	totalBytesWritten += AppendData(pBuffer, MGU_RENDERSTATE_CHUNK_H);

	const wchar* pName = pRenderState->GetName();
	totalBytesWritten += SaveDataString(pBuffer,  _MAKE_ID('n','a','m','e'), pName);
	const wchar* pType = pRenderState->GetType();
	if(pType)
		totalBytesWritten += SaveDataString(pBuffer,  _MAKE_ID('t','y','p','e'), pType);
	const wchar* pValue = pRenderState->GetValue();
	if(pValue)
		totalBytesWritten += SaveDataString(pBuffer,  _MAKE_ID('v','a','l','u'), pValue);

	uint numInputs = pRenderState->GetNumOfInputs();
	totalBytesWritten += SaveData(pBuffer,  _MAKE_ID('n','i','n','s'), &numInputs);
	_DEBUG_ASSERT(numInputs < 65535);

	_LOOPi(numInputs)
	{
		IMGInputResource* pIn = &pRenderState->GetInput(i);
		
		const wchar* pInName = pIn->GetName();
		uint tag = _MAKE_ID('i','n',0x00,0x00);
		tag |= i;
		totalBytesWritten += SaveDataString(pBuffer, tag, pInName);
		const wchar* pInType = pIn->GetType();
		tag = _MAKE_ID('i','t',0x00,0x00);
		tag |= i;
		totalBytesWritten += SaveDataString(pBuffer, tag, pInType);
		const wchar* pInValue = pIn->GetDefaultVal();
		tag = _MAKE_ID('i','v',0x00,0x00);
		tag |= i;
		if(pInValue)
			totalBytesWritten += SaveDataString(pBuffer, tag, pInValue);	
		IMGOutputResource* pOut = pIn->GetBinding();
		uint outVal = (uint)(size_t) pOut;
		tag = _MAKE_ID('i','b',0x00,0x00);
		tag |= i;
		if(pOut)
			totalBytesWritten += SaveData(pBuffer, tag, &outVal);	
			
		IMGOutputResource* pAltOut = pIn->GetAltBinding();
		uint altOutVal = (uint)(size_t) pAltOut;
		tag = _MAKE_ID('i','a',0x00,0x00);
		tag |= i;
		if(pAltOut)
			totalBytesWritten += SaveData(pBuffer, tag, &altOutVal);	
	}

	totalBytesWritten += AppendData(pBuffer, END_CHUNK_H);	
	return totalBytesWritten;
}

uint
SaveFunction(IMGFunction* pFunction, IByteBuffer* pBuffer)
{
	uint totalBytesWritten = 0;

	totalBytesWritten += AppendData(pBuffer, MGU_FUNCTION_CHUNK_H);

	const wchar* pName = pFunction->GetName();
	totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('n','a','m','e'), pName);
	
	const wchar* pType = pFunction->GetType();
	totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('t','y','p','e'), pType);
	
	const wchar* pValue = pFunction->GetValue();
	totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('v','a','l','u'), pValue);
	
	uint fnVal = (uint)(size_t) pFunction;
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('f','n','v','a'), &fnVal);
	
	uint numInputs = pFunction->GetNumOfInputs();
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('n','i','n','s'), &numInputs);
	
	_DEBUG_ASSERT(numInputs < 65535);
	
	_LOOPi(numInputs)
	{
		const wchar* pInName = pFunction->GetInputName(i);
		uint tag = _MAKE_ID('i','n',0x00,0x00);
		tag |= i;
		totalBytesWritten += SaveDataString(pBuffer, tag, pInName);
		const wchar* pInType = pFunction->GetInputType(i);
		tag = _MAKE_ID('i','t',0x00,0x00);
		tag |= i;
		totalBytesWritten += SaveDataString(pBuffer, tag, pInType);
		const wchar* pInValue = pFunction->GetInputDefaultVal(i);
		tag = _MAKE_ID('i','v',0x00,0x00);
		tag |= i;
		if(pInValue)
			totalBytesWritten += SaveDataString(pBuffer, tag, pInValue);
	}
	
	totalBytesWritten += AppendData(pBuffer, END_CHUNK_H);	
	return totalBytesWritten;
}

uint
SaveFunctionOp(IMGFunctionOp* pOp, IByteBuffer* pBuffer)
{
	uint totalBytesWritten = 0;

	totalBytesWritten += AppendData(pBuffer, MGU_FUNCTION_OP_CHUNK_H);

	IMGFunction* pFn = pOp->GetFunction();
	uint fnVal = (uint) (size_t) pFn;
	_DEBUG_ASSERT(pFn);
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('f','n','v','a'), &fnVal);

	uint numIns = pOp->GetNumOfInputs();
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('n','i','n','s'), &numIns);
	
	_LOOPi(numIns)
	{
		IMGInputResource* pIn = &pOp->GetInput(i);
		IMGOutputResource* pBinding = pIn->GetBinding();
		uint bindVal = (uint)(size_t)pBinding;
		uint tag = _MAKE_ID('i','b',0x00,0x00);
		tag |= i;
		if(pBinding)
			totalBytesWritten += SaveData(pBuffer, tag, &bindVal);
			
		IMGOutputResource* pAltBinding = pIn->GetAltBinding();
		uint altBindVal = (uint)(size_t) pAltBinding;
		tag = _MAKE_ID('i','a,',0x00,0x00);
		tag |= i;
		if(pAltBinding)
			totalBytesWritten += SaveData(pBuffer, tag, &altBindVal);	
	}

	IMGOutputResource* pOut = &pOp->GetOutput();
	uint outVal = (uint)(size_t)pOut;
	_DEBUG_ASSERT(pOut);
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('o','u','t',' '), &outVal);
		
	totalBytesWritten += AppendData(pBuffer, END_CHUNK_H);	
	return totalBytesWritten;
}

uint
SaveVertexIn(IMGVertexIn* pIn, IByteBuffer* pBuffer)
{
	uint totalBytesWritten = 0;

	totalBytesWritten += AppendData(pBuffer, MGU_VIN_CHUNK_H);

	const wchar* pName = pIn->GetName();
	totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('n','a','m','e'), pName);
	const wchar* pType = pIn->GetType();
	totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('t','y','p','e'), pType);
	
	IMGOutputResource* pOut = &pIn->GetOutput();
	uint outVal = (uint)(size_t)pOut;
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('o','u','t',' '), &outVal);
	_DEBUG_ASSERT(pOut);
	
	totalBytesWritten += AppendData(pBuffer, END_CHUNK_H);	
	return totalBytesWritten;
}

uint
SaveVertexOut(IMGVertexOut* pOut, IByteBuffer* pBuffer)
{
	uint totalBytesWritten = 0;

	totalBytesWritten += AppendData(pBuffer, MGU_VOUT_CHUNK_H);

	const wchar* pName = pOut->GetName();
	totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('n','a','m','e'), pName);
	const wchar* pType = pOut->GetType();
	totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('t','y','p','e'), pType);

	IMGInputResource* pIn = &pOut->GetInput();
	IMGOutputResource* pBinding =  pIn->GetBinding();
	uint bindVal = (uint)(size_t)pBinding;
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('b','i','n','d'), &bindVal);

	IMGOutputResource* pAltBinding = pIn->GetAltBinding();
	uint altBindVal = (uint)(size_t) pAltBinding;
	if(pAltBinding)
		totalBytesWritten += SaveData(pBuffer, _MAKE_ID('a','l','t','b'), &altBindVal);	
			
	//_DEBUG_ASSERT(pBinding);
	totalBytesWritten += AppendData(pBuffer, END_CHUNK_H);	
	return totalBytesWritten;
}

uint
SavePixelIn(IMGPixelIn* pIn, IByteBuffer* pBuffer)
{
	uint totalBytesWritten = 0;

	totalBytesWritten += AppendData(pBuffer, MGU_PIN_CHUNK_H);

	const wchar* pName = pIn->GetName();
	totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('n','a','m','e'), pName);
	const wchar* pType = pIn->GetType();
	totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('t','y','p','e'), pType);

	IMGOutputResource* pOut = &pIn->GetOutput();
	uint outVal = (uint)(size_t)pOut;
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('o','u','t',' '), &outVal);
	_DEBUG_ASSERT(pOut);

	totalBytesWritten += AppendData(pBuffer, END_CHUNK_H);	
	return totalBytesWritten;
}

uint
SavePixelOut(IMGPixelOut* pOut, IByteBuffer* pBuffer)
{
	uint totalBytesWritten = 0;

	totalBytesWritten += AppendData(pBuffer, MGU_POUT_CHUNK_H);

	const wchar* pName = pOut->GetName();
	totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('n','a','m','e'), pName);
	const wchar* pType = pOut->GetType();
	totalBytesWritten += SaveDataString(pBuffer, _MAKE_ID('t','y','p','e'), pType);

	IMGInputResource* pIn = &pOut->GetInput();
	IMGOutputResource* pBinding =  pIn->GetBinding();
	uint bindVal = (uint)(size_t)pBinding;
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('b','i','n','d'), &bindVal);

	IMGOutputResource* pAltBinding = pIn->GetAltBinding();
	uint altBindVal = (uint)(size_t) pAltBinding;
	if(pAltBinding)
		totalBytesWritten += SaveData(pBuffer, _MAKE_ID('a','l','t','b'), &altBindVal);	
	
	//_DEBUG_ASSERT(pBinding);
	totalBytesWritten += AppendData(pBuffer, END_CHUNK_H);	
	return totalBytesWritten;
}

_NAMESPACE_END