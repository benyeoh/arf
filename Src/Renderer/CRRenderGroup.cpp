//==============================================================================
//
//		CRRenderGroup.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		07/07/10
//
//==============================================================================

#include "RendererFramework.h"

_NAMESPACE_BEGIN

#define _PARTIAL_HASH(val) \
{ \
	uint k = val; \
	k *= m; \
	k ^= k >> r; \
	k *= m; \
	\
	h *= m; \
	h ^= k; \
}

//void RInstancedKey::Set(uint index, RRenderOp* pOp)
//{
//	const uint m = 0x5bd1e995;
//	const int r = 24;
//
//	// Initialize the hash to a 'random' value
//	uint h = 1234567 ^ 12;
//
//	_PARTIAL_HASH(index);
//	_PARTIAL_HASH(pOp->pVBGroup);
//	_PARTIAL_HASH(pOp->pIB);
//	_PARTIAL_HASH(pOp->pEffect);
//	_PARTIAL_HASH(pOp->technique);
//	_PARTIAL_HASH(pOp->pass);
//
//	uint numDynamicParams = pOp->pEffectTemplate->GetNumOfDynamicParams(pOp->technique);
//	_LOOPi(numDynamicParams)
//	{
//		switch(pOp->params[i].type)
//		{
//			case EPT_FLOAT:
//			case EPT_INT:	
//			case EPT_BOOL:	
//			case EPT_VOID_P:
//			case EPT_TEX2D_P:
//			case EPT_TEXRT_P:
//			case EPT_TEXCUBE_P:		
//			case EPT_MAT44_P:
//			case EPT_MAT33_P:
//			case EPT_MAT22_P:
//			case EPT_VEC4_P:
//			case EPT_VEC3_P:
//			case EPT_VEC2_P:
//			case EPT_STRING:
//				_PARTIAL_HASH(pOp->params[i].intVal);
//				break;
//		}
//	}
//
//	h ^= h >> 13;
//	h *= m;
//	h ^= h >> 15;
//
//	this->groupIndex	= index;
//	this->pInstanceOp	= pOp;
//} 


//------------------------------------------------------------------------------
//	RenderGroup
//------------------------------------------------------------------------------

//const static uint MAX_NUM_RENDER_GROUPS = 256;
//
//static RRenderGroup s_RenderGroupPool[MAX_NUM_RENDER_GROUPS];
//static uint s_RGUsed = 0;
//
//inline RRenderGroup* 
//GetRenderGroup()
//{
//	if(s_RGUsed < MAX_NUM_RENDER_GROUPS)
//	{
//		s_RGUsed++;
//		return &(s_RenderGroupPool[s_RGUsed-1]);
//	}
//
//	_DEBUG_ASSERT(FALSE);
//	return NULL;
//}
//
//inline void
//DisposeRenderGroup() { if(s_RGUsed) s_RGUsed--; }
//
//inline void 
//ClearRenderGroup() { s_RGUsed = 0; }

//------------------------------------------------------------------------------
//	RenderOp
//------------------------------------------------------------------------------

//const static uint MAX_NUM_RENDER_OPS = 3000;
//
//static RRenderOp s_RenderOpPool[MAX_NUM_RENDER_OPS];
//static uint s_ROUsed = 0;
//
//inline RRenderOp* 
//GetRenderOp()
//{
//	if(s_ROUsed < MAX_NUM_RENDER_OPS)
//	{
//		s_ROUsed++;
//		return &(s_RenderOpPool[s_ROUsed-1]);
//	}
//	
//	_DEBUG_ASSERT(FALSE);
//	return NULL;
//}
//
//inline void
//DisposeRenderOp() { if(s_ROUsed) s_ROUsed--; }
//
//inline void 
//ClearRenderOps() { s_ROUsed = 0; }

