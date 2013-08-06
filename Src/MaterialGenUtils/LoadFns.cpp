//==============================================================================
//
//		LoadFns.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/17/2008
//
//==============================================================================

#include "MaterialGenUtils.h"

_NAMESPACE_BEGIN

uint
LoadMaterialTemplate(IMGMaterialGen* pGen, IByteBuffer* pBuffer, uint offset, IMGMaterialTemplate** ppTemplate)
{
	uint totalBytesRead = 0;
	uint header = 0;
	uint numBytes = ReadData(pBuffer, offset, &header);

	if(header == MGU_MAT_TEMPLATE_CHUNK_H)
	{
		totalBytesRead += numBytes;

		InputList inputList;
		OutputMap outputMap;
		FunctionMap functionMap;
		VertexShaderMap vertexShaderMap;
		PixelShaderMap pixelShaderMap;
		
		DataMap store;
		totalBytesRead += LoadAllData(pBuffer, &store, offset + totalBytesRead);

		uint* pNumLocals = NULL;
		boolean isFound = GetDataRaw(&store, _MAKE_ID('n','l','o','c'), (void**) &pNumLocals);
		_DEBUG_ASSERT(isFound);
		uint* pNumGlobals = NULL;
		isFound = GetDataRaw(&store, _MAKE_ID('n','g','l','o'), (void**) &pNumGlobals);
		_DEBUG_ASSERT(isFound);
		uint* pNumFunctions = NULL;
		isFound = GetDataRaw(&store, _MAKE_ID('n','f','u','n'), (void**) &pNumFunctions);
		_DEBUG_ASSERT(isFound);
		uint* pNumTechs = NULL;
		isFound = GetDataRaw(&store, _MAKE_ID('n','t','e','c'), (void**) &pNumTechs);
		_DEBUG_ASSERT(isFound);
		uint* pNumVS = NULL;
		isFound = GetDataRaw(&store, _MAKE_ID('n','v','s',' '), (void**) &pNumVS);
		_DEBUG_ASSERT(isFound);
		uint* pNumPS = NULL;
		isFound = GetDataRaw(&store, _MAKE_ID('n','p','s',' '), (void**) &pNumPS);
		_DEBUG_ASSERT(isFound);

		IMGMaterialTemplate* pTemplate = pGen->GetResourceMgr().CreateMaterialTemplate();
		
		_LOOPi(*pNumLocals)
		{
			IMGLocal* pLocal = NULL;
			totalBytesRead += LoadLocal(pGen, pBuffer, offset + totalBytesRead, &pLocal);
			_DEBUG_ASSERT(pLocal);
			pTemplate->AddLocal(pLocal);
		}

		_LOOPi(*pNumGlobals)
		{
			IMGGlobal* pGlobal = NULL;
			totalBytesRead += LoadGlobal(pGen, pBuffer, offset + totalBytesRead, &pGlobal, &inputList, &outputMap);
			_DEBUG_ASSERT(pGlobal);
			pTemplate->AddGlobal(pGlobal);
		}
		
		_LOOPi(*pNumFunctions)
		{
			IMGFunction* pFunc = NULL;
			totalBytesRead += LoadFunction(pGen, pBuffer, offset + totalBytesRead, &pFunc, &functionMap);
			_DEBUG_ASSERT(pFunc);
			pTemplate->AddFunction(pFunc);
		}
		
		_LOOPi(*pNumVS)
		{
			IMGVertexShader* pShader = NULL;
			totalBytesRead += LoadVertexShader(pGen, pBuffer, offset + totalBytesRead, &pShader, &inputList, &outputMap, &functionMap, &vertexShaderMap);
			_DEBUG_ASSERT(pShader);
			pTemplate->AddVertexShader(pShader);
		}
		
		_LOOPi(*pNumPS)
		{
			IMGPixelShader* pShader = NULL;
			totalBytesRead += LoadPixelShader(pGen, pBuffer, offset + totalBytesRead, &pShader, &inputList, &outputMap, &functionMap, &pixelShaderMap);
			_DEBUG_ASSERT(pShader);
			pTemplate->AddPixelShader(pShader);
		}
		
		_LOOPi(*pNumTechs)
		{
			IMGTechnique* pTech = NULL;
			totalBytesRead += LoadTechnique(pGen, pBuffer, offset + totalBytesRead, &pTech, &inputList, &pixelShaderMap, &vertexShaderMap);
			_DEBUG_ASSERT(pTech);
			pTemplate->AddTechnique(pTech);
		}
		
		*ppTemplate = pTemplate;
		
		// Link all the inputs to the outputs
		_ITER_STLi(InputList, inputList)
		{
			OutputMap::iterator outItr = outputMap.find(iitr->bindVal);
			OutputMap::iterator altItr = outputMap.end();
			if(iitr->altBindVal > 0)
				altItr = outputMap.find(iitr->altBindVal);
				
			//_DEBUG_ASSERT(outItr != outputMap.end());
			IMGInputResource* pInput = iitr->pIn;
			if(outItr != outputMap.end())
				pInput->BindValue(outItr->second);
			if(altItr != outputMap.end())
				pInput->BindAltValue(altItr->second);		
		}
		
		byte endHeader = 0;
		totalBytesRead += ReadData(pBuffer, offset + totalBytesRead, &endHeader);
		_DEBUG_ASSERT(endHeader == END_CHUNK_H);
	}

	return totalBytesRead;
}

