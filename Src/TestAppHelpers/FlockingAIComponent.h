//================================================================================
//
//		FlockingAIComponent.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/9/2012
//
//================================================================================

#pragma once

class FlockingBehavior : public CRefCounted<IRefCounted>
{
private:
	float	m_Strength;

public:
	FlockingBehavior()
		: m_Strength(1.0f)
	{
	}

	virtual ~FlockingBehavior()
	{
	}

public:
	void	SetStrength(float strength)		{ m_Strength = strength; }
	float	GetStrength()					{ return m_Strength; }

	_PURE( void Update(float timeDT, AHComponent* pCurFlocking, AHComponent** ppOthers, uint numOthers, gmtl::VecA3f& newDir) )

};

_DECLARE_SMARTPTR(FlockingBehavior)

class FlockingAIComponent : public AHComponent
{
private:
	gmtl::VecA3f	m_DirGather;
	gmtl::VecA3f	m_Axes;
	
	AABox			m_LocalAABox;
	AABox			m_WorldAABoxGather;
	OOBox			m_WorldOOBoxGather;
	
	AABox			m_WorldAABox;
	OOBox			m_WorldOOBox;
	
	gmtl::VecA3f	m_Dir;
	
	float			m_Speed;
	float			m_SteeringSpeed;

	FlockingBehaviorPtr m_FlockingBehaviors[16];
	uint				m_NumBehaviors;

	float*			m_pTimeDT;

	boolean	m_IsAddedToUpdate;
	ISDBSceneContainer* m_pSceneContainer;
    SDBDataHandle   m_SceneHandle;
	float			m_SearchRadius;

public:
	FlockingAIComponent(float* pTimeDT, ISDBSceneContainer* pScene)
		: AHComponent(COMP_AI_FLOCKING)
		, m_Speed(1.0f)
		, m_SteeringSpeed(1.0f)
		, m_NumBehaviors(0)
		, m_pTimeDT(pTimeDT)
		, m_pSceneContainer(NULL)
		, m_IsAddedToUpdate(FALSE)
		, m_SearchRadius(25.0f)
	{
		m_LocalAABox.min[0] = -0.01f;
		m_LocalAABox.min[1] = -0.01f;
		m_LocalAABox.min[2] = -0.01f;

		m_LocalAABox.max[0] = 0.01f;
		m_LocalAABox.max[1] = 0.01f;
		m_LocalAABox.max[2] = 0.01f;

		m_Axes[0] = m_Axes[1] = m_Axes[2] = 1.0f;

		m_Dir[0] = 0.0f;
		m_Dir[1] = 0.0f;
		m_Dir[2] = 1.0f;

		m_DirGather = m_Dir;

		gmtl::MatrixA44f identity;
		gmtl::identity(_CAST_MAT44(identity));
		UpdateWorldBounds(identity);
		SetScene(pScene);
	}

	~FlockingAIComponent()
	{
	}

private:
	void UpdateWorldBounds(const gmtl::MatrixA44f& world);
	void UpdateWorldBoundsFromSpatial(AHSpatialComponent* pSpatial);
	void AddToTempUpdate();

	void UpdateGather(float dt);
	void UpdateStore(float dt);
	void UpdateTemp();

public:
	void SetScene(ISDBSceneContainer* pScene);
	const gmtl::VecA3f& GetDir() { return m_Dir; }
	
	void SetSearchRadius(float rad) { m_SearchRadius = rad; }
	void SetSpeed(float speed) {  m_Speed = speed; }
	void SetSteering(float steering) { m_SteeringSpeed = steering; }
	void SetAxes(const gmtl::VecA3f& axes) { m_Axes = axes; }
	void AddBehavior(FlockingBehavior* pBehavior);

	void Update(uint phaseNum);
	void OnComponentEvent(int eventType, AHComponent* pSrc);

	int GetComponentUpdatePhaseFlags()	{ return (1 << UPDATE_PHASE_GATHER_1) | (1 << UPDATE_PHASE_STORE_1); }

	_IMPL_ALIGNED_ALLOCS(16)

};