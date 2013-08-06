//================================================================================
//
//		CircularMovementAIComponent.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/9/2012
//
//================================================================================

#pragma once

class CircularMovementAIComponent : public AHComponent
{
private:
    gmtl::MatrixA44f    m_RotationMat;
    gmtl::VecA3f        m_TetherPos;
    gmtl::AxisAnglef    m_TetherAxisAngle;

    float			m_Speed;
    float			m_Time;
    float*			m_pTimeDT;

public:
    CircularMovementAIComponent(float* pTimeDT)
        : AHComponent(COMP_AI_CIRCULAR_MOVE)
        , m_Speed(1.0f)
        , m_Time(0.0f)
        , m_pTimeDT(pTimeDT)
    {
        m_TetherPos.set(0.0f, 0.0f, -1.0f);
        m_TetherAxisAngle.setAxis(gmtl::Vec3f(0.0f, 1.0f, 0.0f));
        m_TetherAxisAngle.setAngle(m_Speed);

        gmtl::identity(_CAST_MAT44(m_RotationMat));
    }

    ~CircularMovementAIComponent()
    {
    }

private:
    void UpdateStore(float dt);

public:
    void SetSpeed(float speed) {  m_Speed = speed; }
    void SetRotateAxis(const gmtl::VecA3f& axis);
    void SetTetherPos(const gmtl::VecA3f& tether);

    void Update(uint phaseNum);
    void OnComponentEvent(int eventType, AHComponent* pSrc);

    int GetComponentUpdatePhaseFlags()	{ return (1 << UPDATE_PHASE_STORE_1); }

    _IMPL_ALIGNED_ALLOCS(16)

};