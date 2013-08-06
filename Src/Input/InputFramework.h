//==============================================================================
//
//		InputFramework.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/16/2007
//
//==============================================================================

#ifndef _INPUTFRAMEWORK_H
#define _INPUTFRAMEWORK_H

#include "Input.h"

_NAMESPACE_BEGIN

class CIInput;

extern CIInput* g_pInput;

_NAMESPACE_END

// Additional abstract class implementation includes
#include "InputDefines.h"
#include "CIKeyboard.h"
#include "CIMouse.h"
#include "CIInput.h"

#endif