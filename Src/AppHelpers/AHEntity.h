//==============================================================================
//
//		AHEntity.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/20/2009
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class AHEntity : public CRefCounted<IRefCounted>
{
private: 
	std::vector<AHComponentPtr>	m_ComponentList;
	AHComponent*				m_pComponents[MAX_COMPONENT_TYPES];
	
	std::vector<AHComponent*>	m_pPerFrameUpdate[MAX_NUM_UPDATE_PHASES];
	AHEntityManager*			m_pEntityMgr;

public: 
	AHEntity()
		: m_pEntityMgr(NULL)
	{
		memset(m_pComponents, 0, sizeof(AHComponent*) * MAX_COMPONENT_TYPES);
		//m_Components.Initialize(64, 0.667f);
	}

	virtual ~AHEntity()
	{
		// Remove from entity list/update phases
	}

private:
	void RefreshUpdateList(uint updatePhaseFlag);

public:
	inline void	SetEntityManager(AHEntityManager* pEntityMgr)	{ m_pEntityMgr = pEntityMgr; }
	inline AHEntityManager* GetEntityManager()						{ return m_pEntityMgr; }

	uint	GetNumOfComponents();
	void	AddComponent(AHComponent* pComponent);
	void	AddComponentAtIndex(AHComponent* pComponent, int index);
	void	RemoveComponent(AHComponent* pComponent);
	
	//ComponentList*	GetComponentList(uint compType);
	AHComponent*	GetComponent(uint compType);
	AHComponent*	GetComponentFromIndex(uint index); 

	uint	GetNumOfComponentsInUpdatePhase(int updatePhase);
	AHComponent** GetComponentsInUpdatePhase(int updatePhase);

	void Update(int phaseNum);
};

_DECLARE_SMARTPTR(AHEntity)

_NAMESPACE_END
