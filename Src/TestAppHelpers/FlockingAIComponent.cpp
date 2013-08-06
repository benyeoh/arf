//================================================================================
//
//		FlockingAIComponent.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/9/2012
//
//================================================================================

#include "stdafx.h"

void FlockingAIComponent::AddBehavior(FlockingBehavior* pBehavior)
{
	m_FlockingBehaviors[m_NumBehaviors++] = pBehavior;
}

void FlockingAIComponent::UpdateGather(float dt)
{
	AHSpatialComponent* pSpatial = _CAST_COMP_SPATIAL(GetEntity());

	AHComponent* pComp[512];
	uint numComp = 0;
	if(m_pSceneContainer)
	{
		Sphere querySphere;
		pSpatial->GetWorldPos( _CAST_VECA3(querySphere.center) );
		querySphere.center[3] = m_SearchRadius;

		gmtl::VecA3f span;
		span.set(querySphere.center[3], querySphere.center[3], querySphere.center[3]);

		AABox queryBox;
		VecVecSub(&queryBox.min, &(_CAST_VECA3(querySphere.center)), &span);
		VecVecAdd(&queryBox.max, &(_CAST_VECA3(querySphere.center)), &span);
		numComp = m_pSceneContainer->QuerySphere((void**) pComp, 512, queryBox, querySphere, SDB_FILTER_FLOCKING_COMP);
	}

	gmtl::VecA3f tempDir(0.0f, 0.0f, 0.0f);
	_LOOPi(m_NumBehaviors)
	{				
		gmtl::VecA3f newDir(0.0f, 0.0f, 0.0f);
		m_FlockingBehaviors[i]->Update(dt, this, pComp, numComp, newDir);
		newDir[0] = m_FlockingBehaviors[i]->GetStrength() * newDir[0] * m_Axes[0];
		newDir[1] = m_FlockingBehaviors[i]->GetStrength() * newDir[1] * m_Axes[1];
		newDir[2] = m_FlockingBehaviors[i]->GetStrength() * newDir[2] * m_Axes[2];

		VecVecAdd(&tempDir, &tempDir, &newDir);
		//m_DirGather += newDir;	
	}

	if(gmtl::Math::abs(tempDir[0]) > gmtl::GMTL_EPSILON ||
		gmtl::Math::abs(tempDir[1]) > gmtl::GMTL_EPSILON ||
		gmtl::Math::abs(tempDir[2]) > gmtl::GMTL_EPSILON)
		NormalizeVec(tempDir);
	else
		tempDir = m_Dir;

	// Steer to the new direction
	float steering = m_SteeringSpeed * dt;
	if(steering > 1.0f)
		steering = 1.0f;

	VecVecSlerp(&tempDir, &m_Dir, &tempDir, steering);
	NormalizeVec(tempDir);

	m_DirGather = tempDir;
}

void FlockingAIComponent::UpdateStore(float dt)
{
	m_Dir = m_DirGather;

	AHSpatialComponent* pSpatial = _CAST_COMP_SPATIAL(GetEntity());

	const gmtl::MatrixA44f& worldMat = pSpatial->GetWorldMatrix();
	gmtl::VecA3f pos(worldMat[0][3], worldMat[1][3], worldMat[2][3]);
	float speed = m_Speed * dt;

	gmtl::VecA3f displacement;
	VecScalarMult(&displacement, &m_Dir, speed);
	VecVecAdd(&pos, &pos, &displacement);

	gmtl::VecA3f up(0.0f, 1.0f, 0.0f);
	if(gmtl::Math::abs(m_Dir[1]) > 0.989f)
		up.set(1.0f, 0.0f, 0.0f);

	pSpatial->SetWorldPosAndFacing(pos, m_Dir, up);
}

