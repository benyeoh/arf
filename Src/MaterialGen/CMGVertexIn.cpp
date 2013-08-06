//==============================================================================
//
//		CMGVertexIn.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/8/2008
//
//==============================================================================

#include "MaterialGenFramework.h"

_NAMESPACE_BEGIN

#define __VTXIN_OP_OUT_DELIM _W("__vtxIn")

CMGVertexIn::CMGVertexIn(CMGMaterialGen* pGen, const MGParamSig* pParam)
	: m_pGen(pGen)
	, m_CurFrame(0xFFFFFFFF)
	, m_LastResult(FALSE)
{
	m_Sig = *pParam;

	wstring outName = m_Sig.name + __VTXIN_OP_OUT_DELIM;
	MGParamSig outSig = *pParam;
	outSig.pName = outName.c_str();

	m_pOut = ((CMGResourceMgr*)&m_pGen->GetResourceMgr())->CreateOutputResource(this, &outSig);
}

CMGVertexIn::~CMGVertexIn()
{
}

boolean 
CMGVertexIn::Resolve(CMGMaterialTemplate* pToNotify, uint currentFrame)
{
	if(m_CurFrame != currentFrame)
	{
		m_CurFrame = currentFrame;

		pToNotify->ResolvedVertexIn(this);
		m_LastResult = TRUE;		
	}

	return m_LastResult;
}

_NAMESPACE_END