//==============================================================================
//
//		CRTexture2DD3D11.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, April 09, 2006
//
//==============================================================================

#include "RendererD3D11Framework.h"


_NAMESPACE_BEGIN

byte* CRTexture2DD3D11::MapDirectResource(uint level, uint& pitch)
{
	byte* pBuf;

	// Directly access GPU resource with Map
	CRRendererD3D11* pRenderer = (CRRendererD3D11*) m_pRenderer;
	ID3D11DeviceContext* pContext = pRenderer->GetCurrentContext();

	D3D11_MAPPED_SUBRESOURCE subRes;
	HRESULT hr = pContext->Map(m_pD3DTexture, level, D3D11_MAP_WRITE_DISCARD, 0, &subRes);
	_DEBUG_ASSERT(hr == S_OK);
	pBuf = (byte*) subRes.pData;
	pitch = subRes.RowPitch;

	return pBuf;
}

void CRTexture2DD3D11::UnmapDirectResource(uint level)
{
	// Directly access GPU resource with Map
	CRRendererD3D11* pRenderer = (CRRendererD3D11*) m_pRenderer;
	ID3D11DeviceContext* pContext = pRenderer->GetCurrentContext();

	pContext->Unmap(m_pD3DTexture, level);
}

byte* CRTexture2DD3D11::MapProxyResource(uint level, uint& outPitch)
{
	byte* pBuf;

	CRRendererD3D11* pRenderer = (CRRendererD3D11*) m_pRenderer;
	//ID3D11DeviceContext* pContext = pRenderer->GetCurrentContext();

	uint width = (m_Desc.Width >> level);
	uint height = (m_Desc.Height >> level);
	height = height > 0 ? height : 1;
	width = width > 0 ? width : 1;
	uint pitch = width * GetSizePerElem();
	uint numRows = height;
	if(IsBlockCompressed(m_Desc.Format))
	{
		pitch = ((width + 3) >> 2) * GetSizePerElem();
		numRows = height >> 2;
		numRows = numRows > 0 ? numRows : 1;
	}

	ScratchPad* pPad = pRenderer->GetScratchPad(numRows * pitch);
	pBuf = (byte*) pPad->pMem;
	outPitch = pitch;

	return pBuf;
}

void CRTexture2DD3D11::UnmapProxyResource(uint level)
{
	_DEBUG_ASSERT(m_pLockPad);
	CRRendererD3D11* pRenderer = (CRRendererD3D11*) m_pRenderer;
	ID3D11DeviceContext* pContext = pRenderer->GetCurrentContext();

	D3D11_BOX updateBox;
	updateBox.left = m_WriteOffset[0];
	updateBox.right = updateBox.left + m_WriteOffset[2];
	updateBox.top = m_WriteOffset[1];
	updateBox.bottom = updateBox.top + m_WriteOffset[3];
	updateBox.front = 0;
	updateBox.back = 1;

	D3D11_BOX alignBox = updateBox;

	uint width = (m_Desc.Width >> level);
	uint height = (m_Desc.Height >> level);
	height = height > 0 ? height : 1;
	width = width > 0 ? width : 1;
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

	_DEBUG_ASSERT(m_pLockPad->size >= (numRows * pitch));

	// Offset src data to match dest box
	byte* pSrcData = ((byte*)m_pLockPad->pMem) + alignBox.left * GetSizePerElem() + alignBox.top * pitch;

	pContext->UpdateSubresource(m_pD3DTexture, level, &updateBox, pSrcData, pitch, 0);
	pRenderer->DisposeScratchPad(m_pLockPad);
	m_pLockPad = NULL;		
}


byte* CRTexture2DD3D11::DoLockImmediate(uint level, uint& pitch)
{
	byte* pBuf;

	if(m_Desc.BindFlags & D3D11_USAGE_DYNAMIC)
	{
		pBuf = MapDirectResource(level, pitch);
	}
	else
	{
		m_WriteOffset[0] = m_WriteOffset[1] = 0;
		m_WriteOffset[2] = m_Desc.Width >> level;
		m_WriteOffset[3] = m_Desc.Height >> level;
		m_WriteOffset[2] = m_WriteOffset[2] <= 0 ? 1 : m_WriteOffset[2];
		m_WriteOffset[3] = m_WriteOffset[3] <= 0 ? 1 : m_WriteOffset[3];

		pBuf = MapProxyResource(level, pitch);
	}
	
	return pBuf;		
}

boolean CRTexture2DD3D11::DoUnlockImmediate(uint level)
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

