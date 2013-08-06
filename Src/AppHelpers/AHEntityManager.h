//================================================================================
//
//		AHEntityManager.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		10/30/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

class AHEntityManager : public CRefCounted<IRefCounted>
{
	typedef std::vector<AHComponent*> ComponentList;

	struct PhaseRun : public IPRunnable
	{
		AHEntity** ppStart;
		uint numEntities;
		uint phaseNum;

		int Run();
	};

private:
	std::vector<AHEntityPtr>	m_pEntityList;
	std::vector<AHEntity*>		m_pEntityUpdateLists[MAX_NUM_UPDATE_PHASES];

	IPThreadPool*	m_pThreadPool;
	uint m_NumContexts;

	ComponentList**	m_pCompTempUpdateContext;
	PhaseRun* m_pPhaseRunContext;

public:
	AHEntityManager(IPThreadPool* pThreadPool)
		: m_pThreadPool(NULL)
		, m_pCompTempUpdateContext(NULL)
		, m_pPhaseRunContext(NULL)
		, m_NumContexts(0)
	{
		SetThreadPool(pThreadPool);
	}

	~AHEntityManager()
	{
		SetThreadPool(NULL);
	}

private:
	void SetThreadPool(IPThreadPool* pThreadPool);
	void UpdateThreaded(int updatePhase); 
	void UpdateSingleThreaded(int updatePhase); 

public:
	void AddEntity(AHEntity* pEntity);
	void AddEntityToUpdateList(AHEntity* pEntity, int updatePhase);

	void RemoveEntity(AHEntity* pEntity);
	void RemoveEntityFromUpdateList(AHEntity* pEntity, int updatePhase);

	void AddComponentToTempUpdateList(AHComponent* pComp);

	void Update(int phaseGroup);
	void UpdateComponentsTemp();
};

_DECLARE_SMARTPTR(AHEntityManager)

_NAMESPACE_END