//==============================================================================
//
//		CRTexture2D.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Thursday, April 20, 2006
//
//==============================================================================

#include "RendererFramework.h"


_NAMESPACE_BEGIN

byte* 
CRTexture2D::Lock(uint level, uint& pitch, const gmtl::Vec4i* pToWrite)
{
	return DoLock(level, pitch, pToWrite);
}

boolean 
CRTexture2D::Unlock(uint level)
{
	return DoUnlock(level);
}

uint 
CRTexture2D::GetWidth(uint level) const
{
	return DoGetWidth(level);
}

uint 
CRTexture2D::GetHeight(uint level) const
{
	return DoGetHeight(level);
}

byte* 
CRTexture2D::LockImmediate(uint level, uint& pitch)
{
	return DoLockImmediate(level, pitch);
}

boolean 
CRTexture2D::UnlockImmediate(uint level)
{
	return DoUnlockImmediate(level);
}

IRTextureRT* CRTexture2D::GetTextureRT(uint level)
{
	return DoGetTextureRT(level);
}

_NAMESPACE_END