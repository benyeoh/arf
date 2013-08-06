//==============================================================================
//
//		CDEFXDeferredFX.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		13/10/2009
//
//==============================================================================

#include "DeferredFXFramework.h"

_NAMESPACE_BEGIN

boolean
CDEFXDeferredFX::Initialize(IRRenderer& renderer, IAppCallback& callback)
{
	m_pRenderer	= &renderer;	
	m_pAppCallback = &callback;

	m_ResMgr.SetDeferredFX(this);

	return TRUE;
}

_NAMESPACE_END
