//==============================================================================
//
//		CSDBResourceMgr.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/4/2009
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CSDBResourceMgr : public ISDBResourceMgr
{
private:
	CSDBSceneDB* m_pMain;

public: 
	CSDBResourceMgr()
		: m_pMain(NULL)
	{
	}

	virtual ~CSDBResourceMgr()
	{
	}

public: 
	void Initialize(CSDBSceneDB* pSceneDB);
	
	ISDBSparseLooseOctTree* CreateOctantOrderLooseOctTree(uint levels, float scale);	
	ISDBSparseLooseOctTree* CreateSparseLooseOctTree(uint levels, float scale, uint initSize);	
	ISDBArray* CreateArray();
};

_NAMESPACE_END
