//==============================================================================
//
//		CRTexture3DD3D11.cpp
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Monday, 27 January, 2014
//
//==============================================================================

#include "RendererD3D11Framework.h"

_NAMESPACE_BEGIN

byte* CRTexture3DD3D11::MapDirectResource(uint level, uint& pitch, uint& slicePitch)
{
	byte* pBuf;

	// Directly access GPU resource with Map
	CRRendererD3D11* pRenderer = (CRRendererD3D11*) m_pRenderer;
	ID3D11DeviceContext* pContext = pRenderer->GetCurrentContext();

	D3D11_MAPPED_SUBRESOURCE subRes;
	UINT subResIndex = D3D11CalcSubresource(level, 0, m_Desc.MipLevels);
	HRESULT hr = pContext->Map(m_pD3DTexture, subResIndex, D3D11_MAP_WRITE_DISCARD, 0, &subRes);
	_DEBUG_ASSERT(hr == S_OK);
	pBuf = (byte*) subRes.pData;
	pitch = subRes.RowPitch;
	slicePitch = subRes.DepthPitch;

	return pBuf;
}

void CRTexture3DD3D11::UnmapDirectResource(uint level)
{
	// Directly access GPU resource with Map
	CRRendererD3D11* pRenderer = (CRRendererD3D11*) m_pRenderer;
	ID3D11DeviceContext* pContext = pRenderer->GetCurrentContext();
	UINT subResIndex = D3D11CalcSubresource(level, 0, m_Desc.MipLevels);

	pContext->Unmap(m_pD3DTexture, subResIndex);
}

byte* CRTexture3DD3D11::MapProxyResource(uint level, uint& outPitch, uint& outSlicePitch)
{
	byte* pBuf;

	CRRendererD3D11* pRenderer = (CRRendererD3D11*) m_pRenderer;
	//ID3D11DeviceContext* pContext = pRenderer->GetCurrentContext();

	uint width = (m_Desc.Width >> level);
	uint height = (m_Desc.Height >> level);
	uint depth = (m_Desc.Depth >> level);
	height = height > 0 ? height : 1;
	width = width > 0 ? width : 1;
	depth = depth > 0 ? depth : 1;
	uint pitch = width * GetSizePerElem();
	uint numRows = height;
	if(IsBlockCompressed(m_Desc.Format))
	{
		pitch = ((width + 3) >> 2) * GetSizePerElem();
		numRows = height >> 2;
		numRows = numRows > 0 ? numRows : 1;
	}

	ScratchPad* pPad = pRenderer->GetScratchPad(numRows * pitch * depth);
	pBuf = (byte*) pPad->pMem;
	outPitch = pitch;
	outSlicePitch = pitch * numRows;

	return pBuf;
}

void CRTexture3DD3D11::UnmapProxyResource(uint level)
{
	_DEBUG_ASSERT(m_pLockPad);
	CRRendererD3D11* pRenderer = (CRRendererD3D11*) m_pRenderer;
	ID3D11DeviceContext* pContext = pRenderer->GetCurrentContext();

	D3D11_BOX updateBox;
	updateBox.left = m_WriteStart[0];
	updateBox.right = updateBox.left + m_WriteEnd[0];
	updateBox.top = m_WriteStart[1];
	updateBox.bottom = updateBox.top + m_WriteEnd[1];
	updateBox.front = m_WriteStart[2];
	updateBox.back = updateBox.front + m_WriteEnd[2];

	D3D11_BOX alignBox = updateBox;

	uint width = (m_Desc.Width >> level);
	uint height = (m_Desc.Height >> level);
	uint depth = (m_Desc.Depth >> level);
	height = height > 0 ? height : 1;
	width = width > 0 ? width : 1;
	depth = depth > 0 ? depth : 1;
	uint pitch = width * GetSizePerElem();
	uint numRows = height;
	if(IsBlockCompressed(m_Desc.Format))
	{
		pitch = ((width + 3) >> 2) * GetSizePerElem();
		numRows = height >> 2;
		numRows = numRows > 0 ? numRows : 1;
		alignBox.left >>= 2;
		alignBox.right >>= 2;
		alignBox.top >>= 2;
		alignBox.bottom >>= 2;
	}

	_DEBUG_ASSERT(m_pLockPad->size >= (numRows * pitch * depth));

	// Offset src data to match dest box
	byte* pSrcData = ((byte*)m_pLockPad->pMem) + alignBox.left * GetSizePerElem() + alignBox.top * pitch + alignBox.front * numRows * pitch;
	UINT subResIndex = D3D11CalcSubresource(level, 0, m_Desc.MipLevels);
	
	pContext->UpdateSubresource(m_pD3DTexture, subResIndex, &updateBox, pSrcData, pitch, numRows * pitch);
	pRenderer->DisposeScratchPad(m_pLockPad);
	m_pLockPad = NULL;		
}

