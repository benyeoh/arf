//==============================================================================
//
//		CRTextureCube.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		10/14/2007
//
//==============================================================================

#include "RendererFramework.h"

_NAMESPACE_BEGIN

byte* 
CRTextureCube::Lock(uint level, eRCubeFace face, uint& pitch, const gmtl::Vec4i* pToWrite)
{
	return DoLock(level, face, pitch, pToWrite);
}

boolean 
CRTextureCube::Unlock(uint level, eRCubeFace face)
{
	return DoUnlock(level, face);
}

uint 
CRTextureCube::GetSize(uint level) const
{
	return DoGetSize(level);
}

IRTextureRT* CRTextureCube::GetTextureRT(uint level, eRCubeFace face)
{
	return DoGetTextureRT(level, face);
}

_NAMESPACE_END