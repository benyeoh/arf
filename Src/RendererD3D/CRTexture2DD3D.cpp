//==============================================================================
//
//		CRTexture2DD3D.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, April 09, 2006
//
//==============================================================================

#include "RendererD3D.h"


_NAMESPACE_BEGIN

//boolean
//CRTexture2DD3D::DoUnlock(uint level)
//{
//	D3DLOCKED_RECT lockedRect;
//	
//	m_pD3DTexture->LockRect(level, &lockedRect, NULL, D3DLOCK_DISCARD);
//	byte* pData = m_pMipMapData[level]->GetData();
//
//	switch(m_Format)
//	{
//		case TEXF_DXT5:
//		case TEXF_DXT4:
//		case TEXF_DXT3:
//		case TEXF_DXT2:		
//		case TEXF_DXT1:
//		{
//			uint blockHeight = GetHeight(level) / 4;
//			uint blockWidth = GetWidth(level) / 4;
//			uint blockBitDepth = GetBitDepth();
//			uint blockRowLength = blockWidth * blockBitDepth; 		
//			
//			if(lockedRect.Pitch == blockRowLength)
//			{
//				memcpy(lockedRect.pBits, pData, lockedRect.Pitch * blockHeight);
//			}							
//			else			
//			{
//				uint blockRowLengthPerWord = blockRowLength / sizeof(uint);
//				_DEBUG_ASSERT(blockRowLengthPerWord % sizeof(uint) == 0);				
//				_LOOPi(blockHeight)
//				{
//					_LOOPj(blockRowLengthPerWord)
//					{
//						*( (uint*)(lockedRect.pBits + i * lockedRect.Pitch + j * sizeof(uint)) ) = 
//							*( (uint*)(pData + i * blockRowLength + j * sizeof(uint)) );
//					}
//				}
//			}
//			break;
//		}
//		
//		default:
//		{
//			uint height = GetHeight(level);
//			uint width = GetWidth(level);
//			uint rowLength = GetBitDepth() * width;
//			if(rowLength == lockedRect.Pitch)		
//				memcpy(lockedRect.pBits, pData, rowLength * height);
//			else
//			if(rowLength % sizeof(uint) == 0)
//			{
//				uint rowLengthPerWord = rowLength / sizeof(uint);
//				_LOOPi(height)
//				{
//					_LOOPj(rowLengthPerWord)
//					{
//						*( (uint*)(lockedRect.pBits + i * lockedRect.Pitch + j * sizeof(uint)) ) = 
//							*( (uint*)(pData + i * rowLength + j * sizeof(uint)) );
//					}
//				}
//			}	
//			break;
//		}
//	}
//}

byte* 
CRTexture2DD3D::DoLockImmediate(uint level, uint& pitch)
{
	D3DLOCKED_RECT lockedRect;
	HRESULT hr = m_pD3DTextureSurfaces[level]->LockRect(&lockedRect, NULL, D3DLOCK_DISCARD);
	_DEBUG_ASSERT(hr == D3D_OK);

	pitch = lockedRect.Pitch;

	return (byte*) lockedRect.pBits;		
}

boolean 
CRTexture2DD3D::DoUnlockImmediate(uint level)
{
	HRESULT hr = m_pD3DTextureSurfaces[level]->UnlockRect();
	_DEBUG_ASSERT(hr == D3D_OK);

	return hr == D3D_OK;
}

boolean 
CRTexture2DD3D::DoUnlock(uint level)
{
	if(!m_pOffScreen)
		return FALSE;
		
	HRESULT hr = m_pOffScreenSurfaces[level]->UnlockRect();
	if(FAILED(hr))
		return FALSE;
				
	if(m_DirtyRegions[level].right > 0 &&
		m_DirtyRegions[level].bottom > 0)
	{
		// Update default pool texture
		hr = ((CRRendererD3D*)m_pRenderer)->GetD3DDevice()->UpdateSurface(m_pOffScreenSurfaces[level], 
																		&(m_DirtyRegions[level]),
																		m_pD3DTextureSurfaces[level],
																		(POINT*)&(m_DirtyRegions[level]));
																		
		_DEBUG_ASSERT(SUCCEEDED(hr));
	}
	
	return hr == D3D_OK ? TRUE : FALSE;
}

byte* 
CRTexture2DD3D::DoLock(uint level, uint& pitch, const gmtl::Vec4i* pToWrite)
{		
	if(!m_pOffScreen)
		return NULL;
		
	DWORD flags = D3DLOCK_NO_DIRTY_UPDATE;

	// NOTE: D3D expects rect.right and rect.bottom to be offset by + 1...
	RECT rect;
	rect.left = 0;
	rect.right = GetWidth(level);
	rect.top = 0;
	rect.bottom = GetHeight(level);
	
	if(pToWrite)
	{
		if((*pToWrite)[2] <= 0 || (*pToWrite)[3] <= 0)
			flags |= D3DLOCK_READONLY;
			
		rect.left = (*pToWrite)[0];
		rect.top = (*pToWrite)[1];
		rect.right = rect.left + (*pToWrite)[2];
		rect.bottom = rect.top + (*pToWrite)[3];
	}
	
	m_DirtyRegions[level] = rect;
			
	D3DLOCKED_RECT lockedRect;
	HRESULT hr = m_pOffScreenSurfaces[level]->LockRect(&lockedRect, NULL, flags);
	_DEBUG_ASSERT(hr == D3D_OK);
	
	pitch = lockedRect.Pitch;
	
	return (byte*) lockedRect.pBits;			
}