//struct OpaqueSortLess
//{
//	inline bool operator()(	const RRenderOp* pLhs, const RRenderOp* pRhs ) const
//	{
//		if( pLhs->pEffect != pRhs->pEffect )
//			return pLhs->pEffect < pRhs->pEffect;
//		if( pLhs->technique != pRhs->technique ) 
//			return pLhs->technique < pRhs->technique;
//		if( pLhs->pass != pRhs->pass ) 
//			return pLhs->pass < pRhs->pass;
//		
//		_DEBUG_ASSERT(pLhs->numParams == pRhs->numParams);
//		_DEBUG_ASSERT(pLhs->numMustSortParams == pRhs->numMustSortParams);
//		_DEBUG_ASSERT(pLhs->numMustSortParams <= pRhs->numParams);
//		
//		_LOOPi( pLhs->numMustSortParams )
//		{
//			if( pLhs->params[i] != pRhs->params[i] )
//				return pLhs->params[i] < pRhs->params[i];
//		}
//		
//		// TODO: Sort by vb?
//		
//		return ( pLhs->distance < pRhs->distance );
//	}
//};
//

//------------------------------------------------------------------------------
//	Function Defs
//------------------------------------------------------------------------------

RRenderOp* 
CRRenderGroup::AllocRenderOp()
{
	if(m_NumRenderOps >= m_MaxBufferSize)
	{
		uint newBufferSize = m_MaxBufferSize * 2;
		if(newBufferSize <= 0)
			newBufferSize = 1;

		RRenderOp* pNewBuffer = (RRenderOp*) _MALLOC(sizeof(RRenderOp) * newBufferSize);
		memcpy(pNewBuffer, m_pRenderOps, sizeof(RRenderOp) * m_MaxBufferSize);
		
		if(m_pRenderOps)
			_FREE(m_pRenderOps);
	
		m_pRenderOps = pNewBuffer;
		m_MaxBufferSize = newBufferSize;

		_LOOPi(m_NumRenderOps)
		{
			m_ppRenderOpSorted[i] = &m_pRenderOps[i];
		}
	}

	RRenderOp* pRenderOp = &m_pRenderOps[m_NumRenderOps];
	m_NumRenderOps++;

	//uint numRenderOps = (uint) AtomicInc( (int*) &m_NumRenderOps );
	//RRenderOp* pRenderOp = NULL;
	//if(numRenderOps <= m_MaxBufferSize)
	//{
	//	pRenderOp = &(m_pRenderOps[numRenderOps-1]);
	//	m_ppRenderOpSorted[numRenderOps-1] = pRenderOp;
	//}
	//else
	//{
	//	// Fallback to mutex for expensive allocation when buffer is overflowed
	//	pRenderOp = m_ReservePool.AllocRenderOp();
	//}

	//m_NumRenderOps++;
	return pRenderOp;
}

void CRRenderGroup::AddToList(RRenderOp** pOp, uint numOps)
{
	uint totalSize = m_NumRenderOpsSorted + numOps;
	if(totalSize > m_MaxListSize)
	{
		uint newListSize = m_MaxListSize * 2;
		if(newListSize < totalSize)
			newListSize = totalSize;

		RRenderOp** pNewList = (RRenderOp**) _MALLOC(sizeof(RRenderOp*) * newListSize);
		memcpy(pNewList, m_ppRenderOpSorted, sizeof(RRenderOp*) * m_MaxListSize);

		if(m_ppRenderOpSorted)
			_FREE(m_ppRenderOpSorted );

		m_ppRenderOpSorted = pNewList;
		m_MaxListSize = newListSize;

		if(m_ppRenderOpInstanced)
			_FREE(m_ppRenderOpInstanced);

		m_ppRenderOpInstanced = (RRenderOp**) _MALLOC(sizeof(RRenderOp*) * newListSize);
	}

	_LOOPi(numOps)
	{
		m_ppRenderOpSorted[m_NumRenderOpsSorted + i] = pOp[i];
	}

	m_NumRenderOpsSorted = totalSize;
}

