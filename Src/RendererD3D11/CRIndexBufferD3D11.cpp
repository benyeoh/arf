//==============================================================================
//
//		CRIndexBufferD3D11.cpp
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Sunday, 13 October, 2013
//
//==============================================================================

#include "RendererD3D11Framework.h"

_NAMESPACE_BEGIN

ushort* CRIndexBufferD3D11::DoLock(uint startIndexToWrite, uint numIndicesToWrite)
{
	CRRendererD3D11* pRenderer = (CRRendererD3D11*) m_pRenderer;
	ID3D11DeviceContext* pContext = pRenderer->GetCurrentContext();

	if(numIndicesToWrite == 0)
	{
		// Read required, so prepare buffer for readback
		if(!m_pOffScreen)
		{
			ID3D11Buffer* pOffscreen = pRenderer->GetResourceMgrD3D()->CreateOffscreenIB(m_Desc);
			m_pOffScreen = pOffscreen;
			pOffscreen->Release();
		}

		pContext->UpdateSubresource(m_pOffScreen, 0, NULL, m_pD3DIB, 0, 0);
	}

	byte* pToReturn;
	if(m_pOffScreen)
	{
		D3D11_MAPPED_SUBRESOURCE mapRes;
		HRESULT hr = pContext->Map(m_pOffScreen, 0, D3D11_MAP_READ_WRITE, 0, &mapRes);
		_DEBUG_ASSERT(SUCCEEDED(hr));
		pToReturn = ((byte*) mapRes.pData) + startIndexToWrite * sizeof(ushort);
		m_pLockData = pToReturn;
	}
	else
	{
		ScratchPad* pPad = pRenderer->GetScratchPad(numIndicesToWrite * sizeof(ushort));
		m_pScratchPad = pPad;
		pToReturn = (byte*) pPad->pMem;
	}

	return (ushort*) pToReturn;	
}

boolean CRIndexBufferD3D11::DoUnlock()
{
	CRRendererD3D11* pRenderer = (CRRendererD3D11*) m_pRenderer;
	ID3D11DeviceContext* pContext = pRenderer->GetCurrentContext();

	// Use resource/subresource
	D3D11_BOX startCopy;
	startCopy.left = m_TempStartIndex * sizeof(ushort);
	startCopy.right = (m_TempStartIndex + m_TempNumIndices) * sizeof(ushort);
	startCopy.top = 0;
	startCopy.bottom = 1;
	startCopy.front = 0;
	startCopy.back = 1;

	if(m_pOffScreen)
	{	
		pContext->UpdateSubresource(m_pD3DIB, 0, &startCopy, m_pLockData, 0, 0);
		
		m_pLockData = NULL;

		pContext->Unmap(m_pOffScreen, 0);
	}
	else
	{
		pContext->UpdateSubresource(m_pD3DIB, 0, &startCopy, m_pScratchPad->pMem, 0, 0);
		
		pRenderer->DisposeScratchPad(m_pScratchPad);
		m_pScratchPad = NULL;
	}

	return TRUE;
}

void CRIndexBufferD3D11::SetD3DIB(ID3D11Buffer* pD3DIB)
{
	m_pD3DIB = pD3DIB;
	m_pD3DIB->GetDesc(&m_Desc);
}

void CRIndexBufferD3D11::SetD3DSharedIB(CRIndexBufferD3D11* pParent)
{
	m_pParent = pParent;
	m_pD3DIB = pParent->GetD3DIB();
	m_pOffScreen = pParent->GetOffscreenIB();
	m_pD3DIB->GetDesc(&m_Desc);
}


_NAMESPACE_END