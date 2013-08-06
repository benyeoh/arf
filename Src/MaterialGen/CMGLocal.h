//==============================================================================
//
//		CMGLocal.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/3/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CMGLocal : public CRefCounted<IMGLocal>
{
private: 
	MGParamSigStore m_Sig;
	wstring m_Value;
	wstring m_Preamble;
	wstring m_Postamble;
	
public: 
	CMGLocal(const MGLocalSig* pParam)
	{
		if(pParam->pName)
			m_Sig.name = pParam->pName;
		if(pParam->pType)
			m_Sig.type = pParam->pType;
		if(pParam->pPreamble)
			m_Preamble = pParam->pPreamble;
		if(pParam->pPostamble)
			m_Postamble = pParam->pPostamble;
	}

	virtual ~CMGLocal()
	{
	}
	
public: 
	void SetValue(const wchar* pVal) { m_Value = pVal; }
	const wchar* GetValue() { return m_Value.size() > 0 ? m_Value.c_str() : NULL; }
	const wchar* GetPreamble() { return m_Preamble.size() > 0 ? m_Preamble.c_str() : NULL; }
	const wchar* GetPostamble() { return m_Postamble.size() > 0 ? m_Postamble.c_str() : NULL; }
	const wchar* GetName() { return m_Sig.name.size() > 0 ? m_Sig.name.c_str() : NULL; }
	const wchar* GetType() { return m_Sig.type.size() > 0 ? m_Sig.type.c_str() : NULL; }
};

_NAMESPACE_END