void 
CRRenderGroup::ClearRenderTargets()
{
	_LOOPi(m_RenderTargets.size())
	{
		m_RenderTargets[i]->Release();
	}

	m_RenderTargets.clear();
}

void 
CRRenderGroup::AddRenderTarget(IRTextureRT* pRT)
{
	_DEBUG_ASSERT(pRT);
	pRT->AddRef();
	m_RenderTargets.push_back(pRT);
}

void 
CRRenderGroup::SetDepthBuffer(IRDepthBuffer* pDepth)
{
	m_pDepthBuffer = pDepth;
}

void 
CRRenderGroup::AddDrawOp(IRVertexBufferGroup* pVBGroup,
						 IRIndexBuffer* pIB,
						 IREffect* pEffect,
						 REffectParam* pParams,
						 uint techToUse,
						 RVarData userData,
						 eRPrimitiveType prim)
{
	_DEBUG_ASSERT(pEffect);

	CREffectTemplate* pCREffectTemplate = (CREffectTemplate*)(((CREffect*)pEffect)->GetTemplate());
	_DEBUG_ASSERT(pCREffectTemplate);

	// Compute shader detail level
	uint numTechs = pCREffectTemplate->GetNumOfTechs();
	uint techUsed = techToUse;

	_DEBUG_ASSERT(techUsed >= 0);
	_DEBUG_ASSERT(techUsed < numTechs);

	uint numDynamicParams	= pCREffectTemplate->GetNumOfDynamicParams(techUsed);
	uint numInstancedParams	= pCREffectTemplate->GetNumOfInstancedParams(techUsed);

	_DEBUG_ASSERT((numDynamicParams + numInstancedParams) <= RRenderOp::MAX_NUM_PARAMS);

	// For each pass, add a render op
	//uint numPasses = pCREffectTemplate->GetNumOfPasses(techUsed);
	//_LOOPi(numPasses)
	{
		RRenderOp* pRenderOp = AllocRenderOp();

		_LOOPj(numDynamicParams + numInstancedParams)
			pRenderOp->params[j] = pParams[j];

		// Init the render op
		pRenderOp->pVBGroup				= pVBGroup;
		pRenderOp->pIB					= pIB;
		pRenderOp->pEffect				= pEffect;
		pRenderOp->pEffectTemplate		= pCREffectTemplate;
		pRenderOp->technique			= techUsed;
		pRenderOp->userData				= userData;
		//pRenderOp->numPasses			= numPasses;
		pRenderOp->prim					= prim;

		pRenderOp->numInstancedParams = numInstancedParams;
		if(numInstancedParams == 0)
		{
			pRenderOp->opHash = 0; // Not needed
		}
		else
		{
			const uint m = 0x5bd1e995;
			const int r = 24;

			// Initialize the hash to a 'random' value
			uint h = 1234567 ^ 12;

			//_PARTIAL_HASH(pRenderOp->pVBGroup);
			//_PARTIAL_HASH(pRenderOp->pIB);
			//_PARTIAL_HASH(pRenderOp->pEffect);
			//_PARTIAL_HASH(pRenderOp->technique);
			//_PARTIAL_HASH(pRenderOp->pass);

			uint numDynamicParams = pCREffectTemplate->GetNumOfDynamicParams(techUsed);
			_LOOPi(numDynamicParams)
			{
				switch(pParams[i].type)
				{
				case EPT_FLOAT:
				case EPT_INT:	
				case EPT_BOOL:	
				case EPT_VOID_P:
				case EPT_TEX2D_P:
				//case EPT_TEXRT_P:
				case EPT_TEXCUBE_P:		
				case EPT_MAT44_P:
				case EPT_MAT33_P:
				case EPT_MAT22_P:
				case EPT_VEC4_P:
				case EPT_VEC3_P:
				case EPT_VEC2_P:
				case EPT_STRING:
					_PARTIAL_HASH(pParams[i].intVal);
					break;
				}
			}

			h ^= h >> 13;
			h *= m;
			h ^= h >> 15;

			pRenderOp->opHash = h;

			//AddToRenderGroupInstanced(*m_RenderGroups[groupIndex], &(pParams[numDynamicParams]), numInstancedParams, pRenderOp);
		}

		AddToList(&pRenderOp, 1);

		//m_RenderGroups[groupIndex]->renderOps.push_back( pRenderOp );
	}
}

