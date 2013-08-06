//==============================================================================
//
//		CSHFXShadowFX.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		8/9/2008
//
//==============================================================================

#include "ShadowFXFramework.h"

_NAMESPACE_BEGIN

boolean 
CSHFXShadowFX::Initialize(IRRenderer& renderer, IAppCallback& callback)
{
	m_pRenderer = &renderer;
	m_pAppCallback = &callback;
	
	m_ResMgr.SetShadowFX(this);
	m_Debug.SetShadowFX(this);
	m_Debug.Initialize();
	
	return TRUE;
}	

_NAMESPACE_END