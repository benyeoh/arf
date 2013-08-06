//==============================================================================
//
//		VertexCache.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		2/4/2009
//
//==============================================================================

#pragma once

class VertexCache
{	
private: 
	std::list<uint> m_LRUCache;
	uint			m_CacheSize;
		
public: 
	VertexCache(uint cacheSize)
		: m_CacheSize(cacheSize)
	{
	}

	~VertexCache()
	{
	}

public: 
	boolean RegisterVertex(uint vertIndex);
};
