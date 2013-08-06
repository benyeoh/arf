//==============================================================================
//
//		SEFXUSphereTree.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		12/11/09
//
//==============================================================================

#include "SHExpLightingUtils.h"

_NAMESPACE_BEGIN

SEFXUSphereTree::SEFXUSphereTree(uint numLevels, uint numBranch)
{
	uint curNumSpheres = numBranch;

	_LOOPi(numLevels)
	{
		SEFXUSphereTreeLevel level;
		level.numSpheres		= curNumSpheres;
		level.numValidSpheres	= 0;
		level.pSpheres			= (Sphere*) _ALIGNED_MALLOC(16, curNumSpheres * sizeof(Sphere));
		m_Levels.push_back(level);

		curNumSpheres *= numBranch;
	}
}

SEFXUSphereTree::~SEFXUSphereTree()
{
	_LOOPi(m_Levels.size())
	{
		_ALIGNED_FREE(m_Levels[i].pSpheres);
	}
}


_NAMESPACE_END
