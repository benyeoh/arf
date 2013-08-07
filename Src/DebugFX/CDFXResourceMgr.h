//==============================================================================
//
//		CDFXResourceMgr.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/27/2008
//
//==============================================================================

#pragma once
_NAMESPACE_BEGIN

class CDFXResourceMgr : public IDFXResourceMgr
{
private: 
	DFXRenderData m_WireBox;
	DFXRenderData m_WireSphere;
	DFXRenderData m_Lines;
	DFXRenderData m_Quads;
	
	CDFXDebugFX* m_pDebugFX;
	
public: 
	CDFXResourceMgr()
		: m_pDebugFX(NULL)
	{
	}

	~CDFXResourceMgr()
	{
	}

private:
	void InitializeWireBoxData();
	void InitializeWireSphereData();
	void InitializeLineData();
	void InitializeQuadData();
	
public: 
	void SetDebugFX(CDFXDebugFX* pDebugFX) { m_pDebugFX = pDebugFX; }
	void Initialize();
	
	DFXRenderData& GetWireBoxRenderData() { return m_WireBox; }
	DFXRenderData& GetWireSphereRenderData() { return m_WireSphere; }
	DFXRenderData& GetLineRenderData() { return m_Lines; }
	DFXRenderData& GetQuadRenderData() { return m_Quads; }

};

_NAMESPACE_END
