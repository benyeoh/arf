//==============================================================================
//
//		CRPipeline.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Sunday, February 26, 2006
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

struct RenderGroupSortLess
{
	inline bool operator()(	const RRenderGroup* pLhs, const RRenderGroup* pRhs ) const
	{
		return ( pLhs->groupNum < pRhs->groupNum );
	}
};

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
//struct TranslucentSortLess
//{
//	inline bool operator()(	const RRenderOp* pLhs, const RRenderOp* pRhs ) const
//	{		
//		return ( pLhs->distance > pRhs->distance );
//	}
//};

//------------------------------------------------------------------------------
//	Function Defs
//------------------------------------------------------------------------------

uint
CRPipeline::CreateNewRenderGroup(uint groupNum, RenderOpSort sortFn, IRRenderCallback& callback)
{
	RRenderGroup* pGroup = m_RenderGroupPool.Get();
	
	pGroup->groupNum	= groupNum;
	pGroup->sortFn		= sortFn;
	pGroup->pCallback	= &callback;
	
	// Clear the list of RTs
	pGroup->rt.clear();
	pGroup->pDepthBuffer = NULL;
	
	pGroup->useClearColor = FALSE;
	pGroup->useClearDepth = FALSE;
	
	m_RenderGroups.push_back(pGroup);
	_INC_METRIC(m_pRenderer, numRenderGroups);

	uint index = (uint)(m_RenderGroups.size()-1);
	
	// Overwrite if the key is the same
	//_DEBUG_ASSERT(m_GroupIndices.find(groupNum) == m_GroupIndices.end());
	m_GroupIndices.Add(groupNum, index);
	
	return index;
}

uint 
CRPipeline::GetRenderGroupIndex(uint groupNum)
{
	uint* pIndex = m_GroupIndices.Get(groupNum);
	if(!pIndex)
		return (uint) R_UNKNOWN;
		
	return *pIndex;	
}

void 
CRPipeline::AddRenderTarget(uint groupIndex, IRTextureRT* pRT)
{
	_DEBUG_ASSERT(groupIndex < (uint)(m_RenderGroups.size()));
	m_RenderGroups[groupIndex]->rt.push_back(pRT);
}

void 
CRPipeline::SetDepthBuffer(uint groupIndex, IRDepthBuffer* pDepth)
{
	_DEBUG_ASSERT(groupIndex < (uint)(m_RenderGroups.size()));
	m_RenderGroups[groupIndex]->pDepthBuffer = pDepth;
}

void 
CRPipeline::AddToRenderGroup(uint groupIndex,
							 IRVertexBufferGroup* pVBGroup,
							 IRIndexBuffer* pIB,
							 IREffect* pEffect,
							 REffectParam* pParams,
							 uint techToUse,
							 void* pSortData,
							 eRPrimitiveType prim)
{
	_DEBUG_ASSERT(groupIndex < (uint)(m_RenderGroups.size()));
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
	
	// For each pass, add a render op
	uint numPasses = pCREffectTemplate->GetNumOfPasses(techUsed);
	_LOOPi(numPasses)
	{
		// TODO: Check for null here and flush the pipeline before resuming
		RRenderOp* pRenderOp = m_RenderOpPool.Get();	
		_DEBUG_ASSERT( pRenderOp );
		
		_LOOPj(numDynamicParams + numInstancedParams)
			pRenderOp->params[j] = pParams[j];
			
		// Init the render op
		pRenderOp->pVBGroup				= pVBGroup;
		pRenderOp->pIB					= pIB;
		pRenderOp->pEffect				= pEffect;
		pRenderOp->pEffectTemplate		= pCREffectTemplate;
		pRenderOp->technique			= (byte)(techUsed);
		pRenderOp->pSortingData			= pSortData;
		pRenderOp->pass					= i;
		pRenderOp->prim					= prim;
		
		pRenderOp->numInstancedParams = numInstancedParams;
		if(pRenderOp->numInstancedParams == 0)
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

			uint numDynamicParams = pRenderOp->pEffectTemplate->GetNumOfDynamicParams(pRenderOp->technique);
			_LOOPi(numDynamicParams)
			{
				switch(pRenderOp->params[i].type)
				{
					case EPT_FLOAT:
					case EPT_INT:	
					case EPT_BOOL:	
					case EPT_VOID_P:
					case EPT_TEX2D_P:
					case EPT_TEXRT_P:
					case EPT_TEXCUBE_P:		
					case EPT_MAT44_P:
					case EPT_MAT33_P:
					case EPT_MAT22_P:
					case EPT_VEC4_P:
					case EPT_VEC3_P:
					case EPT_VEC2_P:
					case EPT_STRING:
						_PARTIAL_HASH(pRenderOp->params[i].intVal);
						break;
				}
			}

			h ^= h >> 13;
			h *= m;
			h ^= h >> 15;
			
			pRenderOp->opHash = h;
			
			//AddToRenderGroupInstanced(*m_RenderGroups[groupIndex], &(pParams[numDynamicParams]), numInstancedParams, pRenderOp);
		}
		
		m_RenderGroups[groupIndex]->renderOps.push_back( pRenderOp );
	}
}

