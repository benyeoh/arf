//==============================================================================
//
//		AHPointLightComponent.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/27/2009
//
//==============================================================================

#include "AppHelpers.h"

_NAMESPACE_BEGIN

#if defined(_IS_USE_APPHELPER_LIB_BASIC)

AHPointLightComponent::AHPointLightComponent()
	: AHComponent(COMP_POINTLIGHT)
	, m_pSceneContainer(NULL)
	, m_IsAddedToUpdate(FALSE)
{
	m_BFXPointLight.pos = gmtl::Vec3f(0.0f, 0.0f, 0.0f);
	m_BFXPointLight.invRange = 1.0f / 10.0f;
	m_BFXPointLight.invSize = 1.0f;
	m_BFXPointLight.color = gmtl::Vec4f(1.0f, 1.0f, 1.0f, 1.0f);

	m_DEFXPointLight.pos = m_BFXPointLight.pos;
	m_DEFXPointLight.invRange = m_BFXPointLight.invRange;
	m_DEFXPointLight.color = m_BFXPointLight.color;

	gmtl::identity(_CAST_MAT44(m_RenderWorldMat));

	m_IsShadowMapShared = FALSE;
}

AHPointLightComponent::~AHPointLightComponent()
{
	SetScene(NULL);
}

void	
AHPointLightComponent::SetColor(const gmtl::Vec3f& color)
{
	m_BFXPointLight.color.set(color[0], color[1], color[2], 1.0f);
	m_DEFXPointLight.color = m_BFXPointLight.color;
}

