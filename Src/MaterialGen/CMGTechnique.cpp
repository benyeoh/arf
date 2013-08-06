//==============================================================================
//
//		CMGTechnique.cpp
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
CMGTechnique::Resolve(CMGMaterialTemplate* pToNotify, uint currentFrame)
{
	if(m_CurFrame != currentFrame)
	{
		m_CurFrame = currentFrame;
		pToNotify->BeginTechnique(this);
		
		uint numPasses = (uint) m_PassList.size();
		_LOOPi(numPasses)
		{
			if(!((CMGPass*)(IMGPass*)m_PassList[i])->Resolve(pToNotify, currentFrame))
			{
				m_pGen->GetAppCallback().Log(
					ToStringAuto(_W("Failed to resolve: Technique=%s\n"), m_Name.c_str())
					);
				pToNotify->EndTechnique(this);
				m_LastResult = FALSE;
				return FALSE;
			}
		}
		
		pToNotify->ResolvedTechnique(this);
		pToNotify->EndTechnique(this);
		m_LastResult = TRUE;
	}
	
	return m_LastResult;
}

_NAMESPACE_END
