//==============================================================================
//
//		CRIndexBufferD3D.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Monday, April 24, 2006
//
//==============================================================================

#include "RendererD3D.h"

_NAMESPACE_BEGIN

boolean
CRIndexBufferD3D::DoUnlock()
{
	if(m_TempNumIndices > 0)
	{
		DWORD lockFlags = 0;

		if(GetUsage() == BUF_DYNAMIC)
		{
			uint tempEndIndex = m_TempStartIndex + m_TempNumIndices;
			uint endIndex = m_StartIndex + m_NumIndices;
						
			if((m_TempStartIndex >= endIndex))//(m_TempStartIndex < m_StartIndex && tempEndIndex <= m_StartIndex) || 				
			{
				lockFlags = D3DLOCK_NOOVERWRITE;
			}
			else
			{
				lockFlags = D3DLOCK_DISCARD;
			}
		}

		void* pDest = 0;
		HRESULT hr = m_pD3DIB->Lock(m_TempStartIndex * sizeof(ushort), m_TempNumIndices * sizeof(ushort), &pDest, lockFlags);
		_DEBUG_ASSERT(SUCCEEDED(hr));

		memcpy(pDest, m_pLockData, m_TempNumIndices * sizeof(ushort));
		
		hr = m_pD3DIB->Unlock();
		_DEBUG_ASSERT(SUCCEEDED(hr));
	}
	
	HRESULT hr = m_pOffScreen->Unlock();		
	_DEBUG_ASSERT(SUCCEEDED(hr));
	m_pLockData = NULL;
	
	return hr == D3D_OK;
}

ushort*
CRIndexBufferD3D::DoLock(uint startIndexToWrite, uint numIndicesToWrite)
{		
	DWORD flags = 0;
	if(numIndicesToWrite == 0)
	{
		flags |= D3DLOCK_READONLY;
		if(startIndexToWrite != 0)
			numIndicesToWrite = GetNumIndices();
	}

	HRESULT	hr = m_pOffScreen->Lock(startIndexToWrite * sizeof(ushort),
									numIndicesToWrite * sizeof(ushort),
									&m_pLockData,
									flags);
	_DEBUG_ASSERT(SUCCEEDED(hr));
					
	return (ushort*) m_pLockData;
}

void 
CRIndexBufferD3D::OnLostDevice()
{
	_DEBUG_TRACE(_W("Lost index buffer\n"));

	m_pD3DIB = NULL;
}

void 
CRIndexBufferD3D::OnResetDevice()
{	
	_DEBUG_TRACE(_W("Resetting index buffer\n"));

	if(!m_pParent)
	{
		boolean success = ((CRResourceMgrD3D*)(m_pResourceMgr))->ResetIndexBuffer(this); 
		_DEBUG_ASSERT(success);
	}
	else
	{
		// If it is shared, then just fetch
		// The order of restore will be parent first...
		SetD3DIB(m_pParent->GetD3DIB(), NULL);
	}
}

void 
CRIndexBufferD3D::OnRestoreDevice()
{
}

void CRIndexBufferD3D::SetD3DSharedIB(CRIndexBufferD3D* pParent)
{
	m_pParent = pParent;
	m_pD3DIB = pParent->GetD3DIB();
	m_pOffScreen = pParent->GetOffscreenIB();
}

void 
CRIndexBufferD3D::SetD3DIB(IDirect3DIndexBuffer9* pD3DIB, IDirect3DIndexBuffer9* pOffScreen) 
{ 
	_DEBUG_ASSERT(pD3DIB);

	m_pD3DIB = pD3DIB; 
	
	if(m_pOffScreen && !pOffScreen)
	{
		// Copy to the video RAM buffer from the offscreen buffer
		Lock(0, GetSize());
		Unlock(FALSE);
		
		//HRESULT hr = m_pOffScreen->Lock(0, GetSize() * sizeof(ushort), &m_pLockData, 0);
		//_DEBUG_ASSERT(SUCCEEDED(hr));
		//
		//void* pData = NULL;		
		//hr = m_pD3DIB->Lock(0, GetSize() * sizeof(ushort), &pData, 0);
		//_DEBUG_ASSERT(SUCCEEDED(hr));

		//memcpy(pData, m_pLockData, GetSize() * sizeof(ushort));
		//
		//HRESULT hr = m_pOffScreen->Unlock();
		//_DEBUG_ASSERT(SUCCEEDED(hr));
		//
		//m_pLockData = NULL;
		//
		//hr = m_pD3DIB->Unlock();
		//_DEBUG_ASSERT(SUCCEEDED(hr));
	}
	else
	{
		m_pOffScreen = pOffScreen;
	}
}


_NAMESPACE_END