//void 
//CRPipeline::AddToRenderGroupInstanced(RRenderGroup& renderGroup,
//							REffectParam* pInstancedParams,
//							uint numInstancedParams,
//							RRenderOp* pOp)
//{
//	RInstancedKey keyToAdd;
//	keyToAdd.Set(groupIndex, pOp);
//	
//	RInstancedData* pInstancedData = renderGroup.instancedDatas.Get(keyToAdd);
//	if(!pInstancedData)
//	{
//		pInstancedData = renderGroup.instancedDatas.AddDeferred(keyToAdd);
//		pInstancedData->instancedParamStride = numInstancedParams;
//		renderGroup.instancedKeys.push_back(keyToAdd);
//	}
//	
//	_LOOPi(numInstancedParams)
//	{
//		pInstancedData->instancedParams.push_back(pInstancedParams[i]);
//	}
//}	

void
CRPipeline::DrawRenderOp( RRenderOp* pOp )
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
				// Same pass
				if(m_pPrevOp->pass == pOp->pass)
				{
					// Just apply the varying params
					pEffectTemplate->ApplyDynamicParams(pOp->params, m_pPrevOp->params);
				}
				// Different pass
				else
				{
					pPrevEffectTemplate->EndPass();
					pEffectTemplate->ApplyDynamicParams(pOp->params, m_pPrevOp->params);							
					pEffectTemplate->BeginPass(pOp->pass);
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
				
				pEffectTemplate->BeginPass(pOp->pass);
			}
		}
		else // Different effect
		{
			// Same template
			if(pPrevEffectTemplate == pEffectTemplate)
			{
				// Same technique
				if(pOp->technique == m_pPrevOp->technique)
				{
					// Same pass
					if(pOp->pass == m_pPrevOp->pass)
					{					
						pEffectTemplate->ApplyConstantParams(pEffect->GetParams());			
						pEffectTemplate->ApplyDynamicParams(pOp->params, m_pPrevOp->params);
					}
					// Different pass
					else
					{
						pPrevEffectTemplate->EndPass();
						pEffectTemplate->ApplyConstantParams(pEffect->GetParams());
						pEffectTemplate->ApplyDynamicParams(pOp->params, m_pPrevOp->params);
						pEffectTemplate->BeginPass(pOp->pass);					
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
					
					pEffectTemplate->BeginPass(pOp->pass);		
				}
			}
			else	// Different template
			{
				pPrevEffectTemplate->EndPass();
				pPrevEffectTemplate->EndTechnique();

				pEffectTemplate->BeginTechnique(pOp->technique);

				pEffectTemplate->ApplyConstantParams(pEffect->GetParams());
				pEffectTemplate->ApplyDynamicParams(pOp->params);

				pEffectTemplate->BeginPass(pOp->pass);
			}
		}
	}
	// First op
	else
	{
		pEffectTemplate->BeginTechnique(pOp->technique);
		pEffectTemplate->ApplyConstantParams(pEffect->GetParams());
		pEffectTemplate->ApplyDynamicParams(pOp->params);	// TODO:
		pEffectTemplate->BeginPass(pOp->pass);
	}

	m_pRenderer->RenderPrimitive(pOp);
	
	// Set the reference render op
	m_pPrevOp = pOp;
}

