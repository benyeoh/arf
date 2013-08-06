//==============================================================================
//
//		AHEntity.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/23/2009
//
//==============================================================================

#include "AppHelpers.h"

_NAMESPACE_BEGIN

void	
AHEntity::AddComponent(AHComponent* pComponent)
{
	AddComponentAtIndex(pComponent, m_ComponentList.size());

	//// If the same type already exists
	//uint curType = pComponent->GetType();
	//if(m_pComponents[curType])
	//{
	//	// Remove old component of the same type
	//	uint numParams = (uint) m_ComponentList.size();
	//	_LOOPi(numParams)
	//	{
	//		if(m_ComponentList[i]->GetType() == curType)
	//		{
	//			m_ComponentList.erase(m_ComponentList.begin() + i);
	//			break;
	//		}
	//	}
	//}
	//
	//m_pComponents[curType] = pComponent;
	//m_ComponentList.push_back(pComponent);
	//pComponent->SetEntity(this);
}

void AHEntity::AddComponentAtIndex(AHComponent* pComponent, int index)
{
	AHComponentPtr pTemp = pComponent;

	RemoveComponent(pComponent);

	if(index > (int) m_ComponentList.size())
	{
		index = m_ComponentList.size();
	}

	m_ComponentList.insert(m_ComponentList.begin() + index, pComponent);
	
	uint curType = pComponent->GetType();
	_DEBUG_ASSERT(m_pComponents[curType] == NULL);
	m_pComponents[curType] = pComponent;
	pComponent->SetEntity(this);

	int updatePhase = pComponent->GetComponentUpdatePhaseFlags();
	RefreshUpdateList(updatePhase);

	_LOOPi(m_ComponentList.size())
	{
		m_ComponentList[i]->OnComponentEvent(EVENT_COMP_ADDED, pComponent);
	}
}

void AHEntity::RefreshUpdateList(uint updatePhaseFlag)
{
	_LOOPi(MAX_NUM_UPDATE_PHASES)
	{
		if(updatePhaseFlag & (1 << i))
		{
			m_pPerFrameUpdate[i].clear();

			// Remove from update phase for this entity
			if(m_pEntityMgr)
				m_pEntityMgr->RemoveEntityFromUpdateList(this, i);

			_LOOPj(m_ComponentList.size())
			{
				if(m_ComponentList[j]->GetComponentUpdatePhaseFlags() & (1 << i))
				{
					m_pPerFrameUpdate[i].push_back(m_ComponentList[j]);
				}
			}

			// Add update phase for this entity
			if(m_pEntityMgr)
				m_pEntityMgr->AddEntityToUpdateList(this, i);
		}
	}
}

void AHEntity::RemoveComponent(AHComponent* pComponent)
{
	uint curType = pComponent->GetType();

	// Remove if it is the same object
	_LOOPi(m_ComponentList.size())
	{
		if(m_ComponentList[i] == pComponent)
		{
			// Remove from update phase list
			uint updatePhaseFlag = pComponent->GetComponentUpdatePhaseFlags();

			m_ComponentList.erase(m_ComponentList.begin() + i);
			if(m_pComponents[curType] == pComponent)
				m_pComponents[curType] = NULL;
				
			// Refresh and re-order update lists
			RefreshUpdateList(updatePhaseFlag);

			_LOOPj(m_ComponentList.size())
			{
				m_ComponentList[j]->OnComponentEvent(EVENT_COMP_REMOVED, pComponent);
			}

			break;
		}
	}
}

uint	
AHEntity::GetNumOfComponents()
{
	return (uint) m_ComponentList.size();
}
	
AHComponent*	
AHEntity::GetComponent(uint compType)
{
	_DEBUG_ASSERT(compType < MAX_COMPONENT_TYPES);
	return m_pComponents[compType];
}

AHComponent*	
AHEntity::GetComponentFromIndex(uint index)
{
	_DEBUG_ASSERT(index < GetNumOfComponents());
	return m_ComponentList[index];
}

void AHEntity::Update(int phaseNum)
{
	_LOOPi(m_pPerFrameUpdate[phaseNum].size())
	{
		m_pPerFrameUpdate[phaseNum][i]->Update(phaseNum);
	}
}

uint AHEntity::GetNumOfComponentsInUpdatePhase(int updatePhase)
{
	return (uint) m_pPerFrameUpdate[updatePhase].size();
}

AHComponent** AHEntity::GetComponentsInUpdatePhase(int updatePhase)
{
	return &(m_pPerFrameUpdate[updatePhase][0]);
}


_NAMESPACE_END