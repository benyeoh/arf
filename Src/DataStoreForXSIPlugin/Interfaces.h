//================================================================================
//
//		Interfaces.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		27/9/2011
//
//================================================================================

#pragma once

struct IDataStoreForXSIPlugin
{
	_PURE( void AddRefCountData(uint index, _NAMESPACE IRefCounted* pData) );
	_PURE( _NAMESPACE IRefCounted* GetRefCountData(uint index) );

	_PURE( void ClearAll() );
	_PURE( void ClearRefCountData(uint index) );
};

extern "C"
{
	typedef IDataStoreForXSIPlugin* (*GetDataStoreFn)();
	_DECL_DYNAMIC_EXPORT( IDataStoreForXSIPlugin* GetDataStore() )
}
