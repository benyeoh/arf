//================================================================================
//
//		CRTexture3D.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/24/2012
//
//================================================================================

#pragma once 

_NAMESPACE_BEGIN

class CRTexture3D : public CRTexture< IRTexture3D >
{
public:
	CRTexture3D(CRRenderer* pRenderer)
		: CRTexture<IRTexture3D>(pRenderer)
	{
	}

	virtual ~CRTexture3D()
	{
	}

protected:
	_PURE( byte* DoLock(uint level, uint& rowPitch, uint& slicePitch, const gmtl::Vec3i* pToWriteStart, const gmtl::Vec3i* pToWriteEnd) )
	_PURE( boolean DoUnlock(uint level) )

	_PURE( uint DoGetWidth(uint level) const )
	_PURE( uint DoGetHeight(uint level) const )
	_PURE( uint DoGetDepth(uint level) const )

	_PURE( byte* DoLockImmediate(uint level, uint& rowPitch, uint& slicePitch) )
	_PURE( boolean DoUnlockImmediate(uint level) )

public:
	uint GetWidth(uint level) const;
	uint GetHeight(uint level) const;
	uint GetDepth(uint level) const;

	byte* Lock(uint level, uint& rowPitch, uint& slicePitch, const gmtl::Vec3i* pToWriteStart, const gmtl::Vec3i* pToWriteEnd);
	boolean Unlock(uint level);

	byte* LockImmediate(uint level, uint& rowPitch, uint& slicePitch);
	boolean UnlockImmediate(uint level);
};

_NAMESPACE_END