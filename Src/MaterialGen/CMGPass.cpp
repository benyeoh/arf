//==============================================================================
//
//		CMGPass.cpp
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
CMGPass::Resolve(CMGMaterialTemplate* pToNotify, uint currentFrame)
{
	if(m_CurFrame != currentFrame)
	{
		m_CurFrame = currentFrame;
		
		pToNotify->BeginPass(this);
		boolean isResolved = TRUE;
		if(m_pPS)
			isResolved = ((CMGVertexShaderOp*)(IMGVertexShaderOp*)m_pVS)->Resolve(pToNotify, currentFrame);
			
		if(!isResolved)
		{
			m_pGen->GetAppCallback().Log(
				ToStringAuto(_W("Failed to resolve: Pass=%s\n"), m_Name.c_str())
				);
				
			m_LastResult = FALSE;
			pToNotify->EndPass(this);
			return FALSE;
		}

		isResolved = TRUE;
		if(m_pPS)
			isResolved = ((CMGPixelShaderOp*)(IMGPixelShaderOp*)m_pPS)->Resolve(pToNotify, currentFrame);
		if(!isResolved)
		{
			m_pGen->GetAppCallback().Log(
				ToStringAuto(_W("Failed to resolve: Pass=%s\n"), m_Name.c_str())
				);

			m_LastResult = FALSE;
			pToNotify->EndPass(this);
			return FALSE;
		}		
		
		uint numStates = (uint) m_StateList.size();
		_LOOPi(numStates)
		{
			isResolved = ((CMGRenderState*)(IMGRenderState*) m_StateList[i])->Resolve(pToNotify, currentFrame);
			if(!isResolved)
			{
				m_pGen->GetAppCallback().Log(
					ToStringAuto(_W("Failed to resolve: Pass=%s"), m_Name.c_str())
					);

				m_LastResult = FALSE;
				pToNotify->EndPass(this);
				return FALSE;
			}		
		}
		
		pToNotify->ResolvedPass(this);
		m_LastResult = TRUE;
		pToNotify->EndPass(this);
	}
	
	return m_LastResult;
}

_NAMESPACE_END