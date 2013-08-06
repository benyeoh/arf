//==============================================================================
//
//		CMGPixelOut.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/4/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CMGPixelOut : public CRefCounted<IMGPixelOut>, public IMGShaderNode
{
private: 
	MGParamSigStore m_Sig;
	CMGMaterialGen* m_pGen;
	CMGInputResourcePtr m_pIn;
	MGInputSigStore m_Store;
	
	uint m_CurFrame;
	boolean m_LastResult;
	
public: 
	CMGPixelOut(CMGMaterialGen* pGen, const MGParamSig* pParam);
	virtual ~CMGPixelOut();
	
public: 
	IMGInputResource& GetInput() { return *m_pIn; }
	boolean Resolve(CMGMaterialTemplate* pToNotify, uint currentFrame);
	boolean IsResolved() { return m_LastResult; }
	
	const wchar* GetName() { return m_Sig.name.size() > 0 ? m_Sig.name.c_str() : NULL; }
	const wchar* GetType() { return m_Sig.type.size() > 0 ? m_Sig.type.c_str() : NULL; }
};

_DECLARE_SMARTPTR(CMGPixelOut)

_NAMESPACE_END
