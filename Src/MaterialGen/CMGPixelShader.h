//==============================================================================
//
//		CMGPixelShader.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/4/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CMGPixelShader : public CRefCounted<IMGPixelShader>, public IMGShaderNode
{
private: 
	wstring m_Name;
	wstring m_VerMajor;
	wstring m_VerMinor;
	
	CMGMaterialGen* m_pGen;
	std::vector<IMGPixelInPtr> m_pPixInList;
	std::vector<IMGPixelOutPtr> m_pPixOutList;
	std::vector<IMGFunctionOpPtr> m_pOpList;
	
	uint m_CurFrame;
	boolean m_LastResult;
	
public: 
	CMGPixelShader(CMGMaterialGen* pGen, const wchar* pName)
		: m_pGen(pGen)
		, m_LastResult(FALSE)
		, m_CurFrame(0xFFFFFFFF)
	{
		_DEBUG_ASSERT(pName);
		m_Name = pName;
		m_VerMajor = _W("3");
		m_VerMinor = _W("0");
	}

	virtual ~CMGPixelShader()
	{
	}

public: 
	void AddFunctionOp(IMGFunctionOp* pOp);
	void AddPixelIn(IMGPixelIn* pIn) { m_pPixInList.push_back(pIn); }
	void AddPixelOut(IMGPixelOut* pOut) { m_pPixOutList.push_back(pOut); }
	void SetVersion(const wchar* pMajor, const wchar* pMinor) { m_VerMajor = pMajor; m_VerMinor = pMinor; }
	
	uint GetNumOfPixelIn() { return (uint) m_pPixInList.size(); }
	IMGPixelIn* GetPixelIn(uint index) { return m_pPixInList[index]; }
	uint GetNumOfPixelOut() { return (uint) m_pPixOutList.size(); }
	IMGPixelOut* GetPixelOut(uint index) { return m_pPixOutList[index]; }
	uint GetNumOfFunctionOp() { return (uint) m_pOpList.size(); }
	IMGFunctionOp* GetFunctionOp(uint index) { return m_pOpList[index]; }

	const wchar* GetVersionMajor() { return m_VerMajor.size() > 0 ? m_VerMajor.c_str() : NULL; }
	const wchar* GetVersionMinor() { return m_VerMinor.size() > 0 ? m_VerMinor.c_str() : NULL; }
	const wchar* GetName() { return m_Name.size() > 0 ? m_Name.c_str() : NULL; }
	
	IMGPixelShaderOp* CreateShaderOp();
		
	boolean Resolve(CMGMaterialTemplate* pToNotify, uint currentFrame);
};

_DECLARE_SMARTPTR(CMGPixelShader)

_NAMESPACE_END
