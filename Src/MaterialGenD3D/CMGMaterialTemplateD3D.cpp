//==============================================================================
//
//		CMGMaterialTemplateD3D.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/8/2008
//
//==============================================================================

#include "MaterialGenD3D.h"

_NAMESPACE_BEGIN

#define _NEWLINE _W("\r\n")

void 
CMGMaterialTemplateD3D::DoCompile(IByteBuffer* pBuffer)
{
	uint numLocals = (uint) m_Locals.size();
	wstring out = _W("//=============================== Locals ==========================================") _NEWLINE _NEWLINE;
	_LOOPi(numLocals)
	{
		WriteLocal(out, (CMGLocal*)(IMGLocal*)m_Locals[i]);
	}
	
	out = out + _NEWLINE _NEWLINE _W("//=============================== Globals ==========================================") _NEWLINE _NEWLINE;
	
	uint numGlobals = (uint) m_ResolvedGlobals.size();
	_LOOPi(numGlobals)
	{
		WriteGlobal(out, (CMGGlobal*)(IMGGlobal*)m_ResolvedGlobals[i]);
	}

	out = out + _NEWLINE _NEWLINE _W("//=============================== Functions ==========================================") _NEWLINE _NEWLINE;
	
	uint numFunctions = (uint) m_ResolvedFunctions.size();
	_LOOPi(numFunctions)
	{
		WriteFunction(out, (CMGFunction*)(IMGFunction*)m_ResolvedFunctions[i]);
		out = out + _NEWLINE;
	}

	out = out + _NEWLINE _NEWLINE _W("//=============================== Vertex Shaders ==========================================") _NEWLINE _NEWLINE;
	
	uint numVertexShaders = (uint) m_ResolvedVertexShaders.size();
	_LOOPi(numVertexShaders)
	{
		MGVertexShaderHelper* pShader = &m_ResolvedVertexShaders[i];
		WriteVertexShader(out, pShader);
	}

	out = out + _NEWLINE _NEWLINE _W("//=============================== Pixel Shaders ==========================================") _NEWLINE _NEWLINE;
	
	uint numPixelShaders = (uint) m_ResolvedPixelShaders.size();
	_LOOPi(numPixelShaders)
	{
		MGPixelShaderHelper* pShader = &m_ResolvedPixelShaders[i];
		WritePixelShader(out, pShader);
	}

	out = out + _NEWLINE _NEWLINE _W("//=============================== Techniques ==========================================") _NEWLINE _NEWLINE;
	
	uint numTechs = (uint) m_ResolvedTechniques.size();
	_LOOPi(numTechs)
	{
		MGTechniqueHelper& tech = m_ResolvedTechniques[i];
		_DEBUG_ASSERT(tech.pTech);
		WriteTechnique(out, &tech);
		out = out + _NEWLINE;
	}

	if(pBuffer->GetBufferSize() - pBuffer->GetDataLength() < (uint) (out.size() + 1))
		pBuffer->ResizeBuffer(pBuffer->GetDataLength() + (uint) out.size() + 1);
	
	wcstombs((char*) pBuffer->GetData(), out.c_str(), out.size());
	pBuffer->SetDataLength(pBuffer->GetDataLength() + out.size() * sizeof(char));
	AppendData(pBuffer, (char)0);
	//AppendData(pBuffer, out.c_str(), out.size() * sizeof(wchar));
}

void
CMGMaterialTemplateD3D::WriteLocal(wstring& out, CMGLocal* pLocal)
{
	if(pLocal->GetPreamble())
		out = out + pLocal->GetPreamble() + _W(" ");
	out = out + pLocal->GetType() + _W(" ") + pLocal->GetName();
	if(pLocal->GetPostamble())
		out = out + _NEWLINE + pLocal->GetPostamble();
	if(pLocal->GetValue())
		out = out + _W(" = ") + pLocal->GetValue();

	out = out + _W(";") _NEWLINE;
}

