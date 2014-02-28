//==============================================================================
//
//		CRRendererD3D.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Saturday, February 11, 2006
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

class CRRendererD3D : public CRRenderer
{
	struct VBStream 
	{
		CRVertexBufferD3D* pVB;
		uint flags;
	};

	const static uint MAX_NUM_STREAMS = 4;
	
private:
	IDirect3DDevice9Ptr	m_pD3DDevice;	
    RD3DEffectStateManagerPtr m_pStateManager;	
    CRIndexBufferD3D* m_pPrevIB;

    IDirect3DVertexDeclaration9* m_pPrevDecl;
    VBStream m_PrevStreams[MAX_NUM_STREAMS];
    uint m_PrevNumStreams;

	//CRVertexBufferGroupD3D* m_pPrevVBGroup;
	
    IDirect3D9Ptr		m_pD3D;
    D3DPRESENT_PARAMETERS m_PresentParams;

public:
	CRRendererD3D()
		: CRRenderer()
		//, m_pPrevVBGroup(NULL)
		, m_pPrevIB(NULL)
		, m_pStateManager(NULL)
		, m_PrevNumStreams(0)
		, m_pPrevDecl(0)
	{
		_LOOPi(MAX_NUM_STREAMS)
		{
			m_PrevStreams[i].pVB = NULL;
			m_PrevStreams[i].flags = 0;
		}
	}

	virtual ~CRRendererD3D()
	{
		//_DELETE(m_pStateManager);
	}
	
protected:
	//------------------------------------------------------------------------------
	//	DoInitialize()
	//		
	//------------------------------------------------------------------------------
	boolean	DoInitialize(RDisplayInfo& info, RRendererInfo& rendererInfo);
	boolean DoSetDisplayInfo(RDisplayInfo& info);
	
	boolean DoQueryDeviceReady();
	boolean DoResetDevice();
	void DoClearBackBuffer(const gmtl::Vec4f* pColor, const float* pDepth);
	void DoEndRender();
	void DoBeginRender();
	void DoDisplayFrame();
	
	void DoRenderPrimitive(RRenderOp* pRenderOp);
	
	void DoOnRestoreDevice() {}
	void DoOnResetDevice() {}
	void DoOnLostDevice() 
	{ 
		m_pPrevDecl = NULL; 
		m_pPrevIB = NULL; 
	}

public:
	//------------------------------------------------------------------------------
	//	GetD3DDevice()
	//
	//------------------------------------------------------------------------------	
	IDirect3DDevice9* GetD3DDevice() { return m_pD3DDevice; }
	RD3DEffectStateManager* GetStateManager() { return m_pStateManager; }
	CRResourceMgrD3D& GetResourceMgrD3D() { return *((CRResourceMgrD3D*) m_pResourceMgr); }
};

_NAMESPACE_END
