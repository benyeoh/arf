//================================================================================
//
//		AHDirLightComponent.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/6/2012
//
//================================================================================

#include "AppHelpers.h"

_NAMESPACE_BEGIN

#if defined(_IS_USE_APPHELPER_LIB_BASIC)

AHDirLightComponent::AHDirLightComponent()
	: AHComponent(COMP_DIRLIGHT)
	, m_pSceneContainer(NULL)
	, m_IsAddedToUpdate(FALSE)
{
	m_BFXDirLight.dir		= gmtl::Vec3f(0.0f, -1.0f, 0.0f);
	m_BFXDirLight.invSize	= 1.0f;
	m_BFXDirLight.color		= gmtl::Vec4f(1.0f, 1.0f, 1.0f, 1.0f);

	m_DEFXDirLight.dir		= m_BFXDirLight.dir;
	m_DEFXDirLight.color	= m_BFXDirLight.color;

	gmtl::identity(_CAST_MAT44(m_RenderWorldMat));

	m_IsShadowMapShared = FALSE;

	m_CascadeBoundsMap.Initialize(4, 0.67f);

	m_PosOffset = 100.0f;

    m_SceneHandle.Invalidate();
}

AHDirLightComponent::~AHDirLightComponent()
{
	SetScene(NULL);
}

void AHDirLightComponent::ClearCascadeBounds()
{
	if(m_CascadeBoundsMap.GetNumOfElements() > 0)
	{
		DenseChainedHashMap<size_t, SHFXCascadedFrustumBounds*, MurmurHashSizeTObj>::Iterator itr = m_CascadeBoundsMap.BeginIter();
		while(itr.IsValid())
		{
			SHFXCascadedFrustumBounds** ppVal = itr.GetValue();
			_DEBUG_ASSERT(ppVal != NULL);
			_DEBUG_ASSERT(*ppVal != NULL);

			_ALIGNED_DELETE(*ppVal, SHFXCascadedFrustumBounds);
			itr.Next();
		}

		m_CascadeBoundsMap.Reset();
	}
}

void AHDirLightComponent::SetScene(ISDBSceneContainer* pContainer)
{
	if(m_pSceneContainer && m_SceneHandle.IsValid())
        m_pSceneContainer->Remove(m_SceneHandle);
 
    m_SceneHandle.Invalidate();

	m_pSceneContainer = pContainer;

	if(pContainer)
		AddToTempUpdate();
	else
	{
		ClearCascadeBounds();
	}
}

void AHDirLightComponent::SetColor(const gmtl::Vec3f& color)
{
	m_BFXDirLight.color.set(color[0], color[1], color[2], 1.0f);
	m_DEFXDirLight.color = m_BFXDirLight.color;
}

