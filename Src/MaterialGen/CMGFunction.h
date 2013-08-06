//==============================================================================
//
//		CMGFunction.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/3/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CMGFunction : public CRefCounted<IMGFunction>, public IMGShaderNode
{
private:
	wstring m_Value;
	std::vector<MGInputSigStore> m_InList;
	MGParamSigStore m_Sig;	
	CMGMaterialGen* m_pGen;
	
	uint m_CurFrame;
	boolean m_LastResult;
	
public: 
	CMGFunction(CMGMaterialGen* pGen,
				MGParamSig* pParam,
				const MGInputSig* pIn, 
				uint numInputs)
		: m_pGen(pGen)
		, m_CurFrame(0xFFFFFFFF)
		, m_LastResult(FALSE)
		, m_Value(_W(""))
	{
		_LOOPi(numInputs)
		{
			m_InList.push_back( MGInputSigStore() );
			MGInputSigStore& sig = m_InList.back();
			sig = (pIn[i]);
		}
		
		m_Sig = *pParam;
	}

	virtual ~CMGFunction()
	{
	}

public:
	uint GetNumOfInputs() { return (uint) m_InList.size(); }
	MGInputSigStore* GetInputSig(uint index) { return &(m_InList[index]); }
	MGParamSigStore* GetParamSig() { return &m_Sig; }
	
	const wchar* GetInputDefaultVal(uint index) { return m_InList[index].defaultVal.size() > 0 ? m_InList[index].defaultVal.c_str() : NULL; }
	const wchar* GetInputName(uint index) { return m_InList[index].name.size() > 0 ? m_InList[index].name.c_str() : NULL; }
	const wchar* GetInputType(uint index) { return m_InList[index].type.size() > 0 ? m_InList[index].type.c_str() : NULL; }

	void SetValue(const wchar* val) { m_Value = val; }
	const wchar* GetValue() { return m_Value.size() > 0 ? m_Value.c_str() : NULL; }
	const wchar* GetName() { return m_Sig.name.size() > 0 ? m_Sig.name.c_str() : NULL; }
	const wchar* GetType() { return m_Sig.type.size() > 0 ? m_Sig.type.c_str() : NULL; }

	IMGFunctionOp* CreateFunctionOp();
	
	boolean Resolve(CMGMaterialTemplate* pToNotify, uint currentFrame);
};

_DECLARE_SMARTPTR(CMGFunction)

_NAMESPACE_END
