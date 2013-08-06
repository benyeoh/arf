//==============================================================================
//
//		CSDBSceneDB.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/4/2009
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CSDBSceneDB : public CRefCounted<ISDBSceneDB>
{
private: 
	CSDBResourceMgr	m_ResMgr;
	
public: 
	CSDBSceneDB()
	{
	}

	virtual ~CSDBSceneDB()
	{
	}

public: 
	void	Initialize();
	
	ISDBResourceMgr&	GetResourceMgr() { return m_ResMgr; }
};

_NAMESPACE_END
