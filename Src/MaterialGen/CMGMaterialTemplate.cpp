//==============================================================================
//
//		CMGMaterialTemplate.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/6/2008
//
//==============================================================================

#include "MaterialGenFramework.h"

_NAMESPACE_BEGIN

void 
CMGMaterialTemplate::ResolvedInputResource(CMGInputResource* pIn)
{

}

void 
CMGMaterialTemplate::ResolvedFunctionOp(CMGFunctionOp* pOp)
{
	if(m_pCurrentVS)
	{
		m_pCurrentVS->functionOps.push_back(pOp);
	}	
	else
	{
		m_pCurrentPS->functionOps.push_back(pOp);
	}
}

void 
CMGMaterialTemplate::ResolvedGlobal(CMGGlobal* pGlobal)
{
	m_ResolvedGlobals.push_back(pGlobal);
}

void 
CMGMaterialTemplate::ResolvedOutputResource(CMGOutputResource* pOut)
{

}

void 
CMGMaterialTemplate::ResolvedRenderState(CMGRenderState *pState)
{
	_DEBUG_ASSERT(m_pCurrentPass);
	m_pCurrentPass->states.push_back(pState);
}

void 
CMGMaterialTemplate::BeginPass(CMGPass* pPass)
{
	_DEBUG_ASSERT(m_pCurrentTech);
	m_pCurrentTech->passes.push_back(MGPassHelper());
	m_pCurrentPass = &m_pCurrentTech->passes.back();
	m_pCurrentPass->pPass = NULL;
}

void 
CMGMaterialTemplate::ResolvedPass(CMGPass* pPass)
{
	_DEBUG_ASSERT(m_pCurrentPass);
	m_pCurrentPass->pPass = pPass;
}

void
CMGMaterialTemplate::EndPass(CMGPass* pPass)
{
	_DEBUG_ASSERT(m_pCurrentPass);
	if(!m_pCurrentPass->pPass)
	{
		m_pCurrentTech->passes.pop_back();
	}

	m_pCurrentPass = NULL;
}

void 
CMGMaterialTemplate::ResolvedPixelIn(CMGPixelIn* pIn)
{
	_DEBUG_ASSERT(m_pCurrentPS);
	m_pCurrentPS->pixelIn.push_back(pIn);
}

void 
CMGMaterialTemplate::ResolvedPixelOut(CMGPixelOut* pOut)
{
	_DEBUG_ASSERT(m_pCurrentPS);
	m_pCurrentPS->pixelOut.push_back(pOut);
}

void
CMGMaterialTemplate::BeginPixelShader(CMGPixelShader* pShader)
{
	_DEBUG_ASSERT(!m_pCurrentVS);
	_DEBUG_ASSERT(m_pCurrentPass);
	//m_pCurrentPS = &m_pCurrentPass->ps;
	m_ResolvedPixelShaders.push_back( MGPixelShaderHelper() );
	m_pCurrentPS = &m_ResolvedPixelShaders.back();			
}

void 
CMGMaterialTemplate::ResolvedPixelShader(CMGPixelShader* pShader)
{
	_DEBUG_ASSERT(m_pCurrentPS);
	m_pCurrentPS->pShader = pShader;
}

void 
CMGMaterialTemplate::EndPixelShader(CMGPixelShader* pShader)
{
	if(!m_pCurrentPS->pShader)
		m_ResolvedPixelShaders.pop_back();
	m_pCurrentPS = NULL;
}

void 
CMGMaterialTemplate::BeginVertexShader(CMGVertexShader* pShader)
{
	_DEBUG_ASSERT(!m_pCurrentPS);
	_DEBUG_ASSERT(m_pCurrentPass);
	m_ResolvedVertexShaders.push_back( MGVertexShaderHelper() );
	m_pCurrentVS = &m_ResolvedVertexShaders.back();	
}

void 
CMGMaterialTemplate::ResolvedVertexShader(CMGVertexShader* pShader)
{
	m_pCurrentVS->pShader = pShader;	
}

void 
CMGMaterialTemplate::EndVertexShader(CMGVertexShader* pShader)
{
	if(!m_pCurrentVS->pShader)
		m_ResolvedVertexShaders.pop_back();
	m_pCurrentVS = NULL;
}

void 
CMGMaterialTemplate::ResolvedVertexIn(CMGVertexIn* pIn)
{
	_DEBUG_ASSERT(m_pCurrentVS);
	m_pCurrentVS->vertexIn.push_back(pIn);
}

void 
CMGMaterialTemplate::ResolvedVertexOut(CMGVertexOut* pOut)
{
	_DEBUG_ASSERT(m_pCurrentVS);
	m_pCurrentVS->vertexOut.push_back(pOut);
}

void 
CMGMaterialTemplate::BeginTechnique(CMGTechnique* pTech)
{
	_DEBUG_ASSERT(!m_pCurrentTech);
	m_ResolvedTechniques.push_back(MGTechniqueHelper());
	m_pCurrentTech = &m_ResolvedTechniques.back();
}

void 
CMGMaterialTemplate::ResolvedTechnique(CMGTechnique* pTech)
{
	m_pCurrentTech->pTech = pTech;
}

void 
CMGMaterialTemplate::EndTechnique(CMGTechnique* pTech)
{
	if(!m_pCurrentTech->pTech)
		m_ResolvedTechniques.pop_back();
	
	m_pCurrentTech = NULL;
}

void 
CMGMaterialTemplate::ResolvedFunction(CMGFunction* pFunc)
{
	m_ResolvedFunctions.push_back(pFunc);
}

void 
CMGMaterialTemplate::AddGlobal(IMGGlobal* pGlobal)
{
	m_Globals.push_back(pGlobal);
	((CMGGlobal*)pGlobal)->SetIndex((uint)m_Globals.size()-1);
}

void 
CMGMaterialTemplate::AddTechnique(IMGTechnique* pTech)
{
	m_Techs.push_back(pTech);
}

void 
CMGMaterialTemplate::AddLocal(IMGLocal* pLocal)
{
	m_Locals.push_back(pLocal);
}

void 
CMGMaterialTemplate::AddFunction(IMGFunction* pFunction)
{
	m_Functions.push_back(pFunction);
}

void 
CMGMaterialTemplate::AddVertexShader(IMGVertexShader* pShader)
{
	m_VS.push_back(pShader);
}

void 
CMGMaterialTemplate::AddPixelShader(IMGPixelShader* pShader)
{
	m_PS.push_back(pShader);
}

boolean 
CMGMaterialTemplate::IsGlobalAvailable(uint index)
{
	_DEBUG_ASSERT(m_pGlobalAvailable);
	return m_pGlobalAvailable[index];
}

boolean 
CMGMaterialTemplate::Compile(IByteBuffer* pBuffer, const boolean* pGlobalAvailable)
{
	uint numGlobals = (uint) m_Globals.size();
	//_LOOPi(numGlobals)
	//{
	//	((CMGGlobal*)(IMGGlobal*) m_Globals[i])->SetIndex(i);
	//}
	
	m_pGlobalAvailable = pGlobalAvailable;

	m_CurFrame++;

	m_ResolvedGlobals.clear();
	m_ResolvedTechniques.clear();
	m_ResolvedFunctions.clear();
	m_ResolvedVertexShaders.clear();
	m_ResolvedPixelShaders.clear();
	
	// Resolve globals that are always used
	_LOOPi(numGlobals)
	{
		CMGGlobal* pGlobal = (CMGGlobal*)(IMGGlobal*)m_Globals[i];
		if(pGlobal->GetIsAlwaysUsed())
			pGlobal->Resolve(this, m_CurFrame);
	}
	
	// Resolve all techniques
	uint numTechs = (uint) m_Techs.size();
	_LOOPi(numTechs)
	{
		CMGTechnique* pTech = (CMGTechnique*) (IMGTechnique*) m_Techs[i];
		boolean isResolved = pTech->Resolve(this, m_CurFrame);
		if(!isResolved)
			return FALSE;
	}
	
	DoCompile(pBuffer);
	
	return TRUE;
}

_NAMESPACE_END