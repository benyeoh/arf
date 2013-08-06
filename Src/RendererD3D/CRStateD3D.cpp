//==============================================================================
//
//		CRStateD3D.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, February 26, 2006
//
//==============================================================================

#include "RendererD3D.h"

void
CRStateD3D::DoSetAlphaEnable()
{
}

void
CRStateD3D::DoSetIndexBuffer(IRIndexBuffer* pIB)
{
	_DEBUG_ASSERT(pIB);
	
	CRIndexBufferD3D* pD3DIB = (CRIndexBufferD3D*)(pIB);
	
	_DEBUG_ASSERT(g_pRenderer);
	IDirect3DDevice9* pDevice = ((CRRendererD3D*)(g_pRenderer))->GetD3DDevice();	_DEBUG_ASSERT(pDevice);

	HRESULT hr = pDevice->SetIndices(pD3DIB->GetD3DIB());	_DEBUG_ASSERT( SUCCEEDED(hr) );	
	
	m_pIB = pD3DIB;
}

void
CRStateD3D::DoSetVertexBufferGroup(IRVertexBufferGroup* pVBGroup)
{
	_DEBUG_ASSERT(pVBGroup);
	
	CRVertexBufferGroupD3D* pD3DVBGroup = (CRVertexBufferGroupD3D*)(pVBGroup);
	IDirect3DVertexDeclaration9* pDecl = pD3DVBGroup->GetDeclD3D(); _DEBUG_ASSERT(pDecl);
	
	_DEBUG_ASSERT(g_pRenderer);
	IDirect3DDevice9* pDevice = ((CRRendererD3D*)(g_pRenderer))->GetD3DDevice();	_DEBUG_ASSERT(pDevice);
	
	// Set the vertex declaration
	HRESULT hr = pDevice->SetVertexDeclaration(pDecl);	_DEBUG_ASSERT( SUCCEEDED(hr) );
	
	uint i = 0;
	while( i < pD3DVBGroup->GetNumOfVertexBuffers() )
	{
		CRVertexBufferD3D* pVB = ((CRVertexBufferD3D*)(pD3DVBGroup->GetVertexBuffer(i)));	_DEBUG_ASSERT(pVB);
		IDirect3DVertexBuffer9* pD3DVB = pVB->GetD3DVB();		_DEBUG_ASSERT(pD3DVB);

		hr = pDevice->SetStreamSource(i, pD3DVB, pVB->GetDataStartOffset(), pVB->GetVertexSize());	_DEBUG_ASSERT( SUCCEEDED(hr) );

		++i;
	}
	
	// Reset the streams used previously
	if(m_pVBGroup)
	{
		for(uint j=i; j < m_pVBGroup->GetNumOfVertexBuffers(); ++j)
		{
			hr = pDevice->SetStreamSource(j, NULL, 0, 0);	_DEBUG_ASSERT( SUCCEEDED(hr) );
		}
	}
	
	m_pVBGroup = pD3DVBGroup;
}

void 
CRStateD3D::DoOnRestoreDevice()
{
}

void 
CRStateD3D::DoOnResetDevice()
{
	// TODO: Do we need to clear the vertex streams & declarations here to avoid memory leaks?
}

void 
CRStateD3D::DoOnLostDevice()
{
	
}