void
CMGMaterialTemplateD3D::WriteGlobal(wstring& out, CMGGlobal* pGlobal)
{
	const MGParamSigStore& sig = pGlobal->GetSig();
	out = out + sig.type + _W(" ");
	out = out + sig.name;
	
	if(pGlobal->GetSemantic() != 0xFFFFFFFF)
	{
		out = out + _W(" <int semantic = ");
		out = out + ToStringAuto(_W("%d"), pGlobal->GetSemantic()) + _W(";");
		//if(pGlobal->GetUsage())
		out = out + ToStringAuto(_W(" int usage = %d"), pGlobal->GetUsage()) + _W(";");

		if(pGlobal->GetIsAlwaysUsed())
			out = out + _W(" bool always_used = true;");
		
		uint numDesc = pGlobal->GetNumOfDesc();
		_LOOPi(numDesc)
		{
			const MGInputSigStore& in = pGlobal->GetDescription(i);
			out = out + _W(" ") + in.type;
			out = out + _W(" ") + in.name;
			out = out + _W(" = ") + in.defaultVal + _W(";");		
		}
		
		out = out + _W(">");
	}
	
	if(pGlobal->GetValue())
	{
		out = out + _W(" = ");
		wstring value = pGlobal->GetValue();
		
		// Replace all ocurrances of the input with the output
		uint numIns = pGlobal->GetNumOfInputs();
		_LOOPi(numIns)
		{
			CMGInputResource* pIn = (CMGInputResource*) (IMGInputResource*) (&pGlobal->GetInput(i));
			wstring toSwap = pIn->GetName();
			wstring replacement;
			CMGOutputResource* pBinding = pIn->GetCBinding();
			CMGOutputResource* pAltBinding = (CMGOutputResource*) pIn->GetAltBinding();
			
			if(pBinding && pBinding->IsResolved())
				replacement = replacement + _W("<") + pBinding->GetName() + _W(">");
			else if(pAltBinding && pAltBinding->IsResolved())
				replacement = replacement + _W("<") + pAltBinding->GetName() + _W(">");
			else
				replacement = pIn->GetDefaultVal();
				
			int pos = (int) value.find(toSwap);
			while(pos != wstring::npos)
			{
				value.replace(pos, toSwap.size(), replacement);
				pos = (int) value.find(toSwap, pos + replacement.size());
			}			
		}
		
		out = out + value;
	}
	
	out = out + _W(";") _NEWLINE;
}

void 
CMGMaterialTemplateD3D::WriteFunction(wstring& out, CMGFunction* pFunc)
{	
	MGParamSigStore* pSig = pFunc->GetParamSig(); 
	out = out + pSig->type;
	out = out + _W(" ") + pSig->name;
	
	out = out + _W("(");
	uint numInputs = pFunc->GetNumOfInputs();
	_LOOPi(numInputs)
	{
		if(i > 0)
			out = out + _W(", ");
			
		MGInputSigStore* pIn = pFunc->GetInputSig(i);
		out = out + pIn->type + _W(" ");
		out = out + pIn->name;
	}
	
	out = out + _W(")") _NEWLINE;
	out = out + _W("{") _NEWLINE _W("\t");
	
	wstring func = pFunc->GetValue();
	wstring toSwap = _NEWLINE;
	wstring replacement = _NEWLINE _W("\t");
	int pos = (int) func.find(toSwap);
	while(pos != wstring::npos)
	{
		func.replace(pos, toSwap.size(), replacement);
		pos = (int) func.find(toSwap, pos + replacement.size());
	}	
	
	out = out + func + _NEWLINE;
	out = out + _W("}") _NEWLINE;
}

