//==============================================================================
//	
//      CRC32.h
//
//			Calculates the CRC value given a series of chars
//  
//      Author:     Referenced from some mag
//      Date:       2005/06/20
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

extern "C"
{

//-------------------------------------------------------------------------
//	CRCCalcString()
//		Calcuates a 32-bit CRC on a string
//-------------------------------------------------------------------------
uint CRCCalcString(const char* pString);

//--------------------------------------------------------------------------
//	CRCCalcCharLength()
//--------------------------------------------------------------------------
uint CRCCalcCharLength(const char* pChar, uint length);

}

_NAMESPACE_END