void FlockingAIComponent::UpdateTemp()
{
	// This has to be single-threaded
	if(m_pSceneContainer)
	{
        if(m_SceneHandle.IsValid())
            m_pSceneContainer->Remove(m_SceneHandle);
		
		m_WorldAABox = m_WorldAABoxGather;
		m_WorldOOBox = m_WorldOOBoxGather;

		m_SceneHandle = m_pSceneContainer->Insert(&m_WorldAABox, &m_WorldOOBox, SDB_FILTER_FLOCKING_COMP, (AHComponent*) this);
	}

	m_IsAddedToUpdate = FALSE;
}

void FlockingAIComponent::Update(uint phaseNum)
{
	switch(phaseNum)
	{
	case UPDATE_PHASE_GATHER_1: UpdateGather(*m_pTimeDT); break;
	case UPDATE_PHASE_STORE_1: UpdateStore(*m_pTimeDT); break;
	case UPDATE_PHASE_TEMP_COMP: UpdateTemp(); break;
	}
}

void FlockingAIComponent::AddToTempUpdate()
{
	if(GetEntity() && GetEntity()->GetEntityManager())
	{
		// Add to list for later update of scene db
		if(!m_IsAddedToUpdate)
		{
			// Check move
			if(m_SceneHandle.IsValid())
			{
				if(m_pSceneContainer->UpdateMove(m_SceneHandle, &m_WorldAABoxGather))
				{
					GetEntity()->GetEntityManager()->AddComponentToTempUpdateList(this);
					m_IsAddedToUpdate = TRUE;
				}
				else
				{
					// This is probably in store phase, so it's safe to update
					m_WorldOOBox = m_WorldOOBoxGather;
					m_WorldAABox = m_WorldAABoxGather;
				}
			}
			else
			{
				GetEntity()->GetEntityManager()->AddComponentToTempUpdateList(this);
				m_IsAddedToUpdate = TRUE;
			}
		}
	}
	else
	{
		// No entity manager so manually update
		Update(UPDATE_PHASE_TEMP_COMP);
	}
}

void FlockingAIComponent::SetScene(ISDBSceneContainer* pScene)
{
	if(m_pSceneContainer && m_SceneHandle.IsValid())
        m_pSceneContainer->Remove(m_SceneHandle);

    m_SceneHandle.Invalidate();

	m_pSceneContainer = pScene;

	if(pScene)
		AddToTempUpdate();
}


void FlockingAIComponent::UpdateWorldBounds(const gmtl::MatrixA44f& world)
{
	TransformAABoxToOOBox(&world, &m_LocalAABox, &m_WorldOOBoxGather);
	OOBoxToAABox(&m_WorldOOBoxGather, &m_WorldAABoxGather);

	AddToTempUpdate();
}

void FlockingAIComponent::UpdateWorldBoundsFromSpatial(AHSpatialComponent* pSpatial)
{
	AHSpatialComponent* pSpatialComp = pSpatial;
	_DEBUG_ASSERT(pSpatialComp);

	const gmtl::MatrixA44f& world = pSpatialComp->GetWorldMatrix();
	UpdateWorldBounds(world);
}

void FlockingAIComponent::OnComponentEvent(int eventType, AHComponent* pSrc)
{
	switch(eventType)
	{
	case EVENT_COMP_SPATIAL_CHANGED:
		{
			_DEBUG_ASSERT(pSrc->GetType() == COMP_SPATIAL);
			UpdateWorldBoundsFromSpatial((AHSpatialComponent*) pSrc);
			break;
		}

	case EVENT_COMP_ADDED:
		{
			if(this == pSrc)
			{
				// Initialize
				AHSpatialComponent* pSpatial = (AHSpatialComponent*) GetEntity()->GetComponent(COMP_SPATIAL);
				if(pSpatial)
				{
					pSpatial->AddEventListener(this);
					UpdateWorldBoundsFromSpatial(pSpatial);
				}
			}
			else if(pSrc->GetType() == COMP_SPATIAL)
			{
				AHSpatialComponent* pSpatial = (AHSpatialComponent*) pSrc;
				pSpatial->AddEventListener(this);
				UpdateWorldBoundsFromSpatial(pSpatial);
			}

			break;
		}
	}
}
