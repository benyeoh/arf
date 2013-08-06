//==============================================================================
//
//		CRRendererD3D.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Saturday, February 11, 2006
//
//==============================================================================

#include "RendererD3D.h"


_NAMESPACE_BEGIN

//------------------------------------------------------------------------------
//	DoInitialize()
//		Initializes the D3D to the primary device given the specified
//		info
//------------------------------------------------------------------------------
boolean 
CRRendererD3D::DoInitialize(RDisplayInfo& info, RRendererInfo& rendererInfo)
{
	//m_pPipeline = _NEW CRPipelineD3D(this);
	m_pResourceMgr = _NEW CRResourceMgrD3D(this);
	m_pStateManager = _NEW RD3DEffectStateManager();

	// Get the D3D object
	IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	m_pD3D = pD3D;
	_RELEASE(pD3D);
	if(!m_pD3D)
		return FALSE;
	
	// Ensure backbuffer width and height is divisible by 2
	info.backBufferWidth += (info.backBufferWidth & 0x1);
	info.backBufferHeight += (info.backBufferHeight & 0x1);

	// Set the presentation params
	memset(&m_PresentParams, 0, sizeof(m_PresentParams));
	m_PresentParams.AutoDepthStencilFormat		= D3DFMT_D24S8;
  
    m_PresentParams.BackBufferWidth				= info.backBufferWidth;
    m_PresentParams.BackBufferHeight			= info.backBufferHeight;
    m_PresentParams.BackBufferFormat			= D3DFMT_A8R8G8B8;
	m_PresentParams.EnableAutoDepthStencil		= TRUE;
	m_PresentParams.MultiSampleType				= D3DMULTISAMPLE_NONE;
	m_PresentParams.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;
	m_PresentParams.Windowed					= !info.fullScreen;
	m_PresentParams.hDeviceWindow				= (HWND)(info.hWnd);
	m_PresentParams.SwapEffect					= D3DSWAPEFFECT_DISCARD;
	
	// Create the device
	IDirect3DDevice9* pDevice = NULL;
	// TODO: We will want to be able to choose the adapter/device as well
	if( FAILED( m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, 
									D3DDEVTYPE_HAL, 
									(HWND)(info.hWnd),
									D3DCREATE_PUREDEVICE | D3DCREATE_HARDWARE_VERTEXPROCESSING/* | D3DCREATE_FPU_PRESERVE*/, 
									&m_PresentParams, &pDevice) ) )
	{
		_DEBUG_ASSERT(FALSE);
		return FALSE;
	}
	
	m_pD3DDevice = pDevice;
	_RELEASE(pDevice);
	
	m_pStateManager->Initialize(m_pD3DDevice);
	
	rendererInfo.pixelOffset = 0.5f;
	
	return TRUE;
}

boolean 
CRRendererD3D::DoSetDisplayInfo(RDisplayInfo& info)
{
	// Ensure backbuffer width and height is divisible by 2
	info.backBufferWidth += (info.backBufferWidth & 0x1);
	info.backBufferHeight += (info.backBufferHeight & 0x1);

	// Set the presentation params
	memset(&m_PresentParams, 0, sizeof(m_PresentParams));
	m_PresentParams.AutoDepthStencilFormat		= D3DFMT_D24S8;
    
    m_PresentParams.BackBufferWidth				= info.backBufferWidth;// + (info.backBufferWidth & 0x1);
	m_PresentParams.BackBufferHeight			= info.backBufferHeight;// + (info.backBufferHeight & 0x1);
	m_PresentParams.BackBufferFormat			= D3DFMT_A8R8G8B8;
	m_PresentParams.EnableAutoDepthStencil		= TRUE;
	m_PresentParams.MultiSampleType				= D3DMULTISAMPLE_NONE;
	m_PresentParams.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;
	m_PresentParams.Windowed					= !info.fullScreen;
	m_PresentParams.hDeviceWindow				= (HWND)(info.hWnd);
	m_PresentParams.SwapEffect					= D3DSWAPEFFECT_DISCARD;
	
	return TRUE;
}

boolean 
CRRendererD3D::DoQueryDeviceReady()
{
	HRESULT hr = m_pD3DDevice->TestCooperativeLevel();
	
	switch(hr)
	{
		case D3D_OK:
			//_TRACE(_W("Query OK\n"));
			return TRUE;
		case D3DERR_DEVICENOTRESET:
			//_TRACE(_W("Query Device Not Reset\n"));
			return TRUE;
	}

	//_TRACE(_W("Query NOT READY\n"));	
	return FALSE;
}