void 
CRPipeline::SetClearColor(uint groupIndex, const gmtl::Vec4f& color)
{
	m_RenderGroups[groupIndex]->clearColor = color;
	m_RenderGroups[groupIndex]->useClearColor = TRUE;
}

void 
CRPipeline::SetClearDepth(uint groupIndex, float val)
{
	m_RenderGroups[groupIndex]->clearDepth = val;
	m_RenderGroups[groupIndex]->useClearDepth = TRUE;
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
CRPipeline::RenderInstances(RRenderGroup* pGroup, RRenderOp** ppInstanceOps, uint size)
{
	// Gather all instances
	do
	{
		uint numInstances = DoProcessInstances(ppInstanceOps, size);
		RRenderOp* pToRender = ppInstanceOps[0];
		pToRender->numInstances = numInstances;
		
		// Draw the instances
		pGroup->pCallback->BeginRenderOp(pToRender);
		DrawRenderOp(pToRender);
		pGroup->pCallback->EndRenderOp(pToRender);
		
		size -= numInstances;
		ppInstanceOps = ppInstanceOps + numInstances;
	}while(size > 0);
}
	
boolean
CRPipeline::CompareInstancedOp(RRenderOp* pLhs, RRenderOp* pRhs)
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
			pLhs->technique == pRhs->technique &&
			pRhs->pass == pRhs->pass )
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

