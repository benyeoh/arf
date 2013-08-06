//==============================================================================
//
//		CRPipelineD3D.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Saturday, February 25, 2006
//
//==============================================================================

#include "RendererD3D.h"


_NAMESPACE_BEGIN

uint
CRPipelineD3D::DoProcessInstances(RRenderOp** ppInstanceOps, uint size)
{
	RRenderOp* pRefOp = ppInstanceOps[0];
	((CREffectD3DTemplate*)pRefOp->pEffectTemplate)->SetupInstancing(
		(CRVertexBufferGroupD3D*)pRefOp->pVBGroup, pRefOp->technique);
	CRVertexBufferD3D* pInstVB = ((CRVertexBufferGroupD3D*)pRefOp->pVBGroup)->GetInstancedVB();
	_DEBUG_ASSERT(pInstVB);
	
	// Lock as appropriate and copy instanced data
	uint maxVertices	= pInstVB->GetSize();
	uint curStartVertex	= pInstVB->GetStartVertex() + pInstVB->GetNumVertices();
	uint sizeLeft		= maxVertices - curStartVertex;
	if(size > sizeLeft)
	{
		// Reset
		curStartVertex = 0;
		if(size > maxVertices)
			size = maxVertices;
	}
	
	uint instParamIndex = ((CREffectD3DTemplate*)pRefOp->pEffectTemplate)->GetNumOfDynamicParams(pRefOp->technique);
	
	byte* pVBData = pInstVB->LockImmediate(curStartVertex, size);
	_LOOPi(size)
	{
		byte* pStartVBData = pVBData;
		_DEBUG_ASSERT(IsAlignedToXBytes(pStartVBData, 4));
		
		RRenderOp* pCurOp = ppInstanceOps[i];
		_DEBUG_ASSERT(pCurOp->numInstancedParams == pRefOp->numInstancedParams);
		
		_LOOPj(pRefOp->numInstancedParams)
		{
			REffectParam& instParam = pCurOp->params[instParamIndex+j];
			switch(instParam.type)
			{
				case EPT_VEC4_P:
					*((gmtl::Vec4f*) pVBData) = *(instParam.pVec4);
					pVBData += sizeof(gmtl::Vec4f);
					break;
				case EPT_VEC3_P:		
					*((gmtl::Vec3f*) pVBData) = *(instParam.pVec3);
					pVBData += sizeof(gmtl::Vec3f);
					break;
				case EPT_VEC2_P:		
					*((gmtl::Vec2f*) pVBData) = *(instParam.pVec2);
					pVBData += sizeof(gmtl::Vec2f);
					break;
				case EPT_FLOAT:		
					*((float*) pVBData) = (instParam.floatVal);
					pVBData += sizeof(float);
					break;				
				case EPT_MAT44_P:
					*((gmtl::Matrix44f*) pVBData) = *(instParam.pMat44);
					pVBData += sizeof(gmtl::Matrix44f);
					break;
				case EPT_MAT33_P:
					*((gmtl::Matrix33f*) pVBData) = *(instParam.pMat33);
					pVBData += sizeof(gmtl::Matrix33f);
					break;					
				case EPT_MAT22_P:
					*((gmtl::Matrix22f*) pVBData) = *(instParam.pMat22);
					pVBData += sizeof(gmtl::Matrix22f);
					break;
					
				default: _DEBUG_ASSERT(FALSE); break;
			}
		}
		
		_DEBUG_ASSERT((uint)(pVBData - pStartVBData) == pInstVB->GetVertexSize());
	}
	
	pInstVB->UnlockImmediate(TRUE);
	
	return size;
}

void 
CRPipelineD3D::DoBeginRenderTargets(IRTextureRT** ppRT, uint numRTs)
{
	IDirect3DDevice9* pDevice = ((CRRendererD3D*)m_pRenderer)->GetD3DDevice();
	
	_LOOPi(numRTs)
	{
		IDirect3DSurface9* pLastRT = NULL;
		HRESULT hr = pDevice->GetRenderTarget(i, &pLastRT);
		//_DEBUG_ASSERT(SUCCEEDED(hr));
		
		m_LastRTs.push_back(pLastRT);
		
		CRTextureRTD3D** ppRTD3D = (CRTextureRTD3D**) ppRT;
		IDirect3DSurface9* pRTSurface = NULL;
		hr = ppRTD3D[i]->GetD3DTexture()->GetSurfaceLevel(0, &pRTSurface);
		_DEBUG_ASSERT(SUCCEEDED(hr));
		
		hr = pDevice->SetRenderTarget(i, pRTSurface);
		_DEBUG_ASSERT(SUCCEEDED(hr));
		
		_RELEASE(pLastRT);
		_RELEASE(pRTSurface);
	}
}