void	
AHPointLightComponent::SetRadius(float lightRadius)
{
	m_WorldSphereBV.center[3] = lightRadius;
	m_BFXPointLight.invRange = 1.0f / lightRadius;
	m_DEFXPointLight.invRange = m_BFXPointLight.invRange;

	if(m_pShadowPL)
	{
		m_pShadowPL->SetRangeNoUpdate(lightRadius);
	}

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

void AHPointLightComponent::UpdateWorldBounds(const gmtl::MatrixA44f& world)
{
	m_BFXPointLight.pos.set(world[0][3], world[1][3], world[2][3]);
	m_WorldSphereBV.center[0] = m_BFXPointLight.pos[0];
	m_WorldSphereBV.center[1] = m_BFXPointLight.pos[1];
	m_WorldSphereBV.center[2] = m_BFXPointLight.pos[2];
	m_DEFXPointLight.pos = m_BFXPointLight.pos;

	m_WorldAABoxGather.min.set(m_WorldSphereBV.center[0] - m_WorldSphereBV.center[3], 
		m_WorldSphereBV.center[1] - m_WorldSphereBV.center[3],
		m_WorldSphereBV.center[2] - m_WorldSphereBV.center[3]);
	m_WorldAABoxGather.max.set(m_WorldSphereBV.center[0] + m_WorldSphereBV.center[3], 
		m_WorldSphereBV.center[1] + m_WorldSphereBV.center[3],
		m_WorldSphereBV.center[2] + m_WorldSphereBV.center[3]);

	m_WorldOOBoxGather.corner = m_WorldAABoxGather.min;
	m_WorldOOBoxGather.xExtents[0] = m_WorldAABoxGather.max[0] - m_WorldAABoxGather.min[0];
	m_WorldOOBoxGather.xExtents[1] = 0.0f;
	m_WorldOOBoxGather.xExtents[2] = 0.0f;

	m_WorldOOBoxGather.yExtents[0] = 0.0f;
	m_WorldOOBoxGather.yExtents[1] = m_WorldAABoxGather.max[1] - m_WorldAABoxGather.min[1];
	m_WorldOOBoxGather.yExtents[2] = 0.0f;

	m_WorldOOBoxGather.zExtents[0] = 0.0f;
	m_WorldOOBoxGather.zExtents[1] = 0.0f;
	m_WorldOOBoxGather.zExtents[2] = m_WorldAABoxGather.max[2] - m_WorldAABoxGather.min[2];

	if(m_pShadowPL)
	{
		m_pShadowPL->Update(m_BFXPointLight.pos);
	}

	if(m_pRenderComp)
	{
		gmtl::setScale(_CAST_MAT44(m_RenderWorldMat), m_WorldSphereBV.center[3]);
		gmtl::setTrans(_CAST_MAT44(m_RenderWorldMat), m_DEFXPointLight.pos);

		//m_pRenderComp->UpdateWorldBounds(m_RenderWorldMat);
	}

	AddToTempUpdate();
}

void	
AHPointLightComponent::UpdateWorldBoundsFromSpatial(AHSpatialComponent* pSpatial)
{
	AHSpatialComponent* pSpatialComp = pSpatial;
	_DEBUG_ASSERT(pSpatialComp);

	const gmtl::MatrixA44f& world = pSpatialComp->GetWorldMatrix();
	UpdateWorldBounds(world);
}

void AHPointLightComponent::Update(uint phaseNum)
{
	// This has to be single-threaded
	if(m_pSceneContainer)
	{
        if(m_SceneHandle.IsValid())
		    m_pSceneContainer->Remove(m_SceneHandle);

		m_WorldAABoxBV	= m_WorldAABoxGather;
		m_WorldOOBoxBV	= m_WorldOOBoxGather;

		m_SceneHandle = m_pSceneContainer->Insert(&m_WorldAABoxBV, &m_WorldOOBoxBV, SDB_FILTER_POINTLIGHT_COMP, (AHComponent*) this);
	}

	m_IsAddedToUpdate = FALSE;
}

	
//void	
//AHPointLightComponent::AddToRenderComp(AHRenderComponent* pRenderComp)
//{
//	AHRenderBaseFXParam* pParam = _CAST_REND_PARAM_BASE_FX(pRenderComp);
//	if(pParam)
//	{
//		pParam->AddPointLight(&m_BFXPointLight);
//	}
//
//	AHRenderShadowFXParam* pShadowParam = _CAST_REND_PARAM_SHADOW_FX(pRenderComp);
//	if(pShadowParam && m_pShadowPL)
//	{
//		gmtl::VecA3f diffMin;
//		gmtl::VecA3f diffMax;
//		VecVecSub(&_CAST_VECA4(diffMin), &_CAST_VECA4(pRenderComp->GetWorldAABox().min), &_CAST_VECA4(m_WorldSphereBV.center));
//		VecVecSub(&_CAST_VECA4(diffMax), &_CAST_VECA4(pRenderComp->GetWorldAABox().max), &_CAST_VECA4(m_WorldSphereBV.center));
//
//		// Set the flags if the y-min/y-max is on the back/front hemisphere of the point light
//		uint casterFlag = (diffMin[1] < 0.0f ? 0x10 : 0x01) | (diffMax[1] < 0.0f ? 0x10 : 0x01);
//
//		pShadowParam->AddPointLight(m_pShadowPL, casterFlag);
//
//	}
//}

void	
AHPointLightComponent::SetShadowPointLight(ISHFXCubePointLight* pShadowPL, boolean isShadowMapShared)
{
	m_pShadowPL = pShadowPL;
	if(m_pShadowPL)
	{
		m_pShadowPL->SetRangeNoUpdate(1.0f / m_BFXPointLight.invRange);
		m_pShadowPL->Update(m_BFXPointLight.pos);
	}

	m_IsShadowMapShared = isShadowMapShared;
}

void AHPointLightComponent::SetDeferredPointLight(IDEFXLightGroup* pLightGroup, AHRenderComponent* pRenderComp)
{
	m_pLightGroup = pLightGroup;
	m_pRenderComp = pRenderComp;

	if(m_pRenderComp)
	{
		gmtl::setScale(_CAST_MAT44(m_RenderWorldMat), m_WorldSphereBV.center[3]);
		gmtl::setTrans(_CAST_MAT44(m_RenderWorldMat), m_DEFXPointLight.pos);
	}
}

void AHPointLightComponent::OnComponentEvent(int eventType, AHComponent* pSrc)
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

void AHPointLightComponent::AddToTempUpdate()
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
					m_WorldOOBoxBV = m_WorldOOBoxGather;
					m_WorldAABoxBV = m_WorldAABoxGather;
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

void AHPointLightComponent::SetScene(ISDBSceneContainer* pContainer)
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