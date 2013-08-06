//==============================================================================
//
//		SEFXUVisibilitySpheres.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		23/11/09
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class SEFXUVisibilityTree : public CRefCounted<IRefCounted>
{
private:
	Sphere				m_Coverage;
	gmtl::MatrixA44f	m_CoverageWorld;
	AABox				m_CoverageBV;
	AABox				m_ScreenCoverageBV;

	SEFXUSphereTreePtr	m_pSpheres;
	SEFXUSphereTreePtr	m_pCurSpheres;

public:
	SEFXUVisibilityTree(SEFXUSphereTree* pSphereTree);
	virtual ~SEFXUVisibilityTree();

public:
	void SetScreenCoverageBV(const AABox& screen)		{ m_ScreenCoverageBV = screen; }
	
	void UpdateCoverage(const gmtl::MatrixA44f& world);
	void UpdateSphereLevel(const gmtl::MatrixA44f& world, uint level);	
	void UpdateScreenCoverage(const gmtl::MatrixA44f& viewProj, float nearZ, float farZ);	

	SEFXUSphereTree&		GetCurrentSphereTree()	{ return *m_pCurSpheres; }
	const AABox&			GetCoverageBV()			{ return m_CoverageBV; }
	const AABox&			GetScreenCoverageBV()	{ return m_ScreenCoverageBV; }
	const gmtl::MatrixA44f&	GetCoverageWorld()		{ return m_CoverageWorld; }
	const Sphere&			GetCoverage()			{ return m_Coverage; }					

	_IMPL_ALIGNED_ALLOCS(16)
};

_DECLARE_SMARTPTR(SEFXUVisibilityTree)

_NAMESPACE_END
