//==============================================================================
//
//		CRTextureCubeD3D.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		10/14/2007
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

boolean 
CRTextureCubeD3D::DoUnlock(uint level, eRCubeFace face)
{
	if(!m_pOffScreen)
		return FALSE;

	HRESULT hr = m_pOffScreenSurfaces[face][level]->UnlockRect();
	if(FAILED(hr))
		return FALSE;

	if(m_DirtyRegions[face][level].right > 0 &&
		m_DirtyRegions[face][level].bottom > 0)
	{
		// Update default pool texture
		hr = ((CRRendererD3D*)m_pRenderer)->GetD3DDevice()->UpdateSurface(m_pOffScreenSurfaces[face][level], 
			&(m_DirtyRegions[face][level]),
			m_pD3DTextureSurfaces[face][level],
			(POINT*)&(m_DirtyRegions[face][level]));

		_DEBUG_ASSERT(SUCCEEDED(hr));
	}

	return hr == D3D_OK ? TRUE : FALSE;
}

byte* 
CRTextureCubeD3D::DoLock(uint level, eRCubeFace face, uint& pitch, const gmtl::Vec4i* pToWrite)
{		
	if(!m_pOffScreen)
		return NULL;

	DWORD flags = D3DLOCK_NO_DIRTY_UPDATE;

	// NOTE: D3D expects rect.right and rect.bottom to be offset by + 1...
	RECT rect;
	rect.left = 0;
	rect.right = GetSize(level);
	rect.top = 0;
	rect.bottom = GetSize(level);

	if(pToWrite)
	{
		if((*pToWrite)[2] <= 0 || (*pToWrite)[3] <= 0)
			flags |= D3DLOCK_READONLY;

		rect.left = (*pToWrite)[0];
		rect.top = (*pToWrite)[1];
		rect.right = rect.left + (*pToWrite)[2];
		rect.bottom = rect.top + (*pToWrite)[3];
	}

	m_DirtyRegions[face][level] = rect;

	D3DLOCKED_RECT lockedRect;
	HRESULT hr = m_pOffScreenSurfaces[face][level]->LockRect(&lockedRect, NULL, flags);
	_DEBUG_ASSERT(hr == D3D_OK);

	pitch = lockedRect.Pitch;

	return (byte*) lockedRect.pBits;			
}

uint 
CRTextureCubeD3D::DoGetMipMapLevels() const
{
	return (uint) m_Desc.size();
}

uint 
CRTextureCubeD3D::DoGetSize(uint level) const
{
	_DEBUG_ASSERT(level >= 0 && level < (uint) m_Desc.size());
	return m_Desc[level].Width;
}

void CRTextureCubeD3D::SetD3DTextureRT(IDirect3DCubeTexture9* pTex)
{
	_DEBUG_ASSERT(pTex);

	m_pD3DTexture = pTex;

	_LOOPi(CF_NUM_FACES)
	{
		m_pD3DTextureSurfaces[i].clear();
		m_pOffScreenSurfaces[i].clear();
		m_DirtyRegions[i].clear();

		m_pCubeFaceRT[i].clear();
	}

	m_pOffScreen = NULL;

	m_Desc.clear();

	uint mipMapCount = m_pD3DTexture->GetLevelCount();
	_LOOPi(mipMapCount)
	{
		D3DSURFACE_DESC desc;
		m_pD3DTexture->GetLevelDesc(i, &desc);
		m_Desc.push_back(desc);

		_LOOPj(CF_NUM_FACES)
		{
			IDirect3DSurface9* pD3DSurface = NULL;
			HRESULT hr = m_pD3DTexture->GetCubeMapSurface(CubeFaceToD3D((eRCubeFace)j), i, &pD3DSurface);
			_DEBUG_ASSERT(SUCCEEDED(hr));
			m_pD3DTextureSurfaces[j].push_back(pD3DSurface);
			_RELEASE(pD3DSurface);

			CRTextureRTD3D* pRT = ((CRRendererD3D*) m_pRenderer)->GetResourceMgrD3D().CreateRenderTargetFromSurface(pD3DSurface);
			m_pCubeFaceRT[j].push_back(pRT);
			//m_pCubeFaceRT[j] = pRT;
		}
	}
}