void 
CRPipeline::DrawRenderGroup(RRenderGroup* pGroup)
{
	_DEBUG_ASSERT(pGroup);
	
	RRenderGroup* pRenderGroup = pGroup;

	// Begin setup render group
	// This gives a chance for the application to perform some type of setup
	pRenderGroup->pCallback->BeginRenderGroup( pRenderGroup->groupNum );

	// Collect instances
	//ProcessInstances( pRenderGroup );
	
	// Setup render targets
	uint size = (uint) pRenderGroup->rt.size();
	IRTextureRT** ppRT = NULL;
	if(size > 0)
	{
		ppRT = &(pRenderGroup->rt[0]);
	}

	DoBeginRenderTargets(ppRT, size);
	DoBeginDepthBuffer(pRenderGroup->pDepthBuffer);

	// Clear the back buffers
	const gmtl::Vec4f* pClearColor = NULL;
	const float* pClearDepth = NULL;
	if(pRenderGroup->useClearColor)
		pClearColor = &(pRenderGroup->clearColor);
	if(pRenderGroup->useClearDepth)
		pClearDepth = &(pRenderGroup->clearDepth);
	
	m_pRenderer->ClearBackBuffer(pClearColor, pClearDepth);

	// Sort and render each list in the group
	RenderOpList& renderOps = pRenderGroup->renderOps;
	if(pRenderGroup->sortFn)
	{
		std::sort(renderOps.begin(), renderOps.end(), pRenderGroup->sortFn);
	}

	uint numRenderOps = (uint) renderOps.size();
	uint numInstancedOps = 0;
	
	_DEBUG_ASSERT(((uint)m_InstancedOps.size()) == 0);
	
	_LOOPi(numRenderOps)
	{
		RRenderOp* pCurOp = renderOps[i];
		
		// If the render op is instanced
		if(pCurOp->numInstancedParams > 0)
		{
			// See if there are other instanced ops queued
			if(numInstancedOps > 0)
			{
				// Check if they are not the same type
				RRenderOp* pLastInstancedOp = m_InstancedOps.back();
				if(pLastInstancedOp->opHash != pCurOp->opHash ||
					!CompareInstancedOp(pCurOp, pLastInstancedOp))
				{
					// Then render all the ones before
					RenderInstances(pRenderGroup, &(m_InstancedOps[0]), numInstancedOps);
					m_InstancedOps.clear();
				}
			}
			
			// Add to the current instanced list
			m_InstancedOps.push_back(pCurOp);
			numInstancedOps = (uint) m_InstancedOps.size();
		}
		else
		{
			// Render all stored instances first
			if(numInstancedOps > 0)
			{
				RenderInstances(pRenderGroup, &(m_InstancedOps[0]), numInstancedOps);
				m_InstancedOps.clear();
				numInstancedOps = (uint) m_InstancedOps.size();
			}
			
			pRenderGroup->pCallback->BeginRenderOp(pCurOp);
			DrawRenderOp(pCurOp);		
			pRenderGroup->pCallback->EndRenderOp(pCurOp);
		}
	}
	
	// Finish any instances left in the list
	if(numInstancedOps > 0)
	{
		RenderInstances(pRenderGroup, &(m_InstancedOps[0]), numInstancedOps);
		m_InstancedOps.clear();
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

	// End setup render group
	DoEndRenderTargets();
	DoEndDepthBuffer();

	// Notifies the application that we have finished rendering a group
	pRenderGroup->pCallback->EndRenderGroup( pRenderGroup->groupNum );
}


void
CRPipeline::RenderPipeline()
{	
	// Check that there is no previous op...
	_DEBUG_ASSERT( !m_pPrevOp );

	// Sort the groups in ascending order
	std::sort(m_RenderGroups.begin(), m_RenderGroups.end(), RenderGroupSortLess());
	_ITER_STLi(RenderGroupList, m_RenderGroups)
	{
		RRenderGroup* pRenderGroup = (*iitr);		
		DrawRenderGroup(pRenderGroup);
		
		RenderOpList& renderOps = pRenderGroup->renderOps;
		renderOps.clear();
	}
	
	m_RenderGroups.clear();
	//m_GroupIndices.clear();
		
	//ClearRenderOps();
	//ClearRenderGroup();
	m_RenderOpPool.Reset();
	m_RenderGroupPool.Reset();
}
	
void 
CRPipeline::RenderImmediate(uint groupIndex)
{
	// Check that there is no previous op...
	_DEBUG_ASSERT( !m_pPrevOp );
	
	RRenderGroup* pRenderGroup = m_RenderGroups[groupIndex];
	DrawRenderGroup(pRenderGroup);
	
	RenderOpList& renderOps = pRenderGroup->renderOps;
	
	_LOOPi(renderOps.size())
	{
		m_RenderOpPool.Dispose(renderOps[i]);
	}
	
	renderOps.clear();
	
	m_RenderGroupPool.Dispose(pRenderGroup);
	m_RenderGroups.erase(m_RenderGroups.begin() + groupIndex);
	m_GroupIndices.Remove(pRenderGroup->groupNum);
	//m_GroupIndices.erase(m_GroupIndices.find(pRenderGroup->groupNum));
}

	
uint 
CRPipeline::GetNumOfRenderGroups()
{
	return (uint) m_RenderGroups.size();
}

uint 
CRPipeline::GetRenderGroupID(uint groupIndex)
{
	return m_RenderGroups[groupIndex]->groupNum;
}

uint 
CRPipeline::GetNumOfRenderTargets(uint groupIndex)
{
	return (uint) m_RenderGroups[groupIndex]->rt.size();
}

IRTextureRT* 
CRPipeline::GetRenderTarget(uint groupIndex, uint rtIndex)
{
	return m_RenderGroups[groupIndex]->rt[rtIndex];
}

IRDepthBuffer*
CRPipeline::GetDepthBuffer(uint groupIndex)
{
	return m_RenderGroups[groupIndex]->pDepthBuffer;
}
			   
void
CRPipeline::OnLostDevice()
{
	DoOnLostDevice();
}

void
CRPipeline::OnResetDevice()
{
	DoOnResetDevice();
}

void
CRPipeline::OnRestoreDevice()
{
	DoOnRestoreDevice();
}


_NAMESPACE_END