boolean 
CRRendererD3D::DoResetDevice()
{
	if(SUCCEEDED(m_pD3DDevice->Reset(&m_PresentParams)))
	{
		_TRACE(_W(">>>>>>>>>>>>>>>> Resetted device!\n"));	
		m_pStateManager->Initialize(m_pD3DDevice);
		return TRUE;
	}
	
	//_TRACE(_W("Failed to reset device!\n"));	
	return FALSE;
}

void 
CRRendererD3D::DoClearBackBuffer(const gmtl::Vec4f* pColor, const float* pDepth)
{
	DWORD clear = 0;
	uint clearColor = 0;
	float depth = 1.0f;
	
	if(pColor)
	{
		clear |= D3DCLEAR_TARGET;
		uint red = (uint) ((*pColor)[0] * 255);
		uint green = (uint) ((*pColor)[1] * 255);
		uint blue = (uint) ((*pColor)[2] * 255);
		uint alpha = (uint) ((*pColor)[3] * 255);
		
		clearColor = (alpha << 24) | (red << 16) | (green << 8) | blue;
	}
	
	if(pDepth)
	{
		clear |= (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL);
		depth = *pDepth;
	}
	
	if(clear)
		m_pD3DDevice->Clear(0, NULL, clear, clearColor, depth, 0);
}

void 
CRRendererD3D::DoEndRender()
{
	m_pD3DDevice->EndScene();
	//m_pPrevVBGroup = NULL;
	
	_LOOPi(m_PrevNumStreams)
	{
		 m_PrevStreams[i].pVB = 0;
		 m_PrevStreams[i].flags = 0;
	}
	
	m_PrevNumStreams = 0;
	m_pPrevIB = NULL;
}

void 
CRRendererD3D::DoBeginRender()
{
	m_pD3DDevice->BeginScene();
}

