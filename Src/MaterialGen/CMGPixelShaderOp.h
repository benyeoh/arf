//==============================================================================
//
//		CMGPixelShaderOp.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/17/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CMGPixelShaderOp : public CRefCounted<IMGPixelShaderOp>, public IMGShaderNode
{
private: 
	CMGPixelShaderPtr m_pShader;
	CMGMaterialGen* m_pGen;

	boolean m_IsResolved;
	uint m_CurFrame;

public: 
	CMGPixelShaderOp(CMGMaterialGen* pGen, CMGPixelShader* pShader)
		: m_pGen(pGen)
		, m_CurFrame(0xFFFFFFFF)
		, m_IsResolved(FALSE)
		, m_pShader(pShader)
	{
	}

	virtual ~CMGPixelShaderOp()
	{
	}

public: 
	IMGPixelShader* GetShader() { return m_pShader; }

	boolean Resolve(CMGMaterialTemplate* pToNotify, uint currentFrame);
};

_NAMESPACE_END
