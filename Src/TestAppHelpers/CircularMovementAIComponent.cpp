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

void CircularMovementAIComponent::UpdateStore(float dt)
{
    AHSpatialComponent* pSpatial = _CAST_COMP_SPATIAL(GetEntity());

    gmtl::VecA3f worldPos;
    pSpatial->GetWorldPos(worldPos);

    gmtl::VecA3f offset;
    VecVecSub(&offset, &worldPos, &m_TetherPos);

    // Construct rotation
    gmtl::MatrixA44f rotMat;
    m_TetherAxisAngle.setAngle(dt * m_Speed * gmtl::Math::PI);
    gmtl::setRot(rotMat, m_TetherAxisAngle);
    
    gmtl::VecA3f outputPos;
    TransformVec(&rotMat, &offset, &outputPos);

    VecVecAdd(&worldPos, &m_TetherPos, &outputPos);
    pSpatial->SetWorldPos(worldPos);
}

void CircularMovementAIComponent::SetRotateAxis(const gmtl::VecA3f& axis)
{
    m_TetherAxisAngle.setAxis(_CAST_VEC3(axis));
}

void CircularMovementAIComponent::SetTetherPos(const gmtl::VecA3f& tether)
{
    m_TetherPos = tether;
}

//void CircularMovementAIComponent::UpdateStore(float dt)
//{
//    m_Dir = m_DirGather;
//
//    AHSpatialComponent* pSpatial = _CAST_COMP_SPATIAL(GetEntity());
//
//    const gmtl::MatrixA44f& worldMat = pSpatial->GetWorldMatrix();
//    gmtl::VecA3f pos(worldMat[0][3], worldMat[1][3], worldMat[2][3]);
//    float speed = m_Speed * dt;
//
//    gmtl::VecA3f displacement;
//    VecScalarMult(&displacement, &m_Dir, speed);
//    VecVecAdd(&pos, &pos, &displacement);
//
//    gmtl::VecA3f up(0.0f, 1.0f, 0.0f);
//    if(gmtl::Math::abs(m_Dir[1]) > 0.989f)
//        up.set(1.0f, 0.0f, 0.0f);
//
//    pSpatial->SetWorldPosAndFacing(pos, m_Dir, up);
//}

void CircularMovementAIComponent::Update(uint phaseNum)
{
    UpdateStore(*m_pTimeDT);
}

void CircularMovementAIComponent::OnComponentEvent(int eventType, AHComponent* pSrc)
{
    switch(eventType)
    {
    //case EVENT_COMP_SPATIAL_CHANGED:
    //    {
    //        _DEBUG_ASSERT(pSrc->GetType() == COMP_SPATIAL);
    //        break;
    //    }

    case EVENT_COMP_ADDED:
        {
            if(this == pSrc)
            {
                // Initialize
                AHSpatialComponent* pSpatial = (AHSpatialComponent*) GetEntity()->GetComponent(COMP_SPATIAL);
                if(pSpatial)
                {
                    pSpatial->AddEventListener(this);
                }
            }
            else if(pSrc->GetType() == COMP_SPATIAL)
            {
                AHSpatialComponent* pSpatial = (AHSpatialComponent*) pSrc;
                pSpatial->AddEventListener(this);
            }

            break;
        }
    }
}