boolean CRTexture2DD3D11::DoUnlock(uint level)
{
	CRRendererD3D11* pRenderer = (CRRendererD3D11*) m_pRenderer;
	ID3D11DeviceContext* pContext = pRenderer->GetCurrentContext();

	if(m_pOffscreen)
	{
		// Now copy if there was any writing done
		if((m_WriteOffset[2] > 0) && (m_WriteOffset[3] > 0) && (m_Desc.Usage & D3D11_USAGE_DYNAMIC))
		{
			// This is the case where we have an offscreen buffer but also dynamic flag
			// IE, reading with the dynamic flag

			// TODO: Beware of dynamic flag
			// Basically with dynamic flag the entire buffer is discarded
			// so locking a portion is not supported

			// If dynamic then Map and memcpy entire buffer with D3D11_MAP_WRITE_DISCARD

			// Lock render resource
			uint pitch;
			byte* pBuf = MapDirectResource(level, pitch);

			_DEBUG_ASSERT(m_OffscreenData.pData);
			_DEBUG_ASSERT(m_OffscreenData.RowPitch == pitch);

			// Copy the app results
			uint height = (m_Desc.Height >> level);
			height = height > 0 ? height : 1;
			uint numRows = height;
			if(IsBlockCompressed(m_Desc.Format))
			{
				numRows = height >> 2;
				numRows = numRows > 0 ? numRows : 1;
			}

			memcpy(pBuf, m_OffscreenData.pData, numRows * pitch);
				
			// Unlock
			UnmapDirectResource(level);

			// WARNING: Do we need an immediate context?
			// Apparently, only MAP_DISCARD is allowed for deferred contexts
			pContext->Unmap(m_pOffscreen, level);

			m_OffscreenData.pData = NULL;
			m_OffscreenData.RowPitch = 0;
		}
		else
		{
			// WARNING: Do we need an immediate context?
			// Apparently, only MAP_DISCARD is allowed for deferred contexts
			pContext->Unmap(m_pOffscreen, level);

			m_OffscreenData.pData = NULL;
			m_OffscreenData.RowPitch = 0;

			// We can copy the relevant portion
			D3D11_BOX updateBox;
			updateBox.left = m_WriteOffset[0];
			updateBox.right = m_WriteOffset[0] + m_WriteOffset[2];
			updateBox.top = m_WriteOffset[1];
			updateBox.bottom = m_WriteOffset[1] + m_WriteOffset[3];
			updateBox.front = 0;
			updateBox.back = 1;

			//// Offset the data
			//uint width = (m_Desc.Width >> level);
			//uint height = (m_Desc.Height >> level);
			//height = height > 0 ? height : 1;
			//width = width > 0 ? width : 1;
			//uint pitch = width * GetSizePerElem();
			//uint numRows = height;
			//if(IsBlockCompressed(m_Desc.Format))
			//{
			//	pitch = ((width + 3) >> 2) * GetSizePerElem();
			//	numRows = height >> 2;
			//	numRows = numRows > 0 ? numRows : 1;
			//	alignBox.left >>= 2;
			//	alignBox.right >>= 2;
			//	alignBox.top >>= 2;
			//	alignBox.bottom >>= 2;
			//}

			//_DEBUG_ASSERT(m_OffscreenData.RowPitch == pitch);

			//byte* pSrcData = ((byte*)m_OffscreenData.pData) + alignBox.left * GetSizePerElem() + alignBox.top * m_OffscreenData.RowPitch;
			if((m_WriteOffset[2] > 0) && (m_WriteOffset[3] > 0))
				pContext->CopySubresourceRegion(m_pD3DTexture, level, m_WriteOffset[0], m_WriteOffset[1], 0, m_pOffscreen, level, &updateBox);
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

byte* CRTexture2DD3D11::DoLock(uint level, uint& pitch, const gmtl::Vec4i* pToWrite)
{		
	byte* pBuf = NULL;

	CRRendererD3D11* pRenderer = (CRRendererD3D11*) m_pRenderer;
	ID3D11DeviceContext* pContext = pRenderer->GetCurrentContext();

	if(pToWrite)
	{
		// We need an offscreen buffer if app needs to read data
		// or app is only updating a portion and texture is dynamic
		if( ((*pToWrite)[2] <= 0) || ((*pToWrite)[3] <= 0) || (m_Desc.Usage & D3D11_USAGE_DYNAMIC) )
		{
			if(!m_pOffscreen)
			{
				// Create a staging resource if we do not have
				ID3D11Texture2D* pOffscreen = pRenderer->GetResourceMgrD3D()->CreateOffscreenTexture2D(m_Desc);
				m_pOffscreen = pOffscreen;
				pOffscreen->Release();

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
		// Apparently, only MAP_DISCARD is allowed for deferred contexts	
		HRESULT hr = pContext->Map(m_pOffscreen, level, D3D11_MAP_READ_WRITE, 0, &m_OffscreenData);
		_DEBUG_ASSERT(hr == S_OK);
		pBuf = (byte*) m_OffscreenData.pData;
		pitch = m_OffscreenData.RowPitch;
	}
	else
	{
		// Skip offscreen buffer
		if(m_Desc.Usage & D3D11_USAGE_DYNAMIC)
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

uint CRTexture2DD3D11::DoGetMipMapLevels() const
{
	return (uint) m_Desc.MipLevels;
}

uint CRTexture2DD3D11::DoGetWidth(uint level) const
{
	uint width = m_Desc.Width >> level;
	width = width > 0 ? width : 1;
	return width;
}

uint CRTexture2DD3D11::DoGetHeight(uint level) const
{
	uint height = m_Desc.Height >> level;
	height = height > 0 ? height : 1;
	return height;
}

//void CRTexture2DD3D11::SetD3DTextureRT(IDirect3DTexture9* pTex)
//{
//	_DEBUG_ASSERT(pTex);
//
//	m_pD3DTexture = pTex;
//	m_pD3DTextureSurfaces.clear();
//
//	m_pOffScreen = NULL;	
//	m_pOffScreenSurfaces.clear();
//	m_DirtyRegions.clear();
//	m_Desc.clear();
//	m_pRT.clear();
//
//	uint mipMapCount = m_pD3DTexture->GetLevelCount();
//
//	_LOOPi(mipMapCount)
//	{
//		D3DSURFACE_DESC desc;
//		m_pD3DTexture->GetLevelDesc(i, &desc);
//		m_Desc.push_back(desc);
//		IDirect3DSurface9* pD3DSurface = NULL;
//		HRESULT hr = m_pD3DTexture->GetSurfaceLevel(i, &pD3DSurface);
//		_DEBUG_ASSERT(SUCCEEDED(hr));
//		m_pD3DTextureSurfaces.push_back(pD3DSurface);
//		_RELEASE(pD3DSurface);
//
//		CRTextureRTD3D* pRT = ((CRRendererD3D*) m_pRenderer)->GetResourceMgrD3D().CreateRenderTargetFromSurface(pD3DSurface);
//		m_pRT.push_back(pRT);
//	}
//}

void CRTexture2DD3D11::SetD3DTexture(ID3D11Texture2D* pTex, ID3D11Texture2D* pOffScreen)
{
	CRRendererD3D11* pRenderer = (CRRendererD3D11*) m_pRenderer;
	ID3D11Device* pDevice = pRenderer->GetDevice();

	_DEBUG_ASSERT(pTex);

	m_pD3DTexture = pTex;
	m_pOffscreen = pOffScreen;
	pTex->GetDesc(&m_Desc);

	ID3D11ShaderResourceView* pSRV;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	DXGI_FORMAT srvFormat = m_Desc.Format;
	if(TexFormatFromD3D11Typeless(m_Desc.Format) != TEXF_END)
		srvFormat = TexFormatToD3D11( TexFormatFromD3D11Typeless(m_Desc.Format) );

	srvDesc.Format = srvFormat;
	srvDesc.Texture2D.MipLevels = m_Desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	HRESULT hr = pDevice->CreateShaderResourceView(m_pD3DTexture, &srvDesc, &pSRV);
	_DEBUG_ASSERT(SUCCEEDED(hr));
	m_pSRV = pSRV;
	pSRV->Release();

	if(m_Desc.BindFlags & D3D11_BIND_RENDER_TARGET)
	{
		_LOOPi(m_Desc.MipLevels)
		{
			D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
			rtDesc.Format = srvFormat;
			rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			rtDesc.Texture2D.MipSlice = i;

			CRTextureRTD3D11* pRT = pRenderer->GetResourceMgrD3D()->CreateRenderTargetFromResource(m_pD3DTexture, &rtDesc);
			m_pRT.push_back(pRT);
		}
	}
}

CRTextureRT* CRTexture2DD3D11::DoGetTextureRT(uint level)
{
	if(m_pRT.size() > 0)
		return m_pRT[level];

	return NULL;
}


_NAMESPACE_END