CRTextureRT* CRTextureCubeD3D::DoGetTextureRT(uint mipLevel, eRCubeFace face)
{
	if(m_pCubeFaceRT[face].size() > 0)
		return m_pCubeFaceRT[face][mipLevel];

	return NULL;
}

void 
CRTextureCubeD3D::SetD3DTexture(IDirect3DCubeTexture9* pTex, IDirect3DCubeTexture9* pOffScreen)
{
	_DEBUG_ASSERT(pTex);

	m_pD3DTexture = pTex;
	
	_LOOPi(CF_NUM_FACES)
	{
		m_pD3DTextureSurfaces[i].clear();
	}
	
	if(pTex && !pOffScreen)
	{
		// Reset path

		// Update D3D Texture
		uint mipMapCount = (uint) m_Desc.size();
		_LOOPi(mipMapCount)
		{
			_LOOPj(CF_NUM_FACES)
			{
				IDirect3DSurface9* pD3DSurface = NULL;
				HRESULT hr = m_pD3DTexture->GetCubeMapSurface(CubeFaceToD3D((eRCubeFace)j), i, &pD3DSurface);
				_DEBUG_ASSERT(SUCCEEDED(hr));
				m_pD3DTextureSurfaces[j].push_back(pD3DSurface);
				_RELEASE(pD3DSurface);

				// Check if it's a render target
				if((uint) i < (uint) m_pCubeFaceRT[j].size())
				{
					m_pCubeFaceRT[j][i]->SetD3DSurfaceOnly(pD3DSurface);
				}
				else
				{
					uint pitch = 0;
					Lock(i, (eRCubeFace)j, pitch, NULL);
					Unlock(i, (eRCubeFace)j);
				}
			}			
		}
	}
	else
	{
		// Normal cube texture path

		m_pOffScreen = pOffScreen;	
		
		_LOOPi(CF_NUM_FACES)
		{
			m_pOffScreenSurfaces[i].clear();
			m_DirtyRegions[i].clear();
			m_pCubeFaceRT[i].clear();
		}

		m_Desc.clear();

		uint mipMapCount = m_pD3DTexture->GetLevelCount();
		_LOOPi(mipMapCount)
		{
			D3DSURFACE_DESC desc;
			m_pD3DTexture->GetLevelDesc(i, &desc);
			m_Desc.push_back(desc);
			
			_LOOPj(CF_NUM_FACES)
			{
				IDirect3DSurface9* pD3DSurface = NULL;
				HRESULT hr = m_pD3DTexture->GetCubeMapSurface(CubeFaceToD3D((eRCubeFace)j), i, &pD3DSurface);
				_DEBUG_ASSERT(SUCCEEDED(hr));
				m_pD3DTextureSurfaces[j].push_back(pD3DSurface);
				_RELEASE(pD3DSurface);

				if(m_pOffScreen)
				{
					IDirect3DSurface9* pSurface = NULL;
					hr = m_pOffScreen->GetCubeMapSurface(CubeFaceToD3D((eRCubeFace)j), i, &pSurface);
					_DEBUG_ASSERT(SUCCEEDED(hr));
					m_pOffScreenSurfaces[j].push_back(pSurface);
					RECT rect;
					rect.left = rect.right = rect.top = rect.bottom = 0;
					m_DirtyRegions[j].push_back(rect);
					_RELEASE(pSurface);

					//uint pitch = 0;
					//Lock(i, pitch, NULL);
					//Unlock(i);		
				}
			}
			
		}
	}
}

void 
CRTextureCubeD3D::OnResetDevice()
{
	_DEBUG_TRACE(_W("Resetting texture cube: %s\n"), GetResourceName());

	boolean success = ((CRResourceMgrD3D*)(m_pResourceMgr))->ResetTextureCube(this);
	_DEBUG_ASSERT(success);
}

void 
CRTextureCubeD3D::OnLostDevice()
{
	_DEBUG_TRACE(_W("Lost texture cube: %s\n"), GetResourceName());

	m_pD3DTexture = NULL;
	
	_LOOPi(CF_NUM_FACES)
	{
		m_pD3DTextureSurfaces[i].clear();

		_LOOPj(m_pCubeFaceRT[i].size())
		{
			m_pCubeFaceRT[i][j]->OnLostDevice();
		}
	}
}

void 
CRTextureCubeD3D::OnRestoreDevice()
{
}


_NAMESPACE_END