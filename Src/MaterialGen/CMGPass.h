//==============================================================================
//
//		CMGPass.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/4/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CMGPass : public CRefCounted<IMGPass>, public IMGShaderNode
{
private: 
	CMGMaterialGen* m_pGen;
	wstring m_Name;
	std::vector<IMGRenderStatePtr> m_StateList;
	IMGVertexShaderOpPtr m_pVS;
	IMGPixelShaderOpPtr m_pPS;
	
	uint m_CurFrame;
	boolean m_LastResult;
	
public: 
	CMGPass(CMGMaterialGen* pGen, const wchar* pName)
		: m_pGen(pGen)
		, m_CurFrame(0xFFFFFFFF)
		, m_LastResult(FALSE)
	{
		_DEBUG_ASSERT(pName);
		m_Name = pName;
	}

	virtual ~CMGPass()
	{
	}

public: 
	void SetVertexShaderOp(IMGVertexShaderOp* pShader) { m_pVS = pShader; }
	void SetPixelShaderOp(IMGPixelShaderOp* pShader) { m_pPS = pShader; }
	void AddRenderState(IMGRenderState* pState) { m_StateList.push_back(pState); }
	boolean Resolve(CMGMaterialTemplate* pToNotify, uint currentFrame);
	
	const wchar* GetName() { return m_Name.size() > 0 ? m_Name.c_str() : NULL; }
	
	IMGVertexShaderOp* GetVertexShaderOp() { return m_pVS; }
	IMGPixelShaderOp* GetPixelShaderOp() { return m_pPS; }
	uint GetNumOfRenderStates() { return (uint) m_StateList.size(); }
	IMGRenderState* GetRenderState(uint index) { return m_StateList[index]; }	
	
};

_DECLARE_SMARTPTR(CMGPass)

_NAMESPACE_END

