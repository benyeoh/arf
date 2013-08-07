//==============================================================================
//
//		DFXRenderData.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/28/2008
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

struct DFXRenderData
{
	IRVertexBufferGroupPtr pVBGroup;
	IRVertexBufferPtr pVB;
	IRIndexBufferPtr pIB;
	IREffectPtr pEffect;
};

_NAMESPACE_END