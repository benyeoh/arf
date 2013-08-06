//================================================================================
//
//		RenderOpPool.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/4/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

class RenderOpPool
{
	const static uint RENDER_OP_CHUNK_SIZE = 256;

private:
	ObjectPoolGrowable<RRenderOp, RENDER_OP_CHUNK_SIZE>	m_Pool;
	Mutex	m_Mutex;
	boolean	m_IsInit;

	RRenderOp** m_ppRenderOpBuffer;
	uint		m_BufferSize;
	uint		m_NumRenderOps;

public:
	RenderOpPool()
		: m_ppRenderOpBuffer(NULL)
		, m_NumRenderOps(0)
		, m_BufferSize(0)
	{
		m_Mutex.Initialize();
	}

	~RenderOpPool()
	{
	}

public:
	RRenderOp* AllocRenderOp();
	RRenderOp** GetRenderOpBuffer();

	uint GetNumOfRenderOps();
	void Deinit();
	void Reset();
};

_NAMESPACE_END