uint
LoadLocal(IMGMaterialGen* pGen, IByteBuffer* pBuffer, uint offset, IMGLocal** ppLocal)
{
	uint totalBytesRead = 0;
	uint header = 0;
	uint numBytes = ReadData(pBuffer, offset, &header);

	if(header == MGU_LOCAL_CHUNK_H)
	{
		totalBytesRead += numBytes;

		DataMap store;
		totalBytesRead += LoadAllData(pBuffer, &store, offset + totalBytesRead);
		
		const wchar* pName = NULL;
		GetDataRaw(&store, _MAKE_ID('n','a','m','e'), (void**) &pName);
		const wchar* pType = NULL;
		GetDataRaw(&store, _MAKE_ID('t','y','p','e'), (void**) &pType);
		const wchar* pPre = NULL;
		GetDataRaw(&store, _MAKE_ID('p','r','e',' '), (void**) &pPre);
		const wchar* pPost = NULL;
		GetDataRaw(&store, _MAKE_ID('p','o','s','t'), (void**) &pPost);
		const wchar* pVal = NULL;
		GetDataRaw(&store, _MAKE_ID('v','a','l',' '), (void**) &pVal);
		
		MGLocalSig sig;
		sig.pName = pName;
		sig.pPostamble = pPost;
		sig.pPreamble = pPre;
		sig.pType = pType;
		
		IMGLocal* pLocal = pGen->GetResourceMgr().CreateLocal(&sig);
		if(pVal)
			pLocal->SetValue(pVal);
		*ppLocal = pLocal;
		
		byte endHeader = 0;
		totalBytesRead += ReadData(pBuffer, offset + totalBytesRead, &endHeader);
		_DEBUG_ASSERT(endHeader == END_CHUNK_H);
	}
	
	return totalBytesRead;
}

