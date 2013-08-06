//==============================================================================
//
//		CMGInputResource.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/5/2008
//
//==============================================================================

#include "MaterialGenFramework.h"

_NAMESPACE_BEGIN

boolean
CMGInputResource::Resolve(CMGMaterialTemplate* pToNotify, uint currentFrame)
{
	if(m_CurFrame != currentFrame)
	{
		m_CurFrame = currentFrame;		
		if(!m_pBinding || !m_pBinding->Resolve(pToNotify, currentFrame))
		{
			if(!m_pAltBinding || !m_pAltBinding->Resolve(pToNotify, currentFrame))
			{
				if(m_pSig->defaultVal.size() == 0)
				{
					m_LastResult = FALSE;
					return FALSE;
				}
			}
		}
		
		pToNotify->ResolvedInputResource(this);
		m_LastResult = TRUE;	
	}
	
	return m_LastResult;	
}

_NAMESPACE_END