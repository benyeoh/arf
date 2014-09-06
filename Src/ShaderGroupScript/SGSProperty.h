//==============================================================================
//
//		SGSProperty.h
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Thursday, 19 June, 2014
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

struct SGSProperty
{
	IByteBufferPtr pName;
	SGSExpression expr;

	SGSProperty()
	{
	}
};

_NAMESPACE_END