//==============================================================================
//
//		CMGFunction.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/4/2008
//
//==============================================================================

#include "MaterialGenFramework.h"

_NAMESPACE_BEGIN

IMGFunctionOp* 
CMGFunction::CreateFunctionOp()
{
	return ((CMGResourceMgr*)&m_pGen->GetResourceMgr())->CreateFunctionOp(this);	
}

boolean 
CMGFunction::Resolve(CMGMaterialTemplate* pToNotify, uint currentFrame)
{
	if(currentFrame != m_CurFrame)
	{
		m_CurFrame = currentFrame;
		
		pToNotify->ResolvedFunction(this);
		m_LastResult = TRUE;
	}

	return m_LastResult;
}

_NAMESPACE_END