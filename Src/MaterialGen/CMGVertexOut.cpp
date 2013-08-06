//==============================================================================
//
//		CMGVertexOut.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/8/2008
//
//==============================================================================

#include "MaterialGenFramework.h"

_NAMESPACE_BEGIN

CMGVertexOut::CMGVertexOut(CMGMaterialGen* pGen, const MGParamSig* pParam)
	: m_pGen(pGen)
	, m_LastResult(FALSE)
	, m_CurFrame(0xFFFFFFFF)
{
	m_Sig = *pParam;

	if(pParam->pName)
		m_Store.name = pParam->pName;
	if(pParam->pType)
		m_Store.type = pParam->pType;

	m_pIn = ((CMGResourceMgr*)&m_pGen->GetResourceMgr())->CreateInputResource(&m_Store);
}

CMGVertexOut::~CMGVertexOut()
{
}

boolean 
CMGVertexOut::Resolve(CMGMaterialTemplate* pToNotify, uint currentFrame)
{
	if(m_CurFrame != currentFrame)
	{
		m_CurFrame = currentFrame;
		if(!m_pIn || !m_pIn->Resolve(pToNotify, currentFrame))
		{
			m_LastResult = FALSE;
			return FALSE;
		}

		pToNotify->ResolvedVertexOut(this);
		m_LastResult = TRUE;
	}
	
	return m_LastResult;
}

_NAMESPACE_END