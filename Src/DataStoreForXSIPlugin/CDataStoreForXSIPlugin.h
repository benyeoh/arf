//================================================================================
//
//		CDataStoreForXSIPlugin.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		27/9/2011
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN 

_DECLARE_SMARTPTR(IRefCounted)

_NAMESPACE_END

class CDataStoreForXSIPlugin : public IDataStoreForXSIPlugin
{
private:
	typedef _NAMESPACE DenseChainedHashMap<uint, _NAMESPACE IRefCountedPtr, _NAMESPACE NullUintHash> DataStoreMap;

	DataStoreMap m_DataStore;

public:
	CDataStoreForXSIPlugin()
	{
		m_DataStore.Initialize(128, 0.7f);
	}

	~CDataStoreForXSIPlugin()
	{

	}

public:
	void AddRefCountData(uint index, _NAMESPACE IRefCounted* pData);
	_NAMESPACE IRefCounted* GetRefCountData(uint index);

	void ClearAll();
	void ClearRefCountData(uint index);
};