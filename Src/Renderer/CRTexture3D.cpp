//================================================================================
//
//		CRTexture3D.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/24/2012
//
//================================================================================

#include "RendererFramework.h"


_NAMESPACE_BEGIN

byte* CRTexture3D::Lock(uint level, uint& rowPitch, uint& slicePitch, const gmtl::Vec3i* pToWriteStart, const gmtl::Vec3i* pToWriteEnd)
{
	return DoLock(level, rowPitch, slicePitch, pToWriteStart, pToWriteEnd);
}

boolean CRTexture3D::Unlock(uint level)
{
	return DoUnlock(level);
}

uint CRTexture3D::GetWidth(uint level) const
{
	return DoGetWidth(level);
}

uint CRTexture3D::GetHeight(uint level) const
{
	return DoGetHeight(level);
}

uint CRTexture3D::GetDepth(uint level) const
{
	return DoGetDepth(level);
}

byte* CRTexture3D::LockImmediate(uint level, uint& rowPitch, uint& slicePitch)
{
	return DoLockImmediate(level, rowPitch, slicePitch);
}

boolean CRTexture3D::UnlockImmediate(uint level)
{
	return DoUnlockImmediate(level);
}

_NAMESPACE_END
