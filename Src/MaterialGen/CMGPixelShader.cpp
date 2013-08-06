//==============================================================================
//
//		CMGPixelShader.cpp
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
CMGPixelShader::Resolve(CMGMaterialTemplate* pToNotify, uint currentFrame)
{
	if(m_CurFrame != currentFrame)
	{
		m_CurFrame = currentFrame;
		pToNotify->BeginPixelShader(this);
		
		//uint numIn = (uint) m_pPixInList.size();
		//boolean isResolved = TRUE;
		//_LOOPi(numIn)
		//{
		//	isResolved = m_pPixInList[i]->Resolve(pToNotify, currentFrame);
		//	if(!isResolved)
		//	{
		//		pToNotify->EndPixelShader(this);
		//		m_LastResult = FALSE;
		//		return FALSE;
		//	}
		//}
		//
		//
		uint numOut = (uint) m_pPixOutList.size();
		uint numResolved = 0;
		_LOOPi(numOut)
		{
			boolean isResolved = ((CMGPixelOut*)(IMGPixelOut*)m_pPixOutList[i])->Resolve(pToNotify, currentFrame);
			if(isResolved)
				numResolved++;
		}
		
		if(numResolved > 0)
		{
			pToNotify->ResolvedPixelShader(this);
			m_LastResult = TRUE;
		}
		else
		{
			m_pGen->GetAppCallback().Log(
				ToStringAuto(_W("Failed to resolve: PixelShader=%s\n"), m_Name.c_str())
				);
			m_LastResult = FALSE;
		}
		
		pToNotify->EndPixelShader(this);		
	}
	
	return m_LastResult;
}

void 
CMGPixelShader::AddFunctionOp(IMGFunctionOp* pOp)
{
	uint size = (uint) m_pOpList.size();
	((CMGFunctionOp*)pOp)->UpdateOpIndex(size);
	
	m_pOpList.push_back(pOp);
}

IMGPixelShaderOp* 
CMGPixelShader::CreateShaderOp()
{
	return ((CMGResourceMgr*)&(m_pGen->GetResourceMgr()))->CreatePixelShaderOp(this);
}

_NAMESPACE_END