//================================================================================
//
//		CDataStoreForXSIPlugin.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		27/9/2011
//
//================================================================================

#include "stdafx.h"

void CDataStoreForXSIPlugin::AddRefCountData(uint index, _NAMESPACE IRefCounted* pData)
{
	_DEBUG_ASSERT(m_DataStore.Get(index) == NULL);
	m_DataStore.Add(index, pData);
}

_NAMESPACE IRefCounted* CDataStoreForXSIPlugin::GetRefCountData(uint index)
{
	_NAMESPACE IRefCountedPtr* ppRefCounted = m_DataStore.Get(index);
	if(ppRefCounted)
		return *ppRefCounted;

	return NULL;
}

void CDataStoreForXSIPlugin::ClearAll()
{
	m_DataStore.~DataStoreMap();
	m_DataStore.Initialize(128, 0.7f);
}

void CDataStoreForXSIPlugin::ClearRefCountData(uint index)
{
	m_DataStore.Remove(index);
}