void 
CMGMaterialTemplateD3D::WriteVertexShader(wstring& out, MGVertexShaderHelper* pShader)
{
	// Write vertex in declaration
	wstring vsIn;
	vsIn = _W("struct VS_IN");
	vsIn = vsIn + ((CMGVertexShader*) pShader->pShader)->GetName() + _NEWLINE;
	vsIn = vsIn + _W("{") _NEWLINE;

	uint numIn = (uint) pShader->vertexIn.size();
	_LOOPi(numIn)
	{
		CMGVertexIn* pIn = ((CMGVertexIn*)pShader->vertexIn[i]);
		
		wstring inType = pIn->GetType(); //((CMGOutputResource*)&pIn->GetOutput())->GetType();
		wstring inName = pIn->GetName(); //((CMGOutputResource*)&pIn->GetOutput())->GetName();
		
		vsIn = vsIn + _W("\t") + inType + _W(" ");
		vsIn = vsIn + ToStringAuto(_W("vsinput_%d"), i) + _W(" : ");
		vsIn = vsIn + inName + _W(";") _NEWLINE;
	}
	
	vsIn = vsIn + _W("};") _NEWLINE _NEWLINE;
	out = out + vsIn;
	
	// Write vertex out declaration
	wstring vsOut;
	vsOut = _W("struct VS_OUT");
	vsOut = vsOut + ((CMGVertexShader*) pShader->pShader)->GetName() + _NEWLINE;
	vsOut = vsOut + _W("{") _NEWLINE;
	
	uint numOut = (uint) pShader->vertexOut.size();
	_LOOPi(numOut)
	{
		CMGVertexOut* pOut = ((CMGVertexOut*)pShader->vertexOut[i]);
		if(pOut->IsResolved())
		{
			wstring outType = ((CMGInputResource*)&pOut->GetInput())->GetType();
			wstring outName = ((CMGInputResource*)&pOut->GetInput())->GetName();

			vsOut = vsOut + _W("\t") + outType + _W(" ");
			vsOut = vsOut + ToStringAuto(_W("vsoutput_%d"), i) + _W(" : ");
			vsOut = vsOut + outName + _W(";") _NEWLINE;
		}
	}

	vsOut = vsOut + _W("};") _NEWLINE _NEWLINE;
	out = out + vsOut;
	
	// Write shader
	// Write declaration
	wstring shader;
	shader = _W("VS_OUT");
	shader = shader + ((CMGVertexShader*) pShader->pShader)->GetName() + _W(" ");
	shader = shader + ((CMGVertexShader*) pShader->pShader)->GetName();
	shader = shader + _W("(") + _W("VS_IN") + ((CMGVertexShader*) pShader->pShader)->GetName() + _W(" input)") _NEWLINE;
	shader = shader + _W("{") _NEWLINE;
	shader = shader + _W("\tVS_OUT") + ((CMGVertexShader*) pShader->pShader)->GetName() + _W(" output;") _NEWLINE _NEWLINE;
	
	// Write function ops
	uint numOps = (uint) pShader->functionOps.size();
	_LOOPi(numOps)
	{
		CMGFunctionOp* pOp = (CMGFunctionOp*) pShader->functionOps[i];
		WriteFunctionOp(shader, pOp);
		shader = shader + _NEWLINE;		
	}
	
	shader = shader + _NEWLINE;
	
	// Write output
	_LOOPi(numOut)
	{
		CMGVertexOut* pOut = ((CMGVertexOut*)pShader->vertexOut[i]);

		CMGInputResource* pIn = ((CMGInputResource*)&pOut->GetInput());
		wstring outName = ToStringAuto(_W("vsoutput_%d"), i);//pIn->GetName();
		shader = shader + _W("\toutput.") + outName;
		CMGOutputResource* pBinded = pIn->GetCBinding();
		CMGOutputResource* pAltBinding = (CMGOutputResource*) pIn->GetAltBinding();
		if(pBinded && pBinded->IsResolved())
			shader = shader + _W(" = ") + pBinded->GetName() + _W(";") _NEWLINE;
		else if(pAltBinding && pAltBinding->IsResolved())
			shader = shader + _W(" = ") + pAltBinding->GetName() + _W(";") _NEWLINE;
		else
		{
			if(pIn->GetDefaultVal())
				shader = shader + _W(" = ") + pIn->GetDefaultVal() + _W(";") _NEWLINE;
		}
	}
	
	shader = shader + _W("\treturn output;") _NEWLINE;
	
	// End write shader
	shader = shader + _W("}") _NEWLINE;

	// Replace all vertex in strings in shader
	_LOOPi(numIn)
	{
		CMGVertexIn* pIn = ((CMGVertexIn*)pShader->vertexIn[i]);

		wstring inName = ((CMGOutputResource*)&pIn->GetOutput())->GetName();
		wstring replacement;
		replacement = replacement + _W("input.") + ToStringAuto(_W("vsinput_%d"), i);
		
		int pos = (int) shader.find(inName);
		while(pos != wstring::npos)
		{
			shader.replace(pos, inName.size(), replacement);
			pos = (int) shader.find(inName, pos + replacement.size());
		}
	}
	
	out = out + shader;
}

