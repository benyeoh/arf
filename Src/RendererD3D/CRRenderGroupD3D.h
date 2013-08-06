//==============================================================================
//
//		CRRenderGroupD3D.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		09/07/10
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CRRenderGroupD3D : public CRRenderGroup
{
private:
	std::vector<IDirect3DSurface9Ptr> m_LastRTs;
	IDirect3DSurface9Ptr m_pLastDepthSurface;

public:
	CRRenderGroupD3D(CRRenderer* pRenderer, 
					RenderOpSort sort, 
					uint bufferSize)
		: CRRenderGroup(pRenderer, sort, bufferSize)
	{	
	}

	virtual ~CRRenderGroupD3D()
	{
	}

protected:
	void DoBeginRenderTargets(IRTextureRT** ppRT, uint numRTs);
	void DoBeginDepthBuffer(IRDepthBuffer* pDepthBuffer);

	void DoEndRenderTargets();
	void DoEndDepthBuffer();

	//void DoOnRestoreDevice() {}
	//void DoOnResetDevice() {}
	//void DoOnLostDevice() {}

	uint DoProcessInstances(RRenderOp** ppInstanceOps, uint size);

	void DoSetViewports(uint left, uint top, uint width, uint height);


	//void DoApplyIndexBuffer(CRIndexBuffer* pIB);
	//void DoApplyVertexBufferGroup(CRVertexBufferGroup* pGroup);

public:
};

_NAMESPACE_END
