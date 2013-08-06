//==============================================================================
//
//		CMGOutputResource.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/8/2008
//
//==============================================================================

#include "MaterialGenFramework.h"

_NAMESPACE_BEGIN

boolean 
CMGOutputResource::Resolve(CMGMaterialTemplate* pToNotify, uint currentFrame)
{
	if(m_CurFrame != currentFrame)
	{
		boolean isResolved = m_pOwner->Resolve(pToNotify, currentFrame);
		if(!isResolved)
		{
			m_LastResult = FALSE;
			return FALSE;
		}	
		
		pToNotify->ResolvedOutputResource(this);
		m_LastResult = TRUE;
	}
	
	return m_LastResult;
}

_NAMESPACE_END