boolean CRTexture3DD3D11::DoUnlock(uint level)
{
	CRRendererD3D11* pRenderer = (CRRendererD3D11*) m_pRenderer;
	ID3D11DeviceContext* pContext = pRenderer->GetCurrentContext();

	if(m_pOffscreen)
	{
		// Now copy if there was any writing done
		int xDiff = m_WriteEnd[0] - m_WriteStart[0];
		int yDiff = m_WriteEnd[1] - m_WriteStart[1];
		int zDiff = m_WriteEnd[2] - m_WriteStart[2];

		if((xDiff > 0) && (yDiff > 0) && (zDiff > 0))
		{
			if(m_Desc.Usage & D3D11_USAGE_DYNAMIC)
			{
				// TODO: Beware of dynamic flag
				// Basically with dynamic flag the entire buffer is discarded
				// so locking a portion is not supported

				// If dynamic then Map and memcpy entire buffer with D3D11_MAP_WRITE_DISCARD

				// Lock render resource
				uint pitch;
				uint slicePitch;
				byte* pBuf = MapDirectResource(level, pitch, slicePitch);

				_DEBUG_ASSERT(m_OffscreenData.pData);
				_DEBUG_ASSERT(m_OffscreenData.RowPitch == pitch);
				_DEBUG_ASSERT(m_OffscreenData.DepthPitch == slicePitch);

				// Copy the app results
				uint height = (m_Desc.Height >> level);
				height = height > 0 ? height : 1;
				uint numRows = height;
				uint depth = m_Desc.Depth >> level;
				depth = depth > 0 ? depth : 1;
				if(IsBlockCompressed(m_Desc.Format))
				{
					numRows = height >> 2;
					numRows = numRows > 0 ? numRows : 1;
				}

				memcpy(pBuf, m_OffscreenData.pData, numRows * pitch * depth);

				// Unlock
				UnmapDirectResource(level);

				// WARNING: Do we need an immediate context?
				// Apparently, only MAP_DISCARD is allowed for deferred contexts
				UINT subResIndex = D3D11CalcSubresource(level, 0, m_Desc.MipLevels);
				pContext->Unmap(m_pOffscreen, subResIndex);

				m_OffscreenData.pData = NULL;
				m_OffscreenData.RowPitch = 0;

			}
			else
			{
				// WARNING: Do we need an immediate context?
				// Apparently, only MAP_DISCARD is allowed for deferred contexts
				UINT subResIndex = D3D11CalcSubresource(level, 0, m_Desc.MipLevels);
				pContext->Unmap(m_pOffscreen, subResIndex);

				m_OffscreenData.pData = NULL;
				m_OffscreenData.RowPitch = 0;

				D3D11_BOX updateBox;
				updateBox.left = m_WriteStart[0];
				updateBox.right = updateBox.left + m_WriteEnd[0];
				updateBox.top = m_WriteStart[1];
				updateBox.bottom = updateBox.top + m_WriteEnd[1];
				updateBox.front = m_WriteStart[2];
				updateBox.back = updateBox.front + m_WriteEnd[2];

				pContext->CopySubresourceRegion(m_pD3DTexture, subResIndex, updateBox.left, updateBox.top, 0, m_pOffscreen, subResIndex, &updateBox);
			}
		}

	}
	else
	{
		// We have no offscreen buffer
		// Which means the buffer is either not dynamic or we are writing the whole buffer
		if(m_Desc.Usage & D3D11_USAGE_DYNAMIC)
		{
			UnmapDirectResource(level);
		}
		else
		{
			UnmapProxyResource(level);
		}
	}

	return TRUE;
}