void 
CMGMaterialTemplateD3D::WritePixelShader(wstring& out, MGPixelShaderHelper* pShader)
{
	// Write pixel in declaration
	wstring psIn;
	psIn = _W("struct PS_IN");
	psIn = psIn + ((CMGPixelShader*) pShader->pShader)->GetName() + _NEWLINE;
	psIn = psIn + _W("{") _NEWLINE;

	uint numIn = (uint) pShader->pixelIn.size();
	_LOOPi(numIn)
	{
		CMGPixelIn* pIn = ((CMGPixelIn*)pShader->pixelIn[i]);

		wstring inType = pIn->GetType(); //(CMGOutputResource*)&pIn->GetOutput())->GetType();
		wstring inName = pIn->GetName(); //((CMGOutputResource*)&pIn->GetOutput())->GetName();

		psIn = psIn + _W("\t") + inType + _W(" ");
		psIn = psIn + ToStringAuto(_W("psinput_%d"), i) + _W(" : ");
		psIn = psIn  + inName + _W(";") _NEWLINE;
	}

	psIn = psIn + _W("};") _NEWLINE _NEWLINE;
	out = out + psIn;

	// Write pixel out declaration
	wstring psOut;
	psOut = _W("struct PS_OUT");
	psOut = psOut + ((CMGPixelShader*) pShader->pShader)->GetName() + _NEWLINE;
	psOut = psOut + _W("{") _NEWLINE;

	uint numOut = (uint) pShader->pixelOut.size();
	_LOOPi(numOut)
	{
		CMGPixelOut* pOut = ((CMGPixelOut*)pShader->pixelOut[i]);

		if(pOut->IsResolved())
		{
			wstring outType = ((CMGInputResource*)&pOut->GetInput())->GetType();
			wstring outName = ((CMGInputResource*)&pOut->GetInput())->GetName();

			psOut = psOut + _W("\t") + outType + _W(" ");
			psOut = psOut + ToStringAuto(_W("psoutput_%d"), i) + _W(" : ");
			psOut = psOut + outName + _W(";") _NEWLINE;
		}
	}

	psOut = psOut + _W("};") _NEWLINE _NEWLINE;
	out = out + psOut;

	// Write shader
	// Write declaration
	wstring shader;
	shader = _W("PS_OUT");
	shader = shader + ((CMGPixelShader*) pShader->pShader)->GetName() + _W(" ");
	shader = shader + ((CMGPixelShader*) pShader->pShader)->GetName();
	shader = shader + _W("(") + _W("PS_IN") + ((CMGPixelShader*) pShader->pShader)->GetName() + _W(" input)") _NEWLINE;
	shader = shader + _W("{") _NEWLINE;
	shader = shader + _W("\tPS_OUT") + ((CMGPixelShader*) pShader->pShader)->GetName() + _W(" output;") _NEWLINE _NEWLINE;

	// Write function ops
	uint numOps = (uint) pShader->functionOps.size();
	_LOOPi(numOps)
	{
		CMGFunctionOp* pOp = (CMGFunctionOp*) pShader->functionOps[i];
		WriteFunctionOp(shader, pOp);
		shader = shader + _NEWLINE;		
	}

	shader = shader + _NEWLINE;

	// Write output
	_LOOPi(numOut)
	{
		CMGPixelOut* pOut = ((CMGPixelOut*)pShader->pixelOut[i]);

		CMGInputResource* pIn = ((CMGInputResource*)&pOut->GetInput());
		wstring outName = ToStringAuto(_W("psoutput_%d"), i);//pIn->GetName();
		shader = shader + _W("\toutput.") + outName;
		CMGOutputResource* pBinded = pIn->GetCBinding();
		CMGOutputResource* pAltBinding = (CMGOutputResource*) pIn->GetAltBinding();
		if(pBinded && pBinded->IsResolved())
			shader = shader + _W(" = ") + pBinded->GetName() + _W(";") _NEWLINE;
		else if(pAltBinding && pAltBinding->IsResolved())
			shader = shader + _W(" = ") + pAltBinding->GetName() + _W(";") _NEWLINE;
		else
		{
			if(pIn->GetDefaultVal())
				shader = shader + _W(" = ") + pIn->GetDefaultVal() + _W(";") _NEWLINE;
		}
	}

	shader = shader + _W("\treturn output;") _NEWLINE;

	// End write shader
	shader = shader + _W("}") _NEWLINE;

	// Replace all pixel input in strings in shader
	_LOOPi(numIn)
	{
		CMGPixelIn* pIn = ((CMGPixelIn*)pShader->pixelIn[i]);

		wstring inName = ((CMGOutputResource*)&pIn->GetOutput())->GetName();
		wstring replacement; 
		replacement = replacement + _W("input.") + ToStringAuto(_W("psinput_%d"), i);
		
		int pos = (int) shader.find(inName);
		while(pos != wstring::npos)
		{
			shader.replace(pos, inName.size(), replacement);
			pos = (int) shader.find(inName, pos + replacement.size());
		}
	}

	out = out + shader;
}

