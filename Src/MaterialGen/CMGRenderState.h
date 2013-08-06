//==============================================================================
//
//		CMGRenderState.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/4/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CMGRenderState : public CRefCounted<IMGRenderState>, public IMGShaderNode
{
private: 
	MGParamSigStore m_Sig;
	CMGMaterialGen* m_pGen;
	std::vector<CMGInputResourcePtr> m_InList;
	std::vector<MGInputSigStore> m_InSigs;
	
	uint m_CurFrame;
	boolean m_LastResult;
	wstring m_Value;
	
public: 
	CMGRenderState(CMGMaterialGen* pGen,
				const MGParamSig* pParam, 
				const MGInputSig* pIn, 
				uint numInputs);

	virtual ~CMGRenderState();

public: 
	uint GetNumOfInputs();
	IMGInputResource& GetInput(uint index);
	boolean Resolve(CMGMaterialTemplate* pToNotify, uint currentFrame);
	
	void SetValue(const wchar* pVal) { m_Value = pVal; }
	const wchar* GetValue() { return m_Value.size() > 0 ? m_Value.c_str() : NULL; }

	MGParamSigStore& GetParamSig() { return m_Sig; }
	
	const wchar* GetName() { return m_Sig.name.size() > 0 ? m_Sig.name.c_str() : NULL; }
	const wchar* GetType() { return m_Sig.type.size() > 0 ? m_Sig.type.c_str() : NULL; }
};

_DECLARE_SMARTPTR(CMGRenderState)

_NAMESPACE_END