void 
CRRendererD3D::DoDisplayFrame()
{
	m_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

void
CRRendererD3D::DoRenderPrimitive(RRenderOp* pRenderOp)
{
	CRIndexBufferD3D* pD3DIB = (CRIndexBufferD3D*)(pRenderOp->pIB);
	if(pD3DIB != m_pPrevIB)
	{
		HRESULT hr = m_pD3DDevice->SetIndices(pD3DIB->GetD3DIB());	
		_DEBUG_ASSERT( SUCCEEDED(hr) );	
		m_pPrevIB = pD3DIB;
	}
	
	CRVertexBufferGroupD3D* pD3DVBGroup = (CRVertexBufferGroupD3D*)(pRenderOp->pVBGroup);
	
	// Set the vertex declaration
	//if(pD3DVBGroup != m_pPrevVBGroup)
	IDirect3DVertexDeclaration9* pDecl = 
		pRenderOp->numInstances ? pD3DVBGroup->GetInstancedDeclD3D() : pD3DVBGroup->GetDeclD3D();
	if(m_pPrevDecl != pDecl)
	{
		//IDirect3DVertexDeclaration9* pDecl = pD3DVBGroup->GetDeclD3D();
		HRESULT hr = m_pD3DDevice->SetVertexDeclaration(pDecl);	
		_DEBUG_ASSERT( SUCCEEDED(hr) );
		m_pPrevDecl = pDecl;
	}
	
	uint i = 0;
	uint numVertexBuffers = pD3DVBGroup->GetNumOfVertexBuffers();
	while( i < numVertexBuffers )
	{
		CRVertexBufferD3D* pVB = ((CRVertexBufferD3D*)(pD3DVBGroup->GetVertexBuffer(i)));	
		_DEBUG_ASSERT(pVB);
		uint startVertex = pVB->GetStartVertex();
		uint vertSize = pVB->GetVertexSize();
		_DEBUG_ASSERT(vertSize < 1024);
		uint flags = (startVertex << 10) | vertSize;

		// If the last vb or the start vertex or size is not the same
		if(m_PrevStreams[i].pVB != pVB || m_PrevStreams[i].flags != flags)
		{
			IDirect3DVertexBuffer9* pD3DVB = pVB->GetD3DVB();		
			_DEBUG_ASSERT(pD3DVB);
			HRESULT hr = m_pD3DDevice->SetStreamSource(i, pD3DVB, startVertex * vertSize, vertSize);
			_DEBUG_ASSERT( SUCCEEDED(hr) );
			
			m_PrevStreams[i].pVB = pVB;
			m_PrevStreams[i].flags = flags;
		}
		
		++i;
	}

	// Reset the streams used previously
	for(uint j=i; j < m_PrevNumStreams; ++j)
	{
		HRESULT hr = m_pD3DDevice->SetStreamSource(j, NULL, 0, 0);	
		_DEBUG_ASSERT( SUCCEEDED(hr) );

		m_PrevStreams[j].pVB = NULL;
		m_PrevStreams[j].flags = 0;
	}

	m_PrevNumStreams = numVertexBuffers;

	if(pRenderOp->numInstances > 0)
	{
		// Setup D3D HW instancing
		_LOOPk(numVertexBuffers)
		{
			HRESULT hr = m_pD3DDevice->SetStreamSourceFreq(k, (D3DSTREAMSOURCE_INDEXEDDATA | pRenderOp->numInstances));
			_DEBUG_ASSERT( SUCCEEDED(hr) );
		}
		
		CRVertexBufferD3D* pInstancedVB = ((CRVertexBufferD3D*)(pD3DVBGroup->GetInstancedVB()));
		_DEBUG_ASSERT(pInstancedVB);
		
		IDirect3DVertexBuffer9* pD3DVB = pInstancedVB->GetD3DVB();		
		uint startVertex = pInstancedVB->GetStartVertex();
		uint vertSize = pInstancedVB->GetVertexSize();
	
		HRESULT hr = m_pD3DDevice->SetStreamSourceFreq(numVertexBuffers, (D3DSTREAMSOURCE_INSTANCEDATA | 1));
		_DEBUG_ASSERT( SUCCEEDED(hr) );
		hr = m_pD3DDevice->SetStreamSource(numVertexBuffers, pD3DVB, startVertex * vertSize, vertSize);
		_DEBUG_ASSERT( SUCCEEDED(hr) );
	}
	
	D3DPRIMITIVETYPE d3dPrim = PrimitiveTypeToD3D(pRenderOp->prim);
	uint numPrimitives = pD3DIB->GetNumIndices();	
	switch(pRenderOp->prim)
	{
		case PRIM_LINE:			numPrimitives >>= 1; break;
		case PRIM_TRIANGLE:		numPrimitives /= 3;	break;
		default: _DEBUG_ASSERT(FALSE);
	} 

	uint indexOffset = pD3DIB->GetStartIndex();
	uint numVertices = pD3DVBGroup->GetVertexBuffer(0)->GetNumVertices();
	HRESULT hr = m_pD3DDevice->DrawIndexedPrimitive(d3dPrim, 
													0, 
													0, 
													numVertices, 
													indexOffset, 
													numPrimitives);
													
	if(pRenderOp->numInstances > 0)
	{
		// Reset instancing streams
		_LOOPk(numVertexBuffers)
		{
			HRESULT hr = m_pD3DDevice->SetStreamSourceFreq(k, 1);
			_DEBUG_ASSERT( SUCCEEDED(hr) );
		}
		
		HRESULT hr = m_pD3DDevice->SetStreamSource(numVertexBuffers, NULL, 0, 0);
		_DEBUG_ASSERT( SUCCEEDED(hr) );
		hr = m_pD3DDevice->SetStreamSourceFreq(numVertexBuffers, 1);
		_DEBUG_ASSERT( SUCCEEDED(hr) );
	}
	
	_DEBUG_ASSERT( SUCCEEDED(hr) );
}

//void 
//CRRendererD3D::DoGetBackBuffer(IRTextureRT* pRT)
//{	
//	IDirect3DSurface9* pBBSurface = NULL;
//	CRTextureRTD3D* pD3DRT = (CRTextureRTD3D*)pRT;
//	
//	HRESULT hr = m_pD3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBBSurface);
//	_DEBUG_ASSERT(SUCCEEDED(hr));
//	
//	IDirect3DSurface9* pDest = NULL;
//	hr = pD3DRT->GetD3DTexture()->GetSurfaceLevel(0, &pDest);
//	_DEBUG_ASSERT(SUCCEEDED(hr));
//
//	hr = m_pD3DDevice->StretchRect(pBBSurface, NULL, pDest, NULL, D3DTEXF_LINEAR);
//	_DEBUG_ASSERT(SUCCEEDED(hr));
//	
//	_RELEASE(pBBSurface);
//	_RELEASE(pDest);
//}

_NAMESPACE_END