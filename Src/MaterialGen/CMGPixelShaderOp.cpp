//==============================================================================
//
//		CMGPixelShaderOp.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/17/2008
//
//==============================================================================

#include "MaterialGenFramework.h"

_NAMESPACE_BEGIN

boolean 
CMGPixelShaderOp::Resolve(CMGMaterialTemplate* pToNotify, uint currentFrame)
{
	if(m_CurFrame != currentFrame)
	{
		m_CurFrame = currentFrame;

		if(!m_pShader->Resolve(pToNotify, currentFrame))
		{
			m_IsResolved = FALSE;
			return FALSE;
		}

		m_IsResolved = TRUE;
	}

	return m_IsResolved;	
}


_NAMESPACE_END