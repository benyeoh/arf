//==============================================================================
//
//		CMGVertexIn.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/4/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CMGVertexIn : public CRefCounted<IMGVertexIn>, public IMGShaderNode
{
private: 
	MGParamSigStore m_Sig;
	CMGOutputResourcePtr m_pOut;
	CMGMaterialGen* m_pGen;
	
	uint m_CurFrame;
	boolean m_LastResult;
	
public: 
	CMGVertexIn(CMGMaterialGen* pGen, const MGParamSig* pParam);
	virtual ~CMGVertexIn();

public: 
	IMGOutputResource& GetOutput() { return *m_pOut; }
	boolean Resolve(CMGMaterialTemplate* pToNotify, uint currentFrame);
	
	const wchar* GetName() { return m_Sig.name.size() > 0 ? m_Sig.name.c_str() : NULL; }
	const wchar* GetType() { return m_Sig.type.size() > 0 ? m_Sig.type.c_str() : NULL; }
};

_DECLARE_SMARTPTR(CMGVertexIn)

_NAMESPACE_END
