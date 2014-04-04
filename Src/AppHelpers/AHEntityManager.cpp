//================================================================================
//
//		AHEntityManager.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		10/30/2012
//
//================================================================================

#include "AppHelpers.h"

_NAMESPACE_BEGIN

void AHEntityManager::SetThreadPool(IPThreadPool* pThreadPool)
{
	m_pThreadPool = pThreadPool;
	if(m_pCompTempUpdateContext)
	{
		_DEBUG_ASSERT(m_NumContexts > 0);
		_LOOPi(m_NumContexts)
		{
			_ALIGNED_DELETE(m_pCompTempUpdateContext[i], ComponentList);
		}

		_FREE(m_pCompTempUpdateContext);
	}

	if(m_pPhaseRunContext)
		_DELETE_ARRAY(m_pPhaseRunContext);

	m_pCompTempUpdateContext = NULL;
	m_pPhaseRunContext = NULL;

	if(pThreadPool)
	{
		m_NumContexts = pThreadPool->GetNumThreads() + 1;
		m_pCompTempUpdateContext = (ComponentList**) _MALLOC(sizeof(ComponentList*) * m_NumContexts);

		_LOOPi(m_NumContexts)
		{
			// Allocate aligned memory to avoid false sharing
			m_pCompTempUpdateContext[i] = _ALIGNED_NEW(_CACHE_LINE_SIZE, ComponentList) ComponentList;
		}

		m_pPhaseRunContext = _NEW PhaseRun[m_NumContexts];
	}
}

void AHEntityManager::AddEntity(AHEntity* pEntity)
{
	_DEBUG_ASSERT(pEntity->GetEntityManager() == NULL);

	m_pEntityList.push_back(pEntity);
	pEntity->SetEntityManager(this);

	_LOOPi(MAX_NUM_UPDATE_PHASES)
	{
		if(pEntity->GetNumOfComponentsInUpdatePhase(i) > 0)
			AddEntityToUpdateList(pEntity, i);
	}
}

void AHEntityManager::AddEntityToUpdateList(AHEntity* pEntity, int updatePhase)
{
	_DEBUG_ASSERT(updatePhase < MAX_NUM_UPDATE_PHASES);
	m_pEntityUpdateLists[updatePhase].push_back(pEntity);
}

void AHEntityManager::RemoveEntity(AHEntity* pEntity)
{
	_DEBUG_ASSERT(pEntity->GetEntityManager() == this);

	_LOOPi(MAX_NUM_UPDATE_PHASES)
	{
		if(pEntity->GetNumOfComponentsInUpdatePhase(i) > 0)
			RemoveEntityFromUpdateList(pEntity, i);
	}

	_LOOPi(m_pEntityList.size())
	{
		if(m_pEntityList[i] == pEntity)
		{
			m_pEntityList.erase(m_pEntityList.begin() + i);
			break;
		}
	}
}

void AHEntityManager::RemoveEntityFromUpdateList(AHEntity* pEntity, int updatePhase)
{
	_LOOPi(m_pEntityUpdateLists[updatePhase].size())
	{
		if(m_pEntityUpdateLists[updatePhase][i] == pEntity)
		{
			m_pEntityUpdateLists[updatePhase].erase(m_pEntityUpdateLists[updatePhase].begin() + i);
			break;
		}
	}
}

void AHEntityManager::UpdateThreaded(int updatePhase)
{
	const static uint MIN_NUM_OPS = 2;

	uint numEntities = m_pEntityUpdateLists[updatePhase].size();
	if(numEntities > 0)
	{
		AHEntity** ppStart = &(m_pEntityUpdateLists[updatePhase][0]);

		uint numEntitiesPerContext = numEntities / m_NumContexts;
		if(numEntitiesPerContext < MIN_NUM_OPS)
			numEntitiesPerContext = MIN_NUM_OPS;

		int numToDispatch = (numEntities / numEntitiesPerContext) - 1;
		if(numToDispatch < 0)
			numToDispatch = 0;
		
		_DEBUG_ASSERT(numToDispatch < (int) m_NumContexts);

		_LOOPi(numToDispatch)
		{
			m_pPhaseRunContext[i].numEntities = numEntitiesPerContext;
			m_pPhaseRunContext[i].phaseNum	= updatePhase;
			m_pPhaseRunContext[i].ppStart	= ppStart + (i * numEntitiesPerContext);

			m_pThreadPool->QueueJobUnsafe(m_pPhaseRunContext[i]);
		}

		uint numEntitiesStart = numEntitiesPerContext * numToDispatch;
		uint numEntitiesLeft = numEntities - numEntitiesStart;

		m_pPhaseRunContext[numToDispatch].numEntities = numEntitiesLeft;
		m_pPhaseRunContext[numToDispatch].phaseNum	= updatePhase;
		m_pPhaseRunContext[numToDispatch].ppStart	= ppStart + numEntitiesStart;

		m_pPhaseRunContext[numToDispatch].Run();
		//m_pThreadPool->QueueJobUnsafe(m_pPhaseRunContext[numToDispatch]);

		m_pThreadPool->ProcessJobs();
	}
}

void AHEntityManager::UpdateSingleThreaded(int updatePhase)
{
	uint numEntities = m_pEntityUpdateLists[updatePhase].size();
	_LOOPi(numEntities)
	{
		m_pEntityUpdateLists[updatePhase][i]->Update(updatePhase);
	}
}

void AHEntityManager::Update(int phaseGroup)
{
	// Gather
	int phaseGather = phaseGroup;
	_DEBUG_ASSERT(phaseGather < MAX_NUM_UPDATE_PHASES);
	UpdateThreaded(phaseGather);

	// Next phase: Store
	int phaseStore = phaseGather + 1;
	_DEBUG_ASSERT(phaseStore < MAX_NUM_UPDATE_PHASES);
	UpdateThreaded(phaseStore);

	//// Post. Always single threaded for now
	//int phasePost = phaseStore + 1;
	//_DEBUG_ASSERT(phasePost < MAX_NUM_UPDATE_PHASES);
	//UpdateSingleThreaded(phasePost);

	UpdateComponentsTemp();
}

void AHEntityManager::UpdateComponentsTemp()
{
	// Update temp list. Always single threaded
	_LOOPi(m_NumContexts)
	{
		ComponentList& compList = *(m_pCompTempUpdateContext[i]);

		_LOOPj(compList.size())
		{
			compList[j]->Update(UPDATE_PHASE_TEMP_COMP);
		}

		compList.clear();
	}
}

void AHEntityManager::AddComponentToTempUpdateList(AHComponent* pComp)
{
	uint context = m_pThreadPool->GetCurrentThreadIndex();
	_DEBUG_ASSERT(context < m_NumContexts);

	ComponentList& compList = *(m_pCompTempUpdateContext[context]);
	compList.push_back(pComp);	
}

int AHEntityManager::PhaseRun::Run()
{
	_LOOPi(numEntities)
	{
		ppStart[i]->Update(phaseNum);
	}

	return 0;
}

_NAMESPACE_END