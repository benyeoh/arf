//==============================================================================
//
//		SphereAvoidanceBehavior.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/31/2009
//
//==============================================================================

#pragma once

class SphereAvoidanceBehavior : public FlockingBehavior
{
private: 
	Sphere	m_Sphere;
	float	m_Threshold;
	
public: 
	SphereAvoidanceBehavior()
		: m_Threshold(0.0f)
	{
	}

	virtual ~SphereAvoidanceBehavior()
	{
	}

public: 
	void SetSphere(const Sphere& sphere) { m_Sphere = sphere; }
	void SetThreshold(float threshold)	{ m_Threshold = threshold; }
	
	void Update(float timeDT, AHComponent* pCurFlocking, AHComponent** ppOthers, uint numOthers, gmtl::VecA3f& newDir);
							
	_IMPL_ALIGNED_ALLOCS(16)
};
