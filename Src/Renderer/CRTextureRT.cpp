//==============================================================================
//
//		CRTextureRT.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		9/21/2007
//
//==============================================================================

#include "RendererFramework.h"

_NAMESPACE_BEGIN

uint 
CRTextureRT::GetWidth() const
{
	return DoGetWidth();
}

uint 
CRTextureRT::GetHeight() const
{
	return DoGetHeight();
}

void 
CRTextureRT::CopyFromTextureRT(IRTextureRT* pRT)
{
    return DoCopyFromTextureRT(pRT);
}

boolean CRTextureRT::LockReadOnlyPrepare()
{
	return DoLockReadOnlyPrepare();
}

byte* CRTextureRT::LockReadOnly(uint& pitch)
{
	return DoLockReadOnly(pitch);
}

boolean CRTextureRT::UnlockReadOnly()
{
	return DoUnlockReadOnly();
}

_NAMESPACE_END