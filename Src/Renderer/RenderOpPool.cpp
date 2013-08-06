//================================================================================
//
//		RenderOpPool.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/4/2012
//
//================================================================================

#include "RendererFramework.h"

_NAMESPACE_BEGIN

RRenderOp* RenderOpPool::AllocRenderOp()
{
	RRenderOp* pToReturn;
	{
		MutexScopeLock scopeLock(&m_Mutex);
		
		if(!m_ppRenderOpBuffer)
			m_Pool.Initialize();

		pToReturn = m_Pool.Get();

		if(m_BufferSize <= m_NumRenderOps)
		{
			RRenderOp** ppSrc = m_ppRenderOpBuffer;
			m_ppRenderOpBuffer = (RRenderOp**) _MALLOC( (m_BufferSize + RENDER_OP_CHUNK_SIZE) * sizeof(RRenderOp*) );
			
			if(ppSrc)
			{
				memcpy(m_ppRenderOpBuffer, ppSrc, m_BufferSize * sizeof(RRenderOp*));
				_FREE(ppSrc);
			}

			m_BufferSize += RENDER_OP_CHUNK_SIZE;
		}

		m_ppRenderOpBuffer[m_NumRenderOps] = pToReturn;
		m_NumRenderOps++;
	}

	return pToReturn;
}

void RenderOpPool::Reset()
{
	m_Pool.Reset();
	m_NumRenderOps = 0;
}

RRenderOp** RenderOpPool::GetRenderOpBuffer()
{
	return m_ppRenderOpBuffer;
}

void RenderOpPool::Deinit()
{
	m_Pool.Destroy();
	
	if(m_ppRenderOpBuffer)
	{
		_FREE(m_ppRenderOpBuffer);
		m_ppRenderOpBuffer = NULL;
	}

	m_BufferSize = 0;
	m_NumRenderOps = 0;
}

uint RenderOpPool::GetNumOfRenderOps()
{
	return m_NumRenderOps;
}

_NAMESPACE_END