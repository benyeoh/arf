//==============================================================================
//
//		CIMouse.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/19/2007
//
//==============================================================================

#include "InputFramework.h"

_NAMESPACE_BEGIN

void 
CIMouse::SetPos(const gmtl::Vec3f& pos)
{
	m_Pos = pos;
}

void
CIMouse::SetPosDelta(const gmtl::Vec3f& posDelta)
{
	m_PosDelta = posDelta;
}

void
CIMouse::Update()
{
	DoUpdate();
	
	m_ActiveButtons.clear();
	
	_LOOPi( IMC_END )
	{
		if(m_ButtonStates[i] != I_INACTIVE)
		{
			m_ActiveButtons.push_back( (eIMouseCode) i );
		}
	}
}

void 
CIMouse::ClearMouseStates()
{
	m_ActiveButtons.clear();
	memset(m_ButtonStates, 0, sizeof(m_ButtonStates));
	m_Pos = gmtl::Vec3f(0.0f, 0.0f, 0.0f);
	m_PosDelta = gmtl::Vec3f(0.0f, 0.0f, 0.0f);
}

void 
CIMouse::SetButtonState(eIMouseCode mouseButtonCode, byte state)
{
	_DEBUG_ASSERT(mouseButtonCode > IMC_BEGIN && mouseButtonCode < IMC_END);
	
	m_ButtonStates[mouseButtonCode] = state;
}

uint 
CIMouse::GetNumOfActiveButtons()
{
	return (uint) m_ActiveButtons.size();
}

const eIMouseCode* 
CIMouse::GetActiveButtons()
{
	if(m_ActiveButtons.size() > 0)
		return &(m_ActiveButtons[0]);
		
	return NULL;
}

_NAMESPACE_END