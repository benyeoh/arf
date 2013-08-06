//==============================================================================
//
//		AlignmentBehavior.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/31/2009
//
//==============================================================================

#pragma once

class AlignmentBehavior : public FlockingBehavior
{
private: 
	float m_Radius;
	
public: 
	AlignmentBehavior()
		: m_Radius(1.0f)
	{
	}

	virtual ~AlignmentBehavior()
	{
	}

public: 
	void	SetRadius(float radius)		{ m_Radius = radius; }
	 
	void	Update(float timeDT, AHComponent* pCurFlocking, AHComponent** ppOthers, uint numOthers, gmtl::VecA3f& newDir);

};
