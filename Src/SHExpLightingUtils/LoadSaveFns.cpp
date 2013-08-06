//==============================================================================
//
//		LoadSaveFns.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/29/2007
//
//==============================================================================

#include "SHExpLightingUtils.h"

_NAMESPACE_BEGIN

uint
SaveSphereTreeLevel(SEFXUSphereTreeLevel& level, IByteBuffer* pBuffer)
{
	_DEBUG_ASSERT(pBuffer);

	uint totalBytesWritten = AppendData(pBuffer, SPHERE_TREE_LEVEL_CHUNK_H);	
	
	uint numSpheres = level.numSpheres;
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('n','s','p','h'), &numSpheres);
	uint numValidSpheres = level.numValidSpheres;
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('n','v','s','p'), &numValidSpheres);
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('s','p','h','s'), level.pSpheres, sizeof(Sphere) * numSpheres);

	totalBytesWritten += AppendData(pBuffer, END_CHUNK_H);

	return totalBytesWritten;
}

uint
SaveSphereTree(SEFXUSphereTree* pSphereTree, IByteBuffer* pBuffer)
{
	_DEBUG_ASSERT(pSphereTree);
	_DEBUG_ASSERT(pBuffer);

	uint totalBytesWritten = AppendData(pBuffer, SPHERE_TREE_CHUNK_H);	

	uint numLevels		= pSphereTree->GetNumLevels();
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('n','l','v','l'), &numLevels);
	uint numBranches	= pSphereTree->GetNumBranches();
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('n','b','r','n'), &numBranches);
	totalBytesWritten += SaveData(pBuffer, _MAKE_ID('r','o','o','t'), &pSphereTree->GetRoot(), sizeof(Sphere));

	_LOOPi(numLevels )
	{
		totalBytesWritten += SaveSphereTreeLevel(pSphereTree->GetLevel(i), pBuffer);		
	}

	totalBytesWritten += AppendData(pBuffer, END_CHUNK_H);

	return totalBytesWritten;
}

uint
LoadSphereTreeLevel(IByteBuffer* pBuffer, uint offset, SEFXUSphereTreeLevel& level)
{
	_DEBUG_ASSERT(pBuffer);

	uint totalBytesRead = 0;

	uint header = 0;
	uint bytesRead = ReadData(pBuffer, offset + totalBytesRead, &header);

	if(header == SPHERE_TREE_LEVEL_CHUNK_H)
	{
		totalBytesRead += bytesRead;

		DataMap store;
		totalBytesRead += LoadAllData(pBuffer, &store, offset + totalBytesRead);

		uint numSpheres = 0;
		GetData(&store, _MAKE_ID('n','s','p','h'), &numSpheres);
		uint numValidSpheres = 0;
		GetData(&store, _MAKE_ID('n','v','s','p'), &numValidSpheres);
		Sphere* pSpheres = NULL;
		GetDataRaw(&store, _MAKE_ID('s','p','h','s'), (void**) &pSpheres);
		
		level.numSpheres		= numSpheres;
		level.numValidSpheres	= numValidSpheres;
		level.pSpheres			= (Sphere*) _ALIGNED_MALLOC(16, sizeof(Sphere) * numSpheres);
		memcpy(level.pSpheres, pSpheres, sizeof(Sphere) * numSpheres);

		byte endHeader = 0;
		totalBytesRead += ReadData(pBuffer, offset + totalBytesRead, &endHeader);
		_DEBUG_ASSERT(endHeader == END_CHUNK_H);
	}

	return totalBytesRead;
}

uint
LoadSphereTree(IByteBuffer* pBuffer, uint offset, SEFXUSphereTree** ppSphereTree)
{
	_DEBUG_ASSERT(ppSphereTree);
	_DEBUG_ASSERT(pBuffer);

	uint totalBytesRead = 0;

	uint header = 0;
	uint bytesRead = ReadData(pBuffer, offset + totalBytesRead, &header);

	if(header == SPHERE_TREE_CHUNK_H)
	{
		totalBytesRead += bytesRead;

		DataMap store;
		totalBytesRead += LoadAllData(pBuffer, &store, offset + totalBytesRead);

		uint numLevels = 0;
		GetData(&store, _MAKE_ID('n','l','v','l'), &numLevels);
		uint numBranches = 0;
		GetData(&store, _MAKE_ID('n','b','r','n'), &numBranches);

		SEFXUSphereTree* pSphereTree = _NEW SEFXUSphereTree(numLevels, numBranches);

		GetData(&store, _MAKE_ID('r','o','o','t'), &pSphereTree->GetRoot());

		_LOOPi(numLevels)
		{
			uint bytesRead = LoadSphereTreeLevel(pBuffer, offset + totalBytesRead, pSphereTree->GetLevel(i));
			_DEBUG_ASSERT(bytesRead > 0);

			totalBytesRead += bytesRead;
		}		

		*ppSphereTree = pSphereTree;

		byte endHeader = 0;
		totalBytesRead += ReadData(pBuffer, offset + totalBytesRead, &endHeader);
		_DEBUG_ASSERT(endHeader == END_CHUNK_H);
	}

	return totalBytesRead;
}


_NAMESPACE_END