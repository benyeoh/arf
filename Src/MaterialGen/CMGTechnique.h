//==============================================================================
//
//		CMGTechnique.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/4/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CMGTechnique : public CRefCounted<IMGTechnique>, public IMGShaderNode
{
private: 
	wstring m_Name;
	CMGMaterialGen* m_pGen;
	std::vector<IMGPassPtr> m_PassList;
	
	uint m_CurFrame;
	boolean m_LastResult;
	
public: 
	CMGTechnique(CMGMaterialGen* pGen, const wchar* pName)
		: m_pGen(pGen)
		, m_LastResult(FALSE)
		, m_CurFrame(0xFFFFFFFF)
	{
		_DEBUG_ASSERT(pName);
		m_Name = pName;
	}

	virtual ~CMGTechnique()
	{
	}

public: 
	void AddPass(IMGPass* pPass) { m_PassList.push_back(pPass); }
	boolean Resolve(CMGMaterialTemplate* pToNotify, uint currentFrame);
	const wchar* GetName() { return m_Name.size() > 0 ? m_Name.c_str() : NULL; }
	
	uint GetNumOfPass() { return (uint)  m_PassList.size(); }
	IMGPass* GetPass(uint index) { return m_PassList[index]; }
};

_DECLARE_SMARTPTR(CMGTechnique)

_NAMESPACE_END
