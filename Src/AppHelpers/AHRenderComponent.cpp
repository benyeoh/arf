//==============================================================================
//
//		AHRenderComponent.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/21/2009
//
//==============================================================================

#include "AppHelpers.h"

_NAMESPACE_BEGIN

#if defined(_IS_USE_APPHELPER_LIB_BASIC)

void AHRenderComponent::Render(uint matType, uint libIndex, IBFXBaseFX* pBaseFX, BFXRenderContainer& renderContainer, BFXParamContainer& paramContainer, IBFXMaterialCreateCallback* pCreateCallback)
{
	_LOOPi(m_pRenderMeshList->meshes.size())
	{
		if(m_pRenderMeshList->meshes[i].pMatGroup->GetTemplate()->GetMaterialByType(matType) != NULL)
		{
			pBaseFX->Render(m_pRenderMeshList->meshes[i], 1.0f, matType, libIndex, paramContainer, renderContainer, pCreateCallback);
		}		
	}
}
	
//void
//AHRenderComponent::RenderShadowCaster(IBFXBaseFX* pBaseFX)
//{
//	uint numRenderParams = (uint) m_RenderParamList.size();
//	_LOOPi(numRenderParams)
//	{
//		m_RenderParamList[i]->SetupRenderParams(pBaseFX);
//	}
//
//	uint numObjects = (uint) m_RenderMeshList.meshes.size();
//	
//	_LOOPi(numObjects)
//	{
//		m_RenderMeshList.meshes[i].pMatGroup->SetCurrentMatType(SHFX_MAT_CAST_DIR_LIGHT_SHADOW);
//		pBaseFX->Render((m_RenderMeshList.meshes[i]), 1.0f);
//	}
//}

void AHRenderComponent::Update(uint phaseNum)
{
	// This has to be single-threaded
	if(m_pSceneContainer)
	{
        if(m_SceneHandle.IsValid())
            m_pSceneContainer->Remove(m_SceneHandle);
		
		m_WorldOOBox = m_WorldOOBoxGather;
		m_WorldAABox = m_WorldAABoxGather;

		m_SceneHandle = m_pSceneContainer->Insert(&m_WorldAABox, &m_WorldOOBox, SDB_FILTER_RENDER_COMP, (AHComponent*) this);
	}

	m_IsAddedToUpdate = FALSE;
}

void 
AHRenderComponent::UpdateWorldBounds(const gmtl::MatrixA44f& world)
{
	TransformAABoxToOOBox(&world, &m_LocalAABox, &m_WorldOOBoxGather);
	OOBoxToAABox(&m_WorldOOBoxGather, &m_WorldAABoxGather);

	AddToTempUpdate();
}

void
AHRenderComponent::UpdateWorldBoundsFromSpatial(AHSpatialComponent* pSrc)
{
	AHSpatialComponent* pSpatial = pSrc;
	UpdateWorldBounds(pSpatial->GetWorldMatrix());
}

void AHRenderComponent::OnComponentEvent(int eventType, AHComponent* pSrc)
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

void AHRenderComponent::SetMeshList(BFXUMeshList* pMeshList)
{
	m_pRenderMeshList = pMeshList;
	m_ParamSemanticMap.Initialize(64, 0.7f);
	memset(m_MatTypeMap, 0, sizeof(m_MatTypeMap));

	_LOOPi(m_pRenderMeshList->meshes.size())
	{
		IBFXMaterialGroup* pMatGroup = m_pRenderMeshList->meshes[i].pMatGroup;
		_LOOPj(pMatGroup->GetNumOfParams())
		{
			uint semantic = pMatGroup->GetSemantic(j);
			if(m_ParamSemanticMap.Get(semantic) == NULL)
				m_ParamSemanticMap.Add(semantic, TRUE);
		}

		_LOOPj(pMatGroup->GetTemplate()->GetNumOfMaterials())
		{
			uint matType = pMatGroup->GetTemplate()->GetMaterialTypeByIndex(j);
			m_MatTypeMap[matType] = TRUE;
		}
	}
}

void AHRenderComponent::SetLocalAABox(const AABox& box) 
{ 
	m_LocalAABox = box;
	//m_WorldAABox = box;
	
	if(GetEntity() && GetEntity()->GetComponent(COMP_SPATIAL))
	{
		AHSpatialComponent* pSpatial = (AHSpatialComponent*) GetEntity()->GetComponent(COMP_SPATIAL);
		UpdateWorldBoundsFromSpatial(pSpatial);
	}
	else
	{
		gmtl::MatrixA44f world;
		gmtl::identity(_CAST_MAT44(world));
		UpdateWorldBounds(world);
	}
}

boolean AHRenderComponent::IsParamSemanticUsed(uint semantic)
{
	return m_ParamSemanticMap.Get(semantic) != NULL;
}

boolean AHRenderComponent::IsMatTypeUsed(uint matType)
{
	return m_MatTypeMap[matType];
}

void AHRenderComponent::AddToTempUpdate()
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

void AHRenderComponent::SetScene(ISDBSceneContainer* pContainer)
{ 
	if(m_pSceneContainer && m_SceneHandle.IsValid())
		m_pSceneContainer->Remove(m_SceneHandle);

    m_SceneHandle.Invalidate();

	m_pSceneContainer = pContainer;

	if(pContainer)
		AddToTempUpdate();
}

#endif

_NAMESPACE_END