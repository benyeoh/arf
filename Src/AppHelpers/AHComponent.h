//==============================================================================
//
//		AHComponent.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/18/2009
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class AHComponent : public CRefCounted<IRefCounted>
{
private: 
	uint	m_Type;
	
	AHEntity* m_pEntity;
	
public: 
	AHComponent(uint type)
		: m_Type(type)
		, m_pEntity(NULL)
	{
	}

	virtual ~AHComponent()
	{
	}

public: 
	inline uint			GetType()	{ return m_Type; }

	inline AHEntity*	GetEntity()	{ return m_pEntity; }
	inline void			SetEntity(AHEntity* pEntity)	{ m_pEntity = pEntity; }

	virtual void Update(uint phaseNum) {}
	virtual void OnComponentEvent(int eventType, AHComponent* pSrc) {}
	virtual int GetComponentUpdatePhaseFlags()	{ return 0; }
};

_DECLARE_SMARTPTR(AHComponent)

_NAMESPACE_END
