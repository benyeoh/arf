//==============================================================================
//
//		AABoxAvoidanceBehavior.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/31/2009
//
//==============================================================================

#pragma once

class AABoxAvoidanceBehavior : public FlockingBehavior
{
private: 
	AABox	m_AABox;
	float	m_Threshold;
	
public: 
	AABoxAvoidanceBehavior()
		: m_Threshold(0.0f)
	{
	}

	virtual ~AABoxAvoidanceBehavior()
	{
	}

public:
	void SetAABox(const AABox& box) { m_AABox = box; }
	void SetThreshold(float threshold)	{ m_Threshold = threshold; }

	void Update(float timeDT, AHComponent* pCurFlocking, AHComponent** ppOthers, uint numOthers, gmtl::VecA3f& newDir);

	_IMPL_ALIGNED_ALLOCS(16)
};
