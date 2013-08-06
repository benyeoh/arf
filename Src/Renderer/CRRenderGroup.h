//==============================================================================
//
//		CRPipeline.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, February 26, 2006
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN


//------------------------------------------------------------------------------
//	CRPipeline
//		
//------------------------------------------------------------------------------
class CRRenderGroup : public CRefCounted<IRRenderGroup>
{
	typedef std::vector< IRTextureRT* > RTList;

private:
//	RenderOpPool		m_ReservePool;

	RRenderOp*			m_pRenderOps;
	RRenderOp**			m_ppRenderOpSorted;
	RRenderOp**			m_ppRenderOpInstanced;

	uint				m_MaxBufferSize;
	uint				m_MaxListSize;
	uint				m_NumRenderOps;
	uint				m_NumRenderOpsInstanced;
	uint				m_NumRenderOpsSorted;

	RenderOpSort		m_SortFn;
	
	IRRenderOpCallback*	m_pCallback;
	
	RTList				m_RenderTargets;
	IRDepthBufferPtr	m_pDepthBuffer;
	gmtl::Vec4f			m_ClearColor;
	boolean				m_IsUseClearColor;
	float				m_ClearDepth;
	boolean				m_IsUseClearDepth;

	uint				m_ViewportLeft;
	uint				m_ViewportTop;
	uint				m_ViewportWidth;
	uint				m_ViewportHeight;
	RRenderOp*			m_pPrevOp;

protected:
	CRRenderer*			m_pRenderer;

public:
	CRRenderGroup(CRRenderer* pRenderer, 
				RenderOpSort sort, 
				uint bufferSize)
		: m_pRenderer(pRenderer)
		, m_SortFn(sort)
		, m_IsUseClearColor(FALSE)
		, m_IsUseClearDepth(FALSE)
		, m_ClearDepth(1.0f)
		, m_pRenderOps(NULL)
		, m_MaxBufferSize(bufferSize)
		, m_NumRenderOps(0)
		, m_ppRenderOpSorted(NULL)
		, m_ppRenderOpInstanced(NULL)
		, m_pPrevOp(NULL)
		, m_ViewportWidth(1024)
		, m_ViewportHeight(768)
		, m_ViewportLeft(0)
		, m_ViewportTop(0)
		, m_pDepthBuffer(NULL)
		, m_NumRenderOpsInstanced(0)
		, m_NumRenderOpsSorted(0)
		, m_MaxListSize(bufferSize)
	{
		if(bufferSize > 0)
		{
			m_pRenderOps = (RRenderOp*) _MALLOC(sizeof(RRenderOp) * m_MaxBufferSize);
			m_ppRenderOpSorted = (RRenderOp**) _MALLOC(sizeof(RRenderOp*) * m_MaxListSize);
			m_ppRenderOpInstanced = (RRenderOp**) _MALLOC(sizeof(RRenderOp*) * m_MaxListSize);
		}

		m_ViewportWidth		= pRenderer->GetDisplayInfo().backBufferWidth;
		m_ViewportHeight	= pRenderer->GetDisplayInfo().backBufferHeight;
	}

	virtual ~CRRenderGroup()
	{
		ClearRenderTargets();

		_FREE(m_pRenderOps);
		_FREE(m_ppRenderOpSorted);
		_FREE(m_ppRenderOpInstanced);
	}

private:
	void DrawRenderOp( RRenderOp* pOp );
	boolean CompareInstancedOp(RRenderOp* pLhs, RRenderOp* pRhs);
	void RenderInstances( RRenderOp** pInstanceOps, uint size );
	RRenderOp* AllocRenderOp();

protected:
	//_PURE( void DoOnLostDevice() )
	//_PURE( void DoOnRestoreDevice() )
	//_PURE( void DoOnResetDevice() )

	_PURE( void DoBeginRenderTargets(IRTextureRT** ppRT, uint numRTs) )
	_PURE( void DoBeginDepthBuffer(IRDepthBuffer* pDepthBuffer) )

	_PURE( void DoSetViewports(uint left, uint top, uint width, uint height) )

	_PURE( void DoEndRenderTargets() )
	_PURE( void DoEndDepthBuffer() )

	_PURE( uint DoProcessInstances(RRenderOp** ppInstanceOps, uint size) )

public:
	void SetViewport(uint left, uint top, uint width, uint height);

	void AddRenderTarget(IRTextureRT* pRT);
	void ClearRenderTargets();
	void SetDepthBuffer(IRDepthBuffer* pDepth);
	void SetClearColor(const gmtl::Vec4f& color);
	void SetClearDepth(float val);
	void AddDrawOp(IRVertexBufferGroup* pVBGroup,
				IRIndexBuffer* pIB,
				IREffect* pEffect,
				REffectParam* pParams,
				uint techToUse,
				RVarData userData,
				eRPrimitiveType prim=PRIM_TRIANGLE);

	void Flush();
	void Reset();

	void FillDrawOp(RRenderOp* pDest,
					IRVertexBufferGroup* pVBGroup,
					IRIndexBuffer* pIB,
					IREffect* pEffect,
					REffectParam* pParams,
					uint techToUse,
					RVarData userData,
					eRPrimitiveType prim=PRIM_TRIANGLE);
	void AddToList(RRenderOp** pOp, uint numOps);
	uint GetNumOfDrawOps();

	uint GetNumOfRenderTargets();
	IRTextureRT* GetRenderTarget(uint rtIndex);
	IRDepthBuffer* GetDepthBuffer();
};

_NAMESPACE_END