void CRRenderGroup::FillDrawOp(RRenderOp* pDest,
							 IRVertexBufferGroup* pVBGroup,
							 IRIndexBuffer* pIB,
							 IREffect* pEffect,
							 REffectParam* pParams,
							 uint techToUse,
							 RVarData userData,
							 eRPrimitiveType prim)
{
	_DEBUG_ASSERT(pEffect);

	CREffectTemplate* pCREffectTemplate = (CREffectTemplate*)(((CREffect*)pEffect)->GetTemplate());
	_DEBUG_ASSERT(pCREffectTemplate);

	// Compute shader detail level
	uint numTechs = pCREffectTemplate->GetNumOfTechs();
	uint techUsed = techToUse;

	_DEBUG_ASSERT(techUsed >= 0);
	_DEBUG_ASSERT(techUsed < numTechs);

	uint numDynamicParams	= pCREffectTemplate->GetNumOfDynamicParams(techUsed);
	uint numInstancedParams	= pCREffectTemplate->GetNumOfInstancedParams(techUsed);

	_DEBUG_ASSERT((numDynamicParams + numInstancedParams) <= RRenderOp::MAX_NUM_PARAMS);

	{
		RRenderOp* pRenderOp = pDest;//AllocRenderOp();

		_LOOPj(numDynamicParams + numInstancedParams)
			pRenderOp->params[j] = pParams[j];

		// Init the render op
		pRenderOp->pVBGroup				= pVBGroup;
		pRenderOp->pIB					= pIB;
		pRenderOp->pEffect				= pEffect;
		pRenderOp->pEffectTemplate		= pCREffectTemplate;
		pRenderOp->technique			= techUsed;
		pRenderOp->userData				= userData;
		//pRenderOp->numPasses			= numPasses;
		pRenderOp->prim					= prim;

		pRenderOp->numInstancedParams = numInstancedParams;
		if(numInstancedParams == 0)
		{
			pRenderOp->opHash = 0; // Not needed
		}
		else
		{
			const uint m = 0x5bd1e995;
			const int r = 24;

			// Initialize the hash to a 'random' value
			uint h = 1234567 ^ 12;

			//_PARTIAL_HASH(pRenderOp->pVBGroup);
			//_PARTIAL_HASH(pRenderOp->pIB);
			//_PARTIAL_HASH(pRenderOp->pEffect);
			//_PARTIAL_HASH(pRenderOp->technique);
			//_PARTIAL_HASH(pRenderOp->pass);

			uint numDynamicParams = pCREffectTemplate->GetNumOfDynamicParams(techUsed);
			_LOOPi(numDynamicParams)
			{
				switch(pParams[i].type)
				{
				case EPT_FLOAT:
				case EPT_INT:	
				case EPT_BOOL:	
				case EPT_VOID_P:
				case EPT_TEX2D_P:
				//case EPT_TEXRT_P:
				case EPT_TEXCUBE_P:		
				case EPT_MAT44_P:
				case EPT_MAT33_P:
				case EPT_MAT22_P:
				case EPT_VEC4_P:
				case EPT_VEC3_P:
				case EPT_VEC2_P:
				case EPT_STRING:
					_PARTIAL_HASH(pParams[i].intVal);
					break;
				}
			}

			h ^= h >> 13;
			h *= m;
			h ^= h >> 15;

			pRenderOp->opHash = h;

			//AddToRenderGroupInstanced(*m_RenderGroups[groupIndex], &(pParams[numDynamicParams]), numInstancedParams, pRenderOp);
		}

		//AddToList(pRenderOp);

		//m_RenderGroups[groupIndex]->renderOps.push_back( pRenderOp );
	}
}

