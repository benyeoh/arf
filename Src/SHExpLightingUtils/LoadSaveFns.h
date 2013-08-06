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
SaveSphereTree(SEFXUSphereTree* pSphereTree, IByteBuffer* pBuffer);

uint
LoadSphereTree(IByteBuffer* pBuffer, uint offset, SEFXUSphereTree** ppSphereTree);

_NAMESPACE_END
