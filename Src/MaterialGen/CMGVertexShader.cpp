//==============================================================================
//
//		CMGVertexShader.cpp
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
CMGVertexShader::Resolve(CMGMaterialTemplate* pToNotify, uint currentFrame)
{
	if(m_CurFrame != currentFrame)
	{
		m_CurFrame = currentFrame;
		pToNotify->BeginVertexShader(this);

		uint numOut = (uint) m_pVertOutList.size();
		uint numResolved = 0;
		_LOOPi(numOut)
		{
			boolean isResolved = ((CMGVertexOut*)(IMGVertexOut*)m_pVertOutList[i])->Resolve(pToNotify, currentFrame);
			if(isResolved)
				numResolved++;
			//if(!isResolved)
			//{
			//	m_pGen->GetAppCallback().Log(
			//		ToStringAuto(_W("Failed to resolve: VertexShader=%s\n"), m_Name.c_str())
			//		);
			//		
			//	m_LastResult = FALSE;
			//	pToNotify->EndVertexShader(this);
			//	return FALSE;
			//}
		}
		
		if(numResolved > 0)
		{
			pToNotify->ResolvedVertexShader(this);
			m_LastResult = TRUE;
		}
		else
		{
			m_pGen->GetAppCallback().Log(
				ToStringAuto(_W("Failed to resolve: VertexShader=%s\n"), m_Name.c_str())
				);
			m_LastResult = FALSE;
		}
		
		pToNotify->EndVertexShader(this);		
	}

	return m_LastResult;
}

void 
CMGVertexShader::AddFunctionOp(IMGFunctionOp* pOp)
{
	uint size = (uint) m_pOpList.size();
	((CMGFunctionOp*)pOp)->UpdateOpIndex(size);

	m_pOpList.push_back(pOp);
}

IMGVertexShaderOp* 
CMGVertexShader::CreateShaderOp()
{
	return ((CMGResourceMgr*)&(m_pGen->GetResourceMgr()))->CreateVertexShaderOp(this);
}


_NAMESPACE_END