//==============================================================================
//
//		CRVertexBufferD3D.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Thursday, April 20, 2006
//
//==============================================================================

#include "RendererD3D.h"


_NAMESPACE_BEGIN

byte* 
CRVertexBufferD3D::DoLockImmediate(uint startVertexToWrite, uint numVerticesToWrite)
{
	DWORD lockFlags = 0;

	if(GetUsage() == BUF_DYNAMIC)
	{
		uint tempEndVertex = startVertexToWrite + numVerticesToWrite;
		uint endVertex = m_StartVertex + m_NumVertices;

		if((startVertexToWrite >= endVertex))
		{
			lockFlags = D3DLOCK_NOOVERWRITE;
		}
		else
		{
			lockFlags = D3DLOCK_DISCARD;
		}
	}

	_DEBUG_ASSERT(((numVerticesToWrite + startVertexToWrite) * m_VertexSize) <= (m_Size * m_VertexSize));

	void* pDest = 0;
	HRESULT hr = m_pD3DVB->Lock(startVertexToWrite * m_VertexSize, numVerticesToWrite * m_VertexSize, &pDest, lockFlags);
	_DEBUG_ASSERT(SUCCEEDED(hr));

	return (byte*) pDest;
}

boolean 
CRVertexBufferD3D::DoUnlockImmediate()
{
	HRESULT hr = m_pD3DVB->Unlock();
	_DEBUG_ASSERT(SUCCEEDED(hr));
	
	return TRUE;
}


byte* 
CRVertexBufferD3D::DoLock(uint startVertexToWrite, uint numVerticesToWrite)
{
	m_pLockData = m_pOffScreen->GetData() + (startVertexToWrite * m_VertexSize);
	return (byte*) m_pLockData;	
}

boolean 
CRVertexBufferD3D::DoUnlock()
{
	if(m_TempNumVertices > 0)
	{
		DWORD lockFlags = 0;

		if(GetUsage() == BUF_DYNAMIC)
		{
			uint tempEndVertex = m_TempStartVertex + m_TempNumVertices;
			uint endVertex = m_StartVertex + m_NumVertices;

			if((m_TempStartVertex >= endVertex))
			{
				lockFlags = D3DLOCK_NOOVERWRITE;
			}
			else
			{
				lockFlags = D3DLOCK_DISCARD;
			}
		}

		_DEBUG_ASSERT(((m_TempNumVertices + m_TempStartVertex) * m_VertexSize) <= (m_Size * m_VertexSize));

		void* pDest = 0;
		HRESULT hr = m_pD3DVB->Lock(m_TempStartVertex * m_VertexSize, m_TempNumVertices * m_VertexSize, &pDest, lockFlags);
		_DEBUG_ASSERT(SUCCEEDED(hr));

		memcpy((byte*)pDest, (byte*)m_pLockData, m_TempNumVertices * m_VertexSize);

		hr = m_pD3DVB->Unlock();
		_DEBUG_ASSERT(SUCCEEDED(hr));
	}
		
	m_pLockData = NULL;

	return TRUE;
}

void 
CRVertexBufferD3D::OnLostDevice()
{
	_DEBUG_TRACE(_W("Lost vertex buffer\n"));
	_DEBUG_ASSERT(m_pD3DVB != NULL);

	m_pD3DVB = NULL;
}

void 
CRVertexBufferD3D::OnResetDevice()
{
	_DEBUG_TRACE(_W("Resetting vertex buffer\n"));

	_DEBUG_ASSERT(m_pD3DVB == NULL);

	if(!m_pParent)
	{
		boolean success = ((CRResourceMgrD3D*)(m_pResourceMgr))->ResetVertexBuffer(this); 
		_DEBUG_ASSERT(success);
	}
	else
	{
		SetD3DVB(m_pParent->GetD3DVB(), NULL);
	}

	//SetData( GetData() );
}

void 
CRVertexBufferD3D::OnRestoreDevice()
{
}

void CRVertexBufferD3D::SetD3DSharedVB(CRVertexBufferD3D* pParent)
{
	m_pParent = pParent;
	m_pD3DVB = pParent->GetD3DVB();
	m_pOffScreen = pParent->GetOffscreenVB();
}

void 
CRVertexBufferD3D::SetD3DVB(IDirect3DVertexBuffer9* pD3DVB, IByteBuffer* pOffScreen)
{ 
	_DEBUG_ASSERT(pD3DVB);

	m_pD3DVB = pD3DVB; 
	
	if(m_pOffScreen && !pOffScreen)
	{
		// Copy to the video RAM buffer from the offscreen buffer
		Lock(0, GetSize());
		Unlock(FALSE);
	}
	else
	{
		m_pOffScreen = pOffScreen;
	}
}


_NAMESPACE_END