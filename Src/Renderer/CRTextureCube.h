//==============================================================================
//
//		CRTextureCube.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		10/14/2007
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

class CRTextureCube : public CRTexture< IRTextureCube >
{
public:
	CRTextureCube(CRRenderer* pRenderer)
		: CRTexture< IRTextureCube >(pRenderer)
	{
	}

	virtual ~CRTextureCube()
	{
	}

protected:
	_PURE( byte* DoLock(uint level, eRCubeFace face, uint& pitch, const gmtl::Vec4i* pToWrite) )
	_PURE( boolean DoUnlock(uint level, eRCubeFace face) )

	_PURE( uint DoGetSize(uint level) const )

	_PURE( CRTextureRT* DoGetTextureRT(uint level, eRCubeFace face) )

public:
	byte* Lock(uint level, eRCubeFace face, uint& pitch, const gmtl::Vec4i* pToWrite);
	boolean Unlock(uint level, eRCubeFace face);	
	
	uint GetSize(uint level) const;

	IRTextureRT* GetTextureRT(uint level, eRCubeFace face);
};

_NAMESPACE_END