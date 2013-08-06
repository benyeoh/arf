//==============================================================================
//
//		ConvertFromSPH.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		13/11/09
//
//==============================================================================

#include "stdafx.h"

SEFXUSphereTree* 
ConvertFromSPH(const wchar* pFileName)
{
	FILE* pFile = _wfopen(pFileName, _W("r"));
	_DEBUG_ASSERT(pFile);

	char buffer[1024];
	float x, y, z, r, contrib;

	// Read levels
	uint numLevels		= 0;
	uint numBranches	= 0;
	fgets(buffer, 1023, pFile);
	sscanf(buffer, "%d %d", &numLevels, &numBranches);

	numLevels -= 1;

	SEFXUSphereTree* pSphereTree = _NEW SEFXUSphereTree(numLevels, numBranches);

	// Read root
	fgets(buffer, 1023, pFile);
	sscanf(buffer, "%f %f %f %f %f", &x, &y, &z, &r, &contrib);
	Sphere& root = pSphereTree->GetRoot();
	root.center[0] = x;
	root.center[1] = y;
	root.center[2] = z;
	root.center[3] = r;

	// Read each level
	uint branchTotal = numBranches;
	_LOOPi(numLevels)
	{
		SEFXUSphereTreeLevel& level = pSphereTree->GetLevel(i);
		level.numSpheres = branchTotal;

		_LOOPi(branchTotal)
		{
			fgets(buffer, 1023, pFile);
			sscanf(buffer, "%f %f %f %f %f", &x, &y, &z, &r, &contrib);
			if(r > 0.0f)
			{
				level.pSpheres[level.numValidSpheres].center[0] = x;
				level.pSpheres[level.numValidSpheres].center[1] = y;
				level.pSpheres[level.numValidSpheres].center[2] = z;
				level.pSpheres[level.numValidSpheres].center[3] = r;

				level.numValidSpheres++;
			}
		}

		branchTotal *= numBranches;
	}	

	fclose(pFile);

	return pSphereTree;
}