byte* CRTexture3DD3D11::DoLock(uint level, uint& rowPitch, uint& slicePitch, const gmtl::Vec3i* pToWriteStart, const gmtl::Vec3i* pToWriteEnd)
{		
	byte* pBuf = NULL;

	CRRendererD3D11* pRenderer = (CRRendererD3D11*) m_pRenderer;
	ID3D11DeviceContext* pContext = pRenderer->GetCurrentContext();

	if(pToWriteStart && pToWriteEnd)
	{
		int width = (*pToWriteEnd)[0] - (*pToWriteStart)[0];
		int height = (*pToWriteEnd)[1] - (*pToWriteStart)[1];
		int depth = (*pToWriteEnd)[2] - (*pToWriteStart)[2];

		// We need an offscreen buffer if app needs to read data
		// or app is only updating a portion and texture is dynamic
		if( (width <= 0) || (height <= 0) || || (depth <= 0) || (m_Desc.Usage & D3D11_USAGE_DYNAMIC) )
		{
			if(!m_pOffscreen)
			{
				// Create a staging resource if we do not have
				ID3D11Texture3D* pOffscreen = pRenderer->GetResourceMgrD3D()->CreateOffscreenTexture3D(m_Desc);
				m_pOffscreen = pOffscreen;
				pOffscreen->Release();

				// Read back from GPU and wait... Super slow... Avoid
				pContext->CopyResource(m_pOffscreen, m_pD3DTexture);
			}
		}

		m_WriteStart = *pToWriteStart;
		m_WriteEnd = *pToWriteEnd;
	}
	else
	{
		m_WriteStart[0] = 0;
		m_WriteStart[1] = 0;
		m_WriteStart[2] = 0;

		m_WriteEnd[0] = (m_Desc.Width >> level) > 0 ? (m_Desc.Width >> level) : 1;
		m_WriteEnd[1] = (m_Desc.Height >> level) > 0 ? (m_Desc.Height >> level) : 1;
		m_WriteEnd[2] = (m_Desc.Depth >> level) > 0 ? (m_Desc.Depth >> level) : 1;
	}

	if(m_pOffscreen)
	{
		// Must write to offscreen buffer first if it already exists
		// It will exist if the buffer is dynamic and we are not writing the whole buffer
		// Otherwise it will exist if we attempt to do a read

		// WARNING: Do we need an immediate context?
		// Apparently, only MAP_DISCARD is allowed for deferred contexts	

		UINT subResIndex = D3D11CalcSubresource(level, 0, m_Desc.MipLevels);
		HRESULT hr = pContext->Map(m_pOffscreen, subResIndex, D3D11_MAP_READ_WRITE, 0, &m_OffscreenData);
		_DEBUG_ASSERT(hr == S_OK);
		pBuf = (byte*) m_OffscreenData.pData;
		rowPitch = m_OffscreenData.RowPitch;
		slicePitch = m_OffscreenData.DepthPitch;
	}
	else
	{
		// Skip offscreen buffer
		if(m_Desc.Usage & D3D11_USAGE_DYNAMIC)
		{
			// Make sure that we are writing the whole buffer
			_DEBUG_ASSERT(!pToWriteStart || !pToWriteEnd);
			pBuf = MapDirectResource(level, pitch, slicePitch);
		}
		else
		{
			pBuf = MapProxyResource(level, pitch, slicePitch);
		}
	}

	return pBuf;			
}

byte* CRTexture3DD3D11::DoLockImmediate(uint level, uint& rowPitch, uint& slicePitch)
{
	byte* pBuf;

	if(m_Desc.BindFlags & D3D11_USAGE_DYNAMIC)
	{
		pBuf = MapDirectResource(level, rowPitch, slicePitch);
	}
	else
	{
		m_WriteStart[0] = 0;
		m_WriteStart[1] = 0;
		m_WriteStart[2] = 0;

		m_WriteEnd[0] = (m_Desc.Width >> level) > 0 ? (m_Desc.Width >> level) : 1;
		m_WriteEnd[1] = (m_Desc.Height >> level) > 0 ? (m_Desc.Height >> level) : 1;
		m_WriteEnd[2] = (m_Desc.Depth >> level) > 0 ? (m_Desc.Depth >> level) : 1;

		pBuf = MapProxyResource(level, rowPitch, slicePitch);
	}

	return pBuf;		
}

