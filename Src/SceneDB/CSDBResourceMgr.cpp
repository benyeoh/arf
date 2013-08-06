//==============================================================================
//
//		CSDBResourceMgr.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/4/2009
//
//==============================================================================

#include "SceneDBFramework.h"

_NAMESPACE_BEGIN

void 
CSDBResourceMgr::Initialize(CSDBSceneDB* pSceneDB)
{
	m_pMain = pSceneDB;
}
	
ISDBSparseLooseOctTree* CSDBResourceMgr::CreateSparseLooseOctTree(uint levels, float scale, uint initSize)
{
	CSDBSparseLooseOctTree* pOctTree = _NEW CSDBSparseLooseOctTree(levels, scale, initSize);
	return pOctTree;
}

ISDBSparseLooseOctTree* CSDBResourceMgr::CreateOctantOrderLooseOctTree(uint levels, float scale)
{
	CSDBOctantOrderLooseOctTree* pOctTree = _NEW CSDBOctantOrderLooseOctTree(levels, scale);
	return pOctTree;
}

ISDBArray* CSDBResourceMgr::CreateArray()
{
	CSDBArray* pArray = _NEW CSDBArray();
	return pArray;
}


_NAMESPACE_END