//==============================================================================
//
//		CMGRenderState.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/8/2008
//
//==============================================================================

#include "MaterialGenFramework.h"

_NAMESPACE_BEGIN

CMGRenderState::CMGRenderState(CMGMaterialGen* pGen,
							   const MGParamSig* pParam, 
							   const MGInputSig* pIn, 
							   uint numInputs)
   : m_pGen(pGen)
   , m_CurFrame(0xFFFFFFFF)
   , m_LastResult(FALSE)
{
	m_Sig = *pParam;

	_LOOPi(numInputs)
	{
		m_InSigs.push_back(pIn[i]);
		CMGInputResource* pInRes = ((CMGResourceMgr*)&m_pGen->GetResourceMgr())->CreateInputResource(&(m_InSigs.back()));
		m_InList.push_back(pInRes);
	}
}

CMGRenderState::~CMGRenderState()
{
}

uint 
CMGRenderState::GetNumOfInputs()
{
	return (uint) m_InList.size();
}

IMGInputResource& 
CMGRenderState::GetInput(uint index)
{
	return *(m_InList[index]);
}

boolean 
CMGRenderState::Resolve(CMGMaterialTemplate* pToNotify, uint currentFrame)
{
	if(m_CurFrame != currentFrame)
	{
		m_CurFrame = currentFrame;
		uint size = GetNumOfInputs();
		_LOOPi(size)
		{
			if(!(m_InList[i]->Resolve(pToNotify, currentFrame)))
			{
				m_pGen->GetAppCallback().Log(
					ToStringAuto(_W("Failed to resolve: Renderstate=%s Input=%s\n"), m_Sig.name.c_str(), m_InList[i]->GetName())
					);

				m_LastResult = FALSE;
				return FALSE;
			}
		}
		
		pToNotify->ResolvedRenderState(this);
		m_LastResult = TRUE;
	}
	
	return m_LastResult;
}

_NAMESPACE_END