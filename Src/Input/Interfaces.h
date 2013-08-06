//==============================================================================
//
//		Interfaces.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/14/2007
//
//==============================================================================

#pragma once  

_NAMESPACE_BEGIN

enum eIKeyCode
{
	IKC_BEGIN = -1,
	
	IKC_Q = 0,
	IKC_W,
	IKC_E,
	IKC_R,
	IKC_T,
	IKC_Y,
	IKC_U,
	IKC_I,
	IKC_O,
	IKC_P,
	IKC_LBRACKET,
	IKC_RBRACKET,
	IKC_FRONTSLASH,
	IKC_A,
	IKC_S,
	IKC_D,
	IKC_F,
	IKC_G,
	IKC_H,
	IKC_J,
	IKC_K,
	IKC_L,
	IKC_SEMICOLON,
	IKC_SQUOTE,
	IKC_ENTER,
	IKC_Z,
	IKC_X,
	IKC_C,
	IKC_V,
	IKC_B,
	IKC_N,
	IKC_M,
	IKC_COMMA,
	IKC_DOT,
	IKC_BACKSLASH,
	IKC_SPACE,
	
	IKC_0,
	IKC_1,
	IKC_2,
	IKC_3,
	IKC_4,
	IKC_5,
	IKC_6,
	IKC_7,
	IKC_8,
	IKC_9,
	IKC_MINUS,
	IKC_EQUAL,
	IKC_BACKSPACE,
	IKC_TAB,
	
	IKC_LEFT,
	IKC_UP,
	IKC_RIGHT,
	IKC_DOWN,
	
	IKC_ESC,
	
	IKC_END,
};

enum eIMouseCode
{
	IMC_BEGIN = -1,
	
	IMC_LEFTBUTTON = 0,
	IMC_RIGHTBUTTON,
	IMC_MIDDLEBUTTON,

	IMC_END,
};

struct IIKeyboard
{
	_PURE( uint GetNumOfActiveKeys() )
	_PURE( const eIKeyCode* GetActiveKeys() )
		
	_PURE( uint GetNumOfJustPressedKeys() )
	_PURE( const eIKeyCode* GetJustPressedKeys() )
	
	_PURE( uint GetNumOfJustReleasedKeys() )
	_PURE( const eIKeyCode* GetJustReleasedKeys() )
};

struct IIMouse
{
	_PURE( const gmtl::Vec3f& GetPosDelta() )	

	_PURE( uint GetNumOfActiveButtons() )
	_PURE( const eIMouseCode* GetActiveButtons() )
};

struct IIInput : public IRefCounted
{
	_PURE( void Update() )
	_PURE( boolean Initialize() )
	_PURE( IIKeyboard& GetKeyboard() )
	_PURE( IIMouse& GetMouse() )
};

//////////////////////////////////////////////////////////////////////////
//	
//	Smart Pointers
//
//////////////////////////////////////////////////////////////////////////

_DECLARE_SMARTPTR(IIInput)

_NAMESPACE_END

//////////////////////////////////////////////////////////////////////////
//	 
//	DLL exposed functions
//
//////////////////////////////////////////////////////////////////////////

extern "C"
{
	typedef _NAMESPACE IIInput* (*CreateInputFn)();
	INPUT_API _NAMESPACE IIInput* CreateInput();
}