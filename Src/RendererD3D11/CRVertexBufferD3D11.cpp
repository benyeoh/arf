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
	byte* pBuf = NULL;

	CRRendererD3D11* pRenderer = (CRRendererD3D11*) m_pRenderer;

	if(pToWrite)
	{
		ID3D11DeviceContext* pContext = pRenderer->GetCurrentContext();

		// We need an offscreen buffer if app needs to read data
		// or app is only updating a portion
		if( ((*pToWrite)[2] <= 0) || ((*pToWrite)[3] <= 0) || (m_Desc.Usage & D3D11_USAGE_DEFAULT) )
		{
			if(!m_pOffscreen)
			{
				// TODO: Create a staging resource if we do not have

				// Read back from GPU and wait... Super slow... Avoid
				pContext->CopyResource(m_pOffscreen, m_pD3DTexture);
			}
		}

		m_WriteOffset = *pToWrite;
	}
	else
	{
		m_WriteOffset[0] = m_WriteOffset[1] = 0;
		m_WriteOffset[2] = m_Desc.Width >> level;
		m_WriteOffset[3] = m_Desc.Height >> level;
		m_WriteOffset[2] = m_WriteOffset[2] <= 0 ? 1 : m_WriteOffset[2];
		m_WriteOffset[3] = m_WriteOffset[3] <= 0 ? 1 : m_WriteOffset[3];
	}

	if(m_pOffscreen)
	{
		// Must write to offscreen buffer first if it already exists
		// It will exist if the buffer is dynamic and we are not writing the whole buffer
		// Otherwise it will exist if we attempt to do a read

		// WARNING: Do we need an immediate context?
		ID3D11DeviceContext* pImmedContext = pRenderer->GetImmediateContext();

		HRESULT hr = pImmedContext->Map(m_pOffscreen, level, D3D11_MAP_READ_WRITE, 0, &m_OffscreenData);
		_DEBUG_ASSERT(hr == S_OK);
		pBuf = m_OffscreenData.pSysMem;
		pitch = m_OffscreenData.SysMemPitch;
	}
	else
	{
		// Skip offscreen buffer
		if(m_Desc.Usage & D3D11_USAGE_DEFAULT)
		{
			// Make sure that we are writing the whole buffer
			_DEBUG_ASSERT(!pToWrite);
			pBuf = MapDirectResource(level, pitch);
		}
		else
		{
			pBuf = MapProxyResource(level, pitch);
		}
	}

	return pBuf;	
}

boolean CRVertexBufferD3D11::DoUnlock()
{

}

byte* CRVertexBufferD3D11::DoLockImmediate(uint startVertexToWrite, uint numVerticesToWrite)
{
	D3D11_MAP lockFlags;
	if(m_Desc.Usage & D3D11_USAGE_DYNAMIC)
	{
		uint tempEndVertex = startVertexToWrite + numVerticesToWrite;
		uint endVertex = m_StartVertex + m_NumVertices;

		if((startVertexToWrite >= endVertex))
		{
			lockFlags = D3D11_MAP_WRITE_NO_OVERWRITE;
		}
		else
		{
			lockFlags = D3D11_MAP_WRITE_DISCARD;
		}
	}
	else
		lockFlags = = D3D11_MAP_WRITE;

	_DEBUG_ASSERT(((numVerticesToWrite + startVertexToWrite) * m_VertexSize) <= (m_Size * m_VertexSize));

	CRRendererD3D11* pRenderer = (CRRendererD3D11*) m_pRenderer;
	ID3D11DeviceContext* pContext = pRenderer->GetCurrentContext();

	D3D11_SUBRESOURCE_DATA mappedData;
	HRESULT hr = pContext->Map(m_pD3DVB, 0, lockFlags, 0, &mappedData);
	_DEBUG_ASSERT(SUCCEEDED(hr));
	
	return (byte*) mappedData.pSysMem;
}

boolean CRVertexBufferD3D11::DoUnlockImmediate()
{

}

void CRVertexBufferD3D11::SetD3DVB(IDirect3DVertexBuffer9* pD3DVB, IByteBuffer* pOffScreen)
{

}

void CRVertexBufferD3D11::SetD3DSharedVB(CRVertexBufferD3D* pParent)
{

}


_NAMESPACE_END