uint
LoadGlobal(IMGMaterialGen* pGen, IByteBuffer* pBuffer, uint offset, IMGGlobal** ppGlobal, 
	InputList* pInputs, OutputMap* pOutputs)
{
	uint totalBytesRead = 0;
	uint header = 0;
	uint numBytes = ReadData(pBuffer, offset, &header);

	if(header == MGU_GLOBAL_CHUNK_H)
	{
		totalBytesRead += numBytes;

		DataMap store;
		totalBytesRead += LoadAllData(pBuffer, &store, offset + totalBytesRead);

		const wchar* pName = NULL;
		boolean isFound = GetDataRaw(&store, _MAKE_ID('n','a','m','e'), (void**) &pName);
		_DEBUG_ASSERT(isFound);
		const wchar* pType = NULL;
		isFound = GetDataRaw(&store, _MAKE_ID('t','p','y','e'), (void**) &pType);
		_DEBUG_ASSERT(isFound);
		const wchar* pValue = NULL;
		GetDataRaw(&store, _MAKE_ID('v','a','l','u'), (void**) &pValue);		
		eMGUsage* pUsage = NULL;
		isFound = GetDataRaw(&store, _MAKE_ID('u','s','a','g'), (void**) &pUsage);
		_DEBUG_ASSERT(isFound);
		uint* pSemantic = NULL;
		isFound = GetDataRaw(&store, _MAKE_ID('s','e','m','a'), (void**) &pSemantic);
		_DEBUG_ASSERT(isFound);
		boolean* pIsAlwaysUsed = NULL;
		isFound = GetDataRaw(&store, _MAKE_ID('a','l','u','s'), (void**)&pIsAlwaysUsed);
		_DEBUG_ASSERT(isFound);
		boolean* pIsVolatile = NULL;
		isFound = GetDataRaw(&store, _MAKE_ID('v','o','l','a'), (void**)&pIsVolatile);
		_DEBUG_ASSERT(isFound);

		MGParamSig paramSig;
		paramSig.pName = pName;
		paramSig.pType = pType;
		
		uint* pNumInputs = NULL;
		isFound = GetDataRaw(&store, _MAKE_ID('n','i','m','p'), (void**) &pNumInputs);
		_DEBUG_ASSERT(pNumInputs);
		
		MGInputSig inSigs[64];
		_DEBUG_ASSERT((*pNumInputs) < 65535);
		_LOOPi(*pNumInputs)
		{
			const wchar* pInType = NULL;
			uint tag = _MAKE_ID('t','y',0x00,0x00);
			tag |= i;
			GetDataRaw(&store, tag, (void**) &pInType);
			const wchar* pInName = NULL;
			tag =  _MAKE_ID('n','a',0x00,0x00);
			tag |= i;
			GetDataRaw(&store, tag, (void**) &pInName);
			const wchar* pInVal = NULL;
			tag =  _MAKE_ID('v','a',0x00,0x00);
			tag |= i;
			GetDataRaw(&store, tag, (void**) &pInVal);
			
			inSigs[i].pType = pInType;
			inSigs[i].pName = pInName;
			inSigs[i].pDefaultVal = pInVal;
		}

		IMGGlobal* pGlobal = pGen->GetResourceMgr().CreateGlobal(&paramSig, inSigs, *pNumInputs);
		pGlobal->SetUsage(*pUsage);
		pGlobal->SetSemantic(*pSemantic);
		pGlobal->SetIsAlwaysUsed(*pIsAlwaysUsed);
		pGlobal->SetIsVolatile(*pIsVolatile);
		if(pValue)
			pGlobal->SetValue(pValue);
		
		_LOOPi(*pNumInputs)
		{
			uint defVal = 0;
			uint* pBindVal = &defVal;
			uint* pAltBindVal = &defVal;
			
			uint tag = _MAKE_ID('b','i',0x00,0x00);
			tag |= i;
			GetDataRaw(&store, tag, (void**) &pBindVal);
			tag =  _MAKE_ID('a','l',0x00,0x00);
			tag |= i;
			GetDataRaw(&store, tag, (void**) &pAltBindVal);
			if(pInputs)
				pInputs->push_back(InputLink(*pBindVal, *pAltBindVal, &pGlobal->GetInput(i)));
		}
		
		uint* pOutVal = NULL;
		isFound = GetDataRaw(&store, _MAKE_ID('o','u','t',' '), (void**) &pOutVal);
		_DEBUG_ASSERT(pOutVal);
		
		if(pOutputs)
			pOutputs->insert(std::pair<uint, IMGOutputResource*>(*pOutVal, &pGlobal->GetOutput()));
		
		uint* pNumDesc = NULL;
		isFound = GetDataRaw(&store, _MAKE_ID('n','d','e','s'), (void**) &pNumDesc);
		_DEBUG_ASSERT((*pNumDesc) < 65535);
		_LOOPi(*pNumDesc)
		{
			const wchar* pDescName = NULL;
			uint tag = _MAKE_ID('d','n',0x00,0x00);
			tag |= i;
			GetDataRaw(&store, tag, (void**) &pDescName);
			const wchar* pDescType = NULL;
			tag = _MAKE_ID('d','t',0x00,0x00);
			tag |= i;
			GetDataRaw(&store, tag, (void**) &pDescType);
			const wchar* pDescVal = NULL;
			tag = _MAKE_ID('d','v',0x00,0x00);
			tag |= i;
			GetDataRaw(&store, tag, (void**) &pDescVal);
			MGInputSig descSig;
			descSig.pName = pDescName;
			descSig.pType = pDescType;
			descSig.pDefaultVal = pDescVal;
			pGlobal->AddDescription(&descSig);
		}

		*ppGlobal = pGlobal;
		
		byte endHeader = 0;
		totalBytesRead += ReadData(pBuffer, offset + totalBytesRead, &endHeader);
		_DEBUG_ASSERT(endHeader == END_CHUNK_H);
	}

	return totalBytesRead;
}

uint
LoadFunction(IMGMaterialGen* pGen, IByteBuffer* pBuffer, uint offset, IMGFunction** ppFunction, FunctionMap* pFunctions)
{
	uint totalBytesRead = 0;
	uint header = 0;
	uint numBytes = ReadData(pBuffer, offset, &header);

	if(header == MGU_FUNCTION_CHUNK_H)
	{
		totalBytesRead += numBytes;

		DataMap store;
		totalBytesRead += LoadAllData(pBuffer, &store, offset + totalBytesRead);
		
		const wchar* pName = NULL;
		GetDataRaw(&store, _MAKE_ID('n','a','m','e'), (void**) &pName);
		const wchar* pType = NULL;
		GetDataRaw(&store, _MAKE_ID('t','y','p','e'), (void**) &pType);
		const wchar* pValue = NULL;
		GetDataRaw(&store, _MAKE_ID('v','a','l','u'), (void**) &pValue);

		uint* pNumInputs = NULL;
		GetDataRaw(&store, _MAKE_ID('n','i','n','s'), (void**) &pNumInputs);
		_DEBUG_ASSERT(pNumInputs);
		
		uint* pFnVal = NULL;
		GetDataRaw(&store, _MAKE_ID('f','n','v','a'), (void**) &pFnVal);
		_DEBUG_ASSERT(pFnVal);
		
		MGInputSig inputSigs[64];
		_LOOPi(*pNumInputs)
		{
			const wchar* pInName = NULL;
			GetDataRaw(&store, _MAKE_ID('i','n',0x00,0x00) | i, (void**) &pInName);
			const wchar* pInType = NULL;
			GetDataRaw(&store, _MAKE_ID('i','t',0x00,0x00) | i, (void**) &pInType);
			const wchar* pInValue = NULL;
			GetDataRaw(&store, _MAKE_ID('i','v',0x00,0x00) | i, (void**) &pInValue);
			
			inputSigs[i].pDefaultVal = pInValue;
			inputSigs[i].pName = pInName;
			inputSigs[i].pType = pInType;
		}
		
		MGParamSig sig;
		sig.pName = pName;
		sig.pType = pType;
		
		IMGFunction* pFunc = pGen->GetResourceMgr().CreateFunction(&sig, inputSigs, *pNumInputs);
		if(pValue)
			pFunc->SetValue(pValue);
		*ppFunction = pFunc;

		if(pFunctions)
			pFunctions->insert(std::pair<uint, IMGFunction*>(*pFnVal, pFunc));
			
		byte endHeader = 0;
		totalBytesRead += ReadData(pBuffer, offset + totalBytesRead, &endHeader);
		_DEBUG_ASSERT(endHeader == END_CHUNK_H);
	}

	return totalBytesRead;
}

