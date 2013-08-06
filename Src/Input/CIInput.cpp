//==============================================================================
//
//		CIInput.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/19/2007
//
//==============================================================================

#include "InputFramework.h"

_NAMESPACE_BEGIN

boolean
CIInput::Initialize()
{
	return DoInitialize();
}

void 
CIInput::Update()
{
	if(m_pMouse)
		m_pMouse->Update();
	
	if(m_pKeyboard)	
		m_pKeyboard->Update();
}

_NAMESPACE_END