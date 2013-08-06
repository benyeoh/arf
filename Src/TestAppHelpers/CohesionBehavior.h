//==============================================================================
//
//		CohesionBehavior.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/31/2009
//
//==============================================================================

#pragma once

class CohesionBehavior : public FlockingBehavior
{
private: 
	float m_Radius;
	
public: 
	CohesionBehavior()
		: m_Radius(1.0f)
	{
	}

	virtual ~CohesionBehavior()
	{
	}

public: 
	void	SetRadius(float radius)		{ m_Radius = radius; }

	void	Update(float timeDT, AHComponent* pCurFlocking, AHComponent** ppOthers, uint numOthers, gmtl::VecA3f& newDir);
							
};