uint
LoadVertexShader(IMGMaterialGen* pGen, IByteBuffer* pBuffer, uint offset, IMGVertexShader** ppShader, 
		   InputList* pInputs, OutputMap* pOutputs, 
		   FunctionMap* pFunctions, VertexShaderMap* pVertexShaders)
{
	uint totalBytesRead = 0;
	uint header = 0;
	uint numBytes = ReadData(pBuffer, offset, &header);

	if(header == MGU_VS_CHUNK_H)
	{
		totalBytesRead += numBytes;

		DataMap store;
		totalBytesRead += LoadAllData(pBuffer, &store, offset + totalBytesRead);

		const wchar* pName = NULL;
		GetDataRaw(&store, _MAKE_ID('n','a','m','e'), (void**) &pName);
		_DEBUG_ASSERT(pName);
		const wchar* pMajor = NULL;
		GetDataRaw(&store, _MAKE_ID('m','a','j','r'), (void**) &pMajor);
		_DEBUG_ASSERT(pMajor);
		const wchar* pMinor = NULL;
		GetDataRaw(&store, _MAKE_ID('m','i','n','r'), (void**) &pMinor);
		_DEBUG_ASSERT(pMinor);

		uint* pNumVIns = NULL;
		GetDataRaw(&store, _MAKE_ID('n','v','i','n'), (void**) &pNumVIns);
		_DEBUG_ASSERT(pNumVIns);
		uint* pNumVOuts = NULL;
		GetDataRaw(&store, _MAKE_ID('n','v','o','u'), (void**) &pNumVOuts);
		_DEBUG_ASSERT(pNumVOuts);
		uint* pNumOps = NULL;
		GetDataRaw(&store, _MAKE_ID('n','o','p','s'), (void**) &pNumOps);
		_DEBUG_ASSERT(pNumOps);

		uint* pShaderVal = NULL;
		GetDataRaw(&store, _MAKE_ID('s','h','v','a'), (void**) &pShaderVal);
		_DEBUG_ASSERT(pShaderVal);
		
		IMGVertexShader* pVS = pGen->GetResourceMgr().CreateVertexShader(pName);
		pVS->SetVersion(pMajor, pMinor);
		
		_LOOPi(*pNumVIns)
		{
			IMGVertexIn* pVIn = NULL;
			totalBytesRead += LoadVertexIn(pGen, pBuffer, offset + totalBytesRead, &pVIn, pOutputs);
			_DEBUG_ASSERT(pVIn);
			pVS->AddVertexIn(pVIn);			
		}
		
		_LOOPi(*pNumVOuts)
		{
			IMGVertexOut* pVOut = NULL;
			totalBytesRead += LoadVertexOut(pGen, pBuffer, offset + totalBytesRead, &pVOut, pInputs);
			_DEBUG_ASSERT(pVOut);
			pVS->AddVertexOut(pVOut);
		}
	
		_LOOPi(*pNumOps)
		{
			IMGFunctionOp* pOp = NULL;
			totalBytesRead += LoadFunctionOp(pGen, pBuffer, offset + totalBytesRead, &pOp, pInputs, pOutputs, pFunctions);
			_DEBUG_ASSERT(pOp);
			pVS->AddFunctionOp(pOp);
		}
		
		if(pVertexShaders)
			pVertexShaders->insert(std::pair<uint, IMGVertexShader*>(*pShaderVal, pVS));
			
		*ppShader = pVS;
		
		byte endHeader = 0;
		totalBytesRead += ReadData(pBuffer, offset + totalBytesRead, &endHeader);
		_DEBUG_ASSERT(endHeader == END_CHUNK_H);
	}

	return totalBytesRead;
}

