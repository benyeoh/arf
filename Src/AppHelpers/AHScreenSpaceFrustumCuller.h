//==============================================================================
//
//		AHScreenSpaceFrustumCuller.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/27/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class AHScreenSpaceFrustumCuller : public CRefCounted<IRefCounted>
{
private: 
	gmtl::MatrixA44f m_ViewProj;
	AABox m_Frustum;
	float m_ZNear;
	float m_ZFar;
	
public: 
	AHScreenSpaceFrustumCuller()
		: m_ZNear(0.01f)
		, m_ZFar(100.0f)
	{
		m_Frustum.min = gmtl::VecA3f(-1.0f, -1.0f, 0.0f);
		m_Frustum.max = gmtl::VecA3f(1.0f, 1.0f, 1.0f);
	}

	~AHScreenSpaceFrustumCuller()
	{
	}

public: 
	void SetViewProjMatrixPersp(const gmtl::MatrixA44f& viewProj, float nearZ, float farZ);
	void SetViewProjMatrixOrtho(const gmtl::MatrixA44f& viewProj);
	
	boolean IsInFrustumOrtho(const OOBox& worldSpaceBox, AABox& screenSpaceResult);	
	boolean IsInFrustumPersp(const OOBox& worldSpaceBox, AABox& screenSpaceResult);	
	
	_IMPL_ALIGNED_ALLOCS(16)
};

_DECLARE_SMARTPTR(AHScreenSpaceFrustumCuller)

_NAMESPACE_END