uint 
CRTexture2DD3D::DoGetMipMapLevels() const
{
	return (uint) m_Desc.size();
}

uint 
CRTexture2DD3D::DoGetWidth(uint level) const
{
	_DEBUG_ASSERT(level >= 0 && level < (uint) m_Desc.size());
	return m_Desc[level].Width;
}

uint 
CRTexture2DD3D::DoGetHeight(uint level) const
{
	_DEBUG_ASSERT(level >= 0 && level < (uint) m_Desc.size());
	return m_Desc[level].Height;
}

void CRTexture2DD3D::SetD3DTextureRT(IDirect3DTexture9* pTex)
{
	_DEBUG_ASSERT(pTex);

	m_pD3DTexture = pTex;
	m_pD3DTextureSurfaces.clear();

	m_pOffScreen = NULL;	
	m_pOffScreenSurfaces.clear();
	m_DirtyRegions.clear();
	m_Desc.clear();
	m_pRT.clear();

	uint mipMapCount = m_pD3DTexture->GetLevelCount();

	_LOOPi(mipMapCount)
	{
		D3DSURFACE_DESC desc;
		m_pD3DTexture->GetLevelDesc(i, &desc);	
		m_Desc.push_back(desc);
		IDirect3DSurface9* pD3DSurface = NULL;
		HRESULT hr = m_pD3DTexture->GetSurfaceLevel(i, &pD3DSurface);
		_DEBUG_ASSERT(SUCCEEDED(hr));
		m_pD3DTextureSurfaces.push_back(pD3DSurface);
		_RELEASE(pD3DSurface);

		CRTextureRTD3D* pRT = ((CRRendererD3D*) m_pRenderer)->GetResourceMgrD3D().CreateRenderTargetFromSurface(pD3DSurface);
		m_pRT.push_back(pRT);
	}
}

void 
CRTexture2DD3D::SetD3DTexture(IDirect3DTexture9* pTex, IDirect3DTexture9* pOffScreen)
{
	_DEBUG_ASSERT(pTex);
	
	m_pD3DTexture = pTex;
	m_pD3DTextureSurfaces.clear();
			
	if(pTex && !pOffScreen)
	{
		// Update D3D Texture
		uint mipMapCount = (uint) m_Desc.size();
		_LOOPi(mipMapCount)
		{
			IDirect3DSurface9* pD3DSurface = NULL;
			HRESULT hr = m_pD3DTexture->GetSurfaceLevel(i, &pD3DSurface);
			_DEBUG_ASSERT(SUCCEEDED(hr));
			m_pD3DTextureSurfaces.push_back(pD3DSurface);
			_RELEASE(pD3DSurface);

			if((uint) i < (uint) m_pRT.size())
			{
				m_pRT[i]->SetD3DSurfaceOnly(pD3DSurface);
			}
			else
			{
				uint pitch = 0;
				Lock(i, pitch, NULL);
				Unlock(i);			
			}
		}
	}
	else
	{
		m_pOffScreen = pOffScreen;	
		m_pOffScreenSurfaces.clear();
		m_DirtyRegions.clear();
		m_Desc.clear();
		m_pRT.clear();

		uint mipMapCount = m_pD3DTexture->GetLevelCount();
		
		_LOOPi(mipMapCount)
		{
			D3DSURFACE_DESC desc;
			m_pD3DTexture->GetLevelDesc(i, &desc);
			m_Desc.push_back(desc);
			IDirect3DSurface9* pD3DSurface = NULL;
			HRESULT hr = m_pD3DTexture->GetSurfaceLevel(i, &pD3DSurface);
			_DEBUG_ASSERT(SUCCEEDED(hr));
			m_pD3DTextureSurfaces.push_back(pD3DSurface);
			_RELEASE(pD3DSurface);
			
			if(m_pOffScreen)
			{
				IDirect3DSurface9* pSurface = NULL;
				hr = m_pOffScreen->GetSurfaceLevel(i, &pSurface);
				_DEBUG_ASSERT(SUCCEEDED(hr));
				m_pOffScreenSurfaces.push_back(pSurface);
				RECT rect;
				rect.left = rect.right = rect.top = rect.bottom = 0;
				m_DirtyRegions.push_back(rect);
				_RELEASE(pSurface);

				//uint pitch = 0;
				//Lock(i, pitch, NULL);
				//Unlock(i);		
			}
		}
	}
}

CRTextureRT* CRTexture2DD3D::DoGetTextureRT(uint level)
{
	if(m_pRT.size() > 0)
		return m_pRT[level];

	return NULL;
}

void 
CRTexture2DD3D::OnResetDevice()
{
	_DEBUG_TRACE(_W("Resetting texture: %s\n"), GetResourceName());

	boolean success = ((CRResourceMgrD3D*)(m_pResourceMgr))->ResetTexture2D(this);
	_DEBUG_ASSERT(success);
}

void 
CRTexture2DD3D::OnLostDevice()
{
	_DEBUG_TRACE(_W("Lost texture: %s\n"), GetResourceName());

	m_pD3DTexture = NULL;
	m_pD3DTextureSurfaces.clear();

	_LOOPi(m_pRT.size())
	{
		m_pRT[i]->OnLostDevice();
	}
}

void 
CRTexture2DD3D::OnRestoreDevice()
{
}


_NAMESPACE_END