void AHDirLightComponent::SetLocalAABox(const AABox& bounds)
{
	m_LocalAABox = bounds;

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

void AHDirLightComponent::SetShadowDirLight(ISHFXCascadedDirLight* pShadowDL, float cascadeExp, float posOffset, boolean isShadowMapShared)
{
	m_pShadowDL = pShadowDL;
	m_IsShadowMapShared = isShadowMapShared;
	m_CascadeSplitExp = cascadeExp;
	m_PosOffset = posOffset;

	ClearCascadeBounds();
}

void AHDirLightComponent::UpdateCascadeBounds(const gmtl::MatrixA44f& proj, size_t boundsID)
{
	SHFXCascadedFrustumBounds** ppBounds = m_CascadeBoundsMap.Get(boundsID);
	if(ppBounds == NULL)
	{
		SHFXCascadedFrustumBounds* pBounds = _ALIGNED_NEW(16, SHFXCascadedFrustumBounds) SHFXCascadedFrustumBounds;
		m_CascadeBoundsMap.Add(boundsID, pBounds);
		ppBounds = m_CascadeBoundsMap.Get(boundsID);
		_DEBUG_ASSERT(ppBounds);
	}

	m_pShadowDL->UpdateCascadedFrustumBounds(**ppBounds, m_CascadeSplitExp, _CAST_MAT44(proj));
}

void AHDirLightComponent::UpdateCascadeShadow(const gmtl::VecA3f& camPos, const gmtl::MatrixA44f& view, gmtl::MatrixA44f& proj, size_t boundsID)
{	
	SHFXCascadedFrustumBounds** ppBounds = m_CascadeBoundsMap.Get(boundsID);
	if(ppBounds == NULL)
	{
		UpdateCascadeBounds(proj, boundsID);
		ppBounds = m_CascadeBoundsMap.Get(boundsID);
	}

	UpdateCascadeShadow(camPos, view, **ppBounds);
}

void AHDirLightComponent::UpdateCascadeShadow(const gmtl::VecA3f& camPos, const gmtl::MatrixA44f&view, SHFXCascadedFrustumBounds& bounds)
{
	gmtl::VecA3f viewOffset;
	viewOffset[0] = m_BFXDirLight.dir[0];
	viewOffset[1] = m_BFXDirLight.dir[1];
	viewOffset[2] = m_BFXDirLight.dir[2];

	VecScalarMult(&viewOffset, &viewOffset, -m_PosOffset);
	VecVecAdd(&viewOffset, &viewOffset, &camPos);

	m_pShadowDL->Update(bounds, m_BFXDirLight.dir, _CAST_VEC3(viewOffset), _CAST_MAT44(view));
}

void AHDirLightComponent::AddToTempUpdate()
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
        // Check move
        if(m_SceneHandle.IsValid())
        {
            if(m_pSceneContainer->UpdateMove(m_SceneHandle, &m_WorldAABoxGather))
            {
                // No entity manager so manually update
                Update(UPDATE_PHASE_TEMP_COMP);
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
            // No entity manager so manually update
            Update(UPDATE_PHASE_TEMP_COMP);
        }
	}
}

void AHDirLightComponent::UpdateWorldBoundsFromSpatial(AHSpatialComponent* pSpatial)
{
	AHSpatialComponent* pSpatialComp = pSpatial;
	_DEBUG_ASSERT(pSpatialComp);

	const gmtl::MatrixA44f& world = pSpatialComp->GetWorldMatrix();
	UpdateWorldBounds(world);
}

void AHDirLightComponent::UpdateWorldBounds(const gmtl::MatrixA44f& world)
{
	gmtl::VecA3f newDir;
	TransformVec(&world, &gmtl::VecA3f(0.0f, -1.0f, 0.0f), &newDir);

	m_BFXDirLight.dir = _CAST_VEC3(newDir);	
	m_DEFXDirLight.dir = m_BFXDirLight.dir;

	TransformAABoxToOOBox(&world, &m_LocalAABox, &m_WorldOOBoxGather);
	OOBoxToAABox(&m_WorldOOBoxGather, &m_WorldAABoxGather);

	AddToTempUpdate();
}

void AHDirLightComponent::OnComponentEvent(int eventType, AHComponent* pSrc)
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

void AHDirLightComponent::Update(uint phaseNum)
{
	// This has to be single-threaded
	if(m_pSceneContainer)
	{
        if(m_SceneHandle.IsValid())
            m_pSceneContainer->Remove(m_SceneHandle);

		m_WorldOOBox = m_WorldOOBoxGather;
		m_WorldAABox = m_WorldAABoxGather;

		m_SceneHandle = m_pSceneContainer->Insert(&m_WorldAABox, &m_WorldOOBox, SDB_FILTER_DIRLIGHT_COMP, (AHComponent*) this);
	}

	m_IsAddedToUpdate = FALSE;
}

void AHDirLightComponent::SetDeferredDirLight(IDEFXLightGroup* pLightGroup, AHRenderComponent* pRenderComp)
{
	m_pLightGroup = pLightGroup;
	m_pRenderComp = pRenderComp;
}

#endif
_NAMESPACE_END