//==============================================================================
//
//		CIInput.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/16/2007
//
//==============================================================================

#pragma once


_NAMESPACE_BEGIN

class CIInput : public CRefCounted<IIInput>
{
protected: 
	CIKeyboard* m_pKeyboard;
	CIMouse* m_pMouse;
	
public: 
	CIInput(CIMouse* pMouse, CIKeyboard* pKeyboard)
		: m_pMouse(pMouse)
		, m_pKeyboard(pKeyboard)
	{
	}

	virtual ~CIInput()
	{
		_DELETE(m_pKeyboard);
		_DELETE(m_pMouse);
		
		g_pInput = NULL;
	}

protected:
	_PURE( boolean DoInitialize() )
	
public: 
	boolean Initialize();
	void Update();
	IIKeyboard& GetKeyboard() { return *m_pKeyboard; }
	IIMouse& GetMouse() { return *m_pMouse; }
};


_NAMESPACE_END