uint
LoadVertexIn(IMGMaterialGen* pGen, IByteBuffer* pBuffer, uint offset, IMGVertexIn** ppVIn, OutputMap* pOutputs)
{
	uint totalBytesRead = 0;
	uint header = 0;
	uint numBytes = ReadData(pBuffer, offset, &header);

	if(header == MGU_VIN_CHUNK_H)
	{
		totalBytesRead += numBytes;

		DataMap store;
		totalBytesRead += LoadAllData(pBuffer, &store, offset + totalBytesRead);

		const wchar* pName = NULL;
		GetDataRaw(&store, _MAKE_ID('n','a','m','e'), (void**) &pName);
		const wchar* pType = NULL;
		GetDataRaw(&store, _MAKE_ID('t','y','p','e'), (void**) &pType);
		
		uint* pOutVal = NULL;
		GetDataRaw(&store, _MAKE_ID('o','u','t',' '), (void**) &pOutVal);
		_DEBUG_ASSERT(pOutVal);

		MGParamSig sig;
		sig.pName = pName;
		sig.pType = pType;
		
		IMGVertexIn* pVIn = pGen->GetResourceMgr().CreateVertexIn(&sig);
		if(pOutputs)
			pOutputs->insert(std::pair<uint, IMGOutputResource*>(*pOutVal, &pVIn->GetOutput()));
		
		*ppVIn = pVIn;
		
		byte endHeader = 0;
		totalBytesRead += ReadData(pBuffer, offset + totalBytesRead, &endHeader);
		_DEBUG_ASSERT(endHeader == END_CHUNK_H);
	}

	return totalBytesRead;
}

uint
LoadVertexOut(IMGMaterialGen* pGen, IByteBuffer* pBuffer, uint offset, IMGVertexOut** ppVOut, InputList* pInputs)
{
	uint totalBytesRead = 0;
	uint header = 0;
	uint numBytes = ReadData(pBuffer, offset, &header);

	if(header == MGU_VOUT_CHUNK_H)
	{
		totalBytesRead += numBytes;

		DataMap store;
		totalBytesRead += LoadAllData(pBuffer, &store, offset + totalBytesRead);

		const wchar* pName = NULL;
		GetDataRaw(&store, _MAKE_ID('n','a','m','e'), (void**) &pName);
		const wchar* pType = NULL;
		GetDataRaw(&store, _MAKE_ID('t','y','p','e'), (void**) &pType);
	
		uint defVal = 0;
		uint* pBindVal = &defVal;
		uint* pAltBindVal = &defVal;
		GetDataRaw(&store, _MAKE_ID('b','i','n','d'), (void**) &pBindVal);
		GetDataRaw(&store, _MAKE_ID('a','l','t','b'), (void**) &pAltBindVal);
		_DEBUG_ASSERT(pBindVal);

		MGParamSig sig;
		sig.pName = pName;
		sig.pType = pType;

		IMGVertexOut* pVOut = pGen->GetResourceMgr().CreateVertexOut(&sig);
		if(pInputs)
			pInputs->push_back(InputLink(*pBindVal, *pAltBindVal, &pVOut->GetInput()));

		*ppVOut = pVOut;

		byte endHeader = 0;
		totalBytesRead += ReadData(pBuffer, offset + totalBytesRead, &endHeader);
		_DEBUG_ASSERT(endHeader == END_CHUNK_H);
	}

	return totalBytesRead;
}

uint
LoadFunctionOp(IMGMaterialGen* pGen, IByteBuffer* pBuffer, uint offset, IMGFunctionOp** ppOp, 
	InputList* pInputs, OutputMap* pOutputs,
	FunctionMap* pFunctions)
{
	_DEBUG_ASSERT(pFunctions);
	
	uint totalBytesRead = 0;
	uint header = 0;
	uint numBytes = ReadData(pBuffer, offset, &header);

	if(header == MGU_FUNCTION_OP_CHUNK_H)
	{
		totalBytesRead += numBytes;

		DataMap store;
		totalBytesRead += LoadAllData(pBuffer, &store, offset + totalBytesRead);

		uint* pFnVal = NULL;
		GetDataRaw(&store, _MAKE_ID('f','n','v','a'), (void**) &pFnVal);
		_DEBUG_ASSERT(pFnVal);
		
		FunctionMap::iterator itr =  pFunctions->find(*pFnVal);
		_DEBUG_ASSERT(itr != pFunctions->end());
		IMGFunction* pFunc = itr->second;
		IMGFunctionOp* pOp = pFunc->CreateFunctionOp();
		
		uint* pNumIns = NULL;
		GetDataRaw(&store, _MAKE_ID('n','i','n','s'), (void**) &pNumIns);
		_DEBUG_ASSERT(pNumIns);
		
		_LOOPi(*pNumIns)
		{
			uint defVal = 0;
			uint* pBindVal = &defVal;
			uint* pAltBindVal = &defVal;
			GetDataRaw(&store, _MAKE_ID('i','b',0x00,0x00) | i, (void**) &pBindVal);
			GetDataRaw(&store, _MAKE_ID('i','a,',0x00,0x00) | i, (void**) &pAltBindVal);
			if(pInputs)
				pInputs->push_back(InputLink(*pBindVal, *pAltBindVal, &pOp->GetInput(i)));
		}

		uint* pOutVal = NULL;
		GetDataRaw(&store, _MAKE_ID('o','u','t',' '), (void**) &pOutVal);
		_DEBUG_ASSERT(pOutVal);
		if(pOutputs)
			pOutputs->insert(std::pair<uint, IMGOutputResource*>(*pOutVal, &pOp->GetOutput()));

		*ppOp = pOp;

		byte endHeader = 0;
		totalBytesRead += ReadData(pBuffer, offset + totalBytesRead, &endHeader);
		_DEBUG_ASSERT(endHeader == END_CHUNK_H);
	}

	return totalBytesRead;
}

