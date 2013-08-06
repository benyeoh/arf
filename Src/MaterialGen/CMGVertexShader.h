//==============================================================================
//
//		CMGVertexShader.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/4/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CMGVertexShader : public CRefCounted<IMGVertexShader>, public IMGShaderNode
{
private: 
	wstring m_Name;
	wstring m_VerMajor;
	wstring m_VerMinor;
	
	CMGMaterialGen* m_pGen;
	std::vector<IMGVertexInPtr> m_pVertInList;
	std::vector<IMGVertexOutPtr> m_pVertOutList;
	std::vector<IMGFunctionOpPtr> m_pOpList;

	uint m_CurFrame;
	boolean m_LastResult;
		
public: 
	CMGVertexShader(CMGMaterialGen* pGen, const wchar* pName)
		: m_pGen(pGen)
		, m_CurFrame(0xFFFFFFFF)
		, m_LastResult(FALSE)
	{
		_DEBUG_ASSERT(pName);
		m_Name = pName;
		m_VerMajor = _W("3");
		m_VerMinor = _W("0");
	}

	virtual ~CMGVertexShader()
	{
	}

public: 
	void AddFunctionOp(IMGFunctionOp* pOp);
	void AddVertexIn(IMGVertexIn* pIn) { m_pVertInList.push_back(pIn); }
	void AddVertexOut(IMGVertexOut* pOut) { m_pVertOutList.push_back(pOut); }
	void SetVersion(const wchar* pMajor, const wchar* pMinor) { m_VerMajor = pMajor; m_VerMinor = pMinor; }

	const wchar* GetVersionMajor() { return m_VerMajor.size() > 0 ? m_VerMajor.c_str() : NULL; }
	const wchar* GetVersionMinor() { return m_VerMinor.size() > 0 ? m_VerMinor.c_str() : NULL; }
	const wchar* GetName() { return m_Name.size() > 0 ? m_Name.c_str() : NULL; }
	
	uint GetNumOfVertexIn() { return (uint) m_pVertInList.size(); }
	IMGVertexIn* GetVertexIn(uint index) { return m_pVertInList[index]; }
	uint GetNumOfVertexOut() { return (uint) m_pVertOutList.size(); }
	IMGVertexOut* GetVertexOut(uint index) { return m_pVertOutList[index]; }
	uint GetNumOfFunctionOp() { return (uint) m_pOpList.size(); }
	IMGFunctionOp* GetFunctionOp(uint index) { return m_pOpList[index]; }
	
	IMGVertexShaderOp* CreateShaderOp();
	boolean Resolve(CMGMaterialTemplate* pToNotify, uint currentFrame);
};

_DECLARE_SMARTPTR(CMGVertexShader)

_NAMESPACE_END
