// DataStoreForXSIPlugin.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}


IDataStoreForXSIPlugin* GetDataStore()
{
	static IDataStoreForXSIPlugin* s_pDataStore = NULL;
	if(!s_pDataStore)
		s_pDataStore = _NEW CDataStoreForXSIPlugin;

	return s_pDataStore;
}
