//==============================================================================
//
//		LoadSaveFns.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		3/29/2007
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

uint 
SaveVBGroup(IRVertexBufferGroup* pVBGroup, IByteBuffer* pBuffer);

uint 
SaveVB(IRVertexBuffer* pVB, IByteBuffer* pBuffer);

uint 
SaveIB(IRIndexBuffer* pIB, IByteBuffer* pBuffer);

uint
SaveEffectParam(const REffectParam* pParam, IByteBuffer* pBuffer);

uint
SaveEffect(IREffect* pEffect, IByteBuffer* pBuffer);

uint
LoadVBGroup(IRRenderer* pRenderer, IByteBuffer* pBuffer, uint offset, IRVertexBufferGroup** ppGroup);

uint
LoadVB(IRRenderer* pRenderer, IByteBuffer* pBuffer, uint offset, IRVertexBuffer** ppVB);

uint
LoadIB(IRRenderer* pRenderer, IByteBuffer* pBuffer, uint offset, IRIndexBuffer** ppIB);

uint 
LoadEffectParam(IRRenderer* pRenderer, IByteBuffer* pBuffer, uint offset, REffectParamStore* pParam);

uint
LoadEffect(IRRenderer* pRenderer, IByteBuffer* pBuffer, uint offset, IREffect** ppEffect);

_NAMESPACE_END
