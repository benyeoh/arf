//==============================================================================
//
//		CIKeyboard.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/16/2007
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class CIKeyboard : public IIKeyboard
{
private: 
	byte m_KeyStates[IKC_END];
	std::vector<eIKeyCode> m_ActiveKeys;
	std::vector<eIKeyCode> m_JustPressedKeys;
	std::vector<eIKeyCode> m_JustReleasedKeys;
	
public: 
	CIKeyboard()
	{
		ClearKeyStates();
	}

	virtual ~CIKeyboard()
	{
	}

protected:
	_PURE( void DoUpdate() )
	void SetKeyState(eIKeyCode keyCode, byte state);

public: 
	void ClearKeyStates();
	void Update();

	uint GetNumOfActiveKeys();
	const eIKeyCode* GetActiveKeys();
	
	uint GetNumOfJustPressedKeys();
	const eIKeyCode* GetJustPressedKeys();
	
	uint GetNumOfJustReleasedKeys();
	const eIKeyCode* GetJustReleasedKeys();
	
};


_NAMESPACE_END