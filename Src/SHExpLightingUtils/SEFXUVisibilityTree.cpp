//==============================================================================
//
//		SEFXUVisibilityTree.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		23/11/09
//
//==============================================================================

#include "SHExpLightingUtils.h"

_NAMESPACE_BEGIN

SEFXUVisibilityTree::SEFXUVisibilityTree(SEFXUSphereTree* pSphereTree)
{
	m_pSpheres = pSphereTree;
	m_pCurSpheres = _NEW SEFXUSphereTree(m_pSpheres->GetNumLevels(), m_pSpheres->GetNumBranches());

	_LOOPi(m_pSpheres->GetNumLevels())
	{
		SEFXUSphereTreeLevel& srcLevel = m_pSpheres->GetLevel(i);
		SEFXUSphereTreeLevel& destLevel = m_pCurSpheres->GetLevel(i);
		destLevel.numValidSpheres = srcLevel.numValidSpheres;
	}
}

SEFXUVisibilityTree::~SEFXUVisibilityTree()
{
}

void 
SEFXUVisibilityTree::UpdateCoverage(const gmtl::MatrixA44f& world)
{
	const static float EXPANSION_FACTOR = 7.0f;

	Sphere& root = m_pSpheres->GetRoot();
	TransformVecW1(&_CAST_MATA44(world), &_CAST_VECA3(root.center), &_CAST_VECA3(m_Coverage.center));
	m_Coverage.center[3] = root.center[3] * EXPANSION_FACTOR;

	m_CoverageBV.min = _CAST_VECA3(m_Coverage.center);
	m_CoverageBV.max = _CAST_VECA3(m_Coverage.center);
	m_CoverageBV.min[0] -= m_Coverage.center[3];
	m_CoverageBV.min[1] -= m_Coverage.center[3];
	m_CoverageBV.min[2] -= m_Coverage.center[3];
	m_CoverageBV.max[0] += m_Coverage.center[3];
	m_CoverageBV.max[1] += m_Coverage.center[3];
	m_CoverageBV.max[2] += m_Coverage.center[3];

	gmtl::setScale(_CAST_MAT44(m_CoverageWorld), gmtl::Vec3f(m_Coverage.center[3], m_Coverage.center[3], m_Coverage.center[3]));
	gmtl::setTrans(_CAST_MAT44(m_CoverageWorld), _CAST_VEC3(m_Coverage.center));
}

void 
SEFXUVisibilityTree::UpdateSphereLevel(const gmtl::MatrixA44f& world, uint level)
{
	SEFXUSphereTreeLevel& srcLevel	= m_pSpheres->GetLevel(level);
	SEFXUSphereTreeLevel& destLevel	= m_pCurSpheres->GetLevel(level);

	_DEBUG_ASSERT(srcLevel.numValidSpheres == destLevel.numValidSpheres);

	BatchTransformVecW1(&world, (gmtl::VecA4f*) (srcLevel.pSpheres), (gmtl::VecA4f*) (destLevel.pSpheres), srcLevel.numValidSpheres);

	// Copy the radius of the spheres
	_LOOPi(srcLevel.numValidSpheres)
	{
		destLevel.pSpheres[i].center[3] = srcLevel.pSpheres[i].center[3];
	}
}

void 
SEFXUVisibilityTree::UpdateScreenCoverage(const gmtl::MatrixA44f& viewProj, float nearZ, float farZ)
{
	TransformAndProjectAABoxToAABox(&viewProj, nearZ, farZ, &m_CoverageBV, &m_ScreenCoverageBV);
	VecVecMax(&m_ScreenCoverageBV.min, &m_ScreenCoverageBV.min, &gmtl::VecA3f(-1.0f, -1.0f, 0.0f));
	VecVecMin(&m_ScreenCoverageBV.max, &m_ScreenCoverageBV.max, &gmtl::VecA3f(1.0f, 1.0f, 0.0f));

}

_NAMESPACE_END