uint
LoadPixelShader(IMGMaterialGen* pGen, IByteBuffer* pBuffer, uint offset, IMGPixelShader** ppShader, 
				 InputList* pInputs, OutputMap* pOutputs, 
				 FunctionMap* pFunctions, PixelShaderMap* pPixelShaders)
{
	uint totalBytesRead = 0;
	uint header = 0;
	uint numBytes = ReadData(pBuffer, offset, &header);

	if(header == MGU_PS_CHUNK_H)
	{
		totalBytesRead += numBytes;

		DataMap store;
		totalBytesRead += LoadAllData(pBuffer, &store, offset + totalBytesRead);

		const wchar* pName = NULL;
		GetDataRaw(&store, _MAKE_ID('n','a','m','e'), (void**) &pName);
		_DEBUG_ASSERT(pName);
		const wchar* pMajor = NULL;
		GetDataRaw(&store, _MAKE_ID('m','a','j','r'), (void**) &pMajor);
		_DEBUG_ASSERT(pMajor);
		const wchar* pMinor = NULL;
		GetDataRaw(&store, _MAKE_ID('m','i','n','r'), (void**) &pMinor);
		_DEBUG_ASSERT(pMinor);

		uint* pNumPIns = NULL;
		GetDataRaw(&store, _MAKE_ID('n','p','i','n'), (void**) &pNumPIns);
		_DEBUG_ASSERT(pNumPIns);
		uint* pNumPOuts = NULL;
		GetDataRaw(&store, _MAKE_ID('n','p','o','u'), (void**) &pNumPOuts);
		_DEBUG_ASSERT(pNumPOuts);
		uint* pNumOps = NULL;
		GetDataRaw(&store, _MAKE_ID('n','o','p','s'), (void**) &pNumOps);
		_DEBUG_ASSERT(pNumOps);

		uint* pShaderVal = NULL;
		GetDataRaw(&store, _MAKE_ID('s','h','v','a'), (void**) &pShaderVal);
		_DEBUG_ASSERT(pShaderVal);

		IMGPixelShader* pPS = pGen->GetResourceMgr().CreatePixelShader(pName);
		pPS->SetVersion(pMajor, pMinor);

		_LOOPi(*pNumPIns)
		{
			IMGPixelIn* pPIn = NULL;
			totalBytesRead += LoadPixelIn(pGen, pBuffer, offset + totalBytesRead, &pPIn, pOutputs);
			_DEBUG_ASSERT(pPIn);
			pPS->AddPixelIn(pPIn);			
		}

		_LOOPi(*pNumPOuts)
		{
			IMGPixelOut* pPOut = NULL;
			totalBytesRead += LoadPixelOut(pGen, pBuffer, offset + totalBytesRead, &pPOut, pInputs);
			_DEBUG_ASSERT(pPOut);
			pPS->AddPixelOut(pPOut);
		}

		_LOOPi(*pNumOps)
		{
			IMGFunctionOp* pOp = NULL;
			totalBytesRead += LoadFunctionOp(pGen, pBuffer, offset + totalBytesRead, &pOp, pInputs, pOutputs, pFunctions);
			_DEBUG_ASSERT(pOp);
			pPS->AddFunctionOp(pOp);
		}

		if(pPixelShaders)
			pPixelShaders->insert(std::pair<uint, IMGPixelShader*>(*pShaderVal, pPS));

		*ppShader = pPS;

		byte endHeader = 0;
		totalBytesRead += ReadData(pBuffer, offset + totalBytesRead, &endHeader);
		_DEBUG_ASSERT(endHeader == END_CHUNK_H);
	}

	return totalBytesRead;
}

uint
LoadPixelIn(IMGMaterialGen* pGen, IByteBuffer* pBuffer, uint offset, IMGPixelIn** ppPIn, OutputMap* pOutputs)
{
	uint totalBytesRead = 0;
	uint header = 0;
	uint numBytes = ReadData(pBuffer, offset, &header);

	if(header == MGU_PIN_CHUNK_H)
	{
		totalBytesRead += numBytes;

		DataMap store;
		totalBytesRead += LoadAllData(pBuffer, &store, offset + totalBytesRead);

		const wchar* pName = NULL;
		GetDataRaw(&store, _MAKE_ID('n','a','m','e'), (void**) &pName);
		const wchar* pType = NULL;
		GetDataRaw(&store, _MAKE_ID('t','y','p','e'), (void**) &pType);

		uint* pOutVal = NULL;
		GetDataRaw(&store, _MAKE_ID('o','u','t',' '), (void**) &pOutVal);
		_DEBUG_ASSERT(pOutVal);

		MGParamSig sig;
		sig.pName = pName;
		sig.pType = pType;

		IMGPixelIn* pPIn = pGen->GetResourceMgr().CreatePixelIn(&sig);
		if(pOutputs)
			pOutputs->insert(std::pair<uint, IMGOutputResource*>(*pOutVal, &pPIn->GetOutput()));

		*ppPIn = pPIn;

		byte endHeader = 0;
		totalBytesRead += ReadData(pBuffer, offset + totalBytesRead, &endHeader);
		_DEBUG_ASSERT(endHeader == END_CHUNK_H);
	}

	return totalBytesRead;
}

