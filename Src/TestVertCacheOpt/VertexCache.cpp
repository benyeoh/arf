//==============================================================================
//
//		VertexCache.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		2/4/2009
//
//==============================================================================

#include "stdafx.h"

boolean 
VertexCache::RegisterVertex(uint vertIndex)
{	
	std::list<uint>::iterator itr = std::find(m_LRUCache.begin(), m_LRUCache.end(), vertIndex);
	boolean isCacheHit = itr != m_LRUCache.end();
	if(isCacheHit)
		m_LRUCache.erase(itr);
	else 
	if(m_LRUCache.size() >= m_CacheSize)
		m_LRUCache.pop_back();
	
	m_LRUCache.push_front(vertIndex);
		
	return isCacheHit;	
}

