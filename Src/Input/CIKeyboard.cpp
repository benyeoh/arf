//==============================================================================
//
//		CIKeyboard.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/16/2007
//
//==============================================================================

#include "InputFramework.h"

_NAMESPACE_BEGIN

void 
CIKeyboard::SetKeyState(eIKeyCode keyCode, byte state)
{
	_DEBUG_ASSERT(keyCode > IKC_BEGIN && keyCode < IKC_END);
	
	switch(state)
	{
		case I_ACTIVE:
		{
			switch(m_KeyStates[keyCode])
			{
				case I_INACTIVE:
				{
					_LOOPi((uint)m_JustReleasedKeys.size())
					{
						if(m_JustReleasedKeys[i] == keyCode)
						{
							m_JustReleasedKeys.erase(m_JustReleasedKeys.begin() + i);
							break;
						}
					}
					
					m_ActiveKeys.push_back(keyCode);
					m_JustPressedKeys.push_back(keyCode);
					m_KeyStates[keyCode] = state; 
					break;
				}
			}
			break;
		}
		
		case I_INACTIVE:
		{
			switch(m_KeyStates[keyCode])
			{
				case I_ACTIVE:
				{
					_LOOPi((uint)m_JustPressedKeys.size())
					{
						if(m_JustPressedKeys[i] == keyCode)
						{
							m_JustPressedKeys.erase(m_JustPressedKeys.begin() + i);
							break;
						}
					}
					
					_LOOPi((uint)m_ActiveKeys.size())
					{
						if(m_ActiveKeys[i] == keyCode)
						{
							m_ActiveKeys.erase(m_ActiveKeys.begin() + i);
							break;
						}
					}
					m_JustReleasedKeys.push_back(keyCode);
					m_KeyStates[keyCode] = state; 
					break;
				}	
			}
			break;			
		}
	}
	m_KeyStates[keyCode] = state;
}

void 
CIKeyboard::ClearKeyStates()
{
	memset(m_KeyStates, 0, sizeof(m_KeyStates));
	m_ActiveKeys.clear();
	m_JustPressedKeys.clear();
	m_JustReleasedKeys.clear();
}

void 
CIKeyboard::Update()
{
	m_JustPressedKeys.clear();
	m_JustReleasedKeys.clear();
	
	DoUpdate();
	
	//m_ActiveKeys.clear();
	//
	//_LOOPi( IKC_END )
	//{
	//	if(m_KeyStates[i] != I_INACTIVE)
	//	{
	//		m_ActiveKeys.push_back( (eIKeyCode) i );
	//	}
	//}
}

uint 
CIKeyboard::GetNumOfActiveKeys()
{
	return (uint) m_ActiveKeys.size();
}

const eIKeyCode* 
CIKeyboard::GetActiveKeys()
{
	if(m_ActiveKeys.size() > 0)
		return &(m_ActiveKeys[0]);
		
	return NULL;
}

uint 
CIKeyboard::GetNumOfJustReleasedKeys()
{
	return (uint) m_JustReleasedKeys.size();
}

const eIKeyCode* 
CIKeyboard::GetJustReleasedKeys()
{
	if(m_JustReleasedKeys.size() > 0)
		return &(m_JustReleasedKeys[0]);
		
	return NULL;
}
uint 
CIKeyboard::GetNumOfJustPressedKeys()
{
	return (uint) m_JustPressedKeys.size();
}

const eIKeyCode* 
CIKeyboard::GetJustPressedKeys()
{
	if(m_JustPressedKeys.size() > 0)
		return &(m_JustPressedKeys[0]);
		
	return NULL;
}


_NAMESPACE_END
