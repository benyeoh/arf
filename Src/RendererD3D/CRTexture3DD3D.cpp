//================================================================================
//
//		CRTexture3DD3D.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/24/2012
//
//================================================================================

#include "RendererD3D.h"

_NAMESPACE_BEGIN

byte* CRTexture3DD3D::DoLock(uint level, uint& rowPitch, uint& slicePitch, const gmtl::Vec3i* pToWriteStart, const gmtl::Vec3i* pToWriteEnd)
{
	if(!m_pOffScreen)
		return NULL;

	DWORD flags = 0;//D3DLOCK_NO_DIRTY_UPDATE;

	//// NOTE: D3D expects rect.right and rect.bottom to be offset by + 1...
	D3DBOX box;
	box.Left = 0;
	box.Right = GetWidth(level);
	box.Top = 0;
	box.Bottom = GetHeight(level);
	box.Front = 0;
	box.Back = GetDepth(level);

	if(pToWriteStart && pToWriteEnd)
	{
		if((*pToWriteEnd)[0] <= 0 || (*pToWriteEnd)[1] <= 0 || (*pToWriteEnd)[2] <= 0)
			flags |= D3DLOCK_READONLY;

		box.Left = (*pToWriteStart)[0];
		box.Top = (*pToWriteStart)[1];
		box.Front = (*pToWriteStart)[2];
		box.Right = (*pToWriteEnd)[0];
		box.Bottom = (*pToWriteEnd)[1];
		box.Back = (*pToWriteEnd)[2];
	}

	m_DirtyRegions[level] = box;

	D3DLOCKED_BOX lockedRect;
	HRESULT hr = m_pOffScreenSurfaces[level]->LockBox(&lockedRect, NULL, flags);
	_DEBUG_ASSERT(hr == D3D_OK);

	rowPitch = lockedRect.RowPitch;
	slicePitch = lockedRect.SlicePitch;

	return (byte*) lockedRect.pBits;			
}

boolean CRTexture3DD3D::DoUnlock(uint level)
{
	if(!m_pOffScreen)
		return FALSE;

	HRESULT hr = m_pOffScreenSurfaces[level]->UnlockBox();
	if(FAILED(hr))
		return FALSE;

	if(m_DirtyRegions[level].Right > 0 &&
		m_DirtyRegions[level].Bottom > 0 &&
		m_DirtyRegions[level].Back > 0)
	{
		D3DLOCKED_BOX lockBoxSrc;
		hr = m_pOffScreenSurfaces[level]->LockBox(&lockBoxSrc, NULL, D3DLOCK_READONLY);
		D3DLOCKED_BOX lockBoxDest;
		hr = m_pD3DTextureSurfaces[level]->LockBox(&lockBoxDest, NULL, 0);
		
		memcpy(lockBoxDest.pBits, lockBoxSrc.pBits, lockBoxSrc.SlicePitch * m_Desc[level].Depth);

		hr = m_pD3DTextureSurfaces[level]->UnlockBox();
		hr = m_pOffScreenSurfaces[level]->UnlockBox();

		//// Update default pool texture
		//hr = ((CRRendererD3D*)m_pRenderer)->GetD3DDevice()->UpdateTexture( m_pOffScreenSurfaces[level], 
		//	&(m_DirtyRegions[level]),
		//	m_pD3DTextureSurfaces[level],
		//	(POINT*)&(m_DirtyRegions[level]));

		_DEBUG_ASSERT(SUCCEEDED(hr));
	}

	return hr == D3D_OK ? TRUE : FALSE;
}

uint CRTexture3DD3D::DoGetWidth(uint level) const
{
	_DEBUG_ASSERT(level >= 0 && level < (uint) m_Desc.size());
	return m_Desc[level].Width;
}

uint CRTexture3DD3D::DoGetHeight(uint level) const
{
	_DEBUG_ASSERT(level >= 0 && level < (uint) m_Desc.size());
	return m_Desc[level].Height;
}

uint CRTexture3DD3D::DoGetDepth(uint level) const
{
	_DEBUG_ASSERT(level >= 0 && level < (uint) m_Desc.size());
	return m_Desc[level].Depth;
}

