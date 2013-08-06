//==============================================================================
//
//		CMGOutputResource.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/3/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CMGOutputResource : public CRefCounted<IMGOutputResource>, public IMGShaderNode
{
protected: 
	IMGShaderNode* m_pOwner;
	CMGMaterialGen* m_pGen;
	MGParamSigStore m_Param;
		
	uint m_CurFrame;
	boolean m_LastResult;
	
public: 
	CMGOutputResource(CMGMaterialGen* pGen, IMGShaderNode* pOwner, const MGParamSig* pParam)
		: m_pOwner(pOwner)
		, m_pGen(pGen)
		, m_LastResult(FALSE)
		, m_CurFrame(0xFFFFFFFF)
	{
		m_Param = *pParam;
	}

	virtual ~CMGOutputResource()
	{
	}

public:
	const wchar* GetName() { return m_Param.name.size() > 0 ? m_Param.name.c_str() : NULL; }
	const wchar* GetType() { return m_Param.type.size() > 0 ? m_Param.type.c_str() : NULL; }
	
	void SetName(const wchar* pName)  { m_Param.name = pName; }
	boolean Resolve(CMGMaterialTemplate* pToNotify, uint currentFrame);
	boolean IsResolved() { return m_LastResult; }
};

_DECLARE_SMARTPTR(CMGOutputResource)

_NAMESPACE_END