void 
CRPipelineD3D::DoBeginDepthBuffer(IRDepthBuffer* pDepthBuffer)
{
	if(pDepthBuffer)
	{
		IDirect3DDevice9* pDevice = ((CRRendererD3D*)m_pRenderer)->GetD3DDevice();
		IDirect3DSurface9* pLastDepthSurface = NULL;
		HRESULT hr = pDevice->GetDepthStencilSurface(&pLastDepthSurface);
		_DEBUG_ASSERT(SUCCEEDED(hr));
		
		m_pLastDepthSurface = pLastDepthSurface;
		
		IDirect3DSurface9* pDepthSurface = ((CRDepthBufferD3D*) pDepthBuffer)->GetDepthStencilSurface();		
		hr = pDevice->SetDepthStencilSurface(pDepthSurface);				
		_DEBUG_ASSERT(SUCCEEDED(hr));

		_RELEASE(pLastDepthSurface);
	}
}

void 
CRPipelineD3D::DoEndRenderTargets()
{
	const static uint STAGES_TO_CLEAR = 16;
	
	// NOTE: We clear the texture states here as well (to avoid D3D warnings)
	RD3DEffectStateManager* pStateManager = ((CRRendererD3D*)m_pRenderer)->GetStateManager();
	_LOOPi(STAGES_TO_CLEAR)
	{
		pStateManager->SetTexture(i, NULL);
	}
	
	IDirect3DDevice9* pDevice = ((CRRendererD3D*)m_pRenderer)->GetD3DDevice();

	uint numRTs = (uint) m_LastRTs.size();
	_LOOPi(numRTs)
	{
		HRESULT hr = pDevice->SetRenderTarget(i, m_LastRTs[i]);
		_DEBUG_ASSERT(SUCCEEDED(hr));
	}
	
	m_LastRTs.clear();
}

void 
CRPipelineD3D::DoEndDepthBuffer()
{
	if(m_pLastDepthSurface)
	{
		IDirect3DDevice9* pDevice = ((CRRendererD3D*)m_pRenderer)->GetD3DDevice();
		HRESULT hr = pDevice->SetDepthStencilSurface(m_pLastDepthSurface);				
		_DEBUG_ASSERT(SUCCEEDED(hr));
	}
	
	m_pLastDepthSurface = NULL;
}


//void 
//CRPipelineD3D::DoApplyIndexBuffer(CRIndexBuffer* pIB)
//{
//	_DEBUG_ASSERT(pIB);
//
//	CRIndexBufferD3D* pD3DIB = (CRIndexBufferD3D*)(pIB);
//
//	_DEBUG_ASSERT(m_pRenderer);
//	IDirect3DDevice9* pDevice = ((CRRendererD3D*)(m_pRenderer))->GetD3DDevice();	_DEBUG_ASSERT(pDevice);
//
//	HRESULT hr = pDevice->SetIndices(pD3DIB->GetD3DIB());	_DEBUG_ASSERT( SUCCEEDED(hr) );	
//}
//
//void 
//CRPipelineD3D::DoApplyVertexBufferGroup(CRVertexBufferGroup* pGroup)
//{
//	_DEBUG_ASSERT(pGroup);
//
//	CRVertexBufferGroupD3D* pD3DVBGroup = (CRVertexBufferGroupD3D*)(pGroup);
//	IDirect3DVertexDeclaration9* pDecl = pD3DVBGroup->GetDeclD3D(); _DEBUG_ASSERT(pDecl);
//
//	_DEBUG_ASSERT(m_pRenderer);
//	IDirect3DDevice9* pDevice = ((CRRendererD3D*)(m_pRenderer))->GetD3DDevice();	_DEBUG_ASSERT(pDevice);
//
//	// Set the vertex declaration
//	HRESULT hr = pDevice->SetVertexDeclaration(pDecl);	_DEBUG_ASSERT( SUCCEEDED(hr) );
//
//	uint i = 0;
//	while( i < pD3DVBGroup->GetNumOfVertexBuffers() )
//	{
//		CRVertexBufferD3D* pVB = ((CRVertexBufferD3D*)(pD3DVBGroup->GetVertexBuffer(i)));	_DEBUG_ASSERT(pVB);
//		IDirect3DVertexBuffer9* pD3DVB = pVB->GetD3DVB();		_DEBUG_ASSERT(pD3DVB);
//
//		hr = pDevice->SetStreamSource(i, pD3DVB, pVB->GetDataStartOffset(), pVB->GetVertexSize());	_DEBUG_ASSERT( SUCCEEDED(hr) );
//
//		++i;
//	}
//
//	// Reset the streams used previously
//	for(uint j=i; j < m_PrevStream; ++j)
//	{
//		hr = pDevice->SetStreamSource(j, NULL, 0, 0);	_DEBUG_ASSERT( SUCCEEDED(hr) );
//	}
//
//	m_PrevStream = i;
//}


_NAMESPACE_END