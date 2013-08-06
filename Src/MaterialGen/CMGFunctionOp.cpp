//==============================================================================
//
//		CMGFunctionOp.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/5/2008
//
//==============================================================================

#include "MaterialGenFramework.h"

_NAMESPACE_BEGIN

#define __FUNC_OP_OUT_DELIM _W("__fn")

CMGFunctionOp::CMGFunctionOp(CMGMaterialGen* pGen, CMGFunction* pOwner)
	: m_pGen(pGen)
	, m_CurFrame(0xFFFFFFFF)
	, m_LastResult(FALSE)
	, m_pOwner(pOwner)
{	
	uint numInput = pOwner->GetNumOfInputs();
	_LOOPi(numInput)
	{
		MGInputSigStore* pStore = pOwner->GetInputSig(i);
		CMGInputResource* pRes = ((CMGResourceMgr*)&m_pGen->GetResourceMgr())->CreateInputResource(pStore);
		m_InList.push_back(pRes);
	}

	MGParamSigStore* pParamStore = m_pOwner->GetParamSig();
	MGParamSig sig;
	wstring outName = pParamStore->name + wstring(__FUNC_OP_OUT_DELIM);
	if(pParamStore->name.size() > 0)
		sig.pName = outName.c_str();
	if(pParamStore->type.size() > 0)
		sig.pType = pParamStore->type.c_str();

	m_pOut = ((CMGResourceMgr*)&m_pGen->GetResourceMgr())->CreateOutputResource(this, &sig);
}

CMGFunctionOp::~CMGFunctionOp()
{
}

void 
CMGFunctionOp::UpdateOpIndex(uint index)
{
	// Update the name for the output
	MGParamSigStore* pParamStore = m_pOwner->GetParamSig();
	wstring name = pParamStore->name.c_str();
	name = name + ToStringAuto(_W("_%d"), index);
	wstring outName = name + wstring(__FUNC_OP_OUT_DELIM);
	m_pOut->SetName(outName.c_str());
}

boolean 
CMGFunctionOp::Resolve(CMGMaterialTemplate* pToNotify, uint currentFrame)
{
	if(currentFrame != m_CurFrame)
	{
		m_CurFrame = currentFrame;
		uint numInputs = GetNumOfInputs();
		_LOOPi(numInputs)
		{
			CMGInputResource* pIn = (CMGInputResource*) &GetInput(i);
			boolean isResolved = pIn->Resolve(pToNotify, currentFrame);
			if(!isResolved)
			{
				m_pGen->GetAppCallback().Log(
					ToStringAuto(_W("Failed to resolve: FunctionOp=%s, Input=%s\n"), m_pOwner->GetParamSig()->name.c_str(), pIn->GetName())
					);
				m_LastResult = FALSE;
				return FALSE;
			}					
		}
		
		boolean isResolved = m_pOwner->Resolve(pToNotify, currentFrame);
		if(!isResolved)
		{
			m_pGen->GetAppCallback().Log(
				ToStringAuto(_W("Failed to resolve: FunctionOp=%s, Owner\n"), m_pOwner->GetParamSig()->name.c_str())
				);
			m_LastResult = FALSE;
			return FALSE;
		}
		
		pToNotify->ResolvedFunctionOp(this);
		m_LastResult = TRUE;
	}
	
	return m_LastResult;
}	

_NAMESPACE_END