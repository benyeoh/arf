//==============================================================================
//
//		CRTextureRT.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/21/2007
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

class CRTextureRT : public CRTexture< IRTextureRT >
{
private:
	
public:
	CRTextureRT(CRRenderer* pRenderer)
		: CRTexture< IRTextureRT >(pRenderer)
	{
	}

	virtual ~CRTextureRT()
	{
	}

protected:
	_PURE( uint DoGetWidth() const )
	_PURE( uint DoGetHeight() const )
    _PURE( void DoCopyFromTextureRT(IRTextureRT* pRT) )
	_PURE( boolean DoLockReadOnlyPrepare() )
	_PURE( byte* DoLockReadOnly(uint& pitch) )
	_PURE( boolean DoUnlockReadOnly() )

public:
    void CopyFromTextureRT(IRTextureRT* pRT);

	uint GetWidth() const;
	uint GetHeight() const;

	boolean LockReadOnlyPrepare();
	byte* LockReadOnly(uint& pitch);
	boolean UnlockReadOnly();
};

_NAMESPACE_END