boolean CRTexture3DD3D11::DoUnlockImmediate(uint level)
{
	if(m_Desc.BindFlags & D3D11_USAGE_DYNAMIC)
	{
		UnmapDirectResource(level);
	}
	else
	{
		UnmapProxyResource(level);
	}

	return TRUE;
}


uint CRTexture3DD3D11::DoGetMipMapLevels() const
{
	return (uint) m_Desc.MipLevels;
}

uint CRTexture3DD3D11::DoGetWidth(uint level) const
{
	uint width = m_Desc.Width >> level;
	width = width > 0 ? width : 1;
	return width;
}

uint CRTexture3DD3D11::DoGetHeight(uint level) const
{
	uint height = m_Desc.Height >> level;
	height = height > 0 ? height: 1;
	return height;
}

uint CRTexture3DD3D11::DoGetDepth(uint level) const
{
	uint depth = m_Desc.Depth >> level;
	depth = depth > 0 ? depth: 1;
	return depth;
}

void CRTexture3DD3D11::SetD3DTexture(ID3D11Texture3D* pTex, ID3D11Texture3D* pOffScreen)
{
	CRRendererD3D11* pRenderer = (CRRendererD3D11*) m_pRenderer;
	ID3D11Device* pDevice = pRenderer->GetDevice();

	_DEBUG_ASSERT(pTex);

	m_pD3DTexture = pTex;
	m_pOffscreen = pOffScreen;
	pTex->GetDesc(&m_Desc);

	ID3D11ShaderResourceView* pSRV;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;

	DXGI_FORMAT srvFormat = m_Desc.Format;
	if(TexFormatFromD3D11Typeless(m_Desc.Format) != TEXF_END)
		srvFormat = TexFormatToD3D11( TexFormatFromD3D11Typeless(m_Desc.Format) );

	srvDesc.Format = srvFormat;
	srvDesc.Texture3D.MipLevels = m_Desc.MipLevels;
	srvDesc.Texture3D.MostDetailedMip = 0;
	
	HRESULT hr = pDevice->CreateShaderResourceView(m_pD3DTexture, &srvDesc, &pSRV);
	_DEBUG_ASSERT(SUCCEEDED(hr));
	m_pSRV = pSRV;
	pSRV->Release();
}

CRTextureRT* CRTexture3DD3D11::DoGetTextureRT(uint mipLevel, uint startSlice, uint numSlices)
{
	uint entryHash = (mipLevel << 24) | (startSlice << 12) | numSlices;
	CRTextureRTD3D11Ptr* ppRT = m_RTTable.Get(entryHash);
	CRTextureRTD3D11* pRT;
	if(!ppRT)
	{
		CRRendererD3D11* pRenderer = (CRRendererD3D11*) m_pRenderer;

		D3D11_RENDER_TARGET_VIEW_DESC rtDesc;

		DXGI_FORMAT rtFormat = m_Desc.Format;
		if(TexFormatFromD3D11Typeless(rtFormat) != TEXF_END)
			rtFormat = TexFormatToD3D11( TexFormatFromD3D11Typeless(rtFormat) );

		rtDesc.Format = rtFormat;
		rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
		rtDesc.Texture3D.MipSlice = mipLevel;
		rtDesc.Texture3D.FirstWSlice = startSlice;
		rtDesc.Texture3D.WSize = numSlices;

		CRTextureRTD3D11Ptr pRTTemp = pRenderer->GetResourceMgrD3D()->CreateRenderTargetFromResource(m_pD3DTexture, &rtDesc);
		_DEBUG_ASSERT(pRTTemp);
		m_RTTable.Add(entryHash, pRTTemp);
		pRT = (CRTextureRTD3D11*) pRTTemp;
	}
	else
	{
		pRT = (CRTextureRTD3D11*) *ppRT;
	}

	return pRT;
}


_NAMESPACE_END