uint
LoadPixelOut(IMGMaterialGen* pGen, IByteBuffer* pBuffer, uint offset, IMGPixelOut** ppPOut, InputList* pInputs)
{
	uint totalBytesRead = 0;
	uint header = 0;
	uint numBytes = ReadData(pBuffer, offset, &header);

	if(header == MGU_POUT_CHUNK_H)
	{
		totalBytesRead += numBytes;

		DataMap store;
		totalBytesRead += LoadAllData(pBuffer, &store, offset + totalBytesRead);

		const wchar* pName = NULL;
		GetDataRaw(&store, _MAKE_ID('n','a','m','e'), (void**) &pName);
		const wchar* pType = NULL;
		GetDataRaw(&store, _MAKE_ID('t','y','p','e'), (void**) &pType);

		uint defVal = 0;
		uint* pAltBindVal = &defVal;
		uint* pBindVal = &defVal;
		GetDataRaw(&store, _MAKE_ID('b','i','n','d'), (void**) &pBindVal);
		GetDataRaw(&store, _MAKE_ID('a','l','t','b'), (void**) &pAltBindVal);
		_DEBUG_ASSERT(pBindVal);

		MGParamSig sig;
		sig.pName = pName;
		sig.pType = pType;

		IMGPixelOut* pPOut = pGen->GetResourceMgr().CreatePixelOut(&sig);
		if(pInputs)
			pInputs->push_back(InputLink(*pBindVal, *pAltBindVal, &pPOut->GetInput()));

		*ppPOut = pPOut;

		byte endHeader = 0;
		totalBytesRead += ReadData(pBuffer, offset + totalBytesRead, &endHeader);
		_DEBUG_ASSERT(endHeader == END_CHUNK_H);
	}

	return totalBytesRead;
}

uint
LoadTechnique(IMGMaterialGen* pGen, IByteBuffer* pBuffer, uint offset, IMGTechnique** ppTech, InputList* pInputs,
	PixelShaderMap* pPixelShaders, VertexShaderMap* pVertexShaders)
{
	uint totalBytesRead = 0;
	uint header = 0;
	uint numBytes = ReadData(pBuffer, offset, &header);

	if(header == MGU_TECHNIQUE_CHUNK_H)
	{
		totalBytesRead += numBytes;

		DataMap store;
		totalBytesRead += LoadAllData(pBuffer, &store, offset + totalBytesRead);

		const wchar* pName = NULL;
		GetDataRaw(&store, _MAKE_ID('n','a','m','e'), (void**) &pName);
		_DEBUG_ASSERT(pName);
		uint* pNumPasses = NULL;
		GetDataRaw(&store, _MAKE_ID('n','p','a','s'), (void**) &pNumPasses);
		_DEBUG_ASSERT(pNumPasses);
		
		IMGTechnique* pTech = pGen->GetResourceMgr().CreateTechnique(pName);
		_LOOPi(*pNumPasses)
		{
			IMGPass* pPass = NULL;
			totalBytesRead += LoadPass(pGen, pBuffer, offset + totalBytesRead, &pPass, pInputs, pPixelShaders, pVertexShaders);
			_DEBUG_ASSERT(pPass);
			pTech->AddPass(pPass);
		}

		*ppTech = pTech;

		byte endHeader = 0;
		totalBytesRead += ReadData(pBuffer, offset + totalBytesRead, &endHeader);
		_DEBUG_ASSERT(endHeader == END_CHUNK_H);
	}

	return totalBytesRead;
}

