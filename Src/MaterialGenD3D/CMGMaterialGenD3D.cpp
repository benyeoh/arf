//==============================================================================
//
//		CMGMaterialGenD3D.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/8/2008
//
//==============================================================================

#include "MaterialGenD3D.h"

_NAMESPACE_BEGIN

void 
CMGMaterialGenD3D::DoInitialize(IAppCallback& callback)
{
	m_pResMgr = _NEW CMGResourceMgrD3D(this);
}

_NAMESPACE_END