void
CRRenderGroup::DrawRenderOp( RRenderOp* pOp )
{
	_DEBUG_ASSERT( pOp );

	//IAppCallback& app = m_pRenderer->GetAppCallback();

	// Set the effect
	_DEBUG_ASSERT( pOp->pEffect );
	CREffect* pEffect = (CREffect*)(pOp->pEffect);		
	CREffectTemplate* pEffectTemplate = (CREffectTemplate*)(pOp->pEffectTemplate);
	CRVertexBufferGroup* pVBGroup = (CRVertexBufferGroup*) pOp->pVBGroup;
	CRIndexBuffer* pIB = (CRIndexBuffer*) pOp->pIB;

	_DEBUG_ASSERT( pEffectTemplate );

	if(m_pPrevOp)
	{
		CREffect* pPrevEffect = (CREffect*)(m_pPrevOp->pEffect);
		_DEBUG_ASSERT( pPrevEffect );
		CREffectTemplate* pPrevEffectTemplate = (CREffectTemplate*)(m_pPrevOp->pEffectTemplate); 
		_DEBUG_ASSERT( pPrevEffectTemplate );

		// Same effect
		if(pPrevEffect == pEffect)
		{
			// Same technique
			if(m_pPrevOp->technique == pOp->technique)
			{
				uint numPasses = pEffectTemplate->GetNumOfPasses(pOp->technique);
				if(numPasses <= 1)
				{
					pEffectTemplate->ApplyDynamicParams(pOp->params, m_pPrevOp->params);
					m_pRenderer->RenderPrimitive(pOp);
				}
				else
				{
					pPrevEffectTemplate->EndPass();
					pEffectTemplate->ApplyDynamicParams(pOp->params, m_pPrevOp->params);
					pEffectTemplate->BeginPass(0);
					m_pRenderer->RenderPrimitive(pOp);

					_LOOPi(numPasses - 1)
					{
						pEffectTemplate->EndPass();
						// pEffectTemplate->ApplyDynamicParams(pOp->params);
						pEffectTemplate->BeginPass(i+1);		
						m_pRenderer->RenderPrimitive(pOp);
					}
				}
			}
			// Different technique
			else
			{
				pPrevEffectTemplate->EndPass();
				pPrevEffectTemplate->EndTechnique();

				pEffectTemplate->BeginTechnique(pOp->technique);

				pEffectTemplate->ApplyConstantParams(pEffect->GetParams());
				pEffectTemplate->ApplyDynamicParams(pOp->params, m_pPrevOp->params);

				pEffectTemplate->BeginPass(0);
				m_pRenderer->RenderPrimitive(pOp);

				uint numPasses = pEffectTemplate->GetNumOfPasses(pOp->technique);
				_LOOPi(numPasses - 1)
				{
					pEffectTemplate->EndPass();
					// pEffectTemplate->ApplyDynamicParams(pOp->params);
					pEffectTemplate->BeginPass(i+1);		
					m_pRenderer->RenderPrimitive(pOp);
				}
			}
		}
		else // Different effect
		{
			// Same template and technique
			if(pPrevEffectTemplate == pEffectTemplate && pOp->technique == m_pPrevOp->technique)
			{
				uint numPasses = pEffectTemplate->GetNumOfPasses(pOp->technique);
				if(numPasses <= 1)
				{
					pEffectTemplate->ApplyConstantParams(pEffect->GetParams());			
					pEffectTemplate->ApplyDynamicParams(pOp->params, m_pPrevOp->params);
					m_pRenderer->RenderPrimitive(pOp);
				}
				else
				{
					pPrevEffectTemplate->EndPass();
					pEffectTemplate->ApplyConstantParams(pEffect->GetParams());
					pEffectTemplate->ApplyDynamicParams(pOp->params, m_pPrevOp->params);
					pEffectTemplate->BeginPass(0);
					m_pRenderer->RenderPrimitive(pOp);

					_LOOPi(numPasses - 1)
					{
						pEffectTemplate->EndPass();
						// pEffectTemplate->ApplyDynamicParams(pOp->params);
						pEffectTemplate->BeginPass(i+1);		
						m_pRenderer->RenderPrimitive(pOp);
					}
				}
			}
			// Different template or technique
			else	
			{
				pPrevEffectTemplate->EndPass();
				pPrevEffectTemplate->EndTechnique();

				pEffectTemplate->BeginTechnique(pOp->technique);

				pEffectTemplate->ApplyConstantParams(pEffect->GetParams());
				pEffectTemplate->ApplyDynamicParams(pOp->params);

				pEffectTemplate->BeginPass(0);
				m_pRenderer->RenderPrimitive(pOp);
				uint numPasses = pEffectTemplate->GetNumOfPasses(pOp->technique);

				_LOOPi(numPasses - 1)
				{
					pEffectTemplate->EndPass();
					// pEffectTemplate->ApplyDynamicParams(pOp->params);
					pEffectTemplate->BeginPass(i+1);		
					m_pRenderer->RenderPrimitive(pOp);
				}
			}
		}
	}
	else
	{
		pEffectTemplate->BeginTechnique(pOp->technique);
		pEffectTemplate->ApplyConstantParams(pEffect->GetParams());
		pEffectTemplate->ApplyDynamicParams(pOp->params);	// TODO:
		pEffectTemplate->BeginPass(0);

		m_pRenderer->RenderPrimitive(pOp);
		uint numPasses = pEffectTemplate->GetNumOfPasses(pOp->technique);
		
		_LOOPi(numPasses - 1)
		{
			pEffectTemplate->EndPass();
			// pEffectTemplate->ApplyDynamicParams(pOp->params);
			pEffectTemplate->BeginPass(i+1);		
			m_pRenderer->RenderPrimitive(pOp);
		}
	}

	// Set the reference render op
	m_pPrevOp = pOp;
}

