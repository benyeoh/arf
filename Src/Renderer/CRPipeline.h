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

typedef std::vector< RRenderOp* > RenderOpList;
typedef std::vector< IRTextureRT* > RTList;
//typedef DenseChainedHashMap<RInstancedKey, RInstancedData, RInstancedKeyHash>	InstancedDataMap;
//typedef std::vector< RInstancedKey >											InstancedKeyList;
//	
//struct RInstancedKey
//{
//	uint hashVal;
//	RRenderOp* pInstanceOp;
//	
//	void Set(RRenderOp* pInstanceOp);
//	
//	boolean operator==(const RInstancedKey& other)
//	{
//		if(pInstanceOp->pVBGroup == other.pInstanceOp->pVBGroup &&
//			pInstanceOp->pIB == other.pInstanceOp->pIB &&
//			pInstanceOp->pEffect == other.pInstanceOp->pEffect &&
//			pInstanceOp->technique == other.pInstanceOp->technique &&
//			pInstanceOp->pass == other.pInstanceOp->pass)
//		{
//			uint numDynParams = pInstanceOp->pEffectTemplate->GetNumOfDynamicParams(pInstanceOp->technique);
//			_LOOPi(numDynParams)
//			{
//				if(pInstanceOp->params[i] != other.pInstanceOp->params[i])
//					return FALSE; 
//			}
//			
//			return TRUE;
//		}			
//		
//		return FALSE;
//	}
//};
//
//struct RInstancedKeyHash
//{
//	uint operator()(const RInstancedKey& key)
//	{
//		return key.hashVal;
//	}
//};
//
//struct RInstancedData
//{
//	std::vector<REffectParam> instancedParams;
//	uint instancedParamStride;
//};


//------------------------------------------------------------------------------
//	RRenderGroup
//		
//------------------------------------------------------------------------------
struct RRenderGroup
{	
	uint groupNum;
	
	//InstancedDataMap instancedDatas;
	//InstancedKeyList instancedKeys;
	
	RenderOpList renderOps;
	RenderOpSort sortFn;
	IRRenderCallback* pCallback;
	RTList rt;
	IRDepthBuffer* pDepthBuffer;
	gmtl::Vec4f clearColor;
	boolean useClearColor;
	float clearDepth;
	boolean useClearDepth;
	
	//RRenderGroup()
	//{
	//	instancedDatas.Initialize(64, 0.7f);
	//}
};

//------------------------------------------------------------------------------
//	CRPipeline
//		
//------------------------------------------------------------------------------
class CRPipeline : public IRPipeline
{
	typedef std::vector< RRenderGroup* >					RenderGroupList;
	typedef DenseChainedHashMap<uint, uint, NullUintHash>	RenderGroupIndices;

private:
	RenderGroupIndices		m_GroupIndices;
	RenderGroupList			m_RenderGroups;	
	RRenderOp*				m_pPrevOp;
	
	ObjectPool<RRenderGroup, 1024>	m_RenderGroupPool;
	ObjectPool<RRenderOp, 8192>		m_RenderOpPool;
	
	RenderOpList			m_InstancedOps;
		
protected:
	CRRenderer* m_pRenderer;
	
public:
	CRPipeline(CRRenderer* pRenderer)
		: m_pPrevOp(NULL)
		, m_pRenderer(pRenderer)
	{
		m_GroupIndices.Initialize(64, 0.75f);
	}

	virtual ~CRPipeline()
	{
	}

private:
	void DrawRenderOp( RRenderOp* pOp );
	void DrawRenderGroup( RRenderGroup* pGroup );
	boolean CompareInstancedOp(RRenderOp* pLhs, RRenderOp* pRhs);
	void RenderInstances( RRenderGroup* pGroup, RRenderOp** pInstanceOps, uint size );
	
protected:
	_PURE( void DoOnLostDevice() )
	_PURE( void DoOnRestoreDevice() )
	_PURE( void DoOnResetDevice() )
	
	_PURE( void DoBeginRenderTargets(IRTextureRT** ppRT, uint numRTs) )
	_PURE( void DoBeginDepthBuffer(IRDepthBuffer* pDepthBuffer) )

	_PURE( void DoEndRenderTargets() )
	_PURE( void DoEndDepthBuffer() )

	_PURE( uint DoProcessInstances(RRenderOp** ppInstanceOps, uint size) )
	
	//_PURE( void DoBeginProcessInstance(RRenderGroup* pGroup) )
	//_PURE( void DoEndProcessInstance(RRenderGroup* pGroup) )
	//_PURE( void DoProcessOneInstance(RRenderGroup* pGroup, RInstancedKey& key, RInstancedData& data) )
	
	//_PURE( void DoApplyIndexBuffer(CRIndexBuffer* pIB) )
	//_PURE( void DoApplyVertexBufferGroup(CRVertexBufferGroup* pGroup) )
	
public:
	void OnLostDevice();	
	void OnResetDevice();
	void OnRestoreDevice();
	
	uint GetRenderGroupIndex(uint groupNum);
	uint CreateNewRenderGroup(uint groupNum, RenderOpSort sortFn, IRRenderCallback& callback);
	void AddRenderTarget(uint groupIndex, IRTextureRT* pRT);
	void SetDepthBuffer(uint groupIndex, IRDepthBuffer* pDepth);
	void SetClearColor(uint groupIndex, const gmtl::Vec4f& color);
	void SetClearDepth(uint groupIndex, float val);
	void AddToRenderGroup(uint groupIndex,
						  IRVertexBufferGroup* pVBGroup,
						  IRIndexBuffer* pIB,
						  IREffect* pEffect,
						  REffectParam* pParams,
						  uint techToUse,
						  void* pSortData,
						  eRPrimitiveType prim=PRIM_TRIANGLE);
	//
	//void AddToRenderGroupInstanced(uint groupIndex,
	//						REffectParam* pInstancedParams,
	//						uint numInstancedParams,
	//						RRenderOp* pOp);	
	//
	uint GetNumOfRenderGroups();
	uint GetRenderGroupID(uint groupIndex);
	uint GetNumOfRenderTargets(uint groupIndex);
	IRTextureRT* GetRenderTarget(uint groupIndex, uint rtIndex);
	IRDepthBuffer* GetDepthBuffer(uint groupIndex);
	
	void RenderPipeline();
	void RenderImmediate(uint groupIndex);
};

_NAMESPACE_END