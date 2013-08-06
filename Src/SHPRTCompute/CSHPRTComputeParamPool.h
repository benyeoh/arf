//================================================================================
//
//		CSHPRTComputeParamPool.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/28/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

class CSHPRTComputeParamPool : IBFXParamCallback
{
private:
	const gmtl::Matrix44f*	m_pParaboloidView;

public:
	CSHPRTComputeParamPool()
		: m_pParaboloidView(NULL)
	{
	}

	~CSHPRTComputeParamPool()
	{
	}

public:
	void SetParaboloidViewMatrix(const gmtl::Matrix44f* pMat);

public:
	void GetParam(uint semantic, REffectParam* pToStore);
	boolean IsParamAvailable(uint semantic);
	boolean IsMorePassesRequired();

	void FinishedResolvingEffect();
	void ResetParams();

	// Prevent cache line false sharing
	_IMPL_ALIGNED_ALLOCS(_CACHE_LINE_SIZE)
};

_NAMESPACE_END
