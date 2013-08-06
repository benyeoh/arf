//==============================================================================
//
//		SEFXUSphereTree.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		12/11/09
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

struct SEFXUSphereTreeLevel
{
	uint	numSpheres;
	uint	numValidSpheres;
	Sphere* pSpheres;
};

class SEFXUSphereTree : public CRefCounted<IRefCounted>
{
private:
	Sphere m_Root;

	std::vector<SEFXUSphereTreeLevel> m_Levels;

public:
	SEFXUSphereTree(uint numLevels, uint numBranch);
	virtual ~SEFXUSphereTree();

public:
	SEFXUSphereTreeLevel& GetLevel(uint level) { return m_Levels[level]; }

	Sphere& GetRoot()					{ return m_Root; }
	
	uint	GetNumLevels()				{ return (uint) m_Levels.size(); }
	uint	GetNumSpheres(uint level)	{ return m_Levels[level].numValidSpheres; }
	uint	GetNumBranches()			{ return m_Levels[0].numSpheres; }

	_IMPL_ALIGNED_ALLOCS(16)
};

_DECLARE_SMARTPTR(SEFXUSphereTree)

_NAMESPACE_END