byte* CRTexture3DD3D::DoLockImmediate(uint level, uint& rowPitch, uint& slicePitch)
{
	D3DLOCKED_BOX lockBoxDest;
	HRESULT hr = m_pD3DTextureSurfaces[level]->LockBox(&lockBoxDest, NULL, 0);
	_DEBUG_ASSERT(SUCCEEDED(hr));

	rowPitch = lockBoxDest.RowPitch;
	slicePitch = lockBoxDest.SlicePitch;
	return (byte*) lockBoxDest.pBits;
}

boolean CRTexture3DD3D::DoUnlockImmediate(uint level)
{
	HRESULT hr = m_pD3DTextureSurfaces[level]->UnlockBox();
	_DEBUG_ASSERT(SUCCEEDED(hr));
	return TRUE;
}

uint CRTexture3DD3D::DoGetMipMapLevels() const
{
	return (uint) m_Desc.size();
}

void CRTexture3DD3D::SetD3DTexture(IDirect3DVolumeTexture9* pTex, IDirect3DVolumeTexture9* pOffScreen)
{
	_DEBUG_ASSERT(pTex);

	m_pD3DTexture = pTex;
	m_pD3DTextureSurfaces.clear();

	if(m_pOffScreen && !pOffScreen)
	{
		// Update D3D Texture
		uint mipMapCount = (uint) m_Desc.size();
		_LOOPi(mipMapCount)
		{
			IDirect3DVolume9* pD3DSurface = NULL;
			HRESULT hr = m_pD3DTexture->GetVolumeLevel(i, &pD3DSurface);
			_DEBUG_ASSERT(SUCCEEDED(hr));
			m_pD3DTextureSurfaces.push_back(pD3DSurface);
			_RELEASE(pD3DSurface);

			uint rowPitch = 0;
			uint slicePitch = 0;
			Lock(i, rowPitch, slicePitch, NULL, NULL);
			Unlock(i);			
		}
	}
	else
	{
		m_pOffScreen = pOffScreen;	
		m_pOffScreenSurfaces.clear();
		m_DirtyRegions.clear();
		m_Desc.clear();

		uint mipMapCount = m_pD3DTexture->GetLevelCount();

		_LOOPi(mipMapCount)
		{
			D3DVOLUME_DESC desc;
			m_pD3DTexture->GetLevelDesc(i, &desc);
			m_Desc.push_back(desc);
			IDirect3DVolume9* pD3DSurface = NULL;
			HRESULT hr = m_pD3DTexture->GetVolumeLevel(i, &pD3DSurface);
			_DEBUG_ASSERT(SUCCEEDED(hr));
			m_pD3DTextureSurfaces.push_back(pD3DSurface);
			_RELEASE(pD3DSurface);

			if(m_pOffScreen)
			{
				IDirect3DVolume9* pSurface = NULL;
				hr = m_pOffScreen->GetVolumeLevel(i, &pSurface);
				_DEBUG_ASSERT(SUCCEEDED(hr));
				m_pOffScreenSurfaces.push_back(pSurface);
				D3DBOX box;
				box.Left = box.Right = box.Top = box.Bottom = 0;
				box.Front = box.Back = 0;
				m_DirtyRegions.push_back(box);
				_RELEASE(pSurface);

				//uint pitch = 0;
				//Lock(i, pitch, NULL);
				//Unlock(i);		
			}
		}
	}
}

void CRTexture3DD3D::OnResetDevice()
{
	_DEBUG_TRACE(_W("Resetting texture: %s\n"), GetResourceName());

	boolean success = ((CRResourceMgrD3D*)(m_pResourceMgr))->ResetTexture3D(this);
	_DEBUG_ASSERT(success);
}

void CRTexture3DD3D::OnLostDevice()
{
	_DEBUG_TRACE(_W("Lost texture: %s\n"), GetResourceName());

	m_pD3DTexture = NULL;
	m_pD3DTextureSurfaces.clear();
}

void CRTexture3DD3D::OnRestoreDevice()
{
}

_NAMESPACE_END