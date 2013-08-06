//==============================================================================
//
//		CMGInputResource.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/3/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CMGInputResource : public CRefCounted<IMGInputResource>, public IMGShaderNode
{
private:
	MGInputSigStore* m_pSig;
	CMGOutputResource* m_pBinding;
	CMGOutputResource* m_pAltBinding;
	CMGMaterialGen* m_pGen;
	
	boolean m_LastResult;
	uint m_CurFrame;
	
public: 
	CMGInputResource(CMGMaterialGen* pGen, MGInputSigStore* pSig)
		: m_pSig(pSig)
		, m_pBinding(NULL)
		, m_pAltBinding(NULL)
		, m_pGen(pGen)
		, m_CurFrame(0xFFFFFFFF)
		, m_LastResult(FALSE)
	{
	}

	virtual ~CMGInputResource()
	{
	}

public:
	const wchar* GetDefaultVal() { return m_pSig->defaultVal.size() ? m_pSig->defaultVal.c_str() : NULL; }
	const wchar* GetName() { return m_pSig->name.size() > 0 ? m_pSig->name.c_str() : NULL; }
	const wchar* GetType() { return m_pSig->type.size() > 0 ? m_pSig->type.c_str() : NULL; }
		
	void BindValue(IMGOutputResource* pOutput) { m_pBinding = (CMGOutputResource*) pOutput; }
	void BindAltValue(IMGOutputResource* pOutput) { m_pAltBinding = (CMGOutputResource*) pOutput; }
		
	CMGOutputResource* GetCBinding() { return m_pBinding; }
	IMGOutputResource* GetBinding() { return m_pBinding; }
	IMGOutputResource* GetAltBinding() { return m_pAltBinding; }
	
	boolean Resolve(CMGMaterialTemplate* pToNotify, uint currentFrame);
};

_DECLARE_SMARTPTR(CMGInputResource)

_NAMESPACE_END
