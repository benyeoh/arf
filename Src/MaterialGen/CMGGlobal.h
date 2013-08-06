//==============================================================================
//
//		CMGGlobal.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/4/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CMGGlobal : public CRefCounted<IMGGlobal>, public IMGShaderNode
{
private: 
	std::vector<MGInputSigStore> m_InSigs;
	std::vector<CMGInputResourcePtr> m_InList;
	MGParamSigStore m_Sig;

	uint m_Semantic;
	eMGUsage m_Usage;
	boolean m_IsAlwaysUsed;
	boolean m_IsVolatile;
	
	wstring m_Value;
	std::vector<MGInputSigStore> m_Descriptions;
	CMGMaterialGen* m_pGen;
	
	CMGOutputResourcePtr m_pOut;
	uint m_CurFrame;
	boolean m_LastResult;
	uint m_Index;
	
public: 
	CMGGlobal(CMGMaterialGen* pGen, MGParamSig* pParam, const MGInputSig* pIn, uint numInputs);
	virtual ~CMGGlobal();
	
public: 
	void SetSemantic(uint semantic) { m_Semantic = semantic; }
	void SetUsage(eMGUsage val) { m_Usage = val; }
	void SetValue(const wchar* val) { m_Value = val; }
	void AddDescription(MGInputSig* pSig);
	void SetIsAlwaysUsed(boolean val) { m_IsAlwaysUsed = val; }
	void SetIsVolatile(boolean val) { m_IsVolatile = val; }
		
	void SetIndex(uint val) { m_Index = val; }
	
	const MGParamSigStore& GetSig() { return m_Sig; }
	 
	uint GetSemantic() { return m_Semantic; }
	eMGUsage GetUsage() { return m_Usage; }
	boolean GetIsAlwaysUsed() { return m_IsAlwaysUsed; }
	boolean GetIsVolatile() { return m_IsVolatile; }
	
	uint GetNumOfDesc() { return (uint) m_Descriptions.size(); }
	const MGInputSigStore& GetDescription(uint index) { return m_Descriptions[index]; }	
		
	uint GetNumOfInputs() { return (uint) m_InList.size(); }
	IMGInputResource& GetInput(uint index) { return *(m_InList[index]); }
	IMGOutputResource& GetOutput() { return *m_pOut; }
	
	boolean Resolve(CMGMaterialTemplate* pToNotify, uint currentFrame);
	
	const wchar* GetName() { return m_Sig.name.size() > 0 ? m_Sig.name.c_str() : NULL; }
	const wchar* GetType() { return m_Sig.type.size() > 0 ? m_Sig.type.c_str() : NULL; }
	const wchar* GetValue() { return m_Value.size() > 0 ? m_Value.c_str() : NULL; }

	const wchar* GetDescName(uint i) { return m_Descriptions[i].name.size() ? m_Descriptions[i].name.c_str() : NULL; }
	const wchar* GetDescType(uint i) { return m_Descriptions[i].type.size() ? m_Descriptions[i].type.c_str() : NULL; }
	const wchar* GetDescVal(uint i) { return m_Descriptions[i].defaultVal.size() ? m_Descriptions[i].defaultVal.c_str() : NULL; }
	
};

_DECLARE_SMARTPTR(CMGGlobal)

_NAMESPACE_END
