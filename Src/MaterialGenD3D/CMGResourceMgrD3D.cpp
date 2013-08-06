//==============================================================================
//
//		CMGResourceMgrD3D.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/8/2008
//
//==============================================================================

#include "MaterialGenD3D.h"

_NAMESPACE_BEGIN

IMGMaterialTemplate* 
CMGResourceMgrD3D::DoCreateMaterialTemplate()
{
	return _NEW CMGMaterialTemplateD3D(m_pGen);
}

_NAMESPACE_END