//==============================================================================
//
//		CMGVertexShaderOp.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/17/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CMGVertexShaderOp : public CRefCounted<IMGVertexShaderOp>, public IMGShaderNode
{
private: 
	CMGVertexShaderPtr m_pShader;
	CMGMaterialGen* m_pGen;
	
	boolean m_IsResolved;
	uint m_CurFrame;
	
public: 
	CMGVertexShaderOp(CMGMaterialGen* pGen, CMGVertexShader* pShader)
		: m_pGen(pGen)
		, m_CurFrame(0xFFFFFFFF)
		, m_IsResolved(FALSE)
		, m_pShader(pShader)
	{
	}

	virtual ~CMGVertexShaderOp()
	{
	}

public: 
	IMGVertexShader* GetShader() { return m_pShader; }
	
	boolean Resolve(CMGMaterialTemplate* pToNotify, uint currentFrame);
};

_NAMESPACE_END
