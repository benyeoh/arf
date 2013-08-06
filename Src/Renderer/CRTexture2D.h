//==============================================================================
//
//		CRTexture2D.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		Saturday, March 25, 2006
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

class CRTexture2D : public CRTexture< IRTexture2D >
{
public:
	CRTexture2D(CRRenderer* pRenderer)
		: CRTexture< IRTexture2D >(pRenderer)
	{
	}

	virtual ~CRTexture2D()
	{
	}

protected:
	_PURE( byte* DoLock(uint level, uint& pitch, const gmtl::Vec4i* pToWrite) )
	_PURE( boolean DoUnlock(uint level) )
	
	_PURE( uint DoGetWidth(uint level) const )
	_PURE( uint DoGetHeight(uint level) const )

	_PURE( byte* DoLockImmediate(uint level, uint& pitch) )
	_PURE( boolean DoUnlockImmediate(uint level) )

	_PURE( CRTextureRT* DoGetTextureRT(uint level) ) 

public:
	byte* Lock(uint level, uint& pitch, const gmtl::Vec4i* pToWrite);
	boolean Unlock(uint level);

	// These are for write only updates
	// Values written using these are not available for read back
	byte* LockImmediate(uint level, uint& pitch);
	boolean UnlockImmediate(uint level);

	uint GetWidth(uint level) const;
	uint GetHeight(uint level) const;	

	IRTextureRT* GetTextureRT(uint level); 
};

_NAMESPACE_END