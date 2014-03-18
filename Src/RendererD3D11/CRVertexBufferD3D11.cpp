//==============================================================================
//
//		CRVertexBufferD3D11.cpp
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Sunday, 16 June, 2013
//
//==============================================================================

#include "RendererD3D11Framework.h"

_NAMESPACE_BEGIN

byte* CRVertexBufferD3D11::DoLock(uint startVertexToWrite, uint numVerticesToWrite)
{
	CRRendererD3D11* pRenderer = (CRRendererD3D11*) m_pRenderer;
	ID3D11DeviceContext* pContext = pRenderer->GetCurrentContext();

	if(numVerticesToWrite == 0)
	{
		// Read required, so prepare buffer for readback
		if(!m_pOffScreen)
		{
			ID3D11Buffer* pOffscreen = pRenderer->GetResourceMgrD3D()->CreateOffscreenVB(m_Desc);
			m_pOffScreen = pOffscreen;
			pOffscreen->Release();
		}

		pContext->UpdateSubresource(m_pOffScreen, 0, NULL, m_pD3DVB, 0, 0);
	}

	byte* pToReturn;
	if(m_pOffScreen)
	{
		D3D11_MAPPED_SUBRESOURCE mapRes;
		HRESULT hr = pContext->Map(m_pOffScreen, 0, D3D11_MAP_READ_WRITE, 0, &mapRes);
		_DEBUG_ASSERT(SUCCEEDED(hr));
		pToReturn = ((byte*) mapRes.pData) + startVertexToWrite * m_VertexSize;
		m_pLockData = pToReturn;
	}
	else
	{
		ScratchPad* pPad = pRenderer->GetScratchPad(numVerticesToWrite * m_VertexSize);
		m_pScratchPad = pPad;
		pToReturn = (byte*) pPad->pMem;
	}

	return pToReturn;	
}

boolean CRVertexBufferD3D11::DoUnlock()
{
	CRRendererD3D11* pRenderer = (CRRendererD3D11*) m_pRenderer;
	ID3D11DeviceContext* pContext = pRenderer->GetCurrentContext();

	// Use resource/subresource
	D3D11_BOX startCopy;
	startCopy.left = m_TempStartVertex * m_VertexSize;
	startCopy.right = (m_TempStartVertex + m_TempNumVertices) * m_VertexSize;
	startCopy.top = 0;
	startCopy.bottom = 1;
	startCopy.front = 0;
	startCopy.back = 1;

	if(m_pOffScreen)
	{	
		pContext->UpdateSubresource(m_pD3DVB, 0, &startCopy, m_pLockData, 0, 0);
		
		m_pLockData = NULL;

		pContext->Unmap(m_pOffScreen, 0);
	}
	else
	{
		pContext->UpdateSubresource(m_pD3DVB, 0, &startCopy, m_pScratchPad->pMem, 0, 0);		
		pRenderer->DisposeScratchPad(m_pScratchPad);
		m_pScratchPad = NULL;
	}

	return TRUE;
}

byte* CRVertexBufferD3D11::DoLockImmediate(uint startVertexToWrite, uint numVerticesToWrite)
{
	CRRendererD3D11* pRenderer = (CRRendererD3D11*) m_pRenderer;

	if(m_Desc.Usage & D3D11_USAGE_DYNAMIC)
	{
		D3D11_MAP lockFlags;
		uint endVertex = m_StartVertex + m_NumVertices;

		if((startVertexToWrite >= endVertex))
		{
			lockFlags = D3D11_MAP_WRITE_NO_OVERWRITE;
		}
		else
		{
			lockFlags = D3D11_MAP_WRITE_DISCARD;
		}

		_DEBUG_ASSERT(((numVerticesToWrite + startVertexToWrite) * m_VertexSize) <= (m_Size * m_VertexSize));

		ID3D11DeviceContext* pContext = pRenderer->GetCurrentContext();

		D3D11_MAPPED_SUBRESOURCE mappedData;
		HRESULT hr = pContext->Map(m_pD3DVB, 0, lockFlags, 0, &mappedData);
		_DEBUG_ASSERT(SUCCEEDED(hr));

		return (byte*) mappedData.pData + startVertexToWrite * m_NumVertices;
	}
	else
	{
		// Use resource/subresource
		m_pScratchPad = pRenderer->GetScratchPad(numVerticesToWrite * m_VertexSize);
		return (byte*) m_pScratchPad->pMem;
	}

	return NULL;
}

boolean CRVertexBufferD3D11::DoUnlockImmediate()
{
	CRRendererD3D11* pRenderer = (CRRendererD3D11*) m_pRenderer;
	ID3D11DeviceContext* pContext = pRenderer->GetCurrentContext();

	if(m_Desc.Usage & D3D11_USAGE_DYNAMIC)
	{
		pContext->Unmap(m_pD3DVB, 0);
	}
	else
	{
		// Use resource/subresource
		D3D11_BOX startCopy;
		startCopy.left = m_TempStartVertex * m_VertexSize;
		startCopy.right = (m_TempStartVertex + m_TempNumVertices) * m_VertexSize;
		startCopy.top = 0;
		startCopy.bottom = 1;
		startCopy.front = 0;
		startCopy.back = 1;

		pContext->UpdateSubresource(m_pD3DVB, 0, &startCopy, m_pScratchPad->pMem, 0, 0);
		
		pRenderer->DisposeScratchPad(m_pScratchPad);
		m_pScratchPad = NULL;
	}

	return TRUE;
}

void CRVertexBufferD3D11::SetD3DVB(ID3D11Buffer* pD3DVB)
{
	m_pD3DVB = pD3DVB;
	m_pD3DVB->GetDesc(&m_Desc);
}

void CRVertexBufferD3D11::SetD3DSharedVB(CRVertexBufferD3D11* pParent)
{
	m_pParent = pParent;
	m_pD3DVB = pParent->GetD3DVB();
	m_pOffScreen = pParent->GetOffscreenVB();
	m_pD3DVB->GetDesc(&m_Desc);
}


_NAMESPACE_END