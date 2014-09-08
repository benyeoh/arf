//==============================================================================
//
//		ISGSUserFunction.h
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Thursday, 21 August, 2014
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

struct ISGSUserFunction
{
	_PURE( const char* GetName() )
	_PURE( void GetParameterSig(eSGSValueType** ppParamTypes, uint& numParams) )
	
	_PURE( boolean Execute(const SGSScript* pSrc, SGSValue* parameters, int numParameters, SGSValue* pResults, int& numResults) )
};

_NAMESPACE_END