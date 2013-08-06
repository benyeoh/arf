//==============================================================================
//
//		CMGFunctionOp.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/5/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CMGFunctionOp : public CRefCounted<IMGFunctionOp>, public IMGShaderNode
{
private: 
	CMGFunctionPtr m_pOwner;
	CMGMaterialGen* m_pGen;
	std::vector<CMGInputResourcePtr> m_InList;
	CMGOutputResourcePtr m_pOut;
	
	uint m_CurFrame;
	boolean m_LastResult;
	
public: 
	CMGFunctionOp(CMGMaterialGen* pGen, CMGFunction* pOwner);
	virtual ~CMGFunctionOp();
	
public: 
	uint GetNumOfInputs() { return (uint) m_InList.size(); }
	IMGInputResource& GetInput(uint index) { return *(m_InList[index]); }
	IMGOutputResource& GetOutput() { return *m_pOut; }
	
	void UpdateOpIndex(uint index);
	
	IMGFunction* GetFunction() { return m_pOwner; }
	CMGFunction* GetCFunction() { return m_pOwner; }
	boolean Resolve(CMGMaterialTemplate* pToNotify, uint currentFrame);
};

_DECLARE_SMARTPTR(CMGFunctionOp)

_NAMESPACE_END