void 
CRRenderGroup::SetClearColor(const gmtl::Vec4f& color)
{
	m_ClearColor = color;
	m_IsUseClearColor = TRUE;
}

void 
CRRenderGroup::SetClearDepth(float val)
{
	m_ClearDepth = val;
	m_IsUseClearDepth = TRUE;
}

void 
CRRenderGroup::SetViewport(uint left, uint top, uint width, uint height)
{
	m_ViewportLeft		= left;
	m_ViewportTop		= top;
	m_ViewportWidth		= width;
	m_ViewportHeight	= height;
}

//void 
//CRPipeline::ProcessInstances( RRenderGroup* pGroup )
//{
//	//DoBeginProcessInstance( pGroup );
//	
//	uint numKeys = (uint) pGroup->instancedKeys.size();
//	_LOOPi(numKeys)
//	{
//		RInstancedKey& key = pGroup->instancedKeys[i]; 
//		RInstancedData* pData = pGroup->instancedDatas.Get(key);
//		_DEBUG_ASSERT(pData);
//		
//		DoProcessOneInstance(pGroup, key, *pData);		
//		
//		pGroup->instancedDatas.Remove(key);
//	}
//		
//	pGroup->instancedKeys.clear();
//
//	//DoEndProcessInstance( pGroup );	
//}

void
CRRenderGroup::RenderInstances(RRenderOp** ppInstanceOps, uint size)
{
	// Gather all instances
	do
	{
		uint numInstances = DoProcessInstances(ppInstanceOps, size);
		RRenderOp* pToRender = ppInstanceOps[0];
		pToRender->numInstances = numInstances;

		// Draw the instances
		DrawRenderOp(pToRender);

		size -= numInstances;
		ppInstanceOps = ppInstanceOps + numInstances;
	}while(size > 0);
}

