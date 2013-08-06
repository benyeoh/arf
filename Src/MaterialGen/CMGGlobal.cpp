//==============================================================================
//
//		CMGGlobal.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/5/2008
//
//==============================================================================

#include "MaterialGenFramework.h"

_NAMESPACE_BEGIN

CMGGlobal::CMGGlobal(CMGMaterialGen* pGen, MGParamSig* pParam, const MGInputSig* pIn, uint numInputs)
	: m_pGen(pGen)
	, m_Semantic(0xFFFFFFFF)
	, m_CurFrame(0xFFFFFFFF)
	, m_LastResult(FALSE)
	, m_Index(0xFFFFFFFF)
	, m_IsAlwaysUsed(FALSE)
	, m_IsVolatile(FALSE)
{
	_LOOPi(numInputs)
	{
		m_InSigs.push_back((pIn[i]));
		CMGInputResource* pInRes = ((CMGResourceMgr*)&m_pGen->GetResourceMgr())->CreateInputResource(&(m_InSigs.back()));
		m_InList.push_back(pInRes);
	}

	m_Sig = *pParam;		
	m_pOut = ((CMGResourceMgr*)&m_pGen->GetResourceMgr())->CreateOutputResource(this, pParam);
}

CMGGlobal::~CMGGlobal()
{
}

void 
CMGGlobal::AddDescription(MGInputSig* pSig)
{
	m_Descriptions.push_back(MGInputSigStore());
	MGInputSigStore& store = m_Descriptions.back();
	store = *pSig;
}

boolean 
CMGGlobal::Resolve(CMGMaterialTemplate* pToNotify, uint currentFrame)
{
	if(m_CurFrame != currentFrame)
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
					ToStringAuto(_W("Failed to resolve: Global=%s, Input=%s\n"), m_Sig.name.c_str(), pIn->GetName())
					);
				m_LastResult = FALSE;
				return FALSE;
			}
		}
	
		if(m_IsVolatile)
		{
			// Then it requires application support
			_DEBUG_ASSERT(m_Semantic != 0xFFFFFFFF);
			if(!pToNotify->IsGlobalAvailable(m_Index))
			{
				m_pGen->GetAppCallback().Log(
					ToStringAuto(_W("Failed to resolve: Global=%s, Semantic=%d\n"), m_Sig.name.c_str(), m_Semantic)
					);
					
				m_LastResult = FALSE;
				return FALSE;
			}
		}
			
		pToNotify->ResolvedGlobal(this);
		m_LastResult = TRUE;
	}
	
	return m_LastResult;
}

_NAMESPACE_END