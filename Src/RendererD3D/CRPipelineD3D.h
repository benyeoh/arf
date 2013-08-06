//==============================================================================
//
//		CRPipelineD3D.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Saturday, February 25, 2006
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN


class CRPipelineD3D : public CRPipeline
{
private:
	std::vector<IDirect3DSurface9Ptr> m_LastRTs;
	IDirect3DSurface9Ptr m_pLastDepthSurface;
	
public:
	CRPipelineD3D(CRRenderer* pRenderer)
		: CRPipeline(pRenderer)
	{	
	}

	virtual ~CRPipelineD3D()
	{
	}

protected:
	void DoBeginRenderTargets(IRTextureRT** ppRT, uint numRTs);
	void DoBeginDepthBuffer(IRDepthBuffer* pDepthBuffer);
	
	void DoEndRenderTargets();
	void DoEndDepthBuffer();

	void DoOnRestoreDevice() {}
	void DoOnResetDevice() {}
	void DoOnLostDevice() {}
	
	uint DoProcessInstances(RRenderOp** ppInstanceOps, uint size);
		
	//void DoApplyIndexBuffer(CRIndexBuffer* pIB);
	//void DoApplyVertexBufferGroup(CRVertexBufferGroup* pGroup);

public:
};

_NAMESPACE_END	