void 
CMGMaterialTemplateD3D::WriteFunctionOp(wstring& out, CMGFunctionOp* pOp)
{
	out = out + _W("\t") + pOp->GetOutput().GetType() + _W(" ");
	out = out + pOp->GetOutput().GetName() + _W(" = ");
	MGParamSigStore* pFunction = pOp->GetCFunction()->GetParamSig();
	out = out + pFunction->name + _W("(");
	
	uint numIns = pOp->GetNumOfInputs();
	_LOOPi(numIns)
	{
		if(i > 0)
			out = out + _W(", ");
			
		CMGInputResource* pIn = (CMGInputResource*) &pOp->GetInput(i);
		CMGOutputResource* pBinded = pIn->GetCBinding();
		CMGOutputResource* pAltBinding = (CMGOutputResource*) pIn->GetAltBinding();
		if(pBinded && pBinded->IsResolved())
			out = out + pBinded->GetName();
		else if(pAltBinding && pAltBinding->IsResolved())
			out = out + pAltBinding->GetName();
		else
		{
			_DEBUG_ASSERT(pIn->GetDefaultVal());
			out = out + pIn->GetDefaultVal();
		}
	}
	
	out = out + _W(");") _NEWLINE;
}

void 
CMGMaterialTemplateD3D::WriteRenderState(wstring& out, CMGRenderState* pState)
{
	MGParamSigStore& sig = pState->GetParamSig();
	
	wstring value;
	value = value + pState->GetValue();
	
	// Replace all ocurrances of the input with the output
	uint numIns = pState->GetNumOfInputs();
	_LOOPi(numIns)
	{
		CMGInputResource* pIn = (CMGInputResource*) (IMGInputResource*) (&pState->GetInput(i));
		wstring toSwap = pIn->GetName();
		wstring replacement;
		CMGOutputResource* pBinding = pIn->GetCBinding();
		CMGOutputResource* pAltBinding = (CMGOutputResource*) pIn->GetAltBinding();
		if(pBinding && pBinding->IsResolved())
			replacement = replacement + _W("<") + pBinding->GetName() + _W(">");
		else if(pAltBinding && pAltBinding->IsResolved())
			replacement = replacement + _W("<") + pAltBinding->GetName() + _W(">");
		else
			replacement = pIn->GetDefaultVal();

		int pos = (int) value.find(toSwap);
		while(pos != wstring::npos)
		{
			value.replace(pos, toSwap.size(), replacement);
			pos = (int) value.find(toSwap, pos + replacement.size());
		}			
	}
	
	out = out + _W("\t\t") + sig.name + _W(" = ") + value + _W(";") _NEWLINE;		
}

void
CMGMaterialTemplateD3D::WritePass(wstring& out, MGPassHelper* pPass)
{
	out = out + _W("\tpass ") + ((CMGPass*)pPass->pPass)->GetName() + _NEWLINE;
	out = out + _W("\t{") _NEWLINE;
	
	uint numStates = (uint) pPass->states.size();
	_LOOPi(numStates)
	{
		CMGRenderState* pState = (CMGRenderState*) pPass->states[i];
		WriteRenderState(out, pState);
	}
	
	out = out + _W("\t\tVertexShader = compile vs_") + ((CMGPass*)pPass->pPass)->GetVertexShaderOp()->GetShader()->GetVersionMajor()
		+ _W("_") + ((CMGPass*)pPass->pPass)->GetVertexShaderOp()->GetShader()->GetVersionMinor() + _W(" ") 
		+ ((CMGPass*)pPass->pPass)->GetVertexShaderOp()->GetShader()->GetName() + _W("();") _NEWLINE;
	out = out + _W("\t\tPixelShader = compile ps_") + ((CMGPass*)pPass->pPass)->GetPixelShaderOp()->GetShader()->GetVersionMajor()
		+ _W("_") + ((CMGPass*)pPass->pPass)->GetPixelShaderOp()->GetShader()->GetVersionMinor() + _W(" ") 
		+ ((CMGPass*)pPass->pPass)->GetPixelShaderOp()->GetShader()->GetName() + _W("();") _NEWLINE;

	out = out + _W("\t}") _NEWLINE;
}

void 
CMGMaterialTemplateD3D::WriteTechnique(wstring& out, MGTechniqueHelper* pTech)
{
	out = out + _W("technique ") + ((CMGTechnique*)pTech->pTech)->GetName() + _NEWLINE;
	out = out + _W("{") _NEWLINE;
	
	uint numPasses = (uint) pTech->passes.size();
	_LOOPi(numPasses)
	{	
		MGPassHelper* pPass = &pTech->passes[i];
		WritePass(out, pPass);
		out = out + _NEWLINE;
	}
	
	out = out + _W("};") _NEWLINE;
}

_NAMESPACE_END