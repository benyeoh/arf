//==============================================================================
//
//		CMGMaterialGen.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/6/2008
//
//==============================================================================

#include "MaterialGenFramework.h"

_NAMESPACE_BEGIN

void 
CMGMaterialGen::Initialize(IAppCallback& callback)
{
	if(!m_pCallback)
	{
		DoInitialize(callback);
		m_pCallback = &callback;
	}
}

_NAMESPACE_END