boolean
CRRenderGroup::CompareInstancedOp(RRenderOp* pLhs, RRenderOp* pRhs)
{
	//if(lhsNumVBs == rhsNumVBs)
	{
		//_LOOPi(lhsNumVBs)
		//{
		//	if( ((CRVertexBufferGroup*)pLhs->pVBGroup)->GetVertexBuffer(i) !=
		//		((CRVertexBufferGroup*)pRhs->pVBGroup)->GetVertexBuffer(i) )
		//		return FALSE;
		//}

		if( pLhs->pVBGroup == pRhs->pVBGroup &&
			pLhs->pIB == pRhs->pIB &&
			pLhs->pEffect == pRhs->pEffect &&
			pLhs->technique == pRhs->technique )
		{
			uint numDynamicParams = pLhs->pEffectTemplate->GetNumOfDynamicParams(pLhs->technique);
			_LOOPi(numDynamicParams)
			{
				if(pLhs->params[i] != pRhs->params[i])
					return FALSE;
			}

			return TRUE;
		}
	}

	return FALSE;
}

uint CRRenderGroup::GetNumOfDrawOps()
{
	return m_NumRenderOpsSorted;
}

void 
CRRenderGroup::Flush()
{
	// Collect instances
	//ProcessInstances( pRenderGroup );

	// Setup render targets
	uint size = (uint) m_RenderTargets.size();
	IRTextureRT** ppRT = NULL;
	if(size > 0)
	{
		ppRT = &(m_RenderTargets[0]);
	}

	DoBeginRenderTargets(ppRT, size);
	DoBeginDepthBuffer(m_pDepthBuffer);

	DoSetViewports(m_ViewportLeft, m_ViewportTop, m_ViewportWidth, m_ViewportHeight);

	// Clear the back buffers
	const gmtl::Vec4f* pClearColor = NULL;
	const float* pClearDepth = NULL;
	if(m_IsUseClearColor)
	{
		_DEBUG_ASSERT(m_RenderTargets.size() > 0);
		pClearColor = &(m_ClearColor);
	}

	if(m_IsUseClearDepth)
	{
		_DEBUG_ASSERT(m_pDepthBuffer);
		pClearDepth = &(m_ClearDepth);
	}

	m_pRenderer->ClearBackBuffer(pClearColor, pClearDepth);

	//// Reallocate buffers for render ops in reserve pool
	//uint numReservePoolRenderOps = m_ReservePool.GetNumOfRenderOps();
	//if(numReservePoolRenderOps > 0)
	//{
	//	// This means that the current static buffer overflowed
	//	_DEBUG_ASSERT((numReservePoolRenderOps + m_MaxBufferSize) == m_NumRenderOps);

	//	// Reallocate a new static buffer and copy data
	//	RRenderOp* pSrc = m_pRenderOps;
	//	uint newBufferSize = NextPowOf2(m_NumRenderOps);
	//	m_pRenderOps = (RRenderOp*) _MALLOC( newBufferSize * sizeof(RRenderOp) );

	//	if(pSrc)
	//	{
	//		memcpy(m_pRenderOps, pSrc, m_MaxBufferSize * sizeof(RRenderOp));
	//		_FREE(pSrc);
	//	}

	//	if(m_ppRenderOpSorted)
	//		_FREE(m_ppRenderOpSorted);

	//	m_ppRenderOpSorted = (RRenderOp**) _MALLOC( newBufferSize * sizeof(RRenderOp*) );
	//	_LOOPi(m_MaxBufferSize)
	//	{
	//		m_ppRenderOpSorted[i] = &m_pRenderOps[i];
	//	}

	//	if(m_ppRenderOpInstanced)
	//		_FREE(m_ppRenderOpInstanced);
	//	
	//	m_ppRenderOpInstanced = (RRenderOp**) _MALLOC( newBufferSize * sizeof(RRenderOp*) );

	//	memcpy(m_ppRenderOpSorted + m_MaxBufferSize, m_ReservePool.GetRenderOpBuffer(), numReservePoolRenderOps * sizeof(RRenderOp*));

	//	m_MaxBufferSize = newBufferSize;
	//}

	// Sort and render each list in the group
	//RenderOpList& renderOps = pRenderGroup->renderOps;
	if(m_SortFn)
	{
		std::qsort(m_ppRenderOpSorted, m_NumRenderOpsSorted, sizeof(RRenderOp*), m_SortFn);
		//std::sort(renderOps.begin(), renderOps.end(), pRenderGroup->sortFn);
	}

	_DEBUG_ASSERT(m_NumRenderOpsInstanced == 0);

	_LOOPi(m_NumRenderOpsSorted)
	{
		RRenderOp* pCurOp = m_ppRenderOpSorted[i];

		// If the render op is instanced
		if(pCurOp->numInstancedParams > 0)
		{
			// See if there are other instanced ops queued
			if(m_NumRenderOpsInstanced > 0)
			{
				// Check if they are not the same type
				RRenderOp* pLastInstancedOp = m_ppRenderOpInstanced[m_NumRenderOpsInstanced-1];
				if(pLastInstancedOp->opHash != pCurOp->opHash ||
					!CompareInstancedOp(pCurOp, pLastInstancedOp))
				{
					// Then render all the ones before
					RenderInstances(m_ppRenderOpInstanced, m_NumRenderOpsInstanced);
					m_NumRenderOpsInstanced = 0;
				}
			}

			// Add to the current instanced list
			m_ppRenderOpInstanced[m_NumRenderOpsInstanced] = pCurOp;
			m_NumRenderOpsInstanced++;
		}
		else
		{
			// Render all stored instances first
			if(m_NumRenderOpsInstanced > 0)
			{
				RenderInstances(m_ppRenderOpInstanced, m_NumRenderOpsInstanced);
				m_NumRenderOpsInstanced = 0;
			}

			// Draw the instances
			DrawRenderOp(pCurOp);
		}
	}

	// Finish any instances left in the list
	if(m_NumRenderOpsInstanced > 0)
	{
		RenderInstances(m_ppRenderOpInstanced, m_NumRenderOpsInstanced);
		m_NumRenderOpsInstanced = 0;
	}

	// Make sure to end the current effect
	if(m_pPrevOp)
	{
		CREffectTemplate* pPrevEffectTemplate = (CREffectTemplate*)(m_pPrevOp->pEffectTemplate);
		pPrevEffectTemplate->EndPass();
		pPrevEffectTemplate->EndTechnique();

		// Reset to null
		m_pPrevOp = NULL;
	}

	//if(numReservePoolRenderOps > 0)
	//{
	//	// Deinit to save RAM
	//	m_ReservePool.Deinit();
	//}

	// End setup render group
	DoEndRenderTargets();
	DoEndDepthBuffer();
}


void
CRRenderGroup::Reset()
{	
	m_NumRenderOps = 0;
	m_NumRenderOpsSorted = 0;
	m_NumRenderOpsInstanced = 0;
}

uint 
CRRenderGroup::GetNumOfRenderTargets()
{
	return (uint) m_RenderTargets.size();
}

IRTextureRT* 
CRRenderGroup::GetRenderTarget(uint rtIndex)
{
	return m_RenderTargets[rtIndex];
}

IRDepthBuffer*
CRRenderGroup::GetDepthBuffer()
{
	return m_pDepthBuffer;
}

//void
//CRRenderGroup::OnLostDevice()
//{
//	DoOnLostDevice();
//}
//
//void
//CRRenderGroup::OnResetDevice()
//{
//	DoOnResetDevice();
//}
//
//void
//CRRenderGroup::OnRestoreDevice()
//{
//	DoOnRestoreDevice();
//}


_NAMESPACE_END