uint
LoadPass(IMGMaterialGen* pGen, IByteBuffer* pBuffer, uint offset, IMGPass** ppPass, InputList* pInputs, 
	PixelShaderMap* pPixelShaders, VertexShaderMap* pVertexShaders)
{
	uint totalBytesRead = 0;
	uint header = 0;
	uint numBytes = ReadData(pBuffer, offset, &header);

	if(header == MGU_PASS_CHUNK_H)
	{
		totalBytesRead += numBytes;

		DataMap store;
		totalBytesRead += LoadAllData(pBuffer, &store, offset + totalBytesRead);

		uint *pVsVal = NULL;
		GetDataRaw(&store, _MAKE_ID('v','s','v','a'), (void**) &pVsVal);
		_DEBUG_ASSERT(pVsVal);
		uint* pPsVal = NULL;
		GetDataRaw(&store, _MAKE_ID('p','s','v','a'), (void**) &pPsVal);
		_DEBUG_ASSERT(pPsVal);
		uint* pNumStates = NULL;
		GetDataRaw(&store, _MAKE_ID('n','s','t','a'), (void**) &pNumStates);
		_DEBUG_ASSERT(pNumStates);
		const wchar* pName = NULL;
		GetDataRaw(&store, _MAKE_ID('n','a','m','e'), (void**) &pName);
		_DEBUG_ASSERT(pName);
		
		IMGPass* pPass = pGen->GetResourceMgr().CreatePass(pName);
		VertexShaderMap::iterator vitr = pVertexShaders->find(*pVsVal);
		_DEBUG_ASSERT(vitr != pVertexShaders->end());
		IMGVertexShader* pVS = vitr->second;
		IMGVertexShaderOp* pVSOp = pVS->CreateShaderOp();
		pPass->SetVertexShaderOp(pVSOp);
		PixelShaderMap::iterator pitr = pPixelShaders->find(*pPsVal);
		_DEBUG_ASSERT(pitr != pPixelShaders->end());
		IMGPixelShader* pPS = pitr->second;
		IMGPixelShaderOp* pPSOp = pPS->CreateShaderOp();
		pPass->SetPixelShaderOp(pPSOp);
		
		_LOOPi(*pNumStates)
		{
			IMGRenderState* pState = NULL;
			totalBytesRead += LoadRenderState(pGen, pBuffer, offset + totalBytesRead, &pState, pInputs);
			_DEBUG_ASSERT(pState);
			pPass->AddRenderState(pState);
		}

		*ppPass = pPass;

		byte endHeader = 0;
		totalBytesRead += ReadData(pBuffer, offset + totalBytesRead, &endHeader);
		_DEBUG_ASSERT(endHeader == END_CHUNK_H);
	}

	return totalBytesRead;
}

uint
LoadRenderState(IMGMaterialGen* pGen, IByteBuffer* pBuffer, uint offset, IMGRenderState** ppState, InputList* pInputs)
{
	uint totalBytesRead = 0;
	uint header = 0;
	uint numBytes = ReadData(pBuffer, offset, &header);

	if(header == MGU_RENDERSTATE_CHUNK_H)
	{
		totalBytesRead += numBytes;

		DataMap store;
		totalBytesRead += LoadAllData(pBuffer, &store, offset + totalBytesRead);

		const wchar* pName = NULL;
		GetDataRaw(&store, _MAKE_ID('n','a','m','e'), (void**) &pName);
		_DEBUG_ASSERT(pName);
		const wchar* pType = NULL;
		GetDataRaw(&store,  _MAKE_ID('t','y','p','e'), (void**) &pType);
		const wchar* pValue = NULL;
		GetDataRaw(&store, _MAKE_ID('v','a','l','u'), (void**) &pValue);

		uint* pNumInputs = NULL;
		GetDataRaw(&store, _MAKE_ID('n','i','n','s'), (void**) &pNumInputs);
		_DEBUG_ASSERT(pNumInputs);
		
		MGInputSig inputSigs[64];
		_LOOPi(*pNumInputs)
		{
			const wchar* pInName = NULL;
			GetDataRaw(&store, _MAKE_ID('i','n',0x00,0x00) | i, (void**) &pInName);
			const wchar* pInType = NULL;
			GetDataRaw(&store, _MAKE_ID('i','t',0x00,0x00) | i, (void**) &pInType);
			const wchar* pInValue = NULL;
			GetDataRaw(&store, _MAKE_ID('i','v',0x00,0x00) | i, (void**) &pInValue);	
			
			inputSigs[i].pName = pInName;
			inputSigs[i].pType = pInType;
			inputSigs[i].pDefaultVal = pInValue;
		}
		
		MGParamSig sig;
		sig.pName = pName;
		sig.pType = pType;
		
		IMGRenderState* pState = pGen->GetResourceMgr().CreateRenderState(&sig, inputSigs, *pNumInputs);

		if(pValue)
			pState->SetValue(pValue);

		_LOOPi(*pNumInputs)
		{
			uint defVal = 0;
			uint* pAltBindVal = &defVal;
			uint* pBindVal = &defVal;
			GetDataRaw(&store, _MAKE_ID('i','b',0x00,0x00) | i, (void**) &pBindVal);
			GetDataRaw(&store, _MAKE_ID('i','a',0x00,0x00) | i, (void**) &pAltBindVal);
			if(pInputs)
				pInputs->push_back(InputLink(*pBindVal, *pAltBindVal, &pState->GetInput(i)));
		}
		
		*ppState = pState;

		byte endHeader = 0;
		totalBytesRead += ReadData(pBuffer, offset + totalBytesRead, &endHeader);
		_DEBUG_ASSERT(endHeader == END_CHUNK_H);
	}

	return totalBytesRead;
}
_NAMESPACE_END