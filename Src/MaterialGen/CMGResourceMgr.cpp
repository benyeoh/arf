//==============================================================================
//
//		CMGResourceMgr.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/8/2008
//
//==============================================================================

#include "MaterialGenFramework.h"

_NAMESPACE_BEGIN

IMGMaterialTemplate* 
CMGResourceMgr::CreateMaterialTemplate()
{
	return DoCreateMaterialTemplate();
}

IMGLocal* 
CMGResourceMgr::CreateLocal(const MGLocalSig* pParam)
{
	return _NEW CMGLocal(pParam);
}

IMGGlobal* 
CMGResourceMgr::CreateGlobal(MGParamSig* pParam, const MGInputSig* pIn, uint numInputs)
{
	return _NEW CMGGlobal(m_pGen, pParam, pIn, numInputs);
}

IMGVertexIn* 
CMGResourceMgr::CreateVertexIn(const MGParamSig* pParam)
{
	return _NEW CMGVertexIn(m_pGen, pParam);
}

IMGVertexOut* 
CMGResourceMgr::CreateVertexOut(const MGParamSig* pParam)
{
	return _NEW CMGVertexOut(m_pGen, pParam);
}

IMGPixelIn* 
CMGResourceMgr::CreatePixelIn(const MGParamSig* pParam)
{
	return _NEW CMGPixelIn(m_pGen, pParam);
}

IMGPixelOut* 
CMGResourceMgr::CreatePixelOut(const MGParamSig* pParam)
{
	return _NEW CMGPixelOut(m_pGen, pParam);
}

IMGFunction* 
CMGResourceMgr::CreateFunction(MGParamSig* pParam, const MGInputSig* pIn, uint numInputs)
{
	return _NEW CMGFunction(m_pGen, pParam, pIn, numInputs);
}

IMGVertexShader* 
CMGResourceMgr::CreateVertexShader(const wchar* pName)
{
	return _NEW CMGVertexShader(m_pGen, pName);
}

IMGPixelShader* 
CMGResourceMgr::CreatePixelShader(const wchar* pName)
{
	return _NEW CMGPixelShader(m_pGen, pName);
}

IMGPass* 
CMGResourceMgr::CreatePass(const wchar* pName)
{
	return _NEW CMGPass(m_pGen, pName);
}

IMGTechnique* 
CMGResourceMgr::CreateTechnique(const wchar* pName)
{
	return _NEW CMGTechnique(m_pGen, pName);
}

IMGRenderState* 
CMGResourceMgr::CreateRenderState(const MGParamSig* pParam, const MGInputSig* pIn, uint numInputs)
{
	return _NEW CMGRenderState(m_pGen, pParam, pIn, numInputs);	
}

CMGFunctionOp* 
CMGResourceMgr::CreateFunctionOp(CMGFunction* pOwner)
{
	return _NEW CMGFunctionOp(m_pGen, pOwner);
}

CMGOutputResource* 
CMGResourceMgr::CreateOutputResource(IMGShaderNode* pOwner, const MGParamSig* pParam)
{
	return _NEW CMGOutputResource(m_pGen, pOwner, pParam);
}

CMGInputResource* 
CMGResourceMgr::CreateInputResource(MGInputSigStore* pSig)
{
	return _NEW CMGInputResource(m_pGen, pSig);
}

CMGVertexShaderOp* 
CMGResourceMgr::CreateVertexShaderOp(CMGVertexShader* pShader)
{
	return _NEW CMGVertexShaderOp(m_pGen, pShader);
}


CMGPixelShaderOp* 
CMGResourceMgr::CreatePixelShaderOp(CMGPixelShader* pShader)
{
	return _NEW CMGPixelShaderOp(m_pGen, pShader